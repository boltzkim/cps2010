#ifndef __NeoDDSImpl_H__
#define __NeoDDSImpl_H__

#include "MessagingIF.h"

#define MAX_BINDATA_SIZE	63000
#define KEY_SIZE	4

class NeoDDSImpl : public IMessaging
{
  public:

    NeoDDSImpl()
    {
        _SendQueueSize = 50;
        _DataLen = 100;
        _DomainID = 59;
        _Nic = "";
        _ProfileFile = "perftest.xml";
        _ConfigFile = "perftest.ini";
        _IsReliable = true;
        _IsMulticast = true;
        _BatchSize = 0;
        _InstanceCount = 1;
        _InstanceHashBuckets = -1;
        _Durability = VOLATILE_DURABILITY_QOS;
        _DirectCommunication = true;
        _KeepDurationUsec = 1000;
        _UsePositiveAcks = true;
        _UseSharedMemory = false;
		_LatencyTest = false;
        _UseTcpOnly = false;
        _IsDebug = false;

        _HeartbeatPeriod.sec = 0;
        _HeartbeatPeriod.nanosec = 0;
        _FastHeartbeatPeriod.sec = 0;
        _FastHeartbeatPeriod.nanosec = 0;

        _WaitsetEventCount = 5;
        _WaitsetDelayUsec = 100;
        THROUGHPUT_MULTICAST_ADDR = "239.255.1.1";
        LATENCY_MULTICAST_ADDR = "239.255.1.2";
        ANNOUNCEMENT_MULTICAST_ADDR = "239.255.1.100";
        _ProfileLibraryName = "PerftestQosLibrary";

        _factory = NULL;
        _participant = NULL;
        _subscriber = NULL;
        _publisher = NULL;
        _reader = NULL;
#ifndef NeoDDS 
        _typename = TestData_tTypeSupport::get_type_name();
#else
		_typename = "TestData_t";
#endif

#ifndef NeoDDS 
		_pongSemaphore = NULL;
#else
		memset(&domainparticipantlistner, 0, sizeof(domainparticipantlistner));
		memset(&_cond, NULL, sizeof(cond_t));
		memset(&_mutex, NULL, sizeof(mutex_t));
#endif		
    }

    ~NeoDDSImpl() 
    {
        Shutdown();
    }

    void PrintCmdLineHelp();

    bool ParseConfig(int argc, char *argv[]);

    bool Initialize(int argc, char *argv[]);

    void Shutdown();

    int GetBatchSize() { return _BatchSize; }

    int GetMaxBinDataSize() { return MAX_BINDATA_SIZE; }

    IMessagingWriter *CreateWriter(const char *topic_name);
    
    // Pass null for callback if using IMessagingSubscriber.ReceiveMessage()
    // to get data
#ifndef NeoDDS
    IMessagingReader *CreateReader(const char *topic_name, IMessagingCB *callback);
#else
	IMessagingReader *CreateReader(const char *topic_name, CALLBACK_FUNC callback);
#endif

  private:

    int          _SendQueueSize;
    int          _DataLen;
    int          _DomainID;
    const char  *_Nic;
    const char  *_ProfileFile;
    const char  *_ConfigFile;
    bool         _IsReliable;
    bool         _IsMulticast;
    int          _BatchSize;
    int          _InstanceCount;
    int          _InstanceHashBuckets;
    int          _Durability;
    bool         _DirectCommunication;
    unsigned int _KeepDurationUsec;
    bool         _UsePositiveAcks;
    bool         _UseSharedMemory;
    bool	 _LatencyTest;
    bool         _UseTcpOnly;
    bool         _IsDebug;

    Duration_t   _HeartbeatPeriod;
    Duration_t   _FastHeartbeatPeriod;

    const char          *THROUGHPUT_MULTICAST_ADDR;
    const char          *LATENCY_MULTICAST_ADDR;
    const char          *ANNOUNCEMENT_MULTICAST_ADDR;
    const char          *_ProfileLibraryName;

    DomainParticipantFactory *_factory;
    DomainParticipant        *_participant;
    Subscriber               *_subscriber;
    Publisher                *_publisher;
    DataReader               *_reader;
    const char               *_typename;

#ifdef NeoDDS 
	DomainParticipantListener domainparticipantlistner;
#endif

#ifndef NeoDDS 
    RTIOsapiSemaphore		*_pongSemaphore;
#else
	cond_t	_cond;
	mutex_t	_mutex;
#endif

  public:

    static int          _WaitsetEventCount;
    static unsigned int _WaitsetDelayUsec;
};


#endif // __NeoDDSImpl_H__

