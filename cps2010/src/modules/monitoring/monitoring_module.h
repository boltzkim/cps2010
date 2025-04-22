#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#ifndef monitoring_module
#define monitoring_module 1

#ifdef __cplusplus
extern "C" {
#endif

void register_type_module_monitoring(DomainParticipant* p_participant);

#ifdef __cplusplus
}
#endif

static void insert_monitoring_ParticipantDomain_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
		/*p_para->offset = p_foo_typesupport->offsetcount;
		p_foo_typesupport->offsetcount += p_para->i_size;*/
	}
	{
		dds_parameter_t *p_para = get_new_parameter("domain_id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("guid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("domainName", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ip", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(DomainParticipantQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("b_is_modified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_ParticipantDomain
{
	int32_t id;
	int32_t domain_id;
	int32_t guid[4];
	dds_string domainName;
	dds_string ip;
	DomainParticipantQos qos;
	int32_t b_is_modified;
} monitoring_ParticipantDomain;

static void insert_monitoring_Topic_Parameters(FooTypeSupport* p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("topic_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("topic_type", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("type_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("type", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(TopicQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_Topic
{
	int32_t id;
	int32_t participantdomainguid[4];
	dds_string topic_name;
	int32_t topic_type;
	dds_string type_name;
	dds_string type;
	TopicQos qos;
	int32_t ismodified;
} monitoring_Topic;

static void insert_monitoring_Publisher_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t* p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("parentid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(PublisherQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_Publisher
{
	int32_t id;
	int32_t parentid;
	int32_t participantdomainguid[4];
	PublisherQos qos;
	int32_t ismodified;
} monitoring_Publisher;

static void insert_monitoring_Subscriber_Parameters(FooTypeSupport* p_foo_typesupport)
{
	{
		dds_parameter_t* p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("parentid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(SubscriberQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_Subscriber
{
	int32_t id;
	int32_t parentid;
	int32_t participantdomainguid[4];
	SubscriberQos qos;
	int32_t ismodified;
} monitoring_Subscriber;

static void insert_monitoring_DataWriter_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t* p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("guid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("parentid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("topicid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("topic_type", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("topic_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("type_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("behavior", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("reliability", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("readerEntityId", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("writerEntityId", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(DataWriterQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t* p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_DataWriter
{
	int32_t id;
	int32_t guid[4];
	int32_t parentid;
	int32_t participantdomainguid[4];
	int32_t topicid;
	int32_t topic_type;
	dds_string topic_name;
	dds_string type_name;
	int32_t behavior;
	int32_t reliability;
	int32_t readerEntityId;
	int32_t writerEntityId;
	DataWriterQos qos;
	int32_t ismodified;
} monitoring_DataWriter;

static void insert_monitoring_DataReader_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("guid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("parentid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("topicid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("topic_type", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("topic_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("type_name", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("behavior", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("reliability", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("readerEntityId", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("writerEntityId", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("qos", DDS_STRUCT_TYPE,sizeof(DataReaderQos));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_DataReader
{
	int32_t id;
	int32_t guid[4];
	int32_t parentid;
	int32_t participantdomainguid[4];
	int32_t topicid;
	int32_t topic_type;
	dds_string topic_name;
	dds_string type_name;
	int32_t behavior;
	int32_t reliability;
	int32_t readerEntityId;
	int32_t writerEntityId;
	DataReaderQos qos;
	int32_t ismodified;
} monitoring_DataReader;

static void insert_monitoring_DataBandwidth_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter("type", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("id", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ip", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedDataCount", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedDataCountThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedDataBandwidth", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedDataBandwidthThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendDataCount", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendDataCountThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendDataBandwidth", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendDataBandwidthThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}

typedef struct monitoring_DataBandwidth
{
	int32_t type;
	int32_t id;
	dds_string ip;
	int32_t participantdomainguid[4];
	int32_t receivedDataCount;
	int32_t receivedDataCountThroughput;
	int32_t receivedDataBandwidth[2];
	int32_t receivedDataBandwidthThroughput;
	int32_t sendDataCount;
	int32_t sendDataCountThroughput;
	int32_t sendDataBandwidth[2];
	int32_t sendDataBandwidthThroughput;
} monitoring_DataBandwidth;

static void insert_monitoring_WriterProxy_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("subscriberid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("readerid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("readerguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("remoteWriterGuid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_WriterProxy
{
	int32_t participantdomainguid[4];
	int32_t subscriberid;
	int32_t readerid;
	int32_t readerguid[4];
	int32_t remoteWriterGuid[4];
	int32_t ismodified;
} monitoring_WriterProxy;

static void insert_monitoring_ReaderProxy_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("publisherid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("writerid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("writerguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("remoteReaderGuid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("ismodified", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_ReaderProxy
{
	int32_t participantdomainguid[4];
	int32_t publisherid;
	int32_t writerid;
	int32_t writerguid[4];
	int32_t remoteReaderGuid[4];
	int32_t ismodified;
} monitoring_ReaderProxy;

static void insert_monitoring_WarningErrorConditions_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("entity", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("guid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("conditions", DDS_STRING_TYPE,sizeof(dds_string));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("warningorerror", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("level", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("code", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_WarningErrorConditions
{
	int32_t participantdomainguid[4];
	int32_t entity;
	int32_t guid[4];
	dds_string conditions;
	int32_t warningorerror;
	int32_t level;
	int32_t code;
} monitoring_WarningErrorConditions;

static void insert_monitoring_SamplesinforamtionReader_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("readerid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("readerguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("subscriberid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedcounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedcountsThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("receivedbytes", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("received_bytes_throughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("received_sample_counts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("received_sample_countsThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("received_sample_bytes", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("received_sample_bytes_throughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("lostcount", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("requesteddeadlinesmissedcounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("datareadercachecounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_SamplesinforamtionReader
{
	int32_t participantdomainguid[4];
	int32_t readerid;
	int32_t readerguid[4];
	int32_t subscriberid;
	int32_t receivedcounts;
	int32_t receivedcountsThroughput;
	int32_t receivedbytes[2];
	int32_t received_bytes_throughput;
	int32_t received_sample_counts;
	int32_t received_sample_countsThroughput;
	int32_t received_sample_bytes[2];
	int32_t received_sample_bytes_throughput;
	int32_t lostcount;
	int32_t requesteddeadlinesmissedcounts;
	int32_t datareadercachecounts;
} monitoring_SamplesinforamtionReader;

static void insert_monitoring_SamplesinforamtionWriter_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("writerid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("writerguid", DDS_INTEGER32_TYPE,sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("publisherid", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendcounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendcountsThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendbytes", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendbytesThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendsamplecounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendsamplecountsThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendsamplebytes", DDS_INTEGER32_TYPE,sizeof(int32_t)*2);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("sendsamplebytesThroughput", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("lostcount", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("requesteddeadlinesmissedcounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("datareadercachecounts", DDS_INTEGER32_TYPE,sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_SamplesinforamtionWriter
{
	int32_t participantdomainguid[4];
	int32_t writerid;
	int32_t writerguid[4];
	int32_t publisherid;
	int32_t sendcounts;
	int32_t sendcountsThroughput;
	int32_t sendbytes[2];
	int32_t sendbytesThroughput;
	int32_t sendsamplecounts;
	int32_t sendsamplecountsThroughput;
	int32_t sendsamplebytes[2];
	int32_t sendsamplebytesThroughput;
	int32_t lostcount;
	int32_t requesteddeadlinesmissedcounts;
	int32_t datareadercachecounts;
} monitoring_SamplesinforamtionWriter;

static void insert_monitoring_Log_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("participantdomainguid", DDS_INTEGER32_TYPE, sizeof(int32_t)*4);
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("logtype", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
	{
		dds_parameter_t *p_para = get_new_parameter("msg", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}

typedef struct monitoring_Log
{
	int32_t participantdomainguid[4];
	int32_t logtype;
	dds_string msg;
} monitoring_Log;

#endif
