/*
	이 파일은 OMG-DDS-07-0101.pdf 파일을 참고하여, 만든 헤더 파일이며,
	문서의 7.2.3 DCPS PSM : IDL에 나와 있는 내용을 바탕으로 작성되었다.

	작성자 : 

	이력...
	2010-07-22 : 시작
	2010-07-23 : FooData 이전까지 선언 완료.

*/

#if !defined(__CPSDCPS_H)
#define __CPSDCPS_H


typedef enum BUILTIN_TYPE
{
	NORMAL_TYPE = 0,
	BUILTIN_PARTICIPANT_TYPE,
	BUILTIN_PUBLICATION_TYPE,
	BUILTIN_SUBSCRIPTION_TYPE,
	BUILTIN_TOPIC_TYPE,
	BUILTIN_LIVELINESSP2P_TYPE,

}BUILTIN_TYPE;

static Duration_t TIME_ZERO = {0, 0};
static Duration_t TIME_INVALID = {-1, 0xffffffff};
static Duration_t TIME_INFINITE = {0x7fffffff, 0xffffffff};

/////////////////////////////////////////////////////////////////////
#define in_dds
#define inout_dds

#ifndef __cplusplus
	/*
	typedef int SampleRejectedStatusKind;
	typedef int DurabilityQosPolicyKind;
	typedef int PresentationQosPolicyAccessScopeKind;
	typedef int OwnershipQosPolicyKind;
	typedef int LivelinessQosPolicyKind;
	typedef int ReliabilityQosPolicyKind;
	typedef int DestinationOrderQosPolicyKind;
	typedef int HistoryQosPolicyKind;
	*/
#endif

typedef  char* string;


#ifndef TIME_STRUCT_T
        #define TIME_STRUCT_T
        typedef struct Time_t Time_t;
        struct Time_t {
                long sec;
                unsigned long nanosec;
        };
        typedef struct Duration_t {
                long sec;
                unsigned long nanosec;
        }Duration_t;
#endif

typedef struct BuiltinTopicKey_t BuiltinTopicKey_t;
typedef struct QosPolicyCount QosPolicyCount;

#ifndef ENTITY_DEFINE
	#define ENTITY_DEFINE 1
	typedef struct Entity Entity;
#endif
#ifndef TOPIC_DEFINE
	#define TOPIC_DEFINE 1
	typedef struct Topic Topic;
#endif

#ifndef PUBLISHER_DEFINE
#	define PUBLISHER_DEFINE 1
	typedef struct Publisher Publisher;
#endif

#ifndef DOMAINPARTICIPANT_DEFINE
        #define DOMAINPARTICIPANT_DEFINE 1
        typedef struct DomainParticipant DomainParticipant;
#endif
typedef struct DomainParticipantFactory DomainParticipantFactory;
#ifndef DATAREADER_DEFINE
        #define DATAREADER_DEFINE 1
        typedef struct DataReader DataReader;
#endif
#ifndef DATAWRITER_DEFINE
	#define DATAWRITER_DEFINE 1
	typedef struct DataWriter DataWriter;
#endif

#ifndef SUBSCRIBER_DEFINE
#	define SUBSCRIBER_DEFINE 1
	typedef struct Subscriber Subscriber;
#endif

typedef struct TopicDescription TopicDescription;
typedef struct ContentFilteredTopic ContentFilteredTopic;
typedef struct MultiTopic MultiTopic;
typedef struct ParticipantBuiltinTopicData ParticipantBuiltinTopicData;
typedef struct TopicBuiltinTopicData TopicBuiltinTopicData;
typedef struct SubscriptionBuiltinTopicData SubscriptionBuiltinTopicData;
typedef struct PublicationBuiltinTopicData PublicationBuiltinTopicData;

#ifndef SAMPLEINFO_DEFINE
	typedef struct SampleInfo SampleInfo;
#	define SAMPLEINFO_DEFINE 1
#endif

typedef struct Foo Foo;

typedef struct InconsistentTopicStatus InconsistentTopicStatus;
typedef struct LivelinessChangedStatus LivelinessChangedStatus;
typedef struct LivelinessLostStatus LivelinessLostStatus;
typedef struct OfferedDeadlineMissedStatus OfferedDeadlineMissedStatus;
typedef struct OfferedIncompatibleQosStatus OfferedIncompatibleQosStatus;
typedef struct PublicationMatchedStatus PublicationMatchedStatus;
typedef struct RequestedDeadlineMissedStatus RequestedDeadlineMissedStatus;
typedef struct RequestedIncompatibleQosStatus RequestedIncompatibleQosStatus;
typedef struct SampleRejectedStatus SampleRejectedStatus;
typedef struct SubscriptionMatchedStatus SubscriptionMatchedStatus;
typedef struct SampleLostStatus	SampleLostStatus;

typedef struct Condition Condition;
//waitset by jun
//by jun
typedef struct GuardCondition GuardCondition;
typedef struct StatusCondition StatusCondition;
typedef struct ReadCondition ReadCondition;
typedef struct QueryCondition QueryCondition;

#ifndef WAITSET_DEFINE
	#define WAITSET_DEFINE 1
	typedef struct WaitSet WaitSet;
#endif

typedef struct EntityFactoryQosPolicy EntityFactoryQosPolicy;
typedef struct UserDataQosPolicy UserDataQosPolicy;
typedef struct TopicDataQosPolicy TopicDataQosPolicy;
typedef struct GroupDataQosPolicy GroupDataQosPolicy;
typedef struct TransportPriorityQosPolicy TransportPriorityQosPolicy;
typedef struct LifespanQosPolicy LifespanQosPolicy;
typedef struct DurabilityQosPolicy DurabilityQosPolicy;
typedef struct ReaderDataLifecycleQosPolicy ReaderDataLifecycleQosPolicy;
typedef struct DurabilityServiceQosPolicy DurabilityServiceQosPolicy;
typedef struct DeadlineQosPolicy DeadlineQosPolicy;
typedef struct LatencyBudgetQosPolicy LatencyBudgetQosPolicy;
typedef struct LivelinessQosPolicy LivelinessQosPolicy;
typedef struct ReliabilityQosPolicy ReliabilityQosPolicy;
typedef struct DestinationOrderQosPolicy DestinationOrderQosPolicy;
typedef struct HistoryQosPolicy HistoryQosPolicy;
typedef struct ResourceLimitsQosPolicy ResourceLimitsQosPolicy;
typedef struct OwnershipQosPolicy OwnershipQosPolicy;
typedef struct OwnershipStrengthQosPolicy OwnershipStrengthQosPolicy;
typedef struct WriterDataLifecycleQosPolicy WriterDataLifecycleQosPolicy;
typedef struct PresentationQosPolicy PresentationQosPolicy;
typedef struct PartitionQosPolicy PartitionQosPolicy;
typedef struct TimeBasedFilterQosPolicy TimeBasedFilterQosPolicy;


typedef struct PublisherQos PublisherQos;
typedef struct SubscriberQos SubscriberQos;
typedef struct TopicQos TopicQos;
typedef struct DomainParticipantQos DomainParticipantQos;
typedef struct DomainParticipantFactoryQos DomainParticipantFactoryQos;
typedef struct DataWriterQos DataWriterQos;
typedef struct DataReaderQos DataReaderQos;

//added by kyy
typedef struct RxOQos RxOQos;

typedef struct SubscriberListener SubscriberListener;
typedef struct TopicListener TopicListener;
typedef struct DomainParticipantListener DomainParticipantListener;
typedef struct DataWriterListener DataWriterListener;
typedef struct DataReaderListener DataReaderListener;

//by jun
typedef struct TopicStatus TopicStatus;
typedef struct DataWriterStatus DataWriterStatus;
typedef struct DataReaderStatus DataReaderStatus;

typedef struct SubscriberListenerStatusChangedFlag SubscriberListenerStatusChangedFlag;
typedef struct TopicListenerStatusChangedFlag TopicListenerStatusChangedFlag;
typedef struct DataWriterListenerStatusChangedFlag DataWriterListenerStatusChangedFlag;
typedef struct DataReaderListenerStatusChangedFlag DataReaderListenerStatusChangedFlag;

/////////////////// DCPS PSM : IDL  //////////////////////////////////

#define DOMAINID_TYPE_NATIVE int32_t
#ifndef HANDLE_TYPE_NATIVE
#ifdef _M_IX86
#	define HANDLE_TYPE_NATIVE int32_t
#else
#	define HANDLE_TYPE_NATIVE int64_t
#endif
#endif
#define HANDLE_NIL_NATIVE 0
#define BUILTIN_TOPIC_KEY_TYPE_NATIVE int32_t
#define TheParticipantFactory
#define PARTICIPANT_QOS_DEFAULT
#define TOPIC_QOS_DEFAULT
#define PUBLISHER_QOS_DEFAULT
#define SUBSCRIBER_QOS_DEFAULT
#define DATAWRITER_QOS_DEFAULT
#define DATAREADER_QOS_DEFAULT
#define DATAWRITER_QOS_USE_TOPIC_QOS
#define DATAREADER_QOS_USE_TOPIC_QOS

//////////////////  modules DDS ////////////////////////////////////

typedef DOMAINID_TYPE_NATIVE DomainId_t;

#ifndef HANDLE_TYPE_NATIVE_DEFINE
	#define HANDLE_TYPE_NATIVE_DEFINE 1	
	typedef HANDLE_TYPE_NATIVE InstanceHandle_t;
#endif

struct BuiltinTopicKey_t
{
	//BUILTIN_TOPIC_KEY_TYPE_NATIVE value[3];
	BUILTIN_TOPIC_KEY_TYPE_NATIVE value[4];
};

//typedef sequence<InstanceHandle_t> InstanceHandleSeq;
//typedef InstanceHandle_t	 *InstanceHandleSeq;

typedef struct InstanceHandleSeq
{
	int32_t i_seq;
	InstanceHandle_t **pp_instancehandle;
} InstanceHandleSeq;

static InstanceHandleSeq INIT_INSTANCESEQ = {0,NULL};

typedef enum ReturnCode_t ReturnCode_t;
typedef int32_t QosPolicyId_t;

//typedef sequence<string> StringSeq;
typedef struct StringSeq StringSeq;


struct StringSeq
{
	int32_t i_string;
	string* pp_string;
};


#ifndef TIME_STRUCT_T

	#define TIME_STRUCT_T
	struct Time_t {
		long sec;
		unsigned long nanosec;
	};
	struct Duration_t {
		long sec;
		unsigned long nanosec;
	};
#endif


// ----------------------------------------------------------------------
// Pre-defined values
// ----------------------------------------------------------------------
static const InstanceHandle_t HANDLE_NIL = HANDLE_NIL_NATIVE;
static const int32_t LENGTH_UNLIMITED = -1;
static const int32_t DURATION_INFINITE_SEC = 0x7fffffff;
static const uint32_t DURATION_INFINITE_NSEC = 0x7fffffff;
static const int32_t DURATION_ZERO_SEC = 0;
static const uint32_t DURATION_ZERO_NSEC = 0;
static const int32_t TIME_INVALID_SEC = -1;
static const uint32_t TIME_INVALID_NSEC = 0xffffffff;

// ----------------------------------------------------------------------
// Return codes
// ----------------------------------------------------------------------
#ifndef RETURN_CODE_T_DEFINE
	#define RETURN_CODE_T_DEFINE 1
	typedef enum ReturnCode_t
	{

		RETCODE_OK = 0,
		RETCODE_ERROR = 1,
		RETCODE_UNSUPPORTED = 2,
		RETCODE_BAD_PARAMETER = 3,
		RETCODE_PRECONDITION_NOT_MET = 4,
		RETCODE_OUT_OF_RESOURCES = 5,
		RETCODE_NOT_ENABLED = 6,
		RETCODE_IMMUTABLE_POLICY = 7,
		RETCODE_INCONSISTENT_POLICY = 8,
		RETCODE_ALREADY_DELETED = 9,
		RETCODE_TIMEOUT = 10,
		RETCODE_NO_DATA = 11,
		RETCODE_ILLEGAL_OPERATION = 12
	}ReturnCode_t;
#endif

// ----------------------------------------------------------------------
// Status to support listeners and conditions
// ----------------------------------------------------------------------

//typedef uint32_t StatusKind;
typedef uint32_t StatusMask; // bit-mask StatusKind
typedef enum StatusKind
{
	INCONSISTENT_TOPIC_STATUS = 0x0001 << 0,
	OFFERED_DEADLINE_MISSED_STATUS = 0x0001 << 1,
	REQUESTED_DEADLINE_MISSED_STATUS = 0x0001 << 2,
	OFFERED_INCOMPATIBLE_QOS_STATUS = 0x0001 << 5,
	REQUESTED_INCOMPATIBLE_QOS_STATUS = 0x0001 << 6,
	SAMPLE_LOST_STATUS = 0x0001 << 7,
	SAMPLE_REJECTED_STATUS = 0x0001 << 8,
	DATA_ON_READERS_STATUS = 0x0001 << 9,
	DATA_AVAILABLE_STATUS = 0x0001 << 10,
	LIVELINESS_LOST_STATUS = 0x0001 << 11,
	LIVELINESS_CHANGED_STATUS = 0x0001 << 12,
	PUBLICATION_MATCHED_STATUS = 0x0001 << 13,
	SUBSCRIPTION_MATCHED_STATUS = 0x0001 << 14,
	ALL_ENABLE_STATUS = 0xFFFF
}StatusKind;

struct InconsistentTopicStatus
{
	int32_t total_count;
	int32_t total_count_change;
};

struct SampleLostStatus
{
	int32_t total_count;
	int32_t total_count_change;
};

typedef enum SampleRejectedStatusKind
{
	NOT_REJECTED,
	REJECTED_BY_INSTANCES_LIMIT,
	REJECTED_BY_SAMPLES_LIMIT,
	REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT
}SampleRejectedStatusKind;

struct SampleRejectedStatus
{
	int32_t total_count;
	int32_t total_count_change;
	SampleRejectedStatusKind last_reason;
	InstanceHandle_t last_instance_handle;
};

struct LivelinessLostStatus
{
	int32_t total_count;
	int32_t total_count_change;
};


struct LivelinessChangedStatus
{
	int32_t alive_count;
	int32_t not_alive_count;
	int32_t alive_count_change;
	int32_t not_alive_count_change;
	InstanceHandle_t last_publication_handle;
};

struct OfferedDeadlineMissedStatus
{
	int32_t total_count;
	int32_t total_count_change;
	InstanceHandle_t last_instance_handle;
};

struct RequestedDeadlineMissedStatus
{
	int32_t total_count;
	int32_t total_count_change;
	InstanceHandle_t last_instance_handle;
};

struct QosPolicyCount
{
	QosPolicyId_t policy_id;
	int32_t count;
};

//typedef sequence<QosPolicyCount> QosPolicyCountSeq;
typedef QosPolicyCount* QosPolicyCountSeq;

struct OfferedIncompatibleQosStatus
{
	int32_t total_count;
	int32_t total_count_change;
	QosPolicyId_t last_policy_id;
	QosPolicyCountSeq policies;
};

struct RequestedIncompatibleQosStatus
{
	int32_t total_count;
	int32_t total_count_change;
	QosPolicyId_t last_policy_id;
	QosPolicyCountSeq policies;
};

struct PublicationMatchedStatus
{
	int32_t total_count;
	int32_t total_count_change;
	int32_t current_count;
	int32_t current_count_change;
	InstanceHandle_t last_subscription_handle;
};


struct SubscriptionMatchedStatus
{
	int32_t total_count;
	int32_t total_count_change;
	int32_t current_count;
	int32_t current_count_change;
	InstanceHandle_t last_publication_handle;
};

// ----------------------------------------------------------------------
// Listeners
// ----------------------------------------------------------------------

//interface Listener;
//interface Entity;
//interface TopicDescription;
//interface Topic;
//interface ContentFilteredTopic;
//interface MultiTopic;
//interface DataWriter;
//interface DataReader;
//interface Subscriber;
//interface Publisher;

//typedef sequence<DataReader> DataReaderSeq;

typedef struct DataReaderSeq
{
	int i_seq;
	DataReader** pp_datareaders;
} DataReaderSeq;



//interface Listener {};

#define LISTENER_BODY \
	int	not_used;

struct Listener
{
	LISTENER_BODY
};


//interface TopicListener : Listener {
//	void on_inconsistent_topic(in_dds Topic the_topic,
//	in_dds InconsistentTopicStatus status);
//};

#define TOPICLISTENER_BODY \
	void (*on_inconsistent_topic)(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status);

struct TopicListener
{
	LISTENER_BODY
	TOPICLISTENER_BODY
};

//by jun

struct TopicStatus
{
	InconsistentTopicStatus inconsistent_topic;
};

struct TopicListenerStatusChangedFlag
{
	bool b_inconsistent_topic_status_changed_flag;
};

//interface DataWriterListener : Listener {
//	void on_offered_deadline_missed(in_dds DataWriter writer, in_dds OfferedDeadlineMissedStatus status);
//	void on_offered_incompatible_qos(in_dds DataWriter writer,in_dds OfferedIncompatibleQosStatus status);
//	void on_liveliness_lost(in_dds DataWriter writer,in_dds LivelinessLostStatus status);
//	void on_publication_matched(in_dds DataWriter writer,in_dds PublicationMatchedStatus status);
//};

#define DATAWRITERLISTENER_BODY\
	void (*on_offered_deadline_missed)(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status);\
	void (*on_offered_incompatible_qos)(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status);\
	void (*on_liveliness_lost)(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status);\
	void (*on_publication_matched)(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status);

struct DataWriterListener
{
	LISTENER_BODY
	DATAWRITERLISTENER_BODY
};

//by jun

struct DataWriterStatus
{
	OfferedDeadlineMissedStatus offered_deadline_missed;
	OfferedIncompatibleQosStatus offered_incompatible_qos;
	LivelinessLostStatus liveliness_lost;
	PublicationMatchedStatus publication_matched;

};
struct DataWriterListenerStatusChangedFlag
{
	bool b_liveliness_lost_status_changed_flag;\
	bool b_offered_deadline_missed_status_changed_flag;\
	bool b_offered_incompatible_qos_status_changed_flag;\
	bool b_publication_matched_status_changed_flag;\
};

//interface PublisherListener : DataWriterListener {
//};

typedef struct DataWriterListener PublisherListener;

//interface DataReaderListener : Listener {
//	void on_requested_deadline_missed(in_dds DataReader the_reader,in_dds RequestedDeadlineMissedStatus status);
//	void on_requested_incompatible_qos(in_dds DataReader the_reader,in_dds RequestedIncompatibleQosStatus status);
//	void on_sample_rejected(in_dds DataReader the_reader,in_dds SampleRejectedStatus status);
//	void on_liveliness_changed(in_dds DataReader the_reader,in_dds LivelinessChangedStatus status);
//	void on_data_available(in_dds DataReader the_reader);
//	void on_subscription_matched(in_dds DataReader the_reader,in_dds SubscriptionMatchedStatus status);
//	void on_sample_lost(in_dds DataReader the_reader,in_dds SampleLostStatus status);
//};

#define DATAREADERLISTENER_BODY \
	void (*on_requested_deadline_missed)(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status);\
	void (*on_requested_incompatible_qos)(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status);\
	void (*on_sample_rejected)(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status);\
	void (*on_liveliness_changed)(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status);\
	void (*on_data_available)(in_dds DataReader *p_reader);\
	void (*on_subscription_matched)(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status);\
	void (*on_sample_lost)(in_dds DataReader *the_reader,in_dds SampleLostStatus *status);

typedef void (*CALLBACK_FUNC)(void*);

struct DataReaderListener
{
	LISTENER_BODY\
	DATAREADERLISTENER_BODY
	CALLBACK_FUNC m_CallBack;
};

//by jun
struct DataReaderStatus
{
	RequestedDeadlineMissedStatus requested_deadline_missed;
	RequestedIncompatibleQosStatus requested_incompatible_qos;
	SampleRejectedStatus sample_rejected;
	LivelinessChangedStatus liveliness_changed;
	SubscriptionMatchedStatus subscriber_matched;
	SampleLostStatus sample_lost;
};

struct DataReaderListenerStatusChangedFlag
{
	bool	b_sample_rejected_status_changed_flag;\
	bool	b_liveliness_changed_status_changed_flag;\
	bool	b_requested_deadline_missed_status_changed_flag;\
	bool	b_requested_incompatible_qos_status_changed_flag;\
	bool	b_data_available_status_changed_flag;\
	bool	b_sample_lost_status_changed_flag;\
	bool	b_subscription_matched_status_changed_flag;\
};

//interface SubscriberListener : DataReaderListener {
//	void on_data_on_readers(in_dds Subscriber the_subscriber);
//};

#define SUBSCRIBERLISTENER_BODY \
	DATAREADERLISTENER_BODY\
	void (*on_data_on_readers)(in_dds Subscriber *p_subscriber);\

struct SubscriberListener
{
	LISTENER_BODY\
	SUBSCRIBERLISTENER_BODY
};

//by jun

struct SubscriberListenerStatusChangedFlag
{
	bool b_data_on_readers_status_changed_flag;
};

//interface DomainParticipantListener : TopicListener, PublisherListener, SubscriberListener {
//};

struct DomainParticipantListener
{
	LISTENER_BODY
	TOPICLISTENER_BODY
	DATAWRITERLISTENER_BODY
	SUBSCRIBERLISTENER_BODY
};

// ----------------------------------------------------------------------
// Conditions
// ----------------------------------------------------------------------

//interface Condition {
//	boolean get_trigger_value();
//};

#define CONDITION_BODY\
	bool (*get_trigger_value)(); \
	/*이부분추가*/					\
	bool	b_trigger_value;		\
	uint32_t		condition_type; \
	/*waitset by jun*/					\
	WaitSet	*p_waitset;			
	

struct Condition
{
	CONDITION_BODY
};

//typedef sequence<Condition> ConditionSeq;
typedef struct ConditionSeq
{
	int32_t i_seq;
	Condition** pp_conditions;
} ConditionSeq;

static ConditionSeq INIT_CONDITIONSEQ = {0,NULL};

//interface WaitSet {
//	ReturnCode_t wait(inout_dds ConditionSeq active_conditions,in_dds Duration_t timeout);
//	ReturnCode_t attach_condition(in_dds Condition cond);
//	ReturnCode_t detach_condition(in_dds Condition cond);
//	ReturnCode_t get_conditions(inout_dds ConditionSeq attached_conditions);
//};

struct WaitSet
{
	ReturnCode_t (*wait)(WaitSet* p_waitSet, inout_dds ConditionSeq *p_active_conditions,in_dds Duration_t timeout);
	ReturnCode_t (*attach_condition)(WaitSet* p_waitSet, in_dds Condition *p_cond);
	ReturnCode_t (*detach_condition)(WaitSet* p_waitSet, in_dds Condition *p_cond);
	ReturnCode_t (*get_conditions)(WaitSet* p_waitSet, inout_dds ConditionSeq *p_attached_conditions);
	//
	mutex_t		waitset_lock;
	int32_t		i_conditions;
	Condition** pp_conditions;

	//	waitset by jun
	cond_t	 	waitset_wait;
};


//interface GuardCondition : Condition {
//	ReturnCode_t set_trigger_value(in_dds boolean value);
//};

struct GuardCondition
{
	CONDITION_BODY
	ReturnCode_t (*set_trigger_value)(in_dds bool value);
};


//interface StatusCondition : Condition {
//	StatusMask get_enabled_statuses();
//	ReturnCode_t set_enabled_statuses(in_dds StatusMask mask);
//	Entity get_entity();
//};

struct StatusCondition
{
	CONDITION_BODY
	StatusMask (*get_enabled_statuses)(StatusCondition* p_status_condition);
	ReturnCode_t (*set_enabled_statuses)(StatusCondition* p_status_condition, in_dds StatusMask mask);
	Entity* (*get_entity)(StatusCondition* p_status_condition);
	//
	StatusMask enabled_statuses;
	Entity* p_entity;
};


#ifndef DCPS_STATE_DEFINE
#define DCPS_STATE_DEFINE 1

// Sample states to support reads
typedef uint32_t SampleStateKind;
static const SampleStateKind READ_SAMPLE_STATE = 0x0001 << 0;
static const SampleStateKind NOT_READ_SAMPLE_STATE = 0x0001 << 1;
// This is a bit-mask SampleStateKind
typedef uint32_t SampleStateMask;
static const SampleStateMask ANY_SAMPLE_STATE = 0xffff;
// View states to support reads
typedef uint32_t ViewStateKind;
static const ViewStateKind NEW_VIEW_STATE = 0x0001 << 0;
static const ViewStateKind NOT_NEW_VIEW_STATE = 0x0001 << 1;
// This is a bit-mask ViewStateKind
typedef uint32_t ViewStateMask;
static const ViewStateMask ANY_VIEW_STATE = 0xffff;
// Instance states to support reads
typedef uint32_t InstanceStateKind;
static const InstanceStateKind ALIVE_INSTANCE_STATE = 0x0001 << 0;
static const InstanceStateKind NOT_ALIVE_DISPOSED_INSTANCE_STATE = 0x0001 << 1;
static const InstanceStateKind NOT_ALIVE_NO_WRITERS_INSTANCE_STATE = 0x0001 << 2;
// This is a bit-mask InstanceStateKind
typedef uint32_t InstanceStateMask;
static const InstanceStateMask ANY_INSTANCE_STATE = 0xffff;
static const InstanceStateMask NOT_ALIVE_INSTANCE_STATE = 0x006;

static const StatusMask ANY_STATUS_STATE = 0xffff;

#endif

//interface ReadCondition : Condition {
//	SampleStateMask get_sample_state_mask();
//	ViewStateMask get_view_state_mask();
//	InstanceStateMask get_instance_state_mask();
//	DataReader get_datareader();
//};


#define READCONDITION_BODY\
	CONDITION_BODY\
	SampleStateMask (*get_sample_state_mask)(ReadCondition* p_readcondition);		\
	ViewStateMask (*get_view_state_mask)(ReadCondition* p_readcondition);			\
	InstanceStateMask (*get_instance_state_mask)(ReadCondition* p_readcondition);	\
	DataReader *(*get_datareader)(ReadCondition* p_readcondition);					\
	/*이부분추가*/																	\
	SampleStateMask	l_sampleStateMask;											\
	ViewStateMask	l_viewStateMask;												\
	InstanceStateMask	l_instanceStateMask;										\
	DataReader	*p_datareader;

struct ReadCondition
{
	READCONDITION_BODY
};

//interface QueryCondition : ReadCondition {
//	string get_query_expression();
//	ReturnCode_t get_query_parameters(inout_dds StringSeq query_parameters);
//	ReturnCode_t set_query_parameters(in_dds StringSeq query_parameters);
//};


typedef struct expression_t expression_t;

struct QueryCondition
{
	READCONDITION_BODY
	string (*get_query_expression)(QueryCondition* p_queryCondition);
	ReturnCode_t (*get_query_parameters)(QueryCondition* p_queryCondition, inout_dds StringSeq *p_query_parameters);
	ReturnCode_t (*set_query_parameters)(QueryCondition* p_queryCondition, in_dds StringSeq *p_query_parameters);
	///////
	string query_expression;
	StringSeq query_parameters;
	expression_t *p_expr;
	FooTypeSupport *p_typeSupport;
};


// ----------------------------------------------------------------------
// Qos
// ----------------------------------------------------------------------
static const string USERDATA_QOS_POLICY_NAME = (const string)"UserData";
static const string DURABILITY_QOS_POLICY_NAME = (const string)"Durability";
static const string PRESENTATION_QOS_POLICY_NAME = (const string)"Presentation";
static const string DEADLINE_QOS_POLICY_NAME = (const string)"Deadline";
static const string LATENCYBUDGET_QOS_POLICY_NAME = (const string)"LatencyBudget";
static const string OWNERSHIP_QOS_POLICY_NAME = (const string)"Ownership";
static const string OWNERSHIPSTRENGTH_QOS_POLICY_NAME = (const string)"OwnershipStrength";
static const string LIVELINESS_QOS_POLICY_NAME = (const string)"Liveliness";
static const string TIMEBASEDFILTER_QOS_POLICY_NAME = (const string)"TimeBasedFilter";
static const string PARTITION_QOS_POLICY_NAME = (const string)"Partition";
static const string RELIABILITY_QOS_POLICY_NAME = (const string)"Reliability";
static const string DESTINATIONORDER_QOS_POLICY_NAME = (const string)"DestinationOrder";
static const string HISTORY_QOS_POLICY_NAME = (const string)"History";
static const string RESOURCELIMITS_QOS_POLICY_NAME = (const string)"ResourceLimits";
static const string ENTITYFACTORY_QOS_POLICY_NAME = (const string)"EntityFactory";
static const string WRITERDATALIFECYCLE_QOS_POLICY_NAME = (const string)"WriterDataLifecycle";
static const string READERDATALIFECYCLE_QOS_POLICY_NAME = (const string)"ReaderDataLifecycle";
static const string TOPICDATA_QOS_POLICY_NAME = (const string)"TopicData";
static const string GROUPDATA_QOS_POLICY_NAME = (const string)"TransportPriority";
static const string LIFESPAN_QOS_POLICY_NAME = (const string)"Lifespan";
static const string DURABILITYSERVICE_POLICY_NAME = (const string)"DurabilityService";

static const QosPolicyId_t INVALID_QOS_POLICY_ID = 0;
static const QosPolicyId_t USERDATA_QOS_POLICY_ID = 1;
static const QosPolicyId_t DURABILITY_QOS_POLICY_ID = 2;
static const QosPolicyId_t PRESENTATION_QOS_POLICY_ID = 3;
static const QosPolicyId_t DEADLINE_QOS_POLICY_ID = 4;
static const QosPolicyId_t LATENCYBUDGET_QOS_POLICY_ID = 5;
static const QosPolicyId_t OWNERSHIP_QOS_POLICY_ID = 6;
static const QosPolicyId_t OWNERSHIPSTRENGTH_QOS_POLICY_ID = 7;
static const QosPolicyId_t LIVELINESS_QOS_POLICY_ID = 8;
static const QosPolicyId_t TIMEBASEDFILTER_QOS_POLICY_ID = 9;
static const QosPolicyId_t PARTITION_QOS_POLICY_ID = 10;
static const QosPolicyId_t RELIABILITY_QOS_POLICY_ID = 11;
static const QosPolicyId_t DESTINATIONORDER_QOS_POLICY_ID = 12;
static const QosPolicyId_t HISTORY_QOS_POLICY_ID = 13;
static const QosPolicyId_t RESOURCELIMITS_QOS_POLICY_ID = 14;
static const QosPolicyId_t ENTITYFACTORY_QOS_POLICY_ID = 15;
static const QosPolicyId_t WRITERDATALIFECYCLE_QOS_POLICY_ID = 16;
static const QosPolicyId_t READERDATALIFECYCLE_QOS_POLICY_ID = 17;
static const QosPolicyId_t TOPICDATA_QOS_POLICY_ID = 18;
static const QosPolicyId_t GROUPDATA_QOS_POLICY_ID = 19;
static const QosPolicyId_t TRANSPORTPRIORITY_QOS_POLICY_ID= 20;
static const QosPolicyId_t LIFESPAN_QOS_POLICY_ID = 21;
static const QosPolicyId_t DURABILITYSERVICE_QOS_POLICY_ID = 22;

struct UserDataQosPolicy
{
	//sequence<octet> value;
	//char* value;
	//added by kyy(UserData QoS) StringSeq 구조체로 변경했습니다.(like Partition QoS)
	StringSeq value;
};

struct TopicDataQosPolicy
{
	//sequence<octet> value;
	//char* value;
	//added by kyy(GroupData QoS) StringSeq 구조체로 변경했습니다.(like Partition QoS)
	StringSeq value;
};

struct GroupDataQosPolicy
{
	//sequence<octet> value;
	//char* value;
	//added by kyy(GroupData QoS) StringSeq 구조체로 변경했습니다.(like Partition QoS)
	StringSeq value;
};

struct TransportPriorityQosPolicy
{
	long value;
};

struct LifespanQosPolicy
{
	Duration_t duration;
};

typedef enum DurabilityQosPolicyKind
{
	VOLATILE_DURABILITY_QOS,
	TRANSIENT_LOCAL_DURABILITY_QOS,
	TRANSIENT_DURABILITY_QOS,
	PERSISTENT_DURABILITY_QOS
}DurabilityQosPolicyKind;

struct DurabilityQosPolicy
{
	DurabilityQosPolicyKind kind;
};

typedef enum PresentationQosPolicyAccessScopeKind
{
	INSTANCE_PRESENTATION_QOS,
	TOPIC_PRESENTATION_QOS,
	GROUP_PRESENTATION_QOS
}PresentationQosPolicyAccessScopeKind;


struct PresentationQosPolicy
{
	PresentationQosPolicyAccessScopeKind access_scope;
	bool coherent_access;
	bool ordered_access;
};


struct DeadlineQosPolicy
{
	Duration_t period;
};

struct LatencyBudgetQosPolicy
{
	Duration_t duration;
};

typedef enum OwnershipQosPolicyKind
{
	SHARED_OWNERSHIP_QOS,
	EXCLUSIVE_OWNERSHIP_QOS
}OwnershipQosPolicyKind;

struct OwnershipQosPolicy
{
	OwnershipQosPolicyKind kind;
};

struct OwnershipStrengthQosPolicy
{
	int32_t value;
};

typedef enum LivelinessQosPolicyKind
{
	AUTOMATIC_LIVELINESS_QOS,
	MANUAL_BY_PARTICIPANT_LIVELINESS_QOS,
	MANUAL_BY_TOPIC_LIVELINESS_QOS
}LivelinessQosPolicyKind;


struct LivelinessQosPolicy
{
	LivelinessQosPolicyKind kind;
	Duration_t lease_duration;
};

struct TimeBasedFilterQosPolicy
{
	Duration_t minimum_separation;
};

struct PartitionQosPolicy
{
	StringSeq name;
};

typedef enum ReliabilityQosPolicyKind
{
	BEST_EFFORT_RELIABILITY_QOS = 1,
	RELIABLE_RELIABILITY_QOS
}ReliabilityQosPolicyKind;

struct ReliabilityQosPolicy
{
	ReliabilityQosPolicyKind kind;
	Duration_t max_blocking_time;
};

typedef enum DestinationOrderQosPolicyKind
{
	BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS,
	BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
}DestinationOrderQosPolicyKind;

struct DestinationOrderQosPolicy
{
	DestinationOrderQosPolicyKind kind;
};

typedef enum HistoryQosPolicyKind
{
	KEEP_LAST_HISTORY_QOS,
	KEEP_ALL_HISTORY_QOS
}HistoryQosPolicyKind;

struct HistoryQosPolicy
{
	HistoryQosPolicyKind kind;
	int32_t depth;
};

struct ResourceLimitsQosPolicy
{
	int32_t max_samples;
	int32_t max_instances;
	int32_t max_samples_per_instance;
};

struct EntityFactoryQosPolicy
{
	bool autoenable_created_entities;
};

struct WriterDataLifecycleQosPolicy
{
	bool autodispose_unregistered_instances;
};

struct ReaderDataLifecycleQosPolicy
{
	Duration_t autopurge_nowriter_samples_delay;
	Duration_t autopurge_disposed_samples_delay;
};

struct DurabilityServiceQosPolicy
{
	Duration_t service_cleanup_delay;
	HistoryQosPolicyKind history_kind;
	int32_t history_depth;
	int32_t max_samples;
	int32_t max_instances;
	int32_t max_samples_per_instance;
};

struct DomainParticipantFactoryQos
{
	EntityFactoryQosPolicy entity_factory;
};

struct DomainParticipantQos
{
	UserDataQosPolicy user_data;
	EntityFactoryQosPolicy entity_factory;
};

struct TopicQos
{
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

struct DataWriterQos
{
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

struct PublisherQos
{
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	GroupDataQosPolicy group_data;
	EntityFactoryQosPolicy entity_factory;
};

struct DataReaderQos
{
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

struct SubscriberQos
{
	PresentationQosPolicy presentation;
	PartitionQosPolicy partition;
	GroupDataQosPolicy group_data;
	EntityFactoryQosPolicy entity_factory;
};

//added by kyy
struct RxOQos
{
	DurabilityQosPolicy durability;
	DeadlineQosPolicy deadline;
	LatencyBudgetQosPolicy latency_budget;
	LivelinessQosPolicy liveliness;
	ReliabilityQosPolicy reliability;
	OwnershipQosPolicy ownership;
	DestinationOrderQosPolicy destination_order;
	PresentationQosPolicy presentation;



	//	status by jun
	rtps_reader_t * p_rtps_reader;
	rtps_writer_t * p_rtps_writer;

};

// ----------------------------------------------------------------------

struct ParticipantBuiltinTopicData
{
	BuiltinTopicKey_t key;
	UserDataQosPolicy user_data;
};

struct TopicBuiltinTopicData
{
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

struct PublicationBuiltinTopicData
{
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

struct SubscriptionBuiltinTopicData
{
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

// ----------------------------------------------------------------------
//interface Entity {
//// ReturnCode_t set_qos(in_dds EntityQos qos);
//// ReturnCode_t get_qos(inout_dds EntityQos qos);
//// ReturnCode_t set_listener(in_dds Listener l,in_dds StatusMask mask);
//// Listener get_listener();
//	ReturnCode_t enable();
//	StatusCondition get_statuscondition();
//	StatusMask get_status_changes();
//	InstanceHandle_t get_instance_handle();
//};

#define ENTITY_BODY \
	ReturnCode_t (*enable)(Entity *p_entity);					\
	StatusCondition *(*get_statuscondition)(Entity *p_entity);	\
	StatusMask (*get_status_changes)(Entity *p_entity);			\
	InstanceHandle_t (*get_instance_handle)(Entity *p_entity);	\
	/*추가부분*/												\
	int32_t		i_entity_type;									\
	bool	b_enable;											\
	StatusCondition* p_status_condition;						\
	StatusMask		l_status_mask;								\
	StatusMask		l_status_changes;							\
	mutex_t			entity_lock;								\
	cond_t			entity_wait;								\
	mutex_t			ack_lock;								\
	cond_t			ack_wait;								\
	int32_t			last_sequencenum;							\
	int32_t			test_for_reliablity_division;				\
	int32_t			test_for_reliablity_bulk_start;				\
	int32_t			test_for_reliablity_bulk_last;				\
	int32_t			id;


struct Entity
{
	ENTITY_BODY
};

// ----------------------------------------------------------------------
//interface DomainParticipant : Entity {
//	// Factory interfaces
//	Publisher create_publisher(in_dds PublisherQos qos,in_dds PublisherListener a_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_publisher(in_dds Publisher p);
//	Subscriber create_subscriber(in_dds SubscriberQos qos,in_dds SubscriberListener a_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_subscriber(in_dds Subscriber s);
//	Subscriber get_builtin_subscriber();
//	Topic create_topic(in_dds string topic_name,in_dds string type_name,in_dds TopicQos qos,in_dds TopicListener a_listener,in_dds StatusMask mask);
//
//	ReturnCode_t delete_topic(in_dds Topic a_topic);
//	Topic find_topic(in_dds string topic_name,in_dds Duration_t timeout);
//	TopicDescription lookup_topicdescription(in_dds string name);
//	ContentFilteredTopic create_contentfilteredtopic(in_dds string name,in_dds Topic related_topic,in_dds string filter_expression,in_dds StringSeq expression_parameters);
//	ReturnCode_t delete_contentfilteredtopic(in_dds ContentFilteredTopic a_contentfilteredtopic);
//	MultiTopic create_multitopic(in_dds string name,in_dds string type_name,in_dds string subscription_expression,in_dds StringSeq expression_parameters);
//	ReturnCode_t delete_multitopic(in_dds MultiTopic a_multitopic);
//	ReturnCode_t delete_contained_entities();
//	ReturnCode_t set_qos(in_dds DomainParticipantQos qos);
//	ReturnCode_t get_qos(inout_dds DomainParticipantQos qos);
//	ReturnCode_t set_listener(in_dds DomainParticipantListener a_listener,in_dds StatusMask mask);
//	DomainParticipantListener get_listener();
//	ReturnCode_t ignore_participant(in_dds InstanceHandle_t handle);
//	ReturnCode_t ignore_topic(in_dds InstanceHandle_t handle);
//	ReturnCode_t ignore_publication(in_dds InstanceHandle_t handle);
//	ReturnCode_t ignore_subscription(in_dds InstanceHandle_t handle);
//	DomainId_t get_domain_id();
//	ReturnCode_t assert_liveliness();
//	ReturnCode_t set_default_publisher_qos(in_dds PublisherQos qos);
//	ReturnCode_t get_default_publisher_qos(inout_dds PublisherQos qos);
//	ReturnCode_t set_default_subscriber_qos(in_dds SubscriberQos qos);
//	ReturnCode_t get_default_subscriber_qos(inout_dds SubscriberQos qos);
//	ReturnCode_t set_default_topic_qos(in_dds TopicQos qos);
//	ReturnCode_t get_default_topic_qos(inout_dds TopicQos qos);
//
//	ReturnCode_t get_discovered_participants(inout_dds InstanceHandleSeq participant_handles);
//	ReturnCode_t get_discovered_participant_data(inout_dds ParticipantBuiltinTopicData participant_data,in_dds InstanceHandle_t participant_handle);
//	ReturnCode_t get_discovered_topics(inout_dds InstanceHandleSeq topic_handles);
//	ReturnCode_t get_discovered_topic_data(inout_dds TopicBuiltinTopicData topic_data,in_dds InstanceHandle_t topic_handle);
//	boolean contains_entity(in_dds InstanceHandle_t a_handle);
//	ReturnCode_t get_current_time(inout_dds Time_t current_time);
//};

#ifndef RTPS_PARTICIPANT_T_DEFINE
#	define RTPS_PARTICIPANT_T_DEFINE 1
	typedef struct rtps_participant_t rtps_participant_t;
#endif

struct DomainParticipant
{
	ENTITY_BODY
	Publisher *(*create_publisher)(DomainParticipant *p_domainparticipant, in_dds PublisherQos *p_qos,in_dds PublisherListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_publisher)(DomainParticipant *p_domainparticipant, in_dds Publisher *p_publisher);
	Subscriber *(*create_subscriber)(DomainParticipant *p_domainparticipant, in_dds SubscriberQos *p_qos,in_dds SubscriberListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_subscriber)(DomainParticipant *p_domainparticipant, in_dds Subscriber *p_subscriber);
	Subscriber *(*get_builtin_subscriber)(DomainParticipant *p_domainparticipant);
	Topic *(*create_topic)(DomainParticipant *p_domainparticipant, in_dds string topic_name,in_dds string type_name,in_dds TopicQos *p_qos,in_dds TopicListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_topic)(DomainParticipant *p_domainparticipant, in_dds Topic *p_topic);

	Topic *(*find_topic)(DomainParticipant *p_domainparticipant, in_dds string topic_name,in_dds Duration_t timeout);
	TopicDescription *(*lookup_topicdescription)(DomainParticipant *p_domainparticipant, in_dds string name);
	ContentFilteredTopic *(*create_contentfilteredtopic)(DomainParticipant *p_domainparticipant, in_dds string name,in_dds Topic *p_related_topic,in_dds string filter_expression,in_dds StringSeq expression_parameters);
	ReturnCode_t (*delete_contentfilteredtopic)(DomainParticipant *p_domainparticipant, in_dds ContentFilteredTopic *p_contentfilteredtopic);
	MultiTopic *(*create_multitopic)(DomainParticipant *p_domainparticipant, in_dds string name,in_dds string type_name,in_dds string subscription_expression,in_dds StringSeq expression_parameters);
	ReturnCode_t (*delete_multitopic)(DomainParticipant *p_domainparticipant, in_dds MultiTopic *p_multitopic);
	ReturnCode_t (*delete_contained_entities)(DomainParticipant *p_domainparticipant);
	ReturnCode_t (*set_qos)(DomainParticipant *p_domainparticipant, in_dds DomainParticipantQos *p_qos);
	ReturnCode_t (*get_qos)(DomainParticipant *p_domainparticipant, inout_dds DomainParticipantQos *p_qos);
	ReturnCode_t (*set_listener)(DomainParticipant *p_domainparticipant, in_dds DomainParticipantListener *p_listener,in_dds StatusMask mask);
	DomainParticipantListener *(*get_listener)(DomainParticipant *p_domainparticipant);
	ReturnCode_t (*ignore_participant)(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle);
	ReturnCode_t (*ignore_topic)(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle);
	ReturnCode_t (*ignore_publication)(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle);
	ReturnCode_t (*ignore_subscription)(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle);
	DomainId_t (*get_domain_id)(DomainParticipant *p_domainparticipant);
	ReturnCode_t (*assert_liveliness)(DomainParticipant *p_domainparticipant);
	ReturnCode_t (*set_default_publisher_qos)(in_dds PublisherQos *p_qos);
	ReturnCode_t (*get_default_publisher_qos)(inout_dds PublisherQos *p_qos);
	ReturnCode_t (*set_default_subscriber_qos)(in_dds SubscriberQos *p_qos);
	ReturnCode_t (*get_default_subscriber_qos)(inout_dds SubscriberQos *p_qos);
	ReturnCode_t (*set_default_topic_qos)(in_dds TopicQos *p_qos);
	ReturnCode_t (*get_default_topic_qos)(inout_dds TopicQos *p_qos);

	ReturnCode_t (*get_discovered_participants)(DomainParticipant *p_domainparticipant, inout_dds InstanceHandleSeq *participant_handles);
	ReturnCode_t (*get_discovered_participant_data)(DomainParticipant *p_domainparticipant, inout_dds ParticipantBuiltinTopicData *p_participant_data,in_dds InstanceHandle_t participant_handle);
	ReturnCode_t (*get_discovered_topics)(DomainParticipant *p_domainparticipant, inout_dds InstanceHandleSeq *topic_handles);
	ReturnCode_t (*get_discovered_topic_data)(DomainParticipant *p_domainparticipant, inout_dds TopicBuiltinTopicData *p_topic_data,in_dds InstanceHandle_t topic_handle);
	bool (*contains_entity)(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t a_handle);
	ReturnCode_t (*get_current_time)(DomainParticipant *p_domainparticipant, inout_dds Time_t *p_current_time);

	//추가 부분.
	DomainId_t domain_id;
	int32_t	i_publisher;
	Publisher** pp_publisher;
	int32_t i_subscriber;
	Subscriber** pp_subscriber;
	int32_t i_topic;
	Topic** pp_topic;
	int32_t i_content_filtered_topics;
	ContentFilteredTopic** pp_content_filtered_topics;
	int32_t i_multi_topics;
	MultiTopic** pp_multi_topics;

	DomainParticipantQos qos;
	DomainParticipantListener* p_domain_participant_listener;

	int32_t				i_builtin_topics;
	Topic**				pp_builtin_topics;
	Publisher*			p_builtin_publisher;
	int32_t				i_waitsets;
	WaitSet**			pp_waitsets;
	rtps_participant_t* p_rtps_participant;

	//by jun
	// reader thread 제어하기 위한 변수
	data_fifo_t* p_reader_thread_fifo;
	module_thread_t thread_id;
	bool						b_end;
	//writer by jun
	//writer thread 제어하기 위한 변수들 
	data_fifo_t* p_writer_thread_fifo;
	module_thread_t thread_id2;
	bool						b_end2;
	//writer by jun
	//Writer_Thread2에 이벤트를 전달하기 위한 변수
	cond_t			writer_thread_wait;	
	mutex_t 		writer_thread_lock;
	// Reader와 Writer Thread 생성시 구분하기 위한 변수
	int					i_flag_reader_writer_thread;
};

//interface DomainParticipantFactory {
//	DomainParticipant create_participant(in_dds DomainId_t domain_id,in_dds DomainParticipantQos qos,in_dds DomainParticipantListener a_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_participant(in_dds DomainParticipant a_participant);
//	DomainParticipant lookup_participant(in_dds DomainId_t domain_id);
//	ReturnCode_t set_default_participant_qos(in_dds DomainParticipantQos qos);
//	ReturnCode_t get_default_participant_qos(inout_dds DomainParticipantQos qos);
//	ReturnCode_t set_qos(in_dds DomainParticipantFactoryQos qos);
//	ReturnCode_t get_qos(inout_dds DomainParticipantFactoryQos qos);
//};

struct DomainParticipantFactory
{
	DomainParticipant *(*create_participant)(in_dds DomainId_t domain_id,in_dds DomainParticipantQos *p_qos,in_dds DomainParticipantListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_participant)(in_dds DomainParticipant *p_participant);
	DomainParticipant *(*lookup_participant)(in_dds DomainId_t domain_id);
	ReturnCode_t (*set_default_participant_qos)(in_dds DomainParticipantQos *p_qos);
	ReturnCode_t (*get_default_participant_qos)(inout_dds DomainParticipantQos *p_qos);
	ReturnCode_t (*set_qos)(in_dds DomainParticipantFactoryQos *p_qos);
	ReturnCode_t (*get_qos)(inout_dds DomainParticipantFactoryQos *p_qos);

	//이부분은 추가 부분...
	int32_t		module_id;
};

//#ifdef __cplusplus
//
//class DomainParticipantFactory_wrapper{
//private:
//	int32_t	module_id;
//public:
//	DomainParticipant *create_participant(in_dds DomainId_t domain_id,in_dds DomainParticipantQos *p_qos,in_dds DomainParticipantListener *p_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_participant(in_dds DomainParticipant *p_participant);
//	DomainParticipant *lookup_participant(in_dds DomainId_t domain_id);
//	ReturnCode_t set_default_participant_qos(in_dds DomainParticipantQos *p_qos);
//	ReturnCode_t get_default_participant_qos(inout_dds DomainParticipantQos *p_qos);
//	ReturnCode_t set_qos(in_dds DomainParticipantFactoryQos *p_qos);
//	ReturnCode_t get_qos(inout_dds DomainParticipantFactoryQos *p_qos);
//};
//
//
//#endif



//interface TypeSupport {
//// ReturnCode_t register_type(in_dds DomainParticipant domain,in_dds string type_name);
////	string get_type_name();
//};
typedef struct dds_parameter_t dds_parameter_t;

#define TYPESUPPORT_BODY	\
	int32_t		i_size;			\
	int32_t		i_parameters;	\
	dds_parameter_t** pp_parameters; \
	int32_t		offsetcount;			\



struct TypeSupport
{
	TYPESUPPORT_BODY
};
// ----------------------------------------------------------------------


//interface TopicDescription {
//	string get_type_name();
//	string get_name();
//	DomainParticipant get_participant();
//};

typedef enum TOPIC_TYPE
{
	DEFAULT_TOPIC_TYPE,
	CONTENTFILTER_TOPIC_TYPE,
	MULTITOPIC_TYPE
} TOPIC_TYPE;

#define TOPICDESCRIPTION_BODY				\
	string (*get_type_name)(Topic *p_topic);\
	string (*get_name)(Topic *p_topic);		\
	DomainParticipant *(*get_participant)(Topic *p_topic);	\
	/*이부분 추가.*/						\
	string		topic_name;					\
	string		type_name;					\
	DomainParticipant* p_domain_participant; \
	TOPIC_TYPE			topic_type;			\


struct TopicDescription
{
	TOPICDESCRIPTION_BODY
};

//interface Topic : Entity, TopicDescription {
//	ReturnCode_t set_qos(in_dds TopicQos qos);
//	ReturnCode_t get_qos(inout_dds TopicQos qos);
//	ReturnCode_t set_listener(in_dds TopicListener a_listener,in_dds StatusMask mask);
//	TopicListener get_listener();
//	// Access the status
//	ReturnCode_t get_inconsistent_topic_status(inout_dds InconsistentTopicStatus a_status);
//};

struct Topic
{
	ENTITY_BODY
	TOPICDESCRIPTION_BODY
	ReturnCode_t (*set_qos)(Topic* p_topic, in_dds TopicQos *p_qos);
	ReturnCode_t (*get_qos)(Topic* p_topic, inout_dds TopicQos *p_qos);
	ReturnCode_t (*set_listener)(Topic* p_topic, in_dds TopicListener *p_listener,in_dds StatusMask mask);
	TopicListener *(*get_listener)(Topic* p_topic);
	// Access the status
	ReturnCode_t (*get_inconsistent_topic_status)(Topic* p_topic, inout_dds InconsistentTopicStatus *a_status);

	//이부분추가
	TopicQos topic_qos;
	TopicListener* p_topic_listener;
	//by jun
	TopicStatus status;
	TopicListenerStatusChangedFlag status_changed_flag;
	//
	void*	p_rtps_topic;
};

//interface ContentFilteredTopic : TopicDescription {
//	string get_filter_expression();
//	ReturnCode_t get_expression_parameters(inout_dds StringSeq);
//	ReturnCode_t set_expression_parameters(in_dds StringSeq expression_parameters);
//	Topic get_related_topic();
//};



struct ContentFilteredTopic
{
	ENTITY_BODY
	TOPICDESCRIPTION_BODY
	string (*get_filter_expression)(ContentFilteredTopic* p_topicinitTopicDescription);
	ReturnCode_t (*get_expression_parameters)(ContentFilteredTopic* p_topicinitTopicDescription, inout_dds StringSeq *p_expression_parameters);
	ReturnCode_t (*set_expression_parameters)(ContentFilteredTopic* p_topicinitTopicDescription, in_dds StringSeq *p_expression_parameters);
	Topic *(*get_related_topic)(ContentFilteredTopic* p_topicinitTopicDescription);
	//이 부분 추가.
	Topic *p_related_topic;
	string filter_expression;
	StringSeq expression_parameters;
	expression_t *p_expr;
};

//interface MultiTopic : TopicDescription {
//	string get_subscription_expression();
//	ReturnCode_t get_expression_parameters(inout_dds StringSeq);
//	ReturnCode_t set_expression_parameters(in_dds StringSeq expression_parameters);
//};

struct MultiTopic
{
	ENTITY_BODY
	TOPICDESCRIPTION_BODY
	string (*get_subscription_expression)(MultiTopic* p_multiTopic);
	ReturnCode_t (*get_expression_parameters)(MultiTopic* p_multiTopic, inout_dds StringSeq *p_expression_parameters);
	ReturnCode_t (*set_expression_parameters)(MultiTopic* p_multiTopic, in_dds StringSeq *p_expression_parameters);
	//이 부분 추가.
	string subscription_expression;
	StringSeq expression_parameters;
};

// ----------------------------------------------------------------------

//interface Publisher : Entity {
//	DataWriter create_datawriter(in_dds Topic a_topic,in_dds DataWriterQos qos,in_dds DataWriterListener a_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_datawriter(in_dds DataWriter a_datawriter);
//	DataWriter lookup_datawriter(in_dds string topic_name);
//	ReturnCode_t delete_contained_entities();
//	ReturnCode_t set_qos(in_dds PublisherQos qos);
//	ReturnCode_t get_qos(inout_dds PublisherQos qos);
//	ReturnCode_t set_listener(in_dds PublisherListener a_listener,in_dds StatusMask mask);
//	PublisherListener get_listener();
//	ReturnCode_t suspend_publications();
//	ReturnCode_t resume_publications();
//	ReturnCode_t begin_coherent_changes();
//	ReturnCode_t end_coherent_changes();
//	ReturnCode_t wait_for_acknowledgments(in_dds Duration_t max_wait);
//	DomainParticipant get_participant();
//	ReturnCode_t set_default_datawriter_qos(in_dds DataWriterQos qos);
//	ReturnCode_t get_default_datawriter_qos(inout_dds DataWriterQos qos);
//	ReturnCode_t copy_from_topic_qos(inout_dds DataWriterQos a_datawriter_qos,in_dds TopicQos a_topic_qos);
//};



struct Publisher
{
	ENTITY_BODY
	DataWriter *(*create_datawriter)(Publisher *p_publisher, in_dds Topic *p_topic,in_dds DataWriterQos *p_qos,in_dds DataWriterListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_datawriter)(Publisher *p_publisher, in_dds DataWriter *p_datawriter);
	DataWriter *(*lookup_datawriter)(Publisher *p_publisher, in_dds string topic_name);
	ReturnCode_t (*delete_contained_entities)(Publisher *p_publisher);
	ReturnCode_t (*set_qos)(Publisher *p_publisher, in_dds PublisherQos *p_qos);
	ReturnCode_t (*get_qos)(Publisher *p_publisher, inout_dds PublisherQos *p_qos);
	ReturnCode_t (*set_listener)(Publisher *p_publisher, in_dds PublisherListener *p_listener,in_dds StatusMask mask);
	PublisherListener *(*get_listener)(Publisher *p_publisher);
	ReturnCode_t (*suspend_publications)(Publisher *p_publisher);
	ReturnCode_t (*resume_publications)(Publisher *p_publisher);
	ReturnCode_t (*begin_coherent_changes)(Publisher *p_publisher);
	ReturnCode_t (*end_coherent_changes)(Publisher *p_publisher);
	ReturnCode_t (*wait_for_acknowledgments)(Publisher *p_publisher, in_dds Duration_t max_wait);
	DomainParticipant *(*get_participant)(Publisher *p_publisher);
	ReturnCode_t (*set_default_datawriter_qos)(in_dds DataWriterQos *p_qos);
	ReturnCode_t (*get_default_datawriter_qos)(inout_dds DataWriterQos *p_qos);
	ReturnCode_t (*copy_from_topic_qos)(inout_dds DataWriterQos *p_datawriter_qos,in_dds TopicQos *p_topic_qos);
	/////
	PublisherQos publisher_qos;
	int	i_datawriters;
	DataWriter** pp_datawriters;
	PublisherListener* p_publisher_listener;
	DomainParticipant* p_domain_participant;
	bool		is_suspend;
	bool		begin_coherent;
};

//interface DataWriter : Entity {
//	ReturnCode_t set_qos(in_dds DataWriterQos qos);
//	ReturnCode_t get_qos(inout_dds DataWriterQos qos);
//	ReturnCode_t set_listener(in_dds DataWriterListener a_listener,in_dds StatusMask mask);
//	DataWriterListener get_listener();
//	Topic get_topic();
//	Publisher get_publisher();
//	ReturnCode_t wait_for_acknowledgments(in_dds Duration_t max_wait);
//	// Access the status
//	ReturnCode_t get_liveliness_lost_status(inout_dds LivelinessLostStatus);
//	ReturnCode_t get_offered_deadline_missed_status(inout_dds OfferedDeadlineMissedStatus status);
//	ReturnCode_t get_offered_incompatible_qos_status(inout_dds OfferedIncompatibleQosStatus status);
//	ReturnCode_t get_publication_matched_status(inout_dds PublicationMatchedStatus status);
//	ReturnCode_t assert_liveliness();
//	ReturnCode_t get_matched_subscriptions(inout_dds InstanceHandleSeq subscription_handles);
//	ReturnCode_t get_matched_subscription_data(inout_dds SubscriptionBuiltinTopicData subscription_data,in_dds InstanceHandle_t subscription_handle);
//};


#ifndef RTPS_CACHECHANGE_T_DEFINE
	#define RTPS_CACHECHANGE_T_DEFINE 1
	typedef struct rtps_cachechange_t rtps_cachechange_t;
#endif

#define MAX_KEY_NUM 10

#ifdef Android
#	define key_t cpskey_t
#endif

typedef struct cpskey_t
{
	long		id;
	uint32_t	offset;
	uint32_t	len;
	void*		p_value;
}cpskey_t;


typedef struct key_guid_t
{
	uint8_t		key[16];
}key_guid_t;

typedef struct instanceset_t
{
	uint32_t				i_key;
	cpskey_t				keylist[MAX_KEY_NUM];
	int32_t					max_samples_per_instance;
	int32_t					i_history_depth;
	int32_t					i_messages;
	message_t**				pp_messages;
	int32_t					i_key_guid;
	key_guid_t**			pp_key_guid;

//ignore by jun
	int32_t					ignore_id;

	//	readcondition by jun
	InstanceStateKind		instance_state;
	SampleStateKind 		sample_state;
	ViewStateKind 			view_state;
	///
	bool					register_instance_timestamp_set;
	Time_t					register_instance_timestamp_value;

} instanceset_t;

#define DATAWRITER_BODY \
	ENTITY_BODY\
	ReturnCode_t (*set_qos)(DataWriter* p_datawriter, in_dds DataWriterQos *p_qos);\
	ReturnCode_t (*get_qos)(DataWriter* p_datawriter, inout_dds DataWriterQos *p_qos);\
	ReturnCode_t (*set_listener)(DataWriter* p_datawriter, in_dds DataWriterListener *p_listener,in_dds StatusMask mask);\
	DataWriterListener *(*get_listener)(DataWriter* p_datawriter);\
	Topic *(*get_topic)(DataWriter* p_datawriter);\
	Publisher *(*get_publisher)(DataWriter* p_datawriter);\
	ReturnCode_t (*wait_for_acknowledgments)(DataWriter* p_datawriter, in_dds Duration_t max_wait);\
	/* Access the status*/ \
	ReturnCode_t (*get_liveliness_lost_status)(DataWriter* p_datawriter, inout_dds LivelinessLostStatus *status);\
	ReturnCode_t (*get_offered_deadline_missed_status)(DataWriter* p_datawriter, inout_dds OfferedDeadlineMissedStatus *status);\
	ReturnCode_t (*get_offered_incompatible_qos_status)(DataWriter* p_datawriter, inout_dds OfferedIncompatibleQosStatus *status);\
	ReturnCode_t (*get_publication_matched_status)(DataWriter* p_datawriter, inout_dds PublicationMatchedStatus *status);\
	ReturnCode_t (*assert_liveliness)(DataWriter* p_datawriter);\
	ReturnCode_t (*get_matched_subscriptions)(DataWriter* p_datawriter, inout_dds InstanceHandleSeq *subscription_handles);\
	ReturnCode_t (*get_matched_subscription_data)(DataWriter* p_datawriter, inout_dds SubscriptionBuiltinTopicData *subscription_data,in_dds InstanceHandle_t subscription_handle);\
	\
	/* 이부분 추가 */\
	DataWriterQos datawriter_qos;\
	DataWriterListener* p_datawriter_listener;\
	Topic* p_topic;\
	Publisher* p_publisher;\
	void*	p_related_rtps_writer;\
	bool	begin_coherent;\
	\
	/*by jun */\
	DataWriterStatus status;\
	DataWriterListenerStatusChangedFlag status_changed_flag;\
	int32_t		i_instanceset;\
	instanceset_t**	pp_instanceset;\
	int32_t					i_message_order;\
	message_t**				pp_message_order;\
	BUILTIN_TYPE builtin_type;\


struct DataWriter{
	DATAWRITER_BODY
};

// ----------------------------------------------------------------------
//interface Subscriber : Entity {
//	DataReader create_datareader(in_dds TopicDescription a_topic,in_dds DataReaderQos qos,in_dds DataReaderListener a_listener,in_dds StatusMask mask);
//	ReturnCode_t delete_datareader(in_dds DataReader a_datareader);
//	ReturnCode_t delete_contained_entities();
//	DataReader lookup_datareader(in_dds string topic_name);
//	ReturnCode_t get_datareaders(inout_dds DataReaderSeq readers,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
//	ReturnCode_t notify_datareaders();
//	ReturnCode_t set_qos(in_dds SubscriberQos qos);
//	ReturnCode_t get_qos(inout_dds SubscriberQos qos);
//	ReturnCode_t set_listener(in_dds SubscriberListener a_listener,in_dds StatusMask mask);
//	SubscriberListener get_listener();
//	ReturnCode_t begin_access();
//	ReturnCode_t end_access();
//	DomainParticipant get_participant();
//	ReturnCode_t set_default_datareader_qos(in_dds DataReaderQos qos);
//	ReturnCode_t get_default_datareader_qos(inout_dds DataReaderQos qos);
//	ReturnCode_t copy_from_topic_qos(inout_dds DataReaderQos a_datareader_qos,in_dds TopicQos a_topic_qos);
//};


struct Subscriber {
	ENTITY_BODY
	DataReader *(*create_datareader)(Subscriber *p_subscriber, in_dds /*TopicDescription*/Topic *p_topic,in_dds DataReaderQos *p_qos,in_dds DataReaderListener *p_listener,in_dds StatusMask mask);
	ReturnCode_t (*delete_datareader)(Subscriber *p_subscriber, in_dds DataReader *p_datareader);
	ReturnCode_t (*delete_contained_entities)(Subscriber *p_subscriber);
	DataReader *(*lookup_datareader)(Subscriber *p_subscriber, in_dds string topic_name);
	ReturnCode_t (*get_datareaders)(Subscriber *p_subscriber, inout_dds DataReaderSeq *p_readers,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t (*notify_datareaders)(Subscriber *p_subscriber);
	ReturnCode_t (*set_qos)(Subscriber *p_subscriber, in_dds SubscriberQos *p_qos);
	ReturnCode_t (*get_qos)(Subscriber *p_subscriber, inout_dds SubscriberQos *p_qos);
	ReturnCode_t (*set_listener)(Subscriber *p_subscriber, in_dds SubscriberListener *p_listener,in_dds StatusMask mask);
	SubscriberListener *(*get_listener)(Subscriber *p_subscriber);
	ReturnCode_t (*begin_access)(Subscriber *p_subscriber);
	ReturnCode_t (*end_access)(Subscriber *p_subscriber);
	DomainParticipant *(*get_participant)(Subscriber *p_subscriber);
	ReturnCode_t (*set_default_datareader_qos)(in_dds DataReaderQos *p_qos);
	ReturnCode_t (*get_default_datareader_qos)(inout_dds DataReaderQos *p_qos);
	ReturnCode_t (*copy_from_topic_qos)(inout_dds DataReaderQos *p_datareader_qos,in_dds TopicQos *p_topic_qos);

	//이부분추가.
	SubscriberQos subscriber_qos;
	int i_datareaders;
	DataReader** pp_datareaders;
	SubscriberListener* p_subscriber_listener;
	DomainParticipant* p_domain_participant;
	// by jun
	SubscriberListenerStatusChangedFlag status_changed_flag;
};


//interface DataReader : Entity {
//	ReadCondition create_readcondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
//	QueryCondition create_querycondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states,in_dds string query_expression,in_dds StringSeq query_parameters);
//	ReturnCode_t delete_readcondition(in_dds ReadCondition a_condition);
//	ReturnCode_t delete_contained_entities();
//	ReturnCode_t set_qos(in_dds DataReaderQos qos);
//	ReturnCode_t get_qos(inout_dds DataReaderQos qos);
//	ReturnCode_t set_listener(in_dds DataReaderListener a_listener,in_dds StatusMask mask);
//	DataReaderListener get_listener();
//	TopicDescription get_topicdescription();
//	Subscriber get_subscriber();
//	ReturnCode_t get_sample_rejected_status(inout_dds SampleRejectedStatus status);
//	ReturnCode_t get_liveliness_changed_status(inout_dds LivelinessChangedStatus status);
//	ReturnCode_t get_requested_deadline_missed_status(inout_dds RequestedDeadlineMissedStatus status);
//	ReturnCode_t get_requested_incompatible_qos_status(inout_dds RequestedIncompatibleQosStatus status);
//	ReturnCode_t get_subscription_matched_status(inout_dds SubscriptionMatchedStatus status);
//	ReturnCode_t get_sample_lost_status(inout_dds SampleLostStatus status);
//	ReturnCode_t wait_for_historical_data(in_dds Duration_t max_wait);
//	ReturnCode_t get_matched_publications(inout_dds InstanceHandleSeq publication_handles);
//	ReturnCode_t get_matched_publication_data(inout_dds PublicationBuiltinTopicData publication_data,in_dds InstanceHandle_t publication_handle);
//};

#define DATAREADER_BODY \
	ENTITY_BODY\
	ReadCondition *(*create_readcondition)(DataReader* p_datareader, in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);\
	QueryCondition *(*create_querycondition)(DataReader* p_datareader, in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states,in_dds string query_expression,in_dds StringSeq *p_query_parameters);\
	ReturnCode_t (*delete_readcondition)(DataReader* p_datareader, in_dds ReadCondition *p_condition);\
	ReturnCode_t (*delete_contained_entities)(DataReader* p_datareader);\
	ReturnCode_t (*set_qos)(DataReader* p_datareader, in_dds DataReaderQos *p_qos);\
	ReturnCode_t (*get_qos)(DataReader* p_datareader, inout_dds DataReaderQos *p_qos);\
	ReturnCode_t (*set_listener)(DataReader* p_datareader, in_dds DataReaderListener *p_listener,in_dds StatusMask mask);\
	DataReaderListener *(*get_listener)(DataReader* p_datareader);\
	TopicDescription *(*get_topicdescription)(DataReader* p_datareader);\
	Subscriber *(*get_subscriber)(DataReader* p_datareader);\
	\
	ReturnCode_t (*get_sample_rejected_status)(DataReader* p_datareader, inout_dds SampleRejectedStatus *status);\
	ReturnCode_t (*get_liveliness_changed_status)(DataReader* p_datareader, inout_dds LivelinessChangedStatus *status);\
	ReturnCode_t (*get_requested_deadline_missed_status)(DataReader* p_datareader, inout_dds RequestedDeadlineMissedStatus *status);\
	ReturnCode_t (*get_requested_incompatible_qos_status)(DataReader* p_datareader, inout_dds RequestedIncompatibleQosStatus *status);\
	ReturnCode_t (*get_subscription_matched_status)(DataReader* p_datareader, inout_dds SubscriptionMatchedStatus *status);\
	ReturnCode_t (*get_sample_lost_status)(DataReader* p_datareader, inout_dds SampleLostStatus *status);\
	ReturnCode_t (*wait_for_historical_data)(DataReader* p_datareader, in_dds Duration_t max_wait);\
	ReturnCode_t (*get_matched_publications)(DataReader* p_datareader, inout_dds InstanceHandleSeq *publication_handles);\
	ReturnCode_t (*get_matched_publication_data)(DataReader* p_datareader, inout_dds PublicationBuiltinTopicData *publication_data,in_dds InstanceHandle_t publication_handle);\
	/*이부분 추가*/\
	DataReaderQos datareader_qos;\
	Subscriber* p_subscriber;\
	Topic* p_topic;\
	int32_t	i_read_conditions;\
	ReadCondition** pp_read_conditions;\
	int32_t	i_query_conditions;\
	QueryCondition** pp_query_conditions;\
	DataReaderListener* p_datareader_listener;\
	void*				p_related_rtps_reader;\
	/*added by kyy(Presentation QoS)*/\
	int32_t first_coh_number;\
	int32_t last_coh_number;\
	bool	is_coherent_set_end;\
	/*by jun*/\
	DataReaderStatus status;\
	DataReaderListenerStatusChangedFlag status_changed_flag;\
	int32_t			i_instanceset;\
	instanceset_t**	pp_instanceset;\
	int32_t					i_message_order;\
	message_t**				pp_message_order;\
	int32_t			len;\
	int32_t			max_len;\
	bool			owned;\
	BUILTIN_TYPE builtin_type;\

struct DataReader
{
	DATAREADER_BODY
};


struct SampleInfo
{
	SampleStateKind sample_state;
	ViewStateKind view_state;
	InstanceStateKind instance_state;
	Time_t source_timestamp;
	InstanceHandle_t instance_handle;
	InstanceHandle_t publication_handle;
	int32_t disposed_generation_count;
	int32_t no_writers_generation_count;
	int32_t sample_rank;
	int32_t generation_rank;
	int32_t absolute_generation_rank;
	bool valid_data;
	Time_t disposed_timestamp; //by kki (reader data lifecycle)
};

//typedef sequence<SampleInfo> SampleInfoSeq;
typedef struct SampleInfoSeq
{
	int32_t i_seq;
	SampleInfo** pp_sample_infos;
} SampleInfoSeq;

static SampleInfoSeq INIT_SAMPLEINFOSEQ = {0, NULL};

#include <foo.h>

typedef struct Foo SampleData;

typedef struct DataSample
{
	SampleData data;
	SampleInfo sample_info;
} DataSample;

typedef enum BEHAVIOR_KIND
{
	DEFAULT_TYPE,
	STATELESS_TYPE,
	STATEFUL_TYPE
} BEHAVIOR_KIND;


#include <expression.h>
#include <message.h>

#endif
