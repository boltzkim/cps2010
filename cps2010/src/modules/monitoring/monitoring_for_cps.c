/*
	Monitoring을 하기 위한 모듈로서 일단 단순하게 관리하도록 한다.

	이력
	2012-08-8
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include "monitoring_application.h"
#include "../rtps/rtps.h"


struct monitoring_sys_t
{
	bool isAgent;
	char* p_local_ip;
	char* p_domain_name;
	mutex_t monitoring_lock;
	cond_t monitoring_wait;
	module_thread_t threadID;
	int polling_sec;
	bool b_end;
};
///////////////

enum MODIFY_TYPE
{
	INSERT_MODE,
	DELETE_MODE
} MODIFY_TYPE;




////////////////
static int Open(module_object_t*);
static void Close(module_object_t*);


static void add_participant_for_cps(monitoring_t*, DomainParticipant*);
static void delete_participant_for_cps(monitoring_t*, DomainParticipant*);

static void add_publisher_for_cps(monitoring_t*, Publisher*);
static void delete_publisher_for_cps(monitoring_t*, Publisher*);

static void add_subscriber_for_cps(monitoring_t*, Subscriber*);
static void delete_subscriber_for_cps(monitoring_t*, Subscriber*);

static void add_topic_for_cps(monitoring_t*, Topic*);
static void delete_topic_for_cps(monitoring_t*, Topic*);

static void add_datareader_for_cps(monitoring_t*, DataReader*);
static void delete_datareader_for_cps(monitoring_t*, DataReader*);

static void add_datawriter_for_cps(monitoring_t*, DataWriter*);
static void delete_datawriter_for_cps(monitoring_t*, DataWriter*);

static void add_writerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writer_proxy);
static void delete_writerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writer_proxy);

static void add_readerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_reader_proxy);
static void delete_readerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_reader_proxy);

static void warning_condition(monitoring_t* p_monitoring, int32_t entity, char* condition, int32_t warningorerror, int32_t level, int32_t code);

static void service_monitoring_thread(module_object_t* p_this);

#define MODULE_PREFIX "monitoring-"

#define ENABLE_TEXT "Agent"
#define ENABLE_LONG_TEXT "Is Agent"

module_define_start(monitoring_for_cps)
	set_category(CAT_MONITORING);
	set_description("Monitoring For CPS");
	set_capability("monitoring", 200);
	set_callbacks(Open, Close);
	add_shortcut("monitoring_for_cps");
	add_bool(MODULE_PREFIX"agent", true, NULL, ENABLE_TEXT, ENABLE_LONG_TEXT, false);
	add_bool(MODULE_PREFIX"agent-enable", true, NULL, ENABLE_TEXT, ENABLE_LONG_TEXT, false);
	add_integer(MODULE_PREFIX"pollingsec",1,NULL, "Polling sec", "Polling sec", false);
module_define_end()


static int Open(module_object_t* p_this)
{
	monitoring_t* p_monitoring = (monitoring_t*)p_this;
	module_t* p_module = p_this->p_module;
	value_t val;
	monitoring_sys_t* p_sys;


	trace_msg(OBJECT(p_this), TRACE_LOG, "Open Monitoring..");

	if (!p_module)
	{
		trace_msg(OBJECT(p_this), TRACE_ERROR, "Can't Use Module \"Monitoring For CPS\" module. p_module is NULL.");
		return MODULE_ERROR_CREATE;
	}
	else
	{
		return MODULE_ERROR_CREATE;
	}

	p_sys = p_monitoring->p_sys = malloc(sizeof(monitoring_sys_t));
	if (!p_monitoring->p_sys)
	{
		trace_msg(OBJECT(p_this), TRACE_ERROR, "Can't Allocate monitoring_sys_t in \"Monitoring For CPS\" module.");
		return MODULE_ERROR_MEMORY;
	}

	memset(p_monitoring->p_sys, '\0', sizeof(monitoring_sys_t));



	var_create(p_this, MODULE_PREFIX"agent", VAR_BOOL | VAR_DOINHERIT);
	var_get(p_this, MODULE_PREFIX"agent", &val);

	p_monitoring->p_sys->p_local_ip = get_default_local_address();
	p_monitoring->p_sys->p_domain_name = "zento";

	if (val.b_bool == false)
	{
		p_monitoring->p_sys->isAgent = false;
	}
	else
	{
		p_monitoring->p_sys->isAgent = true;
	}

	var_create(p_this, MODULE_PREFIX"pollingsec", VAR_INTEGER | VAR_DOINHERIT);
	var_get(p_this, MODULE_PREFIX"pollingsec", &val);

	p_sys->polling_sec = val.i_int;

	p_sys->b_end = false;

	p_monitoring->addParticipant = add_participant_for_cps;
	p_monitoring->deleteParticipant = delete_participant_for_cps;
	p_monitoring->addPublisher = add_publisher_for_cps;
	p_monitoring->deletePublisher = delete_publisher_for_cps;
	p_monitoring->addSubscriber = add_subscriber_for_cps;
	p_monitoring->deleteSubscriber = delete_subscriber_for_cps;
	p_monitoring->addTopic = add_topic_for_cps;
	p_monitoring->deleteTopic = delete_topic_for_cps;
	p_monitoring->addDataReader = add_datareader_for_cps;
	p_monitoring->deleteDataReader = delete_datareader_for_cps;
	p_monitoring->addDataWriter = add_datawriter_for_cps;
	p_monitoring->deleteDataWriter = delete_datawriter_for_cps;
	p_monitoring->add_writerproxy = add_writerproxy_for_cps;
	p_monitoring->deleteWriterProxy = delete_writerproxy_for_cps;
	p_monitoring->addReaderProxy = add_readerproxy_for_cps;
	p_monitoring->deleteReaderProxy = delete_readerproxy_for_cps;
	p_monitoring->warningcodition = warning_condition;



	trace_msg(OBJECT(p_this), TRACE_LOG, "start monitoring_wrapper_application..");
	monitoring_wrapper_application();
	trace_msg(OBJECT(p_this), TRACE_LOG, "end Open Monitoring..");

	mutex_init(&p_sys->monitoring_lock);
	cond_init(&p_sys->monitoring_wait);

	if (!(p_monitoring->p_sys->threadID = thread_create2(OBJECT(p_monitoring), &p_sys->monitoring_wait, &p_sys->monitoring_lock, "Monitoring ", (void*)service_monitoring_thread, 0, false)))
	{
		Close(OBJECT(p_monitoring));
		return MODULE_ERROR_CREATE;
	}

	//add builtin_application
	//



	return MODULE_SUCCESS;
}


extern void monitoring_quit();
static void Close(module_object_t* p_this)
{
	monitoring_t* p_monitoring = (monitoring_t*)p_this;
	monitoring_sys_t* p_sys = p_monitoring->p_sys;

	monitoring_quit();
	p_sys->b_end = true;

	if (p_monitoring->p_sys->threadID)
		thread_join2(p_sys->threadID);


	mutex_destroy(&p_sys->monitoring_lock);
	cond_destroy(&p_sys->monitoring_wait);

	FREE(p_monitoring->p_sys);
}


static int32_t participant_guid[4];
static bool b_is_get_participant_guid = false;


static void add_participant_for_cps(monitoring_t* p_monitoring, DomainParticipant* p_participant)
{
	rtps_participant_t* p_rtps_participant = p_participant->p_rtps_participant;
	int size = sizeof(long);

	monitoring_ParticipantDomain a_participant_domain_data;
	memset(&a_participant_domain_data, 0, sizeof(monitoring_ParticipantDomain));

	a_participant_domain_data.id = p_participant->id;
	a_participant_domain_data.domain_id = p_participant->domain_id;
	memcpy(a_participant_domain_data.guid, &p_participant->p_rtps_participant->guid, sizeof(p_participant->p_rtps_participant->guid));
	if (b_is_get_participant_guid == false)
	{
		memcpy(participant_guid, &p_participant->p_rtps_participant->guid, sizeof(p_participant->p_rtps_participant->guid));
		b_is_get_participant_guid = true;
	}
	set_string(&a_participant_domain_data.ip, p_monitoring->p_sys->p_local_ip);
	set_string(&a_participant_domain_data.domainName, p_monitoring->p_sys->p_domain_name);
	a_participant_domain_data.qos = p_participant->qos;
	a_participant_domain_data.b_is_modified = INSERT_MODE;

	write_ParticipantDomain(a_participant_domain_data);
}

static void delete_participant_for_cps(monitoring_t* p_monitoring, DomainParticipant* p_participant)
{
	monitoring_ParticipantDomain a_participant_domain_data;
	memset(&a_participant_domain_data, 0, sizeof(monitoring_ParticipantDomain));

	a_participant_domain_data.id = p_participant->id;
	a_participant_domain_data.domain_id = p_participant->domain_id;
	set_string(&a_participant_domain_data.ip, p_monitoring->p_sys->p_local_ip);
	memcpy(a_participant_domain_data.guid, &p_participant->p_rtps_participant->guid, sizeof(p_participant->p_rtps_participant->guid));
	set_string(&a_participant_domain_data.domainName, p_monitoring->p_sys->p_domain_name);
	a_participant_domain_data.qos = p_participant->qos;
	a_participant_domain_data.b_is_modified = DELETE_MODE;

	write_ParticipantDomain(a_participant_domain_data);
}

static void add_publisher_for_cps(monitoring_t* p_monitoring, Publisher* p_publisher)
{
	monitoring_Publisher a_publisher_data;
	memset(&a_publisher_data, 0, sizeof(monitoring_Publisher));

	a_publisher_data.id = p_publisher->id;
	a_publisher_data.parentid = p_publisher->p_domain_participant->id;
	memcpy(a_publisher_data.participantdomainguid, &p_publisher->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_publisher_data.qos = p_publisher->publisher_qos;
	a_publisher_data.ismodified = INSERT_MODE;

	write_Publisher(a_publisher_data);
}

static void delete_publisher_for_cps(monitoring_t* p_monitoring, Publisher* p_publisher)
{
	monitoring_Publisher a_publisher_data;
	memset(&a_publisher_data, 0, sizeof(monitoring_Publisher));

	a_publisher_data.id = p_publisher->id;
	a_publisher_data.parentid = p_publisher->p_domain_participant->id;
	memcpy(a_publisher_data.participantdomainguid, &p_publisher->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_publisher_data.qos = p_publisher->publisher_qos;
	a_publisher_data.ismodified = DELETE_MODE;

	write_Publisher(a_publisher_data);
}


static void add_subscriber_for_cps(monitoring_t* p_monitoring, Subscriber* p_subscriber)
{
	monitoring_Subscriber a_subscriber_data;
	memset(&a_subscriber_data, 0, sizeof(monitoring_Subscriber));

	a_subscriber_data.id = p_subscriber->id;
	a_subscriber_data.parentid = p_subscriber->p_domain_participant->id;
	memcpy(a_subscriber_data.participantdomainguid, &p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_subscriber_data.qos = p_subscriber->subscriber_qos;
	a_subscriber_data.ismodified = INSERT_MODE;

	write_Subscriber(a_subscriber_data);
}

static void delete_subscriber_for_cps(monitoring_t* p_monitoring, Subscriber* p_subscriber)
{
	monitoring_Subscriber a_subscriber_data;
	memset(&a_subscriber_data, 0, sizeof(monitoring_Subscriber));

	a_subscriber_data.id = p_subscriber->id;
	a_subscriber_data.parentid = p_subscriber->p_domain_participant->id;
	memcpy(a_subscriber_data.participantdomainguid, &p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_subscriber_data.qos = p_subscriber->subscriber_qos;
	a_subscriber_data.ismodified = DELETE_MODE;

	write_Subscriber(a_subscriber_data);
}

static void add_topic_for_cps(monitoring_t* p_monitoring, Topic* p_topic)
{
	monitoring_Topic a_topic_data;
	memset(&a_topic_data, 0, sizeof(monitoring_Topic));

	a_topic_data.id = p_topic->id;

	memcpy(a_topic_data.participantdomainguid, &p_topic->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));

	set_string(&a_topic_data.topic_name, p_topic->topic_name);
	a_topic_data.topic_type = p_topic->topic_type;
	set_string(&a_topic_data.type_name, p_topic->type_name);
	a_topic_data.qos = p_topic->topic_qos;
	a_topic_data.ismodified = INSERT_MODE;

	write_Topic(a_topic_data);
}

static void delete_topic_for_cps(monitoring_t* p_monitoring, Topic* p_topic)
{
	monitoring_Topic a_topic_data;
	memset(&a_topic_data, 0, sizeof(monitoring_Topic));

	a_topic_data.id = p_topic->id;

	memcpy(a_topic_data.participantdomainguid, &p_topic->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));

	set_string(&a_topic_data.topic_name, p_topic->topic_name);
	a_topic_data.topic_type = p_topic->topic_type;
	set_string(&a_topic_data.type_name, p_topic->type_name);
	a_topic_data.qos = p_topic->topic_qos;
	a_topic_data.ismodified = DELETE_MODE;

	write_Topic(a_topic_data);
}

static void add_datareader_for_cps(monitoring_t* p_monitoring, DataReader* p_datareader)
{
	monitoring_DataReader a_datareader_data;
	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;

	memset(&a_datareader_data, 0, sizeof(monitoring_DataReader));

	a_datareader_data.id = p_datareader->id;
	memcpy(a_datareader_data.guid, &p_rtpsReader->guid, sizeof(GUID_t));
	a_datareader_data.parentid = p_datareader->p_subscriber->id;
	memcpy(a_datareader_data.participantdomainguid, &p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_datareader_data.topicid = p_datareader->p_topic->id;
	set_string(&a_datareader_data.topic_name, p_datareader->p_topic->topic_name);
	a_datareader_data.topic_type = (int32_t)p_datareader->p_topic->topic_type;
	set_string(&a_datareader_data.type_name, p_datareader->p_topic->type_name);
	a_datareader_data.behavior = p_rtpsReader->behavior_type;
	a_datareader_data.reliability = p_rtpsReader->reliability_level;
	a_datareader_data.qos = p_datareader->datareader_qos;
	a_datareader_data.ismodified = INSERT_MODE;

	write_DataReader(a_datareader_data);
}

static void delete_datareader_for_cps(monitoring_t* p_monitoring, DataReader* p_datareader)
{
	monitoring_DataReader a_datareader_data;
	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;

	memset(&a_datareader_data, 0, sizeof(monitoring_DataReader));

	a_datareader_data.id = p_datareader->id;
	memcpy(a_datareader_data.guid, &p_rtpsReader->guid, sizeof(GUID_t));
	a_datareader_data.parentid = p_datareader->p_subscriber->id;
	memcpy(a_datareader_data.participantdomainguid, &p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_datareader_data.topicid = p_datareader->p_topic->id;
	set_string(&a_datareader_data.topic_name, p_datareader->p_topic->topic_name);
	a_datareader_data.topic_type = (int32_t)p_datareader->p_topic->topic_type;
	set_string(&a_datareader_data.type_name, p_datareader->p_topic->type_name);
	a_datareader_data.behavior = p_rtpsReader->behavior_type;
	a_datareader_data.reliability = p_rtpsReader->reliability_level;
	a_datareader_data.qos = p_datareader->datareader_qos;
	a_datareader_data.ismodified = DELETE_MODE;

	write_DataReader(a_datareader_data);
}

static void add_datawriter_for_cps(monitoring_t* p_monitoring, DataWriter* p_datawriter)
{
	monitoring_DataWriter a_datawriter_data;

	rtps_writer_t* p_rtps_writer = p_datawriter->p_related_rtps_writer;

	memset(&a_datawriter_data, 0, sizeof(monitoring_DataWriter));

	a_datawriter_data.id = p_datawriter->id;
	memcpy(a_datawriter_data.guid, &p_rtps_writer->guid, sizeof(GUID_t));
	a_datawriter_data.parentid = p_datawriter->p_publisher->id;
	memcpy(a_datawriter_data.participantdomainguid, &p_datawriter->p_publisher->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_datawriter_data.topicid = p_datawriter->p_topic->id;
	set_string(&a_datawriter_data.topic_name, p_datawriter->p_topic->topic_name);
	a_datawriter_data.topic_type = p_datawriter->p_topic->topic_type;
	set_string(&a_datawriter_data.type_name, p_datawriter->p_topic->type_name);

	a_datawriter_data.behavior = p_rtps_writer->behavior_type;
	a_datawriter_data.reliability = p_rtps_writer->reliability_level;
	a_datawriter_data.qos = p_datawriter->datawriter_qos;
	a_datawriter_data.ismodified = INSERT_MODE;

	write_DataWriter(a_datawriter_data);
}

static void delete_datawriter_for_cps(monitoring_t* p_monitoring, DataWriter* p_datawriter)
{
	monitoring_DataWriter a_datawriter_data;

	rtps_writer_t* p_rtps_writer = p_datawriter->p_related_rtps_writer;

	memset(&a_datawriter_data, 0, sizeof(monitoring_DataWriter));

	a_datawriter_data.id = p_datawriter->id;
	memcpy(a_datawriter_data.guid, &p_rtps_writer->guid, sizeof(GUID_t));
	a_datawriter_data.parentid = p_datawriter->p_publisher->id;
	memcpy(a_datawriter_data.participantdomainguid, &p_datawriter->p_publisher->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	a_datawriter_data.topicid = p_datawriter->p_topic->id;
	set_string(&a_datawriter_data.topic_name, p_datawriter->p_topic->topic_name);
	a_datawriter_data.topic_type = p_datawriter->p_topic->topic_type;
	set_string(&a_datawriter_data.type_name, p_datawriter->p_topic->type_name);
	a_datawriter_data.behavior = p_rtps_writer->behavior_type;
	a_datawriter_data.reliability = p_rtps_writer->reliability_level;
	a_datawriter_data.qos = p_datawriter->datawriter_qos;
	a_datawriter_data.ismodified = DELETE_MODE;

	write_DataWriter(a_datawriter_data);
}

static void add_writerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	monitoring_WriterProxy a_writerproxy_data;

	if (p_statefulreader->p_rtps_participant->p_participant->domain_id == -800)
		return;

	memset(&a_writerproxy_data, 0, sizeof(monitoring_WriterProxy));

	a_writerproxy_data.readerid = p_statefulreader->p_datareader->id;
	memcpy(a_writerproxy_data.participantdomainguid, &p_statefulreader->p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	memcpy(a_writerproxy_data.readerguid, &p_statefulreader->guid, sizeof(GUID_t));
	memcpy(a_writerproxy_data.remoteWriterGuid, &p_writerproxy->remote_writer_guid, sizeof(GUID_t));
	a_writerproxy_data.subscriberid = p_statefulreader->p_datareader->p_subscriber->id;

	a_writerproxy_data.ismodified = INSERT_MODE;

	//printf("add_writerproxy_for_cps\r\n");

	write_WriterProxy(a_writerproxy_data);
}

static void delete_writerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	monitoring_WriterProxy a_writerproxy_data;

	if (p_statefulreader->p_rtps_participant->p_participant->domain_id == -800)
		return;

	memset(&a_writerproxy_data, 0, sizeof(monitoring_WriterProxy));

	a_writerproxy_data.readerid = p_statefulreader->p_datareader->id;
	memcpy(a_writerproxy_data.participantdomainguid, &p_statefulreader->p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, sizeof(GUID_t));
	memcpy(a_writerproxy_data.readerguid, &p_statefulreader->guid, sizeof(GUID_t));
	memcpy(a_writerproxy_data.remoteWriterGuid, &p_writerproxy->remote_writer_guid, sizeof(GUID_t));
	a_writerproxy_data.subscriberid = p_statefulreader->p_datareader->p_subscriber->id;


	a_writerproxy_data.ismodified = DELETE_MODE;



	write_WriterProxy(a_writerproxy_data);
}


static void add_readerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy)
{
	monitoring_ReaderProxy a_readerproxy_data;


	if (p_statefulwriter->p_rtps_participant->p_participant->domain_id == -800)
		return;

	memset(&a_readerproxy_data, 0, sizeof(monitoring_ReaderProxy));



	a_readerproxy_data.writerid = p_statefulwriter->p_datawriter->id;
	memcpy(a_readerproxy_data.participantdomainguid, &p_statefulwriter->p_rtps_participant->guid, sizeof(GUID_t));
	memcpy(a_readerproxy_data.writerguid, &p_statefulwriter->guid, sizeof(GUID_t));
	memcpy(a_readerproxy_data.remoteReaderGuid, &p_readerproxy->remote_reader_guid, sizeof(GUID_t));
	a_readerproxy_data.publisherid = p_statefulwriter->p_datawriter->p_publisher->id;

	a_readerproxy_data.ismodified = INSERT_MODE;

	//printf("add_readerproxy_for_cps\r\n");

	write_ReaderProxy(a_readerproxy_data);
}

static void delete_readerproxy_for_cps(monitoring_t* p_monitoring, rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy)
{
	monitoring_ReaderProxy a_readerproxy_data;

	if (p_statefulwriter->p_rtps_participant->p_participant->domain_id == -800)
		return;

	memset(&a_readerproxy_data, 0, sizeof(monitoring_ReaderProxy));

	a_readerproxy_data.writerid = p_statefulwriter->p_datawriter->id;
	memcpy(a_readerproxy_data.participantdomainguid, &p_statefulwriter->p_rtps_participant->guid, sizeof(GUID_t));
	memcpy(a_readerproxy_data.writerguid, &p_statefulwriter->guid, sizeof(GUID_t));
	memcpy(a_readerproxy_data.remoteReaderGuid, &p_readerproxy->remote_reader_guid, sizeof(GUID_t));
	a_readerproxy_data.publisherid = p_statefulwriter->p_datawriter->p_publisher->id;
	a_readerproxy_data.ismodified = DELETE_MODE;

	write_ReaderProxy(a_readerproxy_data);
}


static void warning_condition(monitoring_t* p_monitoring, int32_t entity, char* p_condition, int32_t warning_or_error, int32_t level, int32_t code)
{
	monitoring_WarningErrorConditions a_warning_error_conditions_data;


	if (b_is_get_participant_guid == false)
		return;

	memset(&a_warning_error_conditions_data, 0, sizeof(monitoring_WarningErrorConditions));

	memcpy(a_warning_error_conditions_data.participantdomainguid, participant_guid, sizeof(GUID_t));
	set_string(&a_warning_error_conditions_data.conditions, p_condition);
	a_warning_error_conditions_data.entity = entity;
	a_warning_error_conditions_data.warningorerror = warning_or_error;
	a_warning_error_conditions_data.level = level;
	a_warning_error_conditions_data.code = code;

	write_WarningErrorConditions(a_warning_error_conditions_data);
}


int32_t getSendCount();
int64_t getSendBytes();

int32_t getReceivedCount();
int64_t getReceivedBytes();


int32_t getSendCountThroughput();
int32_t getSendBytesThroughput();

int32_t getreceived_count_throughput();
int32_t getreceived_bytes_throughput();

void resetThroughput();

static void service_monitoring_thread(module_object_t* p_this)
{
	monitoring_t* p_monitoring = (monitoring_t*)p_this;
	monitoring_sys_t* p_sys = p_monitoring->p_sys;
	rtps_participant_t** pp_rtps_participants;
	rtps_participant_t* p_rtps_participant;
	int i_rtps_participants;
	int i, j;
	rtps_endpoint_t* p_rtps_endpoint;
	monitoring_SamplesinforamtionReader a_samplesinforamtion_reader_data;
	monitoring_SamplesinforamtionWriter a_samplesinforamtion_writer_data;
	rtps_reader_t* p_rtps_reader;
	rtps_writer_t* p_rtps_writer;

	memset(&a_samplesinforamtion_reader_data, 0, sizeof(monitoring_SamplesinforamtionReader));
	memset(&a_samplesinforamtion_writer_data, 0, sizeof(monitoring_SamplesinforamtionWriter));

	while (!p_sys->b_end)
	{
		msleep(p_sys->polling_sec * 1000 * 1000);
		////
		{
			//////////////
			rtps_participant_lock();

			pp_rtps_participants = get_rtps_participants(&i_rtps_participants);

			for (i = 0; i < i_rtps_participants; i++)
			{
				p_rtps_participant = pp_rtps_participants[i];

				if (p_rtps_participant == NULL)
				{
					break;
				}


				if (i == 0)
				{
					monitoring_DataBandwidth a_data_bandwidth_data;
					int64_t sendbytes = 0;
					int64_t receivedbytes = 0;


					memset(&a_data_bandwidth_data, 0, sizeof(monitoring_DataBandwidth));


					memcpy(a_data_bandwidth_data.participantdomainguid, &p_rtps_participant->guid, sizeof(GUID_t));

					a_data_bandwidth_data.sendDataCount = getSendCount();
					sendbytes = getSendBytes();
					receivedbytes = getReceivedBytes();
					a_data_bandwidth_data.receivedDataCount = getReceivedCount();
					memcpy(a_data_bandwidth_data.sendDataBandwidth, &sendbytes, sizeof(int64_t));
					memcpy(a_data_bandwidth_data.receivedDataBandwidth, &receivedbytes, sizeof(int64_t));
					set_string(&a_data_bandwidth_data.ip, p_sys->p_local_ip);

					a_data_bandwidth_data.receivedDataCountThroughput = getreceived_count_throughput();
					a_data_bandwidth_data.receivedDataBandwidthThroughput = getreceived_bytes_throughput();

					a_data_bandwidth_data.sendDataCountThroughput = getSendCountThroughput();
					a_data_bandwidth_data.sendDataBandwidthThroughput = getSendBytesThroughput();

					resetThroughput();

					write_DataBandwidth(a_data_bandwidth_data);
				}


				for (j = 0; j < p_rtps_participant->i_endpoint; j++)
				{
					p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];

					if ((p_rtps_endpoint->i_entity_type == READER_ENTITY))
					{
						p_rtps_reader = (rtps_reader_t*)p_rtps_endpoint;

						memcpy(a_samplesinforamtion_reader_data.participantdomainguid, &p_rtps_reader->p_rtps_participant->guid, sizeof(GUID_t));
						a_samplesinforamtion_reader_data.readerid = p_rtps_reader->p_datareader->id;
						memcpy(a_samplesinforamtion_reader_data.readerguid, &p_rtps_reader->guid, sizeof(GUID_t));
						a_samplesinforamtion_reader_data.receivedcounts = p_rtps_reader->received_count;
						memcpy(a_samplesinforamtion_reader_data.receivedbytes, &p_rtps_reader->received_bytes, sizeof(int64_t));
						a_samplesinforamtion_reader_data.received_sample_counts = p_rtps_reader->received_sample_count;
						memcpy(a_samplesinforamtion_reader_data.received_sample_bytes, &p_rtps_reader->received_sample_bytes, sizeof(int64_t));
						a_samplesinforamtion_reader_data.lostcount = 0;


						a_samplesinforamtion_reader_data.receivedcountsThroughput = p_rtps_reader->received_count_throughput;
						a_samplesinforamtion_reader_data.received_bytes_throughput = p_rtps_reader->received_bytes_throughput;

						a_samplesinforamtion_reader_data.received_sample_countsThroughput = p_rtps_reader->received_sample_count_throughput;
						a_samplesinforamtion_reader_data.received_sample_bytes_throughput = p_rtps_reader->received_sample_bytes_throughput;

						a_samplesinforamtion_reader_data.subscriberid = p_rtps_reader->p_datareader->p_subscriber->id;

						p_rtps_reader->received_count_throughput = 0;
						p_rtps_reader->received_bytes_throughput = 0;
						p_rtps_reader->received_sample_count_throughput = 0;
						p_rtps_reader->received_sample_bytes_throughput = 0;

						write_SamplesinforamtionReader(a_samplesinforamtion_reader_data);
					}
					else if ((p_rtps_endpoint->i_entity_type == WRITER_ENTITY))
					{
						p_rtps_writer = (rtps_writer_t*)p_rtps_endpoint;


						memcpy(a_samplesinforamtion_writer_data.participantdomainguid, &p_rtps_writer->p_rtps_participant->guid, sizeof(GUID_t));
						a_samplesinforamtion_writer_data.writerid = p_rtps_writer->p_datawriter->id;
						memcpy(a_samplesinforamtion_writer_data.writerguid, &p_rtps_writer->guid, sizeof(GUID_t));
						a_samplesinforamtion_writer_data.sendcounts = p_rtps_writer->send_count;
						memcpy(a_samplesinforamtion_writer_data.sendbytes, &p_rtps_writer->send_bytes, sizeof(int64_t));
						a_samplesinforamtion_writer_data.sendsamplecounts = p_rtps_writer->send_sample_count;
						memcpy(a_samplesinforamtion_writer_data.sendsamplebytes, &p_rtps_writer->send_sample_bytes, sizeof(int64_t));
						a_samplesinforamtion_writer_data.lostcount = 0;

						a_samplesinforamtion_writer_data.sendcountsThroughput = p_rtps_writer->send_count_throughput;
						a_samplesinforamtion_writer_data.sendbytesThroughput = p_rtps_writer->send_bytes_throughput;
						a_samplesinforamtion_writer_data.sendsamplecountsThroughput = p_rtps_writer->send_sample_count_throughput;
						a_samplesinforamtion_writer_data.sendsamplebytesThroughput = p_rtps_writer->send_sample_bytes_throughput;

						a_samplesinforamtion_writer_data.publisherid = p_rtps_writer->p_datawriter->p_publisher->id;

						p_rtps_writer->send_count_throughput = 0;
						p_rtps_writer->send_bytes_throughput = 0;
						p_rtps_writer->send_sample_count_throughput = 0;
						p_rtps_writer->send_sample_bytes_throughput = 0;


						write_SamplesinforamtionWriter(a_samplesinforamtion_writer_data);
					}
				}
			}

			rtps_participant_unlock();
		}
	}
}

static void (*read_participantdomain_func)(monitoring_ParticipantDomain a_participantdomain_data) = NULL;
static void (*read_topic_func)(monitoring_Topic a_topic_data) = NULL;
static void (*read_publisher_func)(monitoring_Publisher a_publisher_data) = NULL;
static void (*read_subscriber_func)(monitoring_Subscriber a_subscriber_data) = NULL;
static void (*read_datawriter_func)(monitoring_DataWriter a_datawriter_data) = NULL;
static void (*read_datareader_func)(monitoring_DataReader a_datareader_data) = NULL;
static void (*read_data_bandwidth_func)(monitoring_DataBandwidth a_data_bandwidth_data) = NULL;
static void (*read_writerproxy_func)(monitoring_WriterProxy a_writerproxy_data) = NULL;
static void (*read_readerproxy_func)(monitoring_ReaderProxy a_readerproxy_data) = NULL;
static void (*read_warning_error_conditions_func)(monitoring_WarningErrorConditions a_warning_error_conditions_data) = NULL;
static void (*read_samplesinforamtion_reader_func)(monitoring_SamplesinforamtionReader a_samplesinforamtion_reader_data) = NULL;
static void (*read_samplesinforamtion_writer_func)(monitoring_SamplesinforamtionWriter a_samplesinforamtion_writer_data) = NULL;


void read_ParticipantDomain(monitoring_ParticipantDomain a_participantdomain_data)
{
	printf("read_ParticipantDomain\r\n");
	if (read_participantdomain_func)
	{
		read_participantdomain_func(a_participantdomain_data);
	}
}

void read_Topic(monitoring_Topic a_topic_data)
{
	printf("read_Topic\r\n");
	if (read_topic_func)
	{
		read_topic_func(a_topic_data);
	}
}

void read_Publisher(monitoring_Publisher a_Publisher_data)
{
	printf("read_Publisher\r\n");
	if (read_publisher_func)
	{
		read_publisher_func(a_Publisher_data);
	}
}

void read_Subscriber(monitoring_Subscriber a_Subscriber_data)
{
	printf("read_Subscriber\r\n");
	if (read_subscriber_func)
	{
		read_subscriber_func(a_Subscriber_data);
	}
}

void read_DataWriter(monitoring_DataWriter a_DataWriter_data)
{
	printf("read_DataWriter\r\n");
	if (read_datawriter_func)
	{
		read_datawriter_func(a_DataWriter_data);
	}
}

void read_DataReader(monitoring_DataReader a_DataReader_data)
{
	printf("read_DataReader\r\n");
	if (read_datareader_func)
	{
		read_datareader_func(a_DataReader_data);
	}
}
void read_DataBandwidth(monitoring_DataBandwidth a_DataBandwidth_data)
{
	if (read_data_bandwidth_func)
	{
		read_data_bandwidth_func(a_DataBandwidth_data);
	}
}

void read_WriterProxy(monitoring_WriterProxy a_WriterProxy_data)
{
	printf("read_WriterProxy\r\n");
	if (read_writerproxy_func)
	{
		read_writerproxy_func(a_WriterProxy_data);
	}
}

void read_ReaderProxy(monitoring_ReaderProxy a_ReaderProxy_data)
{
	printf("read_ReaderProxy\r\n");
	if (read_readerproxy_func)
	{
		read_readerproxy_func(a_ReaderProxy_data);
	}
}

void read_WarningErrorConditions(monitoring_WarningErrorConditions a_WarningErrorConditions_data)
{
	if (read_warning_error_conditions_func)
	{
		read_warning_error_conditions_func(a_WarningErrorConditions_data);
	}

}
void read_SamplesinforamtionReader(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data)
{
	if (read_samplesinforamtion_reader_func)
	{
		read_samplesinforamtion_reader_func(a_SamplesinforamtionReader_data);
	}
}
void read_SamplesinforamtionWriter(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data)
{
	if (read_samplesinforamtion_writer_func)
	{
		read_samplesinforamtion_writer_func(a_SamplesinforamtionWriter_data);
	}
}




void setCallFunc_read_ParticipantDomain(void (*read_ParticipantDomain_wrapper)(monitoring_ParticipantDomain a_ParticipantDomain_data))
{
	read_participantdomain_func = read_ParticipantDomain_wrapper;
}


void setCallFunc_read_Topic(void (*read_Topic_wrapper)(monitoring_Topic a_Topic_data))
{
	read_topic_func = read_Topic_wrapper;
}

void setCallFunc_read_Publisher(void (*read_Publisher_wrapper)(monitoring_Publisher a_Publisher_data))
{
	read_publisher_func = read_Publisher_wrapper;
}

void setCallFunc_read_Subscriber(void (*read_Subscriber_wrapper)(monitoring_Subscriber a_Subscriber_data))
{
	read_subscriber_func = read_Subscriber_wrapper;
}

void setCallFunc_read_DataWriter(void (*read_DataWriter_wrapper)(monitoring_DataWriter a_DataWriter_data))
{
	read_datawriter_func = read_DataWriter_wrapper;
}

void setCallFunc_read_DataReader(void (*read_DataReader_wrapper)(monitoring_DataReader a_DataReader_data))
{
	read_datareader_func = read_DataReader_wrapper;
}

void setCallFunc_read_DataBandwidth(void (*read_DataBandwidth_wrapper)(monitoring_DataBandwidth a_DataBandwidth_data))
{
	read_data_bandwidth_func = read_DataBandwidth_wrapper;
}


void setCallFunc_read_WriterProxy(void (*read_WriterProxy_wrapper)(monitoring_WriterProxy a_WriterProxy_data))
{
	read_writerproxy_func = read_WriterProxy_wrapper;
}

void setCallFunc_read_ReaderProxy(void (*read_ReaderProxy_wrapper)(monitoring_ReaderProxy a_ReaderProxy_data))
{
	read_readerproxy_func = read_ReaderProxy_wrapper;
}

void setCallFunc_read_WarningErrorConditions(void (*read_WarningErrorConditions_wrapper)(monitoring_WarningErrorConditions a_WarningErrorConditions_data))
{
	read_warning_error_conditions_func = read_WarningErrorConditions_wrapper;
}

void setCallFunc_read_SamplesinforamtionReader(void (*read_SamplesinforamtionReader_wrapper)(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data))
{
	read_samplesinforamtion_reader_func = read_SamplesinforamtionReader_wrapper;
}

void setCallFunc_read_SamplesinforamtionWriter(void (*read_SamplesinforamtionWriter_wrapper)(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data))
{
	read_samplesinforamtion_writer_func = read_SamplesinforamtionWriter_wrapper;
}




void monitoring__add_writerproxy_for_builtin(rtps_reader_t* p_rtps_reader)
{
	rtps_statefulreader_t* p_statefulreader;
	int i, i_size;

	if (p_rtps_reader->behavior_type == STATEFUL_TYPE)
	{
		p_statefulreader = (rtps_statefulreader_t*)p_rtps_reader;

		READER_LOCK((rtps_reader_t*)p_statefulreader);

		i_size = p_statefulreader->i_matched_writers;

		for (i = 0; i < i_size; i++)
		{
			monitoring__add_writerproxy(p_statefulreader, p_statefulreader->pp_matched_writers[i]);
		}

		READER_UNLOCK((rtps_reader_t*)p_statefulreader);
	}
}

void monitoring__add_readerproxy_for_builtin(rtps_writer_t* p_rtps_writer)
{
	rtps_statefulwriter_t* p_statefulwriter;
	int i, i_size;

	if (p_rtps_writer->behavior_type == STATEFUL_TYPE)
	{
		p_statefulwriter = (rtps_statefulwriter_t*)p_rtps_writer;

		mutex_lock(&p_statefulwriter->object_lock);

		i_size = p_statefulwriter->i_matched_readers;

		for (i = 0; i < i_size; i++)
		{
			monitoring__add_readerproxy(p_statefulwriter, p_statefulwriter->pp_matched_readers[i]);
		}

		mutex_unlock(&p_statefulwriter->object_lock);
	}
}