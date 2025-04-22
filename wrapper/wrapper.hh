#define in_dds
#define inout_dds

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;


#ifndef RETURN_CODE_T_DEFINE
	#define RETURN_CODE_T_DEFINE 1
	typedef int32_t ReturnCode_t;
#endif

typedef uint32_t SampleStateMask;


struct Time_t {
	int32_t sec;
	uint32_t nanosec;
};
typedef struct Duration_t {
	int32_t sec;
	uint32_t nanosec;
}Duration_t;


#ifndef HANDLE_TYPE_NATIVE
	#define HANDLE_TYPE_NATIVE long
#endif
#ifndef HANDLE_TYPE_NATIVE_DEFINE
        #define HANDLE_TYPE_NATIVE_DEFINE 1
        typedef HANDLE_TYPE_NATIVE InstanceHandle_t;
#endif

struct Entity
{
	int32_t		i_entity_type;
};

struct DataWriter{
	int32_t		i_entity_type;
};

struct DataReader{
	int32_t		i_entity_type;
};


struct Publisher{
	int32_t		i_entity_type;
};

struct Subscriber{
	int32_t		i_entity_type;
};

struct DomainParticipant{
	int32_t		i_entity_type;
};

struct Topic{
	int32_t		i_entity_type;
};

struct MultiTopic{
	int32_t		i_entity_type;
};

struct FooDataReader{
	int32_t		i_entity_type;
};

struct FooDataWriter{
	int32_t		i_entity_type;
};

struct WaitSet
{

	int32_t		i_conditions;

};


struct ContentFilteredTopic
{
	int32_t		i_entity_type;
};

typedef  char* string;

struct StringSeq
{
	int32_t i_string;
	string* pp_string;
};

#define BUILTIN_TOPIC_KEY_TYPE_NATIVE int32_t


struct BuiltinTopicKey_t
{
	//BUILTIN_TOPIC_KEY_TYPE_NATIVE value[3];
	BUILTIN_TOPIC_KEY_TYPE_NATIVE value[4];
};


typedef uint32_t InstanceStateMask;
typedef uint32_t SampleStateKind;
typedef uint32_t InstanceStateKind;
typedef uint32_t ViewStateKind;

typedef int32_t QosPolicyId_t;

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

enum DurabilityQosPolicyKind
{
	VOLATILE_DURABILITY_QOS,
	TRANSIENT_LOCAL_DURABILITY_QOS,
	TRANSIENT_DURABILITY_QOS,
	PERSISTENT_DURABILITY_QOS
};

struct DurabilityQosPolicy
{
	DurabilityQosPolicyKind kind;
};

enum PresentationQosPolicyAccessScopeKind
{
	INSTANCE_PRESENTATION_QOS,
	TOPIC_PRESENTATION_QOS,
	GROUP_PRESENTATION_QOS
};


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

enum OwnershipQosPolicyKind
{
	SHARED_OWNERSHIP_QOS,
	EXCLUSIVE_OWNERSHIP_QOS
};

struct OwnershipQosPolicy
{
	OwnershipQosPolicyKind kind;
};

struct OwnershipStrengthQosPolicy
{
	int32_t value;
};

enum LivelinessQosPolicyKind
{
	AUTOMATIC_LIVELINESS_QOS,
	MANUAL_BY_PARTICIPANT_LIVELINESS_QOS,
	MANUAL_BY_TOPIC_LIVELINESS_QOS
};


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

enum ReliabilityQosPolicyKind
{
	BEST_EFFORT_RELIABILITY_QOS = 1,
	RELIABLE_RELIABILITY_QOS
};

struct ReliabilityQosPolicy
{
	ReliabilityQosPolicyKind kind;
	Duration_t max_blocking_time;
};

enum DestinationOrderQosPolicyKind
{
	BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS,
	BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
};

struct DestinationOrderQosPolicy
{
	DestinationOrderQosPolicyKind kind;
};

enum HistoryQosPolicyKind
{
	KEEP_LAST_HISTORY_QOS,
	KEEP_ALL_HISTORY_QOS
};

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


typedef uint32_t StatusKind;
typedef uint32_t StatusMask; // bit-mask StatusKind
static const StatusKind INCONSISTENT_TOPIC_STATUS = 0x0001 << 0;
static const StatusKind OFFERED_DEADLINE_MISSED_STATUS = 0x0001 << 1;
static const StatusKind REQUESTED_DEADLINE_MISSED_STATUS = 0x0001 << 2;
static const StatusKind OFFERED_INCOMPATIBLE_QOS_STATUS = 0x0001 << 5;
static const StatusKind REQUESTED_INCOMPATIBLE_QOS_STATUS = 0x0001 << 6;
static const StatusKind SAMPLE_LOST_STATUS = 0x0001 << 7;
static const StatusKind SAMPLE_REJECTED_STATUS = 0x0001 << 8;
static const StatusKind DATA_ON_READERS_STATUS = 0x0001 << 9;
static const StatusKind DATA_AVAILABLE_STATUS = 0x0001 << 10;
static const StatusKind LIVELINESS_LOST_STATUS = 0x0001 << 11;
static const StatusKind LIVELINESS_CHANGED_STATUS = 0x0001 << 12;
static const StatusKind PUBLICATION_MATCHED_STATUS = 0x0001 << 13;
static const StatusKind SUBSCRIPTION_MATCHED_STATUS = 0x0001 << 14;
static const StatusKind ALL_ENABLE_STATUS = 0xFFFF;

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

enum SampleRejectedStatusKind
{
	NOT_REJECTED,
	REJECTED_BY_INSTANCES_LIMIT,
	REJECTED_BY_SAMPLES_LIMIT,
	REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT
};

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

#define DOMAINID_TYPE_NATIVE int32_t
typedef DOMAINID_TYPE_NATIVE DomainId_t;

typedef enum SUPPORT_TYPE
{
	DDS_STRING_TYPE,
	DDS_BYTE_TYPE,
	DDS_INTEGER8_TYPE,
	DDS_INTEGER16_TYPE,
	DDS_INTEGER32_TYPE,
	DDS_INTEGER64_TYPE,
	DDS_UINTEGER8_TYPE,
	DDS_UINTEGER16_TYPE,
	DDS_UINTEGER32_TYPE,
	DDS_UINTEGER64_TYPE,
	DDS_FLOAT_TYPE,
	DDS_DOUBLE_TYPE,
	DDS_BOOLEAN_TYPE,
	DDS_CHARACTER_TYPE,
	DDS_STRUCT_TYPE
} SUPPORT_TYPE;

struct Condition
{
	bool	b_trigger_value;
	uint32_t		condition_type;
};

struct QueryCondition
{
	bool	b_trigger_value;
	uint32_t		condition_type;
};

struct ReadCondition
{
	bool	b_trigger_value;
	uint32_t		condition_type;
};

struct StatusCondition
{
	bool	b_trigger_value;
	uint32_t		condition_type;
};

typedef uint32_t ViewStateMask;

struct TopicDescription
{
	string		topic_name;
	string		type_name;
};


struct TopicListener
{
	int	not_used;
};


struct SubscriberListener
{
	int	not_used;
};

struct PublisherListener
{
	int	not_used;
};

struct DomainParticipantListener
{
	int	not_used;
};

struct DataWriterListener
{
	int	not_used;
};

struct DataReaderListener
{
	int	not_used;
};

struct FooTypeSupport
{
	int32_t		i_size;
	int32_t		i_parameters;
};

typedef struct InstanceHandleSeq
{
	int32_t i_seq;
	InstanceHandle_t **pp_instancehandle;
} InstanceHandleSeq;

struct Foo
{
	int32_t dummy;
};


typedef struct FooSeq
{
	int32_t i_seq;
	Foo** pp_foo;
} FooSeq;

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


typedef struct SampleInfoSeq
{
	int32_t i_seq;
	SampleInfo** pp_sample_infos;
} SampleInfoSeq;

typedef struct ConditionSeq
{
	int32_t i_seq;
	Condition** pp_conditions;
} ConditionSeq;

typedef struct DataReaderSeq
{
	int i_seq;
	DataReader** pp_datareaders;
} DataReaderSeq;

struct message_t
{
	int				i_datasize;	
	void*			v_data;
};

