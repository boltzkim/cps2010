%module(directors="1") cpsmodule 
%{ 

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <rtps.h>
#include <monitoring_application.h>

#include "jni.h"
JavaVM *_jvm = 0;

#ifdef __cplusplus
extern "C" {
#endif

void setCallFunc_read_ParticipantDomain(void (*read_ParticipantDomain_wrapper)(monitoring_ParticipantDomain a_ParticipantDomain_data));
void setCallFunc_read_Topic(void (*read_Topic_wrapper)(monitoring_Topic a_Topic_data));
void setCallFunc_read_Publisher(void (*read_Publisher_wrapper)(monitoring_Publisher a_Publisher_data));
void setCallFunc_read_Subscriber(void (*read_Subscriber_wrapper)(monitoring_Subscriber a_Subscriber_data));
void setCallFunc_read_DataWriter(void (*read_DataWriter_wrapper)(monitoring_DataWriter a_DataWriter_data));
void setCallFunc_read_DataReader(void (*read_DataReader_wrapper)(monitoring_DataReader a_DataReader_data));
void setCallFunc_read_DataBandwidth(void (*read_DataBandwidth_wrapper)(monitoring_DataBandwidth a_DataBandwidth_data));
void setCallFunc_read_WriterProxy(void (*read_WriterProxy_wrapper)(monitoring_WriterProxy a_WriterProxy_data));
void setCallFunc_read_ReaderProxy(void (*read_ReaderProxy_wrapper)(monitoring_ReaderProxy a_ReaderProxy_data));
void setCallFunc_read_WarningErrorConditions(void (*read_WarningErrorConditions_wrapper)(monitoring_WarningErrorConditions a_WarningErrorConditions_data));
void setCallFunc_read_SamplesinforamtionReader(void (*read_SamplesinforamtionReader_wrapper)(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data));
void setCallFunc_read_SamplesinforamtionWriter(void (*read_SamplesinforamtionWriter_wrapper)(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data));


#ifdef __cplusplus
}
#endif

%}


struct UserDataQosPolicy {
	//sequence<octet> value;
	char* value;
};

struct TopicDataQosPolicy {
	//sequence<octet> value;
	char* value;
};

struct GroupDataQosPolicy {
	//sequence<octet> value;
	char* value;
};

struct TransportPriorityQosPolicy {
	long value;
};

struct LifespanQosPolicy {
	Duration_t duration;
};

enum DurabilityQosPolicyKind {
	VOLATILE_DURABILITY_QOS,
	TRANSIENT_LOCAL_DURABILITY_QOS,
	TRANSIENT_DURABILITY_QOS,
	PERSISTENT_DURABILITY_QOS
};

struct DurabilityQosPolicy {
	DurabilityQosPolicyKind kind;
};

enum PresentationQosPolicyAccessScopeKind {
	INSTANCE_PRESENTATION_QOS,
	TOPIC_PRESENTATION_QOS,
	GROUP_PRESENTATION_QOS
};


struct PresentationQosPolicy {
	PresentationQosPolicyAccessScopeKind access_scope;
	bool coherent_access;
	bool ordered_access;
};


struct DeadlineQosPolicy {
	Duration_t period;
};

struct LatencyBudgetQosPolicy {
	Duration_t duration;
};

enum OwnershipQosPolicyKind {
	SHARED_OWNERSHIP_QOS,
	EXCLUSIVE_OWNERSHIP_QOS
};

struct OwnershipQosPolicy {
	OwnershipQosPolicyKind kind;
};

struct OwnershipStrengthQosPolicy {
	int32_t value;
};

enum LivelinessQosPolicyKind {
	AUTOMATIC_LIVELINESS_QOS,
	MANUAL_BY_PARTICIPANT_LIVELINESS_QOS,
	MANUAL_BY_TOPIC_LIVELINESS_QOS
};


struct LivelinessQosPolicy {
	LivelinessQosPolicyKind kind;
	Duration_t lease_duration;
};

struct TimeBasedFilterQosPolicy {
	Duration_t minimum_separation;
};

struct PartitionQosPolicy {
	StringSeq name;
};

enum ReliabilityQosPolicyKind {
	BEST_EFFORT_RELIABILITY_QOS = 1,
	RELIABLE_RELIABILITY_QOS
};

struct ReliabilityQosPolicy {
	ReliabilityQosPolicyKind kind;
	Duration_t max_blocking_time;
};

enum DestinationOrderQosPolicyKind {
	BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS,
	BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
};

struct DestinationOrderQosPolicy {
	DestinationOrderQosPolicyKind kind;
};

enum HistoryQosPolicyKind {
	KEEP_LAST_HISTORY_QOS,
	KEEP_ALL_HISTORY_QOS
};

struct HistoryQosPolicy {
	HistoryQosPolicyKind kind;
	int32_t depth;
};

struct ResourceLimitsQosPolicy {
	int32_t max_samples;
	int32_t max_instances;
	int32_t max_samples_per_instance;
};

struct EntityFactoryQosPolicy {
	bool autoenable_created_entities;
};

struct WriterDataLifecycleQosPolicy {
	bool autodispose_unregistered_instances;
};

struct ReaderDataLifecycleQosPolicy {
	Duration_t autopurge_nowriter_samples_delay;
	Duration_t autopurge_disposed_samples_delay;
};

struct DurabilityServiceQosPolicy {
	Duration_t service_cleanup_delay;
	HistoryQosPolicyKind history_kind;
	int32_t history_depth;
	int32_t max_samples;
	int32_t max_instances;
	int32_t max_samples_per_instance;
};

struct DomainParticipantFactoryQos {
	EntityFactoryQosPolicy entity_factory;
};

struct DomainParticipantQos {
	UserDataQosPolicy user_data;
	EntityFactoryQosPolicy entity_factory;
};

struct TopicQos {
	TopicDataQosPolicy topic_data;
	DurabilityQosPolicy durability;
	DurabilityServiceQosPolicy durability_service;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	DestinationOrderQosPolicy destination_order;
	HistoryQosPolicy history;
	ResourceLimitsQosPolicy resource_limits;
	TransportPriorityQosPolicy transport_priority;
	LifespanQosPolicy lifespan;

	OwnershipQosPolicy ownership;
};

struct DataWriterQos {
	DurabilityQosPolicy durability;
	DurabilityServiceQosPolicy durability_service;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	DestinationOrderQosPolicy destination_order;
	HistoryQosPolicy history;
	ResourceLimitsQosPolicy resource_limits;
	TransportPriorityQosPolicy transport_priority;
	LifespanQosPolicy lifespan;
	UserDataQosPolicy user_data;
	OwnershipQosPolicy ownership;
	OwnershipStrengthQosPolicy ownership_strength;
	WriterDataLifecycleQosPolicy writer_data_lifecycle;
};

struct PublisherQos {
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	GroupDataQosPolicy group_data;
	EntityFactoryQosPolicy entity_factory;
};

struct DataReaderQos {
	DurabilityQosPolicy durability;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	DestinationOrderQosPolicy destination_order;
	HistoryQosPolicy history;
	ResourceLimitsQosPolicy resource_limits;
	UserDataQosPolicy user_data;
	OwnershipQosPolicy ownership;
	TimeBasedFilterQosPolicy time_based_filter;
	ReaderDataLifecycleQosPolicy reader_data_lifecycle;
};

struct SubscriberQos {
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	GroupDataQosPolicy group_data;
	EntityFactoryQosPolicy entity_factory;
};

struct RxOQos {
	DurabilityQosPolicy durability;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	OwnershipQosPolicy ownership;
	DestinationOrderQosPolicy destination_order;
	PresentationQosPolicy presentation;	

};


struct ParticipantBuiltinTopicData {
	BuiltinTopicKey_t key;
	UserDataQosPolicy user_data;
};

struct TopicBuiltinTopicData {
	BuiltinTopicKey_t key;
	string name;
	string type_name;
	DurabilityQosPolicy durability;
	DurabilityServiceQosPolicy durability_service;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	TransportPriorityQosPolicy transport_priority;
	LifespanQosPolicy lifespan;
	DestinationOrderQosPolicy destination_order;
	HistoryQosPolicy history;
	ResourceLimitsQosPolicy resource_limits;
	OwnershipQosPolicy ownership;
	TopicDataQosPolicy topic_data;
};

struct PublicationBuiltinTopicData {
	BuiltinTopicKey_t key;
	BuiltinTopicKey_t participant_key;
	string topic_name;
	string type_name;
	DurabilityQosPolicy durability;
	DurabilityServiceQosPolicy durability_service;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	LifespanQosPolicy lifespan;
	UserDataQosPolicy user_data;
	OwnershipQosPolicy ownership;
	OwnershipStrengthQosPolicy ownership_strength;
	DestinationOrderQosPolicy destination_order;
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	TopicDataQosPolicy topic_data;
	GroupDataQosPolicy group_data;
};

struct SubscriptionBuiltinTopicData {
	BuiltinTopicKey_t key;
	BuiltinTopicKey_t participant_key;
	string topic_name;
	string type_name;
	DurabilityQosPolicy durability;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	OwnershipQosPolicy ownership;
	DestinationOrderQosPolicy destination_order;
	UserDataQosPolicy user_data;
	TimeBasedFilterQosPolicy time_based_filter;
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	TopicDataQosPolicy topic_data;
	GroupDataQosPolicy group_data;
};



%include "typemaps.i"
%include "arrays_java.i"
%apply int[] {int *};

%include "core.h"
%include "cpsdcps.h"
%include "dcps_func.h"
%include "module_common.h"
%include "rtps_message.h"
%include "rtps.h"
%include "util.h"




%include "monitoring_module.h"
%include "monitoring_application.h"

%feature("director");

%inline %{



class CpsCallback {
public:
	virtual ~CpsCallback() {}
	virtual void read_ParticipantDomain(monitoring_ParticipantDomain a_ParticipantDomain_data){}
	virtual void read_Topic(monitoring_Topic a_Topic_data){}
	virtual void read_Publisher(monitoring_Publisher a_Publisher_data){}
	virtual void read_Subscriber(monitoring_Subscriber a_Subscriber_data){}
	virtual void read_DataWriter(monitoring_DataWriter a_DataWriter_data){}
	virtual void read_DataReader(monitoring_DataReader a_DataReader_data){}
	virtual void read_DataBandwidth(monitoring_DataBandwidth a_DataBandwidth_data){}
	virtual void read_WriterProxy(monitoring_WriterProxy a_WriterProxy_data){}
	virtual void read_ReaderProxy(monitoring_ReaderProxy a_ReaderProxy_data){}
	virtual void read_WarningErrorConditions(monitoring_WarningErrorConditions a_WarningErrorConditions_data){}
	virtual void read_SamplesinforamtionReader(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data){}
	virtual void read_SamplesinforamtionWriter(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data){}
};


static CpsCallback *registeredCallbackObject = NULL;


void read_ParticipantDomain_wrapper(monitoring_ParticipantDomain a_ParticipantDomain_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_ParticipantDomain(a_ParticipantDomain_data);
	}
}

void read_Topic_wrapper(monitoring_Topic a_Topic_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_Topic(a_Topic_data);
	}
}

void read_Publisher_wrapper(monitoring_Publisher a_Publisher_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_Publisher(a_Publisher_data);
	}
}


void read_Subscriber_wrapper(monitoring_Subscriber a_Subscriber_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_Subscriber(a_Subscriber_data);
	}
}


void read_DataWriter_wrapper(monitoring_DataWriter a_DataWriter_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_DataWriter(a_DataWriter_data);
	}
}


void read_DataReader_wrapper(monitoring_DataReader a_DataReader_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_DataReader(a_DataReader_data);
	}
}


void read_DataBandwidth_wrapper(monitoring_DataBandwidth a_DataBandwidth_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_DataBandwidth(a_DataBandwidth_data);
	}
}


void read_WriterProxy_wrapper(monitoring_WriterProxy a_WriterProxy_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_WriterProxy(a_WriterProxy_data);
	}
}


void read_ReaderProxy_wrapper(monitoring_ReaderProxy a_ReaderProxy_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_ReaderProxy(a_ReaderProxy_data);
	}
}


void read_WarningErrorConditions_wrapper(monitoring_WarningErrorConditions a_WarningErrorConditions_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_WarningErrorConditions(a_WarningErrorConditions_data);
	}
}


void read_SamplesinforamtionReader_wrapper(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_SamplesinforamtionReader(a_SamplesinforamtionReader_data);
	}
}


void read_SamplesinforamtionWriter_wrapper(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data)
{
	if(registeredCallbackObject)
	{
		registeredCallbackObject->read_SamplesinforamtionWriter(a_SamplesinforamtionWriter_data);
	}
}


void setCallbackObject(CpsCallback *callback) {
	registeredCallbackObject = callback;

	setCallFunc_read_ParticipantDomain(read_ParticipantDomain_wrapper);
	setCallFunc_read_Topic(read_Topic_wrapper);
	setCallFunc_read_Publisher(read_Publisher_wrapper);
	setCallFunc_read_Subscriber(read_Subscriber_wrapper);
	setCallFunc_read_DataWriter(read_DataWriter_wrapper);
	setCallFunc_read_DataReader(read_DataReader_wrapper);
	setCallFunc_read_DataBandwidth(read_DataBandwidth_wrapper);
	setCallFunc_read_WriterProxy(read_WriterProxy_wrapper);
	setCallFunc_read_ReaderProxy(read_ReaderProxy_wrapper);
	setCallFunc_read_WarningErrorConditions(read_WarningErrorConditions_wrapper);
	setCallFunc_read_SamplesinforamtionReader(read_SamplesinforamtionReader_wrapper);
	setCallFunc_read_SamplesinforamtionWriter(read_SamplesinforamtionWriter_wrapper);

}

%}
