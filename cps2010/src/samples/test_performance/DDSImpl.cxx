#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include "test.h"

#include "MessagingIF.h"

#include "perftest.h"

#include "DDSImpl.h"

#include "Property.h"



#pragma warning(push)
#pragma warning(disable : 4996)
#define STRNCASECMP     _strnicmp

#define IS_OPTION(str, option) (STRNCASECMP(str, option, strlen(str)) == 0)

int          NeoDDSImpl::_WaitsetEventCount;
unsigned int NeoDDSImpl::_WaitsetDelayUsec;


/*********************************************************
 * Shutdown
 */
void NeoDDSImpl::Shutdown()
{
#if 0 
    if (_participant != NULL) {
        perftest_cpp::MilliSleep(2000);

        if (_reader != NULL) {
            _subscriber->delete_datareader(_reader);
        }

        perftest_cpp::MilliSleep(4000);

        _participant->delete_contained_entities();
        DDSTheParticipantFactory->delete_participant(_participant);
    }


    if(_pongSemaphore != NULL) {
	RTIOsapiSemaphore_delete(_pongSemaphore);
	_pongSemaphore = NULL;
    }

    DDSDomainParticipantFactory::finalize_instance();
#else
	quit();
#endif
}


/*********************************************************
 * PrintCmdLineHelp
 */
void NeoDDSImpl::PrintCmdLineHelp()
{
   const char *usage_string =
        /**************************************************************************/
        "\t-sendQueueSize <number> - Sets number of samples (or batches) in send\n"
        "\t                          queue, default 50\n"
        "\t-domain <ID>            - RTI DDS Domain, default 59\n"
        "\t-qosprofile <filename>  - Name of XML file for DDS Qos profiles, \n"
        "\t                          default perftest.xml\n"
        "\t-nic <ipaddr>           - Use only the nic specified by <ipaddr>.\n"
        "\t                          If unspecificed, use all available interfaces\n"
        "\t-nomulticast            - Do not use multicast to send data, default\n"
        "\t                          use multicast\n"
        "\t-multicastAddress <ipaddr> - Multicast address to use for receiving \n"
        "\t                          latency/announcement (pub) or \n"
        "\t                          throughtput(sub) data.\n"
        "\t                          If unspecified: latency 239.255.1.2,\n"
        "\t                          announcement 239.255.1.100,\n"
	"\t                          throughput 239.255.1.1\n"
        "\t-bestEffort             - Run test in best effort mode, default reliable\n"
        "\t-batchSize <bytes>      - Size in bytes of batched message, default 0\n"
        "\t                          (no batching)\n"
        "\t-noPositiveAcks         - Disable use of positive acks in reliable \n"
        "\t                          protocol, default use positive acks\n"
        "\t-keepDurationUsec <usec> - Minimum time (us) to keep samples when\n"
        "\t                          positive acks are disabled, default 1000 us\n"
        "\t-enableSharedMemory     - Enable use of shared memory transport, default\n"
        "\t                          shared memory not enabled\n"
        "\t-enableTcpOnly          - Enable use of TCP transport and disable all\n"
        "\t                          the other transports, default do not use\n"
        "\t                          tcp transport\n"
        "\t-heartbeatPeriod <sec>:<nanosec>     - Sets the regular heartbeat period\n"
        "\t                          for throughput DataWriter, default 0:0\n"
        "\t                          (use XML QoS Profile value)\n"
        "\t-fastHeartbeatPeriod <sec>:<nanosec> - Sets the fast heartbeat period\n"
        "\t                          for the throughput DataWriter, default 0:0\n"
        "\t                          (use XML QoS Profile value)\n"
        "\t-durability <0|1|2|3>   - Set durability QOS, 0 - volatile,\n"
        "\t                          1 - transient local, 2 - transient, \n"
        "\t                          3 - persistent, default 0\n"
        "\t-noDirectCommunication  - Use brokered mode for persistent durability\n"
        "\t-instanceHashBuckets <#count> - Number of hash buckets for instances.\n"
        "\t                          If unspecified, same as number of\n"
        "\t                          instances.\n"
        "\t-waitsetDelayUsec <usec>  - UseReadThread related. Allows you to\n"
        "\t                          process incoming data in groups, based on the\n"
        "\t                          time rather than individually. It can be used \n"
        "\t                          combined with -waitsetEventCount,\n"
        "\t                          default 100 usec\n"
        "\t-waitsetEventCount <count> - UseReadThread related. Allows you to\n"
        "\t                          process incoming data in groups, based on the\n"
        "\t                          number of samples rather than individually. It\n"
        "\t                          can be used combined with -waitsetDelayUsec,\n"
        "\t                          default 5\n"
        ;

    fprintf(stderr, usage_string);
}


/*********************************************************
 * ParseConfig
 */
bool NeoDDSImpl::ParseConfig(int argc, char *argv[])
{
    int i;
    int sec = 0;
    unsigned int nanosec = 0;
    // first scan for configFile

    for (i = 1; i < argc; ++i) 
    {
        if (IS_OPTION(argv[i], "-configFile")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') {
                fprintf(stderr, "Missing <fileName> after -configFile\n");
                return false;
            }
            _ConfigFile = argv[i];
        }
    }

    // now load configuration values from config file
    if (_ConfigFile != NULL)
    {
        QosDictionary *configSource;

        try {
            configSource = new QosDictionary(_ConfigFile);
        } catch (std::logic_error e) {
            fprintf(stderr, "Problem loading configuration file.\n");
            fprintf(stderr, "%s\n", e.what());
            return false;
        }

        QosProfile* config = configSource->get("perftest");
        
        if (config == NULL)
        {
            fprintf(stderr, "Could not find section [perftest] in file %s\n", _ConfigFile);
            return false;
        }

        _DataLen        = config->get_int("data length", _DataLen);
        _InstanceCount  = config->get_int("instances", _InstanceCount);
		_LatencyTest	= config->get_bool("run latency test", _LatencyTest);
        _IsDebug		= config->get_bool("is debug", _IsDebug);

        config = configSource->get("RTIImpl");
        
        if (config == NULL)
        {
            fprintf(stderr, "Could not find section [RTIImpl] in file %s\n", _ConfigFile);
            return false;
        }

        _SendQueueSize = config->get_int("send queue size", _SendQueueSize);
        _DomainID = config->get_int("domain", _DomainID);
        _ProfileFile = config->get_string("qos profile file", _ProfileFile);
        _Nic = config->get_string("interface", _Nic);
        _IsMulticast = config->get_bool("is multicast", _IsMulticast);
        _IsReliable = config->get_bool("is reliable", _IsReliable);
        _BatchSize = config->get_int("batch size", _BatchSize);
        _KeepDurationUsec = (unsigned int)config->get_int("keep duration usec", (int)_KeepDurationUsec);
        _UsePositiveAcks = config->get_bool("use positive acks", _UsePositiveAcks);
        _UseSharedMemory = config->get_bool("enable shared memory", _UseSharedMemory);
        _UseTcpOnly = config->get_bool("enable tcp only", _UseTcpOnly);
        _WaitsetEventCount = config->get_int("waitset event count", _WaitsetEventCount);
        _WaitsetDelayUsec = (unsigned int)config->get_int("waitset delay usec", (int)_WaitsetDelayUsec);
        _Durability = config->get_int("durability", _Durability);
        _DirectCommunication = config->get_bool("direct communication", _DirectCommunication);
        _HeartbeatPeriod.sec = config->get_int(
            "heartbeat period sec", _HeartbeatPeriod.sec);
        _HeartbeatPeriod.nanosec = config->get_int(
            "heartbeat period nanosec", _HeartbeatPeriod.nanosec);
        _FastHeartbeatPeriod.sec = config->get_int(
            "fast heartbeat period sec", _FastHeartbeatPeriod.sec);
        _FastHeartbeatPeriod.nanosec = config->get_int(
            "fast heartbeat period nanosec", _FastHeartbeatPeriod.nanosec);
        _InstanceHashBuckets = config->get_int(
            "instance hash buckets", _InstanceHashBuckets);
    }

    // now load everything else, command line params override config file
    for (i = 0; i < argc; ++i) 
    {
        if (IS_OPTION(argv[i], "-dataLen")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <length> after -dataLen\n");
                return false;
            }
            _DataLen = strtol(argv[i], NULL, 10);
            if (_DataLen < perftest_cpp::OVERHEAD_BYTES)
            {
                fprintf(stderr, "-dataLen must be >= %d\n", perftest_cpp::OVERHEAD_BYTES);
                return false;
            }
            if (_DataLen > TestMessage::MAX_DATA_SIZE)
            {
                fprintf(stderr, "-dataLen must be <= %d\n", TestMessage::MAX_DATA_SIZE);
                return false;
            }
            if (_DataLen > MAX_BINDATA_SIZE)
            {
                fprintf(stderr, "-dataLen must be <= %d\n", MAX_BINDATA_SIZE);
                return false;
            }
        }
        else if (IS_OPTION(argv[i], "-sendQueueSize")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <count> after -sendQueueSize\n");
                return false;
            }
            _SendQueueSize = strtol(argv[i], NULL, 10);
        }
        else if (IS_OPTION(argv[i], "-heartbeatPeriod"))
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <period> after -heartbeatPeriod\n");
                return false;
            }

            sec = 0;
            nanosec = 0;

            if (sscanf(argv[i],"%d:%d",&sec,&nanosec) != 2) {
                fprintf(stderr, "-heartbeatPeriod value must have the format <sec>:<nanosec>\n");
                return false;
            }

            if (sec > 0 || nanosec > 0) {
                _HeartbeatPeriod.sec = sec;
                _HeartbeatPeriod.nanosec = nanosec;
            }
        }
        else if (IS_OPTION(argv[i], "-fastHeartbeatPeriod"))
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <period> after -fastHeartbeatPeriod\n");
                return false;
            }

            sec = 0;
            nanosec = 0;

            if (sscanf(argv[i],"%d:%d",&sec,&nanosec) != 2) {
                fprintf(stderr, "-fastHeartbeatPeriod value must have the format <sec>:<nanosec>\n");
                return false;
            }

            if (sec > 0 || nanosec > 0) {
                _FastHeartbeatPeriod.sec = sec;
                _FastHeartbeatPeriod.nanosec = nanosec;
            }
        }
        else if (IS_OPTION(argv[i], "-domain")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <id> after -domain\n");
                return false;
            }
            _DomainID = strtol(argv[i], NULL, 10);
        } 
        else if (IS_OPTION(argv[i], "-qosprofile")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <filename> after -qosprofile\n");
                return false;
            }
            _ProfileFile = argv[i];
        }
        else if (IS_OPTION(argv[i], "-multicastAddress")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <multicast address> after -multicastAddress\n");
                return false;
            }
            THROUGHPUT_MULTICAST_ADDR = argv[i];
            LATENCY_MULTICAST_ADDR = argv[i];
            ANNOUNCEMENT_MULTICAST_ADDR = argv[i];
        }
        else if (IS_OPTION(argv[i], "-nic")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <address> after -nic\n");
                return false;
            }
            _Nic = argv[i];
        } 
        else if (IS_OPTION(argv[i], "-nomulticast") )
        {
            _IsMulticast = false;
        } 
        else if (IS_OPTION(argv[i], "-bestEffort")) 
        {
            _IsReliable = false;
        }
        else if (IS_OPTION(argv[i], "-durability")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <kind> after -durability\n");
                return false;
            }       
            _Durability = strtol(argv[i], NULL, 10);

            if ((_Durability < 0) || (_Durability > 3)) 
            {
                fprintf(stderr, "durability kind must be 0(volatile), 1(transient local), 2(transient), or 3(persistent) \n");
                return false;
            }
        }
        else if (IS_OPTION(argv[i], "-noDirectCommunication")) 
        {
            _DirectCommunication = false;
        }
        else if (IS_OPTION(argv[i], "-instances")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <count> after -instances\n");
                return false;
            }
            _InstanceCount = strtol(argv[i], NULL, 10);

            if (_InstanceCount <= 0) 
            {
                fprintf(stderr, "instance count cannot be negative or zero\n");
                return false;
            }
        }
        else if (IS_OPTION(argv[i], "-instanceHashBuckets")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <count> after -instanceHashBuckets\n");
                return false;
            }
            _InstanceHashBuckets = strtol(argv[i], NULL, 10);

            if (_InstanceHashBuckets <= 0 && _InstanceHashBuckets != -1) 
            {
                fprintf(stderr, "instance hash buckets cannot be negative or zero\n");
                return false;
            }
        }
        else if (IS_OPTION(argv[i], "-batchSize")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <#bytes> after -batchSize\n");
                return false;
            }
            _BatchSize = strtol(argv[i], NULL, 10);

            if (_BatchSize < 0) 
            {
                fprintf(stderr, "batch size cannot be negative\n");
                return false;
            }
        } 
        else if (IS_OPTION(argv[i], "-keepDurationUsec")) 
        {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <usec> after -keepDurationUsec\n");
                return false;
            }
            _KeepDurationUsec = strtol(argv[i], NULL, 10);
            if (_KeepDurationUsec < 0) 
            {
                fprintf(stderr, "keep duration usec cannot be negative\n");
                return false;
            }
        }                
        else if (IS_OPTION(argv[i], "-noPositiveAcks") )
        {
            _UsePositiveAcks = false;
        } 
        else if (IS_OPTION(argv[i], "-enableSharedMemory") )
        {
            _UseSharedMemory = true;
        } 
        else if (IS_OPTION(argv[i], "-enableTcpOnly") )
        {
            _UseTcpOnly = true;
        } 
        
        else if (IS_OPTION(argv[i], "-debug")) 
        {
#if 0 
            NDDSConfigLogger::get_instance()->
                set_verbosity(NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
#else
			// nothing for debug option
#endif
        }
        else if (IS_OPTION(argv[i], "-waitsetDelayUsec")) {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <usec> after -waitsetDelayUsec\n");
                return false;
            }
            _WaitsetDelayUsec = (unsigned int) strtol (argv[i], NULL, 10);
            if (_WaitsetDelayUsec < 0) 
            {
                fprintf(stderr, "waitset delay usec cannot be negative\n");
                return false;
            }
        }
        else if (IS_OPTION(argv[i], "-waitsetEventCount")) {
            if ((i == (argc-1)) || *argv[++i] == '-') 
            {
                fprintf(stderr, "Missing <count> after -waitsetEventCount\n");
                return false;
            }
            _WaitsetEventCount = strtol (argv[i], NULL, 10);
            if (_WaitsetEventCount < 0) 
            {
                fprintf(stderr, "waitset event count cannot be negative\n");
                return false;
            }
        } 
	else if (IS_OPTION(argv[i], "-latencyTest")) 
        {
            _LatencyTest = true;
        }
        else if (IS_OPTION(argv[i], "-configFile")) {
            /* Ignore config file */
            ++i;
        } else {
            if (i > 0) {
                fprintf(stderr, "%s: not recognized\n", argv[i]);
                return false;
            }
        }
    }

    return true;
}


/*********************************************************
 * DomainListener
 */
#ifndef NeoDDS
class DomainListener : public DDSDomainParticipantListener
{
    virtual void on_inconsistent_topic(
        Topic *topic,
        const InconsistentTopicStatus& /*status*/)
    {
        fprintf(stderr,"Found inconsistent topic %s of type %s.\n",
			topic->topic_name, topic->type_name);
			//topic->get_name(), topic->get_type_name());
        fflush(stderr);
    }

    virtual void on_offered_incompatible_qos(
        DataWriter *writer,
        const OfferedIncompatibleQosStatus &status)
    {
        fprintf(stderr,"Found incompatible reader for writer %s QoS is %d.\n",
			writer->p_topic->topic_name, status.last_policy_id); 
			   //->get_topic()->get_name(), status.last_policy_id);
        fflush(stderr);
    }

    virtual void on_requested_incompatible_qos(
        DataReader *reader,
        const RequestedIncompatibleQosStatus &status)
    {
        fprintf(stderr,"Found incompatible writer for reader %s QoS is %d.\n",
			reader->p_topic->topic_name, status.last_policy_id); 
               //reader->get_topicdescription()->get_name(), status.last_policy_id);
        fflush(stderr);
    }
};
#else
static void on_inconsistent_topic(
    Topic *topic,
    InconsistentTopicStatus *status)
{
    fprintf(stderr,"Found inconsistent topic %s of type %s.\n",
		topic->topic_name, topic->type_name);
		//topic->get_name(), topic->get_type_name());
    fflush(stderr);
}

static void on_offered_incompatible_qos(
    DataWriter *writer,
    OfferedIncompatibleQosStatus *status)
{
    fprintf(stderr,"Found incompatible reader for writer %s QoS is %d.\n",
		writer->p_topic->topic_name, status->last_policy_id); 
			//->get_topic()->get_name(), status.last_policy_id);
    fflush(stderr);
}

static void on_requested_incompatible_qos(
    DataReader *reader,
    RequestedIncompatibleQosStatus *status)
{
    fprintf(stderr,"Found incompatible writer for reader %s QoS is %d.\n",
		reader->p_topic->topic_name, status->last_policy_id); 
            //reader->get_topicdescription()->get_name(), status.last_policy_id);
    fflush(stderr);
}
#endif

int gettimeofday(struct timeval* tp, int* tz)
 {
	LARGE_INTEGER tickNow;
	static LARGE_INTEGER tickFrequency;
	static BOOL tickFrequencySet = FALSE;
	if (tickFrequencySet == FALSE) {
		QueryPerformanceFrequency(&tickFrequency);
		tickFrequencySet = TRUE;
	}
	QueryPerformanceCounter(&tickNow);
	tp->tv_sec = (long) (tickNow.QuadPart / tickFrequency.QuadPart);
	tp->tv_usec = (long) (((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);
	
	return 0;
 }

/*********************************************************
 * RTIPublisher
 */
class RTIPublisher : public IMessagingWriter
{
  private:
	DataWriter *_dw;
    FooDataWriter *_writer;
    TestData_t data;
    int _num_instances;
    unsigned long _instance_counter;
    InstanceHandle_t *_instance_handles;
	//RTIOsapiSemaphore *_pongSemaphore;
	bool _LatencyTest;
	cond_t	*_cond;
	mutex_t	*_mutex; 
#if 1 // 20140320
	int send_count;
#endif

 public:
    //RTIPublisher(DataWriter *writer, int num_instances, RTIOsapiSemaphore * pongSemaphore)
	RTIPublisher(DataWriter *writer, int num_instances, bool LatencyTest, cond_t *cond, mutex_t *mutex)
    {
		_dw = writer;
        //_writer = TestData_tDataWriter::narrow(writer);
		_writer = (FooDataWriter *)writer;
        //data.bin_data.maximum(0);

        _num_instances = num_instances;
        _instance_counter = 0;
        _instance_handles = 
            (InstanceHandle_t *) malloc(sizeof(InstanceHandle_t)*num_instances);
		
		// _pongSemaphore = pongSemaphore;
		_LatencyTest = LatencyTest;
		_cond = cond;
		_mutex = mutex;

        for (int i=0; i<_num_instances; ++i)
        {
            data.key[0] = (char) (i);
            data.key[1] = (char) (i >> 8);
            data.key[2] = (char) (i >> 16);
            data.key[3] = (char) (i >> 24);

            _instance_handles[i] = _writer->register_instance(_writer, (Foo*)&data);
        }
#if 1 // 20140320
		send_count = 0;
#endif
    }

    void Flush()
    {
        //_writer->flush();
    }

    bool Send(TestMessage &message)
    {
        ReturnCode_t retcode;
        int key = 0;

        data.entity_id = message.entity_id;
        data.seq_num = message.seq_num;
        data.timestamp_sec = message.timestamp_sec;
        data.timestamp_usec = message.timestamp_usec;
        data.latency_ping = message.latency_ping;
		data.sent_flag = message.sent_flag;
		if(message.size != 0)
			memcpy(data.bin_data, message.data, (size_t)TEST_DATA_LENGTH); 

		if (_num_instances > 1) {
            key = _instance_counter++ % _num_instances;
            data.key[0] = (char) (key);
            data.key[1] = (char) (key >> 8);
            data.key[2] = (char) (key >> 16);
            data.key[3] = (char) (key >> 24);
        }

#if 0 // 20140319
		struct timeval now;
		time_t timer;
		struct tm t;

		gettimeofday(&now, NULL);		
		timer = time(NULL);
		localtime_s(&t, &timer);

		printf("[%02d:%02d:%02d.%03d] count %d \n", 
			t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec/1000, ++send_count);
#endif

        retcode = _writer->write(_writer, (Foo*)&data, _instance_handles[key]);

        if (retcode != RETCODE_OK)
        {
            fprintf(stderr,"Write error %d.\n", retcode);
            return false;
        }

        return true;
    }

    void WaitForReaders(int numSubscribers)
    {
        PublicationMatchedStatus status;

        while (true)
        {
            _writer->get_publication_matched_status((DataWriter *)_writer, &status);
            if (status.current_count >= numSubscribers)
            {
                break;
            }
            perftest_cpp::MilliSleep(1000);
        }
    }

    bool waitForPingResponse() 
    {
		/*
		if(_pongSemaphore != NULL) 
		{
			if(!RTIOsapiSemaphore_take(_pongSemaphore, NULL)) 
			{
			fprintf(stderr,"Unexpected error taking semaphore\n");
			return false;
			}
		}
		*/
		if(_LatencyTest)
		{
			mutex_lock(_mutex);

			if(!cond_wait(_cond, _mutex))
			{
				fprintf(stderr,"Unexpected error taking semaphore\n");
				return false;
			}

			mutex_unlock(_mutex);
		}

		return true;
    }

    /* time out in milliseconds */
    bool waitForPingResponse(int timeout) 
    {
		/*
        struct RTINtpTime blockDurationIn;
        RTINtpTime_packFromMillisec(blockDurationIn, 0, timeout);

		if(_pongSemaphore != NULL) 
		{
			if(!RTIOsapiSemaphore_take(_pongSemaphore, &blockDurationIn)) 
			{
			fprintf(stderr,"Unexpected error taking semaphore\n");
			return false;
			}
		}
		*/

		if(_LatencyTest)
		{
			mutex_lock(_mutex);

			if(!cond_waittimed2(_cond, _mutex, timeout))
			{
				fprintf(stderr,"Unexpected error taking semaphore\n");
				return false;
			}

			mutex_unlock(_mutex);
		}

		return true;
    }    

    bool notifyPingResponse() 
    {
		/*
		if(_pongSemaphore != NULL) 
		{
			if(!RTIOsapiSemaphore_give(_pongSemaphore)) 
			{
			fprintf(stderr,"Unexpected error giving semaphore\n");
			return false;
			}
		}
		*/

		if(_LatencyTest)
		{
			if(!cond_signal(_cond))
			{
				fprintf(stderr,"Unexpected error giving semaphore\n");
				return false;
			}
		}

		return true;
    }
};

#ifdef NeoDDS
#define _REMOVE_ELEM( p_ar, i_oldsize, i_pos )										\
    do																				\
    {																				\
        if( (i_oldsize) - (i_pos) - 1 )												\
        {																			\
            memmove( (p_ar) + (i_pos),												\
                     (p_ar) + (i_pos) + 1,											\
                     ((i_oldsize) - (i_pos) - 1) * sizeof( *(p_ar) ) );				\
        }																			\
        if( i_oldsize > 1 )															\
        {																			\
            (p_ar) = (Foo**)realloc( p_ar, ((i_oldsize) - 1) * sizeof( *(p_ar) ) );	\
        }																			\
        else																		\
        {																			\
            FREE( p_ar );															\
            (p_ar) = NULL;															\
        }																			\
        (i_oldsize)--;																\
    }																				\
    while( 0 )
#endif

/*********************************************************
 * ReceiverListener
 */
#ifndef NeoDDS 
class ReceiverListener : public DDSDataReaderListener
#else
class ReceiverListener
#endif
{
  private:
    //TestMessage       _message;
    //IMessagingCB     *_callback;

  public:
	DataReaderListener	datareaderListener;

	ReceiverListener(CALLBACK_FUNC pCallBack)
	{
		datareaderListener.m_CallBack = pCallBack;
		datareaderListener.on_data_available = this->on_data_available;	
	}

    static void on_data_available(DataReader *reader)
    {		
		ReturnCode_t retcode;
		FooDataReader *pFDR = (FooDataReader*)reader;
		FooSeq fseq = INIT_FOOSEQ;                
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;  
		CALLBACK_FUNC cb = reader->p_datareader_listener->m_CallBack;

		retcode = pFDR->take(pFDR, &fseq, &sSeq, LENGTH_UNLIMITED,
				ANY_SAMPLE_STATE, ANY_VIEW_STATE,
				ANY_INSTANCE_STATE);

		if (retcode == RETCODE_NO_DATA) 
		{
			return;
		}
		else if (retcode != RETCODE_OK) 
		{
			printf("take error %d\n", retcode);
			return;
		}
		
		if(fseq.i_seq)
		{
			message_t *p_message = NULL;

			while(fseq.i_seq)
			{
				TestData_t *_data;
				TestMessage	testMessage;

				p_message = (message_t *)fseq.pp_foo[0];

				memset(&testMessage, NULL, sizeof(testMessage));

				testMessage.size = p_message->i_datasize; // real data size
				
				_data = (TestData_t *)p_message->v_data;				
				
				testMessage.entity_id = _data->entity_id;
                testMessage.seq_num = _data->seq_num;
                testMessage.timestamp_sec = _data->timestamp_sec;
                testMessage.timestamp_usec = _data->timestamp_usec;
                testMessage.latency_ping = _data->latency_ping;                
                testMessage.data = _data->bin_data;			
				testMessage.sent_flag = _data->sent_flag; // replacement for message size

				//printf("%ld\n", _data->seq_num);
				

				if(cb)
					cb(&testMessage);

				message_release(p_message);
				_REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
				FREE(fseq.pp_foo);
				FREE(sSeq.pp_sample_infos);
			}
		}
	}
};

/*********************************************************
 * RTISubscriber
 */
class RTISubscriber : public IMessagingReader
{
  private:
	DataReader *_dr;
    FooDataReader *_reader;
    FooSeq         _data_seq;
    SampleInfoSeq     _info_seq;
    TestMessage           _message;
    WaitSet           *_waitset;
    ConditionSeq       _active_conditions;

    int      _data_idx;
    bool      _no_data;

  public:

    RTISubscriber(DataReader *reader)
    {
		_dr = reader;
        //_reader = TestData_tDataReader::narrow(reader);
		_reader = (FooDataReader *)reader;

        // null listener means using receive thread
		if (_reader->get_listener((DataReader *)_reader) == NULL) {
			/*
            WaitSetProperty_t property;
            property.max_event_count         = NeoDDSImpl::_WaitsetEventCount;
            property.max_event_delay.sec     = (int)NeoDDSImpl::_WaitsetDelayUsec / 1000000;
            property.max_event_delay.nanosec = (NeoDDSImpl::_WaitsetDelayUsec % 1000000) * 1000;
			*/
            _waitset = waitset_new();
           
            StatusCondition *reader_status;
            reader_status = reader->get_statuscondition((Entity*)reader);
            reader_status->set_enabled_statuses(reader_status, DATA_AVAILABLE_STATUS);
            _waitset->attach_condition(_waitset, (Condition *)reader_status);
        }
    }

    virtual void Shutdown()
    {
		/*
        // loan may be outstanding during shutdown
        _reader->return_loan(_data_seq, _info_seq);
		*/
    }

    TestMessage *ReceiveMessage()
    {
        ReturnCode_t retcode;
        int seq_length;

		Duration_t waittime = TIME_INFINITE; 
		ConditionSeq condseq = INIT_CONDITIONSEQ;

        while (true) {
            // no outstanding reads
            if (_no_data)
            {
                _waitset->wait(_waitset, &condseq, waittime);

				/*
                if (_active_conditions.length() == 0)
                {
                    //printf("Read thread woke up but no data\n.");
                    //return NULL;
                    continue;
                }   
				*/

				retcode = _reader->take(_reader, &_data_seq, &_info_seq, LENGTH_UNLIMITED,
					ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
					

                if (retcode == RETCODE_NO_DATA)
                {
                    //printf("Called back no data.\n");
                    //return NULL;
                    continue;
                }
                else if (retcode != RETCODE_OK)
                {
                    fprintf(stderr,"Error during taking data %d.\n", retcode);
                    return NULL;
                }

                _data_idx = 0;
                _no_data = false;
            }

			/*
            seq_length = _data_seq.length();
            // check to see if hit end condition
            if (_data_idx == seq_length)
            {
                _reader->return_loan(_data_seq, _info_seq);
                _no_data = true;
                continue;
            }
			

            // skip non-valid data
            while ( (_info_seq[_data_idx].valid_data == false) && 
                    (++_data_idx < seq_length));

             // may have hit end condition
             if (_data_idx == seq_length) { continue; }

            _message.entity_id = _data_seq[_data_idx].entity_id;
            _message.seq_num = _data_seq[_data_idx].seq_num;
            _message.timestamp_sec = _data_seq[_data_idx].timestamp_sec;
            _message.timestamp_usec = _data_seq[_data_idx].timestamp_usec;
            _message.latency_ping = _data_seq[_data_idx].latency_ping;
            _message.size = _data_seq[_data_idx].bin_data.length();
            _message.data = (char *)_data_seq[_data_idx].bin_data.get_contiguous_bufferI();

            ++_data_idx;
			*/
            return &_message;
        }
    }

    void WaitForWriters(int numPublishers)
    {
        SubscriptionMatchedStatus status;

        while (true)
        {
            _reader->get_subscription_matched_status((DataReader *)_reader, &status);

            if (status.current_count >= numPublishers)
            {
                break;
            }
            perftest_cpp::MilliSleep(1000);
        }
    }
};


/*********************************************************
 * Initialize
 */
bool NeoDDSImpl::Initialize(int argc, char *argv[])
{       
	DomainParticipantQos qos; 
	TestData_tTypeSupport tsp;

    if (!ParseConfig(argc, argv))
    {
        return false;
    }

	// IP Configuration
	// ...

	// Domain Configuration
	//domain_port_set(_DomainID);

	// DomainParticipantFactory
	_factory = DomainParticipantFactory_get_instance();

	// Semaphore related
	if(_LatencyTest)
	{
		cond_init(&_cond);
		mutex_init(&_mutex);
	}


	// DomainParticipant QoS
	memset(&qos, 0, sizeof(DomainParticipantQos));
	static_get_default_participant_qos(&qos);

	// domainparticipant listener
	domainparticipantlistner.on_inconsistent_topic = on_inconsistent_topic;
	domainparticipantlistner.on_offered_incompatible_qos = on_offered_incompatible_qos;
	domainparticipantlistner.on_requested_incompatible_qos = on_requested_incompatible_qos;

    // Creates the participant
    _participant = _factory->create_participant(
        _DomainID, &qos, &domainparticipantlistner,
        INCONSISTENT_TOPIC_STATUS |
        OFFERED_INCOMPATIBLE_QOS_STATUS |
        REQUESTED_INCOMPATIBLE_QOS_STATUS);
	
    if (_participant == NULL)
    {
        fprintf(stderr,"Problem creating participant.\n");
        return false;
    }

	// Register the types and create the topics
	tsp.register_topic_type(_participant);

    // Create the DDSPublisher and DDSSubscriber
    {
        PublisherQos pub_qos;
		memset(&pub_qos, 0, sizeof(PublisherQos));
		static_get_default_publisher_qos(&pub_qos);

        pub_qos.presentation.access_scope = TOPIC_PRESENTATION_QOS;
        pub_qos.presentation.ordered_access = true;

        _publisher = _participant->create_publisher(_participant, &pub_qos, NULL, 0);

        if (_publisher == NULL)
        {
            fprintf(stderr,"Problem creating publisher.\n");
            return false;
        }

        SubscriberQos sub_qos;
		memset(&sub_qos, 0, sizeof(SubscriberQos));
		static_get_default_subscriber_qos(&sub_qos);

        sub_qos.presentation.access_scope = TOPIC_PRESENTATION_QOS;
        sub_qos.presentation.ordered_access = true;

        _subscriber = _participant->create_subscriber(_participant, &sub_qos, NULL, 0);

        if (_subscriber == NULL)
        {
            fprintf(stderr,"Problem creating subscriber.\n");
            return false;
        }
    }

    return true;
}

/*********************************************************
 * CreateWriter
 */
IMessagingWriter *NeoDDSImpl::CreateWriter(const char *topic_name)
{
    DataWriter *writer = NULL;
    DataWriterQos dw_qos;
    char *qos_profile = NULL;
	
    Topic *topic = _participant->create_topic(_participant,
                       (char *)topic_name, (char *)_typename,
                       NULL, NULL,
                       0);

	memset(&dw_qos, 0, sizeof(DataWriterQos));
	static_get_default_datawriter_qos(&dw_qos);

    if (topic == NULL)
    {
        fprintf(stderr,"Problem creating topic %s.\n", topic_name);
        return NULL;
    }

    if (strcmp(topic_name, perftest_cpp::_ThroughputTopicName) == 0)
    {
        if (_UsePositiveAcks)
        {
            qos_profile = "ThroughputQos";
        }
        else
        {
            qos_profile = "NoAckThroughputQos";
        }
    }
    else if (strcmp(topic_name, perftest_cpp::_LatencyTopicName) == 0)
    {
        if (_UsePositiveAcks)
        {
            qos_profile = "LatencyQos";
        }
        else
        {
            qos_profile = "NoAckLatencyQos";
        }
    }
    else if (strcmp(topic_name, perftest_cpp::_AnnouncementTopicName) == 0)
    {
        qos_profile = "AnnouncementQos";
    }
    else
    {
        fprintf(stderr,"topic name must either be %s or %s or %s.\n",
               perftest_cpp::_ThroughputTopicName, perftest_cpp::_LatencyTopicName,
               perftest_cpp::_AnnouncementTopicName);
        return NULL;
    }
	/*
    if (_factory->get_datawriter_qos_from_profile(dw_qos, _ProfileLibraryName, qos_profile)
        != DDS_RETCODE_OK)
    {
        fprintf(stderr,"No QOS Profile named \"%s\" found in QOS Library \"%s\" in file %s.\n",
                qos_profile, _ProfileLibraryName, _ProfileFile);
        return NULL;
    }
    */

	/*
    if (_UsePositiveAcks) 
    {
        dw_qos.protocol.rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration.sec = (int)_KeepDurationUsec/1000000;
        dw_qos.protocol.rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration.nanosec = _KeepDurationUsec%1000000;
    }
	*/
	printf("... %s\n", _IsReliable? "TRUE" : "FALSE");
    // only force reliability on throughput/latency topics
    if (strcmp(topic_name, perftest_cpp::_AnnouncementTopicName) != 0)
    {		
        if (_IsReliable)
        {
            dw_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
#if 1 // 20140319
            dw_qos.history.kind = KEEP_ALL_HISTORY_QOS;
            //dw_qos.reliability.max_blocking_time = TIME_INFINITE;
			dw_qos.reliability.max_blocking_time.sec = 10;
			dw_qos.reliability.max_blocking_time.nanosec = 0;

#endif
        }
        else
        {
            dw_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
#if 0 // 20140319
            dw_qos.history.kind = KEEP_ALL_HISTORY_QOS;
#endif
        }
    }

	dw_qos.resource_limits.max_samples = 5000; // 20140327

    // These QOS's are only set for the Throughput datawriter
    if ((strcmp(qos_profile,"ThroughputQos") == 0) ||
        (strcmp(qos_profile,"NoAckThroughputQos") == 0))
    {
        if (_BatchSize > 0)
        {
#if 0 
            dw_qos.batch.enable = true;
            dw_qos.batch.max_samples = DDS_LENGTH_UNLIMITED;
            dw_qos.batch.thread_safe_write = false;
            dw_qos.batch.max_flush_delay = DDS_DURATION_INFINITE;
            dw_qos.batch.max_data_bytes = _BatchSize;
            dw_qos.resource_limits.max_samples = DDS_LENGTH_UNLIMITED;
            dw_qos.writer_resource_limits.max_batches = _SendQueueSize;
#else
			dw_qos.resource_limits.max_samples = LENGTH_UNLIMITED; // 20140325
#endif
        } else {

            //dw_qos.resource_limits.max_samples = 1000; // 20140325
        }

		/*
        if (_HeartbeatPeriod.sec > 0 || _HeartbeatPeriod.nanosec > 0) {
            // set the heartbeat_period
            dw_qos.protocol.rtps_reliable_writer.heartbeat_period =
                _HeartbeatPeriod;
            // make the late joiner heartbeat compatible
            dw_qos.protocol.rtps_reliable_writer.late_joiner_heartbeat_period =
                _HeartbeatPeriod;
        }

        if (_FastHeartbeatPeriod.sec > 0 || _FastHeartbeatPeriod.nanosec > 0) {
            // set the fast_heartbeat_period
            dw_qos.protocol.rtps_reliable_writer.fast_heartbeat_period =
                _FastHeartbeatPeriod;
        }
		*/

        //dw_qos.resource_limits.initial_samples = _SendQueueSize;

        dw_qos.resource_limits.max_samples_per_instance
            = dw_qos.resource_limits.max_samples;


        dw_qos.durability.kind = (DurabilityQosPolicyKind)_Durability;
        //dw_qos.durability.direct_communication = _DirectCommunication;

		/*
        dw_qos.protocol.rtps_reliable_writer.heartbeats_per_max_samples = _SendQueueSize / 10;

        dw_qos.protocol.rtps_reliable_writer.low_watermark = _SendQueueSize * 1 / 10;
        dw_qos.protocol.rtps_reliable_writer.high_watermark = _SendQueueSize * 9 / 10;

        dw_qos.protocol.rtps_reliable_writer.max_send_window_size = 
            dw_qos.resource_limits.max_samples;
        dw_qos.protocol.rtps_reliable_writer.min_send_window_size = 
            dw_qos.resource_limits.max_samples;
		*/
    }

    if (((strcmp(qos_profile,"LatencyQos") == 0) || 
        (strcmp(qos_profile,"NoAckLatencyQos") == 0)) &&
        !_DirectCommunication && 
        (_Durability == TRANSIENT_DURABILITY_QOS ||
         _Durability == PERSISTENT_DURABILITY_QOS)) {
        dw_qos.durability.kind = (DurabilityQosPolicyKind)_Durability;
        //dw_qos.durability.direct_communication = _DirectCommunication;
    }

    dw_qos.resource_limits.max_instances = _InstanceCount;
    //dw_qos.resource_limits.initial_instances = _InstanceCount;

	/*
    if (_InstanceCount > 1) {
        if (_InstanceHashBuckets > 0) {
            dw_qos.resource_limits.instance_hash_buckets =
                _InstanceHashBuckets;
        } else {
            dw_qos.resource_limits.instance_hash_buckets = _InstanceCount;
        }
    }
	*/

#if 1 // 20140317
    writer = _publisher->create_datawriter(_publisher, topic, &dw_qos, NULL,0);
#else
	writer = _publisher->create_datawriter(_publisher, topic, NULL, NULL,0);
#endif
    
    if (writer == NULL)
    {
        fprintf(stderr,"Problem creating writer.\n");
        return NULL;
    }

    RTIPublisher *pub = new RTIPublisher(writer, _InstanceCount, _LatencyTest, &_cond, &_mutex);

    return pub;
}
    
/*********************************************************
 * CreateReader
 */
#ifndef NeoDDS
IMessagingReader *NeoDDSImpl::CreateReader(const char *topic_name, 
                                           IMessagingCB *callback)
#else
IMessagingReader *NeoDDSImpl::CreateReader(const char *topic_name, 
                                           CALLBACK_FUNC callback)
#endif
{
    ReceiverListener *reader_listener = NULL;
    DataReader *reader = NULL;
    DataReaderQos dr_qos;
    const char *qos_profile = NULL;

	Topic *topic = _participant->create_topic(_participant,
                    (char *)topic_name, (char *)_typename,
                    NULL, NULL,
                    0);

	memset(&dr_qos, 0, sizeof(DataReaderQos));
	static_get_default_datareader_qos(&dr_qos);
		
    if (topic == NULL)
    {
        fprintf(stderr,"Problem creating topic %s.\n", topic_name);
        return NULL;
    }

    if (strcmp(topic_name, perftest_cpp::_ThroughputTopicName) == 0)
    {
        if (_UsePositiveAcks)
        {
            qos_profile = "ThroughputQos";
        }
        else
        {
            qos_profile = "NoAckThroughputQos";
        }
    }
    else if (strcmp(topic_name, perftest_cpp::_LatencyTopicName) == 0)
    {
        if (_UsePositiveAcks)
        {
            qos_profile = "LatencyQos";
        }
        else
        {
            qos_profile = "NoAckLatencyQos";
        }
    }
    else if (strcmp(topic_name, perftest_cpp::_AnnouncementTopicName) == 0)
    {
        qos_profile = "AnnouncementQos";
    }
    else
    {
        fprintf(stderr,"topic name must either be %s or %s or %s.\n",
               perftest_cpp::_ThroughputTopicName, perftest_cpp::_LatencyTopicName,
               perftest_cpp::_AnnouncementTopicName);
        return NULL;
    }
	/*
    if (_factory->get_datareader_qos_from_profile(dr_qos, _ProfileLibraryName, qos_profile)
        != DDS_RETCODE_OK)
    {
        fprintf(stderr,"No QOS Profile named \"%s\" found in QOS Library \"%s\" in file %s.\n",
                qos_profile, _ProfileLibraryName, _ProfileFile);
        return NULL;
    }
	*/

	memset(&dr_qos, 0, sizeof(dr_qos));
	static_get_default_datareader_qos(&dr_qos);

    // only force reliability on throughput/latency topics
    if (strcmp(topic_name, perftest_cpp::_AnnouncementTopicName) != 0)
    {
        if (_IsReliable)
        {
            dr_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
#if 1 // 20140319
            dr_qos.history.kind = KEEP_ALL_HISTORY_QOS;
#endif
        }
        else
        {
            dr_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
#if 0 // 20140319
            dr_qos.history.kind = KEEP_ALL_HISTORY_QOS;
#endif
        }
    }

    // only apply durability on Throughput datareader
    if ((strcmp(qos_profile,"ThroughputQos") == 0) ||
        (strcmp(qos_profile,"NoAckThroughputQos") == 0))
    {
        dr_qos.durability.kind = (DurabilityQosPolicyKind)_Durability;
        //dr_qos.durability.direct_communication = _DirectCommunication;
    }

    if (((strcmp(qos_profile,"LatencyQos") == 0) || 
        (strcmp(qos_profile,"NoAckLatencyQos") == 0)) &&
        !_DirectCommunication && 
        (_Durability == TRANSIENT_DURABILITY_QOS ||
         _Durability == PERSISTENT_DURABILITY_QOS)) {
        dr_qos.durability.kind = (DurabilityQosPolicyKind)_Durability;
        //dr_qos.durability.direct_communication = _DirectCommunication;
    }

#if 1 // byungung, 20140328
	dr_qos.resource_limits.max_samples = 5000; // 20140325
#endif

    //dr_qos.resource_limits.initial_instances = _InstanceCount;
    //dr_qos.resource_limits.max_instances = _InstanceCount;
    /*
    if (_InstanceCount > 1) {
        if (_InstanceHashBuckets > 0) {
            dr_qos.resource_limits.instance_hash_buckets =
                _InstanceHashBuckets;
        } else {
            dr_qos.resource_limits.instance_hash_buckets = _InstanceCount;
        }
    }
	
    if (!_UseTcpOnly) {
    
        if (_IsMulticast)
        {
            const char *multicast_addr;

            if (strcmp(topic_name, perftest_cpp::_ThroughputTopicName) == 0)
            {
                multicast_addr = THROUGHPUT_MULTICAST_ADDR;
            }
            else  if (strcmp(topic_name, perftest_cpp::_LatencyTopicName) == 0)
            {
                multicast_addr = LATENCY_MULTICAST_ADDR;
            }
            else 
            {
                multicast_addr = ANNOUNCEMENT_MULTICAST_ADDR;
            }

            dr_qos.multicast.value.ensure_length(1,1);
            dr_qos.multicast.value[0].receive_address = DDS_String_dup(multicast_addr);
            dr_qos.multicast.value[0].receive_port = 0;
            dr_qos.multicast.value[0].transports.length(0);
        }
    }
	*/

    if (callback != NULL)
    {
       /*  NDDSConfigLogger::get_instance()->
        set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API, 
                                  NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);*/
        //reader_listener = new ReceiverListener(callback);
		reader_listener = new ReceiverListener(callback);
#if 1 // 20140317
		reader = _subscriber->create_datareader(_subscriber,
            topic, &dr_qos, &reader_listener->datareaderListener, DATA_AVAILABLE_STATUS);
#else
		reader = _subscriber->create_datareader(_subscriber,
            topic, NULL, &reader_listener->datareaderListener, DATA_AVAILABLE_STATUS);
#endif
    }
    else
    {
#if 1 // 20140317
        reader = _subscriber->create_datareader(_subscriber,
            topic, &dr_qos, NULL, DATA_AVAILABLE_STATUS);
#else
		reader = _subscriber->create_datareader(_subscriber,
            topic, NULL, NULL, DATA_AVAILABLE_STATUS);
#endif
    }

    if (reader == NULL)
    {
        fprintf(stderr,"Problem creating reader.\n");
        return NULL;
    }

    if (!strcmp(topic_name, perftest_cpp::_ThroughputTopicName) ||
        !strcmp(topic_name, perftest_cpp::_LatencyTopicName)) {
        _reader = reader;
    }

    IMessagingReader *sub = new RTISubscriber(reader);
    return sub;
}

#pragma warning(pop)
