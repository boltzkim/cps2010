%module(directors="1") cpsmodule 
%{ 

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <rtps.h>


#ifdef __cplusplus
extern "C" {
#endif

	static jintArray SWIG_JavaArrayOutInt (JNIEnv *jenv, int *result, jsize sz);
	static JNIEnv *jenvstatic = NULL;
	jsize FillMeInAsSizeCannotBeDeterminedAutomatically = 0;

#ifdef __cplusplus
}
#endif

%}


%typemap(in) char ** (jint size) {
    int i = 0;
    size = jenv->GetArrayLength($input);
    $1 = (char **) malloc((size+1)*sizeof(char *));
    /* make a copy of each string */
    for (i = 0; i<size; i++) {
        jstring j_string = (jstring)jenv->GetObjectArrayElement($input, i);
        const char * c_string = jenv->GetStringUTFChars(j_string, 0);
        $1[i] = (char *)malloc((strlen(c_string)+1)*sizeof(char));
        strcpy($1[i], c_string);
        jenv->ReleaseStringUTFChars(j_string, c_string);
        jenv->DeleteLocalRef(j_string);
    }
    $1[i] = 0;
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) char ** {
    int i;
    for (i=0; i<size$argnum-1; i++)
      free($1[i]);
    free($1);
}

/* This allows a C function to return a char ** as a Java String array */
%typemap(out) char ** {
    int i;
    int len=0;
    jstring temp_string;
    const jclass clazz = jenv->FindClass("java/lang/String");

    while ($1[len]) len++;    
    jresult = jenv->NewObjectArray(len, clazz, NULL);
    /* exception checking omitted */

    for (i=0; i<len; i++) {
      temp_string = jenv->NewStringUTF(*result++);
      jenv->SetObjectArrayElement(jresult, i, temp_string);
      jenv->DeleteLocalRef(temp_string);
    }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) char ** "jobjectArray"
%typemap(jtype) char ** "String[]"
%typemap(jstype) char ** "String[]"

/* These 2 typemaps handle the conversion of the jtype to jstype typemap type
   and vice versa */
%typemap(javain) char ** "$javainput"
%typemap(javaout) char ** {
    return $jnicall;
  }





typedef unsigned char       uint8_t;
	typedef signed char         int8_t;
	typedef unsigned short      uint16_t;
	typedef signed short        int16_t;
	typedef unsigned int        uint32_t;
	typedef signed int          int32_t;


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


//////////////

struct Time_t {
		int32_t sec;
		uint32_t nanosec;
};
struct Duration_t {
		int32_t sec;
		uint32_t nanosec;
};

struct StringSeq
{
	int32_t i_string;
	string *pp_string;
};


struct Foo {
	int32_t dummy;
};

%extend Foo{
	Foo* allocation(int size){
		return (Foo*)malloc(size);
	}
};


struct FooSeq
{
	int32_t i_seq;
	Foo **pp_foo;
};


%extend FooSeq{
	Foo *getFoo(int i)
	{
		return $self->pp_foo[i];
	}
};

struct DomainParticipantListener{
	int	NotUsed;
};

struct SubscriberListener{
	int	NotUsed;
};

struct PublisherListener{
	int	NotUsed;
};

struct TopicListener{
	int	NotUsed;
};


struct DataWriterListener{
	int	NotUsed;
};

struct DataReaderListener{
	int	NotUsed;
};


struct ContentFilteredTopic{
	int32_t		i_entityType;
	bool	b_enable;

};


typedef enum SUPPORT_TYPE
{
	DDS_STRING_TYPE,
	DDS_INTEGER_TYPE,
	DDS_FLOAT_TYPE,
	DDS_CHARACTER_TYPE,
	DDS_STRUCT_TYPE
}SUPPORT_TYPE;

struct dds_parameter_t
{
	SUPPORT_TYPE	type;
	int				i_size;
	char			*fieldname;
	union{
		char		*string;
		long		integer;
		double		real;
		char		character;
	}value;
	bool			is_key;
	long			id;
};


struct FooTypeSupport{
	int32_t		i_size;
	int32_t		i_parameters;
	dds_parameter_t **pp_parameters;
};

%extend FooTypeSupport{
	ReturnCode_t register_type(DomainParticipant *participant, char* type_name)
	{
		return $self->register_type($self, participant, type_name);
	}
	char* get_type_name()
	{
		return $self->get_type_name($self);
	}
	dds_parameter_t *getNewParameter(char *fieldname, SUPPORT_TYPE type, int i_size)
	{
		return getNewParameter(fieldname, type, i_size);
	}
	dds_parameter_t *getNewParameterKey(char *fieldname, SUPPORT_TYPE type, int i_size)
	{
		return getNewParameterKey(fieldname, type, i_size);
	}
	FooTypeSupport *getNewFooTypeSupport()
	{
		return getNewFooTypeSupport();
	}
	void insert_param(dds_parameter_t *p_para)
	{
		INSERT_PARAM($self->pp_parameters,$self->i_parameters, $self->i_parameters, p_para);
		p_para->offset = $self->offsetcount;
		$self->offsetcount += p_para->i_size;
		$self->i_size += p_para->i_size;
	}
	int getValueInteger(Foo *foo, char *fieldname)
	{
		int i=0;
		int intvalue = -1;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				memcpy(&intvalue, ((char*)foo)+p_parameter->offset, sizeof(int));
				break;
			}
		}

		return intvalue;
	}
	char *getValueString(Foo *foo, char *fieldname)
	{
		int i=0;
		char* stringvalue= NULL;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				stringvalue = strdup( ((char*)foo)+p_parameter->offset);
				break;
			}
		}

		return stringvalue;
	}
	char **getValueStringArray(Foo *foo, char *fieldname, int size)
	{
		int i=0;
		char** stringvalue= NULL;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				stringvalue = (char**)malloc(p_parameter->i_size);
				memcpy(stringvalue, ((char*)foo)+p_parameter->offset, p_parameter->i_size);
				break;
			}
		}

		return stringvalue;
	}
	jintArray getValueIntegerArray(Foo *foo, char *fieldname, int size)
	{
		int i=0;
		dds_parameter_t *p_parameter;
		jintArray jresult = 0 ;
		int32_t *result = 0 ;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				result = (int32_t *)(int32_t *) ((char*)foo)+p_parameter->offset;
				jresult = SWIG_JavaArrayOutInt(jenvstatic, (int *)result, size); 
				break;
			}
		}


		return jresult;

	}
	int getStringTypeSize()
	{
		return sizeof(dds_string);
	}
	int getTypeSize(char *type)
	{
		FooTypeSupport *typesupprt = getTypeSupport(type);
		if(typesupprt != NULL)
		{
			return typesupprt->i_size;
		}else{
			return 256;
		}
	}
	void setValueInteger(Foo *foo, char *fieldname, int value)
	{
		int i=0;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				*((int*)((char*)foo)+p_parameter->offset) = value;
				break;
			}
		}
	}
	void setValueString(Foo *foo, char *fieldname, char *value)
	{
		int i=0;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				setString(((dds_string *)(char*)foo)+p_parameter->offset, value);
				break;
			}
		}
	}
	jobject getValueStruct(Foo *foo, jclass object, char *fieldname)
	{
		jclass cls = jenvstatic->GetObjectClass(object);
		jmethodID mid;
		int i=0;
		dds_parameter_t *p_parameter;


		if(cls == NULL)
		{
			return NULL;
		}

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				mid = jenvstatic->GetMethodID (cls, "init","(kr/re/etri/cps/Foo)V");
				object = jenvstatic->NewObject(cls, mid, ((char*)foo)+p_parameter->offset);
				break;
			}
		}

		

		return object;
	}
	void setValueStruct(Foo *foo, char *fieldname, Foo *value)
	{
		int i=0;
		dds_parameter_t *p_parameter;

		for(i=0; i < $self->i_parameters; i++)
		{
			p_parameter = $self->pp_parameters[i];

			if(strcmp(p_parameter->fieldname, fieldname) == 0)
			{
				Foo *dst = ((Foo*)((char*)foo)+p_parameter->offset);
				memcpy(dst, value, p_parameter->i_size);
				break;
			}
		}
	}
};


struct DataReader{
	int32_t		i_entityType;
	
};


%extend DataReader{
	ReadCondition *create_readcondition(SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->create_readcondition($self, sample_states, view_states, instance_states);
	}
	QueryCondition *create_querycondition(SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states,char* query_expression,StringSeq *query_parameters)
	{
		return $self->create_querycondition($self, sample_states, view_states, instance_states, query_expression, query_parameters);
	}
	ReturnCode_t delete_readcondition(ReadCondition *condition)
	{
		return $self->delete_readcondition($self, condition);
	}
	ReturnCode_t delete_contained_entities()
	{
		return $self->delete_contained_entities($self);
	}
	ReturnCode_t set_qos(DataReaderQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(DataReaderQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(DataReaderListener *listener,StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	DataReaderListener *get_listener()
	{
		return $self->get_listener($self);
	}
	TopicDescription *get_topicdescription()
	{
		return $self->get_topicdescription($self);
	}
	Subscriber *get_subscriber()
	{
		return $self->get_subscriber($self);
	}
	ReturnCode_t get_sample_rejected_status(SampleRejectedStatus *status)
	{
		return $self->get_sample_rejected_status($self, status);
	}
	ReturnCode_t get_liveliness_changed_status(LivelinessChangedStatus *status)
	{
		return $self->get_liveliness_changed_status($self, status);
	}
	ReturnCode_t get_requested_deadline_missed_status(RequestedDeadlineMissedStatus *status)
	{
		return $self->get_requested_deadline_missed_status($self, status);
	}
	ReturnCode_t get_requested_incompatible_qos_status(RequestedIncompatibleQosStatus *status)
	{
		return $self->get_requested_incompatible_qos_status($self, status);
	}
	ReturnCode_t get_subscription_matched_status(SubscriptionMatchedStatus *status)
	{
		return $self->get_subscription_matched_status($self, status);
	}
	ReturnCode_t get_sample_lost_status(SampleLostStatus *status)
	{
		return $self->get_sample_lost_status($self, status);
	}
	ReturnCode_t wait_for_historical_data(Duration_t max_wait)
	{
		return $self->wait_for_historical_data($self, max_wait);
	}
	ReturnCode_t get_matched_publications(InstanceHandleSeq *publication_handles)
	{
		return $self->get_matched_publications($self, publication_handles);
	}
	ReturnCode_t get_matched_publication_data(PublicationBuiltinTopicData *publication_data,InstanceHandle_t publication_handle)
	{
		return $self->get_matched_publication_data($self, publication_data, publication_handle);
	}
}


struct FooDataReader{
	int32_t		i_entityType;
	
};


%extend FooDataReader{
	ReadCondition *create_readcondition(SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->create_readcondition((DataReader*)$self, sample_states, view_states, instance_states);
	}
	QueryCondition *create_querycondition(SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states,char* query_expression,StringSeq *query_parameters)
	{
		return $self->create_querycondition((DataReader*)$self, sample_states, view_states, instance_states, query_expression, query_parameters);
	}
	ReturnCode_t delete_readcondition(ReadCondition *condition)
	{
		return $self->delete_readcondition((DataReader*)$self, condition);
	}
	ReturnCode_t delete_contained_entities()
	{
		return $self->delete_contained_entities((DataReader*)$self);
	}
	ReturnCode_t set_qos(DataReaderQos *qos)
	{
		return $self->set_qos((DataReader*)$self, qos);
	}
	ReturnCode_t get_qos(DataReaderQos *qos)
	{
		return $self->get_qos((DataReader*)$self, qos);
	}
	ReturnCode_t set_listener(DataReaderListener *listener,StatusMask mask)
	{
		return $self->set_listener((DataReader*)$self, listener, mask);
	}
	DataReaderListener *get_listener()
	{
		return $self->get_listener((DataReader*)$self);
	}
	TopicDescription *get_topicdescription()
	{
		return $self->get_topicdescription((DataReader*)$self);
	}
	Subscriber *get_subscriber()
	{
		return $self->get_subscriber((DataReader*)$self);
	}
	ReturnCode_t get_sample_rejected_status(SampleRejectedStatus *status)
	{
		return $self->get_sample_rejected_status((DataReader*)$self, status);
	}
	ReturnCode_t get_liveliness_changed_status(LivelinessChangedStatus *status)
	{
		return $self->get_liveliness_changed_status((DataReader*)$self, status);
	}
	ReturnCode_t get_requested_deadline_missed_status(RequestedDeadlineMissedStatus *status)
	{
		return $self->get_requested_deadline_missed_status((DataReader*)$self, status);
	}
	ReturnCode_t get_requested_incompatible_qos_status(RequestedIncompatibleQosStatus *status)
	{
		return $self->get_requested_incompatible_qos_status((DataReader*)$self, status);
	}
	ReturnCode_t get_subscription_matched_status(SubscriptionMatchedStatus *status)
	{
		return $self->get_subscription_matched_status((DataReader*)$self, status);
	}
	ReturnCode_t get_sample_lost_status(SampleLostStatus *status)
	{
		return $self->get_sample_lost_status((DataReader*)$self, status);
	}
	ReturnCode_t wait_for_historical_data(Duration_t max_wait)
	{
		return $self->wait_for_historical_data((DataReader*)$self, max_wait);
	}
	ReturnCode_t get_matched_publications(InstanceHandleSeq *publication_handles)
	{
		return $self->get_matched_publications((DataReader*)$self, publication_handles);
	}
	ReturnCode_t get_matched_publication_data(PublicationBuiltinTopicData *publication_data,InstanceHandle_t publication_handle)
	{
		return $self->get_matched_publication_data((DataReader*)$self, publication_data, publication_handle);
	}
	ReturnCode_t read(FooSeq *data_values, SampleInfoSeq *sample_infos, long max_samples, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
	{
		return $self->read($self, data_values, sample_infos, max_samples, sample_states, view_states, instance_states);
	}
	ReturnCode_t take(FooSeq *data_values, SampleInfoSeq *sample_infos, long max_samples, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
	{
		return $self->take($self, data_values, sample_infos, max_samples, sample_states, view_states, instance_states);
	}
	ReturnCode_t read_w_condition(FooSeq data_values, SampleInfoSeq sample_infos, long max_samples, ReadCondition a_condition)
	{
		return $self->read_w_condition($self, data_values, sample_infos, max_samples, a_condition);
	}
	ReturnCode_t take_w_condition(FooSeq data_values,SampleInfoSeq sample_infos,long max_samples,ReadCondition a_condition)
	{
		return $self->take_w_condition($self, data_values, sample_infos, max_samples, a_condition);
	}
	ReturnCode_t read_next_sample(Foo data_value,SampleInfo sample_info)
	{
		return $self->read_next_sample($self, data_value, sample_info);
	}
	ReturnCode_t take_next_sample(Foo data_value,SampleInfo sample_info)
	{
		return $self->take_next_sample($self, data_value, sample_info);
	}
	ReturnCode_t read_instance(FooSeq *data_values,SampleInfoSeq *sample_infos,long max_samples,InstanceHandle_t a_handle,SampleStateMask sample_states,ViewStateMask view_states, InstanceStateMask instance_states)
	{
		return $self->read_instance($self, data_values, sample_infos, max_samples, a_handle, sample_states, view_states, instance_states);
	}
	ReturnCode_t take_instance(FooSeq *data_values,SampleInfoSeq *sample_infos,long max_samples,InstanceHandle_t a_handle,SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->take_instance($self, data_values, sample_infos, max_samples, a_handle, sample_states, view_states, instance_states);
	}
	ReturnCode_t read_next_instance(FooSeq data_values,SampleInfoSeq sample_infos,long max_samples,InstanceHandle_t previous_handle,SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->read_next_instance($self, data_values, sample_infos, max_samples, previous_handle, sample_states, view_states, instance_states);
	}
	ReturnCode_t take_next_instance(FooSeq data_values,SampleInfoSeq sample_infos,long max_samples,InstanceHandle_t previous_handle,SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->take_next_instance($self, data_values, sample_infos, max_samples, previous_handle, sample_states, view_states, instance_states);
	}
	ReturnCode_t get_key_value(Foo *key_holder, InstanceHandle_t handle)
	{
		return $self->get_key_value($self, key_holder, handle);
	}
	InstanceHandle_t lookup_instance(Foo *key_holder)
	{
		return $self->lookup_instance($self, key_holder);
	}
};

struct DataWriter{
	int32_t		i_entityType;
};


%extend DataWriter{
	ReturnCode_t set_qos(DataWriterQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(DataWriterQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(DataWriterListener *listener,StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	DataWriterListener *get_listener()
	{
		return $self->get_listener($self);
	}
	Topic *get_topic()
	{
		return $self->get_topic($self);
	}
	Publisher *get_publisher()
	{
		return $self->get_publisher($self);
	}
	ReturnCode_t wait_for_acknowledgments(Duration_t max_wait)
	{
		return $self->wait_for_acknowledgments($self, max_wait);
	}
	ReturnCode_t get_liveliness_lost_status(LivelinessLostStatus *status)
	{
		return $self->get_liveliness_lost_status($self, status);
	}
	ReturnCode_t get_offered_deadline_missed_status(OfferedDeadlineMissedStatus *status)
	{
		return $self->get_offered_deadline_missed_status($self, status);
	}
	ReturnCode_t get_offered_incompatible_qos_status(OfferedIncompatibleQosStatus *status)
	{
		return $self->get_offered_incompatible_qos_status($self, status);
	}
	ReturnCode_t get_publication_matched_status(PublicationMatchedStatus *status)
	{
		return $self->get_publication_matched_status($self, status);
	}
	ReturnCode_t assert_liveliness()
	{
		return $self->assert_liveliness($self);
	}
	ReturnCode_t get_matched_subscriptions(InstanceHandleSeq *subscription_handles)
	{
		return $self->get_matched_subscriptions($self, subscription_handles);
	}
	ReturnCode_t get_matched_subscription_data(SubscriptionBuiltinTopicData *subscription_data,InstanceHandle_t subscription_handle)
	{
		return $self->get_matched_subscription_data($self, subscription_data, subscription_handle);
	}
};

struct FooDataWriter{
	int32_t		i_entityType;
};


%extend FooDataWriter{
	ReturnCode_t set_qos(DataWriterQos *qos)
	{
		return $self->set_qos((DataWriter *)$self, qos);
	}
	ReturnCode_t get_qos(DataWriterQos *qos)
	{
		return $self->get_qos((DataWriter *)$self, qos);
	}
	ReturnCode_t set_listener(DataWriterListener *listener,StatusMask mask)
	{
		return $self->set_listener((DataWriter *)$self, listener, mask);
	}
	DataWriterListener *get_listener()
	{
		return $self->get_listener((DataWriter *)$self);
	}
	Topic *get_topic()
	{
		return $self->get_topic((DataWriter *)$self);
	}
	Publisher *get_publisher()
	{
		return $self->get_publisher((DataWriter *)$self);
	}
	ReturnCode_t wait_for_acknowledgments(Duration_t max_wait)
	{
		return $self->wait_for_acknowledgments((DataWriter *)$self, max_wait);
	}
	ReturnCode_t get_liveliness_lost_status(LivelinessLostStatus *status)
	{
		return $self->get_liveliness_lost_status((DataWriter *)$self, status);
	}
	ReturnCode_t get_offered_deadline_missed_status(OfferedDeadlineMissedStatus *status)
	{
		return $self->get_offered_deadline_missed_status((DataWriter *)$self, status);
	}
	ReturnCode_t get_offered_incompatible_qos_status(OfferedIncompatibleQosStatus *status)
	{
		return $self->get_offered_incompatible_qos_status((DataWriter *)$self, status);
	}
	ReturnCode_t get_publication_matched_status(PublicationMatchedStatus *status)
	{
		return $self->get_publication_matched_status((DataWriter *)$self, status);
	}
	ReturnCode_t assert_liveliness()
	{
		return $self->assert_liveliness((DataWriter *)$self);
	}
	ReturnCode_t get_matched_subscriptions(InstanceHandleSeq *subscription_handles)
	{
		return $self->get_matched_subscriptions((DataWriter *)$self, subscription_handles);
	}
	ReturnCode_t get_matched_subscription_data(SubscriptionBuiltinTopicData *subscription_data,InstanceHandle_t subscription_handle)
	{
		return $self->get_matched_subscription_data((DataWriter *)$self, subscription_data, subscription_handle);
	}

	InstanceHandle_t register_instance(Foo *p_instance_data)
	{
		return $self->register_instance($self, p_instance_data);
	}
	InstanceHandle_t register_instance_w_timestamp(Foo *instance_data,Time_t source_timestamp)
	{
		return $self->register_instance_w_timestamp($self, instance_data, source_timestamp);
	}
	ReturnCode_t unregister_instance(Foo *instance_data, InstanceHandle_t handle)
	{
		return $self->unregister_instance($self, instance_data, handle);
	}
	ReturnCode_t unregister_instance_w_timestamp(Foo *instance_data, InstanceHandle_t handle, Time_t source_timestamp)
	{
		return $self->unregister_instance_w_timestamp($self, instance_data, handle, source_timestamp);
	}
	ReturnCode_t write(Foo *instance_data, InstanceHandle_t handle)
	{
		return $self->write($self, instance_data, handle);
	}
	ReturnCode_t write_w_timestamp(Foo *instance_data, InstanceHandle_t handle, Time_t source_timestamp)
	{
		return $self->write_w_timestamp($self, instance_data, handle, source_timestamp);
	}
	ReturnCode_t dispose(Foo *instance_data, InstanceHandle_t instance_handle)
	{
		return $self->dispose($self, instance_data, instance_handle);
	}
	ReturnCode_t dispose_w_timestamp(Foo *instance_data, InstanceHandle_t instance_handle, Time_t source_timestamp)
	{
		return $self->dispose_w_timestamp($self, instance_data, instance_handle, source_timestamp);
	}
	ReturnCode_t get_key_value(Foo *key_holder, InstanceHandle_t handle)
	{
		return $self->get_key_value($self, key_holder, handle);
	}
	InstanceHandle_t lookup_instance(Foo *key_holder)
	{
		return $self->lookup_instance($self, key_holder);
	}
};


struct Publisher {
	int32_t		i_entityType;
	PublisherQos	publisherQos;
};

%extend Publisher{
	DataWriter *create_datawriter(Topic *topic,DataWriterQos *qos,DataWriterListener *listener,StatusMask mask)
	{
		return $self->create_datawriter($self, topic, qos, listener, mask);
	}
	FooDataWriter *create_foodatawriter(Topic *topic,DataWriterQos *qos,DataWriterListener *listener,StatusMask mask)
	{
		return (FooDataWriter *)$self->create_datawriter($self, topic, qos, listener, mask);
	}
	ReturnCode_t delete_datawriter(DataWriter *datawriter)
	{
		return $self->delete_datawriter($self, datawriter);
	}
	DataWriter *lookup_datawriter(char* topic_name)
	{
		return $self->lookup_datawriter($self, topic_name);
	}
	ReturnCode_t delete_contained_entities()
	{
		return $self->delete_contained_entities($self);
	}
	ReturnCode_t set_qos(PublisherQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(PublisherQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(PublisherListener *listener,StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	PublisherListener *get_listener()
	{
		return $self->get_listener($self);
	}
	ReturnCode_t suspend_publications()
	{
		return $self->suspend_publications($self);
	}
	ReturnCode_t resume_publications()
	{
		return $self->resume_publications($self);
	}
	ReturnCode_t begin_coherent_changes()
	{
		return $self->begin_coherent_changes($self);
	}
	ReturnCode_t end_coherent_changes()
	{
		return $self->end_coherent_changes($self);
	}
	ReturnCode_t wait_for_acknowledgments(Duration_t max_wait)
	{
		return $self->wait_for_acknowledgments($self, max_wait);
	}
	DomainParticipant *get_participant()
	{
		return $self->get_participant($self);
	}
	ReturnCode_t set_default_datawriter_qos(DataWriterQos *qos)
	{
		return $self->set_default_datawriter_qos(qos);
	}
	ReturnCode_t get_default_datawriter_qos(DataWriterQos *qos)
	{
		return $self->get_default_datawriter_qos(qos);
	}
	ReturnCode_t copy_from_topic_qos(DataWriterQos *datawriter_qos,TopicQos *topic_qos)
	{
		return $self->copy_from_topic_qos(datawriter_qos, topic_qos);
	}
};

struct Subscriber {
	int32_t		i_entityType;
};

%extend Subscriber{
	DataReader *create_datareader(Topic *topic,DataReaderQos *qos,DataReaderListener *listener,StatusMask mask)
	{
		return $self->create_datareader($self, topic, qos, listener, mask);
	}
	FooDataReader *create_foodatareader(Topic *topic,DataReaderQos *qos,DataReaderListener *listener,StatusMask mask)
	{
		return (FooDataReader *)$self->create_datareader($self, topic, qos, listener, mask);
	}
	ReturnCode_t delete_datareader(DataReader *datareader)
	{
		return $self->delete_datareader($self, datareader);
	}
	ReturnCode_t delete_contained_entities()
	{
		return $self->delete_contained_entities($self);
	}
	DataReader *lookup_datareader(char* topic_name)
	{
		return $self->lookup_datareader($self, topic_name);
	}
	ReturnCode_t get_datareaders(DataReaderSeq *readers,SampleStateMask sample_states,ViewStateMask view_states,InstanceStateMask instance_states)
	{
		return $self->get_datareaders($self, readers, sample_states, view_states, instance_states);
	}
	ReturnCode_t notify_datareaders()
	{
		return $self->notify_datareaders($self);
	}
	ReturnCode_t set_qos(SubscriberQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(SubscriberQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(SubscriberListener *listener,StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	SubscriberListener *get_listener()
	{
		return $self->get_listener($self);
	}
	ReturnCode_t begin_access()
	{
		return $self->begin_access($self);
	}
	ReturnCode_t end_access()
	{
		return $self->end_access($self);
	}
	DomainParticipant *get_participant()
	{
		return $self->get_participant($self);
	}
	ReturnCode_t set_default_datareader_qos(DataReaderQos *qos)
	{
		return $self->set_default_datareader_qos(qos);
	}
	ReturnCode_t get_default_datareader_qos(DataReaderQos *qos)
	{
		return $self->get_default_datareader_qos(qos);
	}
	ReturnCode_t copy_from_topic_qos(DataReaderQos *datareader_qos,TopicQos *topic_qos)
	{
		return $self->copy_from_topic_qos(datareader_qos, topic_qos);
	}
};


typedef enum TOPIC_TYPE{
	DEFAULT_TOPIC_TYPE,
	CONTENTFILTER_TOPIC_TYPE,
	MULTITOPIC_TYPE
}TOPIC_TYPE;

struct Topic {
	int32_t				i_entityType;
	char*				topic_name;
	char*				type_name;
	DomainParticipant		*p_domainParticipant;
	TOPIC_TYPE			topic_type;
};


%extend Topic{
	char* get_type_name()
	{
		return $self->get_type_name($self);
	}
	char* get_name()
	{
		return $self->get_name($self);
	}
	DomainParticipant *get_participant(Topic topic)
	{
		return $self->get_participant($self);
	}
	ReturnCode_t set_qos(TopicQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(TopicQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(TopicListener *listener, StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	TopicListener *get_listener()
	{
		return $self->get_listener($self);
	}
	ReturnCode_t get_inconsistent_topic_status(InconsistentTopicStatus *a_status)
	{
		return $self->get_inconsistent_topic_status($self, a_status);
	}
};

struct DomainParticipant {
	int32_t		i_entityType;

};

%extend DomainParticipant{
	Publisher *create_publisher(PublisherQos *qos,PublisherListener *listener, StatusMask mask)
	{
		return $self->create_publisher($self, qos, listener, mask);
	}

	ReturnCode_t delete_publisher(Publisher *publisher)
	{
		return $self->delete_publisher($self, publisher);
	}
	Subscriber *create_subscriber(SubscriberQos *qos,SubscriberListener *listener,StatusMask mask)
	{
		return $self->create_subscriber($self, qos, listener, mask);
	}
	ReturnCode_t delete_subscriber(Subscriber *subscriber)
	{
		return $self->delete_subscriber($self, subscriber);
	}
	Subscriber *get_builtin_subscriber()
	{
		return $self->get_builtin_subscriber($self);
	}
	Topic *create_topic(char* topic_name,char* type_name,TopicQos *qos,TopicListener *listener, StatusMask mask)
	{
		return $self->create_topic($self, topic_name, type_name, qos, listener, mask);
	}
	ReturnCode_t delete_topic(Topic *topic)
	{
		return $self->delete_topic($self, topic);
	}
	Topic *find_topic(char* topic_name,Duration_t timeout)
	{
		return $self->find_topic($self, topic_name, timeout);
	}
	TopicDescription *lookup_topicdescription(char* name)
	{
		return $self->lookup_topicdescription($self, name);
	}
	ContentFilteredTopic *create_contentfilteredtopic(char* name,Topic *related_topic,char *filter_expression,StringSeq expression_parameters)
	{
		return $self->create_contentfilteredtopic($self, name, related_topic, filter_expression, expression_parameters);
	}
	ReturnCode_t delete_contentfilteredtopic(ContentFilteredTopic *contentfilteredtopic)
	{
		return $self->delete_contentfilteredtopic($self, contentfilteredtopic);
	}
	MultiTopic *create_multitopic(char *name,char* type_name,char* subscription_expression,StringSeq expression_parameters)
	{
		return $self->create_multitopic($self, name, type_name, subscription_expression, expression_parameters);
	}
	ReturnCode_t delete_multitopic(MultiTopic *multitopic)
	{
		return $self->delete_multitopic($self, multitopic);
	}
	ReturnCode_t delete_contained_entities()
	{
		return $self->delete_contained_entities($self);
	}
	ReturnCode_t set_qos(DomainParticipantQos *qos)
	{
		return $self->set_qos($self, qos);
	}
	ReturnCode_t get_qos(DomainParticipantQos *qos)
	{
		return $self->get_qos($self, qos);
	}
	ReturnCode_t set_listener(DomainParticipantListener *listener,StatusMask mask)
	{
		return $self->set_listener($self, listener, mask);
	}
	DomainParticipantListener *get_listener()
	{
		return $self->get_listener($self);
	}
	ReturnCode_t ignore_participant(InstanceHandle_t handle)
	{
		return $self->ignore_participant($self, handle);
	}
	ReturnCode_t ignore_topic(InstanceHandle_t handle)
	{
		return $self->ignore_topic($self, handle);
	}
	ReturnCode_t ignore_publication(InstanceHandle_t handle)
	{
		return $self->ignore_publication($self, handle);
	}
	ReturnCode_t ignore_subscription(InstanceHandle_t handle)
	{
		return $self->ignore_subscription($self, handle);
	}
	DomainId_t get_domain_id()
	{
		return $self->get_domain_id($self);
	}
	ReturnCode_t assert_liveliness()
	{
		return $self->assert_liveliness($self);
	}
	ReturnCode_t set_default_publisher_qos(PublisherQos *qos)
	{
		return $self->set_default_publisher_qos(qos);
	}
	ReturnCode_t get_default_publisher_qos(PublisherQos *qos)
	{
		return $self->get_default_publisher_qos(qos);
	}
	ReturnCode_t set_default_subscriber_qos(SubscriberQos *qos)
	{
		return $self->set_default_subscriber_qos(qos);
	}
	ReturnCode_t get_default_subscriber_qos(SubscriberQos *qos)
	{
		return $self->get_default_subscriber_qos(qos);
	}
	ReturnCode_t set_default_topic_qos(TopicQos *qos)
	{
		return $self->set_default_topic_qos(qos);
	}
	ReturnCode_t get_default_topic_qos(TopicQos *qos)
	{
		return $self->get_default_topic_qos(qos);
	}

	ReturnCode_t get_discovered_participants(InstanceHandleSeq *participant_handles)
	{
		return $self->get_discovered_participants($self, participant_handles);
	}
	ReturnCode_t get_discovered_participant_data(ParticipantBuiltinTopicData *participant_data,InstanceHandle_t participant_handle)
	{
		return $self->get_discovered_participant_data($self, participant_data, participant_handle);
	}
	ReturnCode_t get_discovered_topics(InstanceHandleSeq *topic_handles)
	{
		return $self->get_discovered_topics($self, topic_handles);
	}
	ReturnCode_t get_discovered_topic_data(TopicBuiltinTopicData *topic_data,InstanceHandle_t topic_handle)
	{
		return $self->get_discovered_topic_data($self, topic_data, topic_handle);
	}
	bool contains_entity(InstanceHandle_t a_handle)
	{
		return $self->contains_entity($self, a_handle);
	}
	ReturnCode_t get_current_time(Time_t *current_time)
	{
		return $self->get_current_time($self, current_time);
	}
};

struct DomainParticipantFactory {

	//이부분은 추가 부분...
	int32_t		module_id;
};

%extend DomainParticipantFactory{
	DomainParticipantFactory *DomainParticipantFactory_get_instance(){
		return DomainParticipantFactory_get_instance();
	}
	DomainParticipant *create_participant(DomainId_t domain_id,DomainParticipantQos *qos,DomainParticipantListener *listener,StatusMask mask)
	{
		return $self->create_participant(domain_id, qos, listener, mask);
	}
	ReturnCode_t delete_participant(DomainParticipant *participant){
		return $self->delete_participant(participant);
	}
	DomainParticipant *lookup_participant(DomainId_t domain_id)
	{
		return $self->lookup_participant(domain_id);
	}
	ReturnCode_t set_default_participant_qos(DomainParticipantQos *qos)
	{
		return $self->set_default_participant_qos(qos);
	}
	ReturnCode_t get_default_participant_qos(DomainParticipantQos *qos)
	{
		return $self->get_default_participant_qos(qos);
	}
	ReturnCode_t set_qos(DomainParticipantFactoryQos *qos)
	{
		return $self->set_qos(qos);
	}
	ReturnCode_t get_qos(DomainParticipantFactoryQos *qos)
	{
		return $self->get_qos(qos);
	}
};


/////////////////


typedef uint32_t StatusKind;
typedef uint32_t StatusMask;


%include "typemaps.i"
%include "arrays_java.i"
%apply int[] {int *};

%include "core.h"

///////////////////////////////////////////////////////







%include "cpsdcps.h"
%include "dcps_func.h"
%include "module_common.h"
%include "rtps_message.h"
%include "rtps.h"
%include "util.h"





%feature("director");

%inline %{

%}



