/*
	dcps에서 사용되는 함수들 등록...
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#if !defined(__DCPS_FUNC_H)
#define __DCPS_FUNC_H

//#define WHERE_TOPICDESCRIPTION(x) (TopicDescription *)(((char*)x)+sizeof(Entity))
#define WHERE_TOPICDESCRIPTION(x) (TopicDescription *)(((char*)x)+(long)&x->get_type_name - (long)&x->enable)

#define REMOVE_STRING_SEQ(x)						\
	while(x.i_string)								\
	{												\
		FREE(x.pp_string[0]);						\
		REMOVE_ELEM( x.pp_string, x.i_string, 0);	\
	}												\
	x.i_string = 0;									\
	FREE(x.pp_string);


//#define REMOVE_STRING_SEQ(x)						\
//	for(i=0; i < x.i_string; i++)					\
//	{												\
//		FREE(x.pp_string[i]);						\
//		REMOVE_ELEM( x.pp_string, x.i_string, i);	\
//	}												\
//	x.i_string = 0;									\
//	FREE(x.pp_string);


#define BUILTIN_PARTICIPANT_DOMAIN_ID -999

static const Duration_t DURATIOIN_ZERO = {0, 0};

#define PARTICIPANTINFO_NAME	"DCPSParticipant"
#define TOPICINFO_NAME			"DCPSTopic"
#define PUBLICATIONINFO_NAME	"DCPSPublication"
#define SUBSCRIPTIONINFO_NAME	"DCPSSubscription"
#define LIVELINESSP2P_NAME		"DCPSLivenessP2P"

#define PARTICIPANTINFO_TYPE   "participantInfoType"
#define TOPICINFO_TYPE         "topicInfoType"
#define PUBLICATIONINFO_TYPE   "publicationInfoType"
#define SUBSCRIPTIONINFO_TYPE  "subscriptionInfoType"
#define LIVELINESSP2PINFO_NAME	"livenessp2pInfoType"

#define BUILTIN_PUBLISHER_PARTITION_QOS "builtin_partition"

enum BUILTIN_TOPIC_ID
{
	PARTICIPANTINFO_ID,
	TOPICINFO_ID,
	PUBLICATIONINFO_ID,
	SUBSCRIPTIONINFO_ID
};

enum CONDITION_TYPE
{
	NORMAL_CONDITION,
	STATUS_CONDITION,
	QUERY_CONDITION,
	READ_CONDITION,
	GUARD_CONDITION
};

enum ENTITY_TYPE
{
	ENTITY_ENTITY,
	DOMAINPARTICIPANT_ENTITY,
	PUBLISHER_ENTITY,
	SUBSCRIBER_ENTITY,
	TOPIC_ENTITY,
	DATAREADER_ENTITY,
	DATAWRITER_ENTITY
};

struct dds_string
{
	int32_t size;
	char value[256];
};

typedef struct dds_string2
{
	int32_t size;
	char* p_dummy;
}dds_string2;


enum THREAD_TYPE
{
	READER_THREAD,
	WRITER_THREAD
};






#ifdef __cplusplus
extern "C" {
#endif

	DomainParticipantFactory* DomainParticipantFactory_get_instance();


	void init_entity(Entity* p_entity);
	void init_publisher(Publisher* p_publisher);
	void init_topic(Topic* p_topic);
	void init_topic_description(TopicDescription* p_topic_description);
	void init_domain_paticipant(DomainParticipant* p_domain_participant);
	void init_datawriter(DataWriter* p_datawriter);
	void init_subscriber(Subscriber* p_subscriber);
	void init_content_filtered_topic(ContentFilteredTopic* p_topic_init_topic_description);
	void init_multi_topic(MultiTopic* p_multi_topic);
	void init_datareader(DataReader* p_datareader);
	void init_read_condition(ReadCondition* p_read_condition);
	void init_condition(Condition* p_condition);
	void init_query_condition(QueryCondition* p_query_condition);
	void init_status_condition(StatusCondition* p_status_condition);
	void init_waitset(WaitSet* p_waitset);


	void destroy_entity(Entity* p_entity);
	void destroy_domain_participant(DomainParticipant* p_domain_participant, bool b_except_domain_paticipant);
	void destroy_publisher(Publisher* p_publisher);
	void destroy_subscriber(Subscriber* p_subscriber);
	void destroy_topic(Topic* p_topic);
	void destroy_domain_participant_factory(DomainParticipantFactory* p_domain_participant_factory);
	void destroy_topic_description(TopicDescription* p_topic_description);
	void destroy_content_filtered_topic(ContentFilteredTopic* p_topic_init_topic_description);
	void destroy_multi_topic(MultiTopic* p_multi_topic);
	void destroy_datawriter(DataWriter* p_datawriter);
	void destroy_datareader(DataReader* p_datareader);
	void destroy_read_condition(ReadCondition* p_read_condition);
	void destroy_condition(Condition* p_condition);
	void destroy_query_condition(QueryCondition* p_query_condition);
	void destroy_read_condition_except_read_condition(ReadCondition* p_read_condition);
	void destroy_status_condition(StatusCondition* p_status_condition);
	void destroy_waitset(WaitSet* p_waitset);

	ReturnCode_t static_set_default_participant_qos(in_dds DomainParticipantQos *p_qos);
	ReturnCode_t static_get_default_participant_qos(inout_dds DomainParticipantQos *p_qos);
	ReturnCode_t static_set_default_publisher_qos(in_dds PublisherQos *p_qos);
	ReturnCode_t static_get_default_publisher_qos(inout_dds PublisherQos *p_qos);
	ReturnCode_t static_set_default_subscriber_qos(in_dds SubscriberQos *p_qos);
	ReturnCode_t static_get_default_subscriber_qos(inout_dds SubscriberQos *p_qos);
	ReturnCode_t static_set_default_topic_qos(in_dds TopicQos *p_qos);
	ReturnCode_t static_get_default_topic_qos(inout_dds TopicQos *p_qos);
	ReturnCode_t static_set_default_datawriter_qos(in_dds DataWriterQos *p_qos);
	ReturnCode_t static_get_default_datawriter_qos(inout_dds DataWriterQos *p_qos);
	ReturnCode_t static_set_default_datareader_qos(in_dds DataReaderQos *p_qos);
	ReturnCode_t static_get_default_datareader_qos(inout_dds DataReaderQos *p_qos);

	int get_domain_participant_factory_module_id();
	WaitSet* waitset_new();

	void quit();
	bool is_quit();
	bool is_terminated();

	char *get_string(char *p_chars, int *p_size);

	//////////

	expression_t *expression_parse (const char *expression);
	void expression_check_field(expression_t *p_expr, FooTypeSupport *p_footypesupport);
	int set_string_rtps_to_type_support(dds_string2* p_string, char* p_data, int i_parameter_string_size, int packet_size, long offset);
	long caculate_message_data_type_support_to_rtps(message_t *p_message, FooTypeSupport* p_type_support, char* p_send_value, int i_type_support_size, char* p_value);

	instanceset_t *instaceset_new(message_t *p_message, uint32_t depth, uint32_t max_samples_per_instance);
	void insert_instanceset_datareader(DataReader *p_datareader, rtps_cachechange_t *p_rtps_cachechange, message_t *p_message);
	void insert_instanceset_datawriter(DataWriter *p_datawriter, rtps_cachechange_t *p_rtps_cachechange, message_t *p_message);
	void instanceset_remove(Entity *p_entity, instanceset_t *p_instaceset);
	void remove_in_order(Entity* p_entity, message_t* p_message);
	void remove_in_instanceset(message_t *p_message);
	

	/////////////////////

	ReturnCode_t status_condition_plain_status_set(Entity* p_entity, StatusKind kind);
	ReturnCode_t data_on_readers_statuscondition(Entity * subscriber_entity);
	ReturnCode_t data_available_statuscondition(Entity * reader_entity) ;
	ReturnCode_t change_DataOnReaders_DataAvailable_status(DataReader* p_datareader);
	ReturnCode_t change_RequestedDeadlineMissed_status(DataReader* p_datareader,InstanceHandle_t handle);
	ReturnCode_t change_RequestedIncompatibleQoS_status(DataReader* p_datareader,QosPolicyId_t id);
	ReturnCode_t change_OfferedIncompatibleQoS_status(DataWriter* p_datawriter, QosPolicyId_t id);
	ReturnCode_t change_SubscriberMathed_status(DataReader* p_datareader, int current_count, InstanceHandle_t handle);
	ReturnCode_t change_SubscriberMathed_CurrentCount_status(DataReader* p_datareader, int current_count);
	ReturnCode_t change_PublicationMathced_status(DataWriter* p_datawriter, int current_count, InstanceHandle_t handle);
	ReturnCode_t change_publication_mathced_current_count_status(DataWriter* p_datawriter, int current_count);
	ReturnCode_t change_OfferedDeadlineMissed_status(DataWriter * p_datawriter,InstanceHandle_t handle);
	ReturnCode_t change_SampleRejected_status(DataReader * p_datareader,SampleRejectedStatusKind reason, InstanceHandle_t handle);
	ReturnCode_t change_sample_lost_status(DataReader* p_datareader);
	ReturnCode_t change_topic_status(Topic* p_topic);
	void init_subscriber_status(Subscriber* p_subscriber);
	void init_topic_status(Topic* p_topic);
	void get_key_guid(message_t* p_message, uint8_t* p_data, int size);
	DataReader* get_builtin_participant_reader();
	DataReader* get_builtin_topic_reader();
	DataReader* get_builtin_publisher_reader();
	DataReader* get_builtin_subscriber_reader();


	void get_builtin_participant_data(ParticipantBuiltinTopicData* p_participant_data, void* p_cache);
	void get_publication_builtin_topic_data(PublicationBuiltinTopicData* p_publication_data, void* p_cache);
	bool is_matched_remote_publisher(char* p_topic_name, char* p_type_name, void* p_cache);
	bool is_matched_remote_subscriber(char* p_topic_name, char* p_type_name, void* p_cache);
	void get_subscription_builtin_topic_data(SubscriptionBuiltinTopicData* p_subscription_data, void* p_cache);
	void get_builtin_topic_data(TopicBuiltinTopicData* p_topic_data, void* p_cache);

	void ignore_writerproxy(void* v_reader, void* v_key);
	void ignore_writerproxy_guid_prefix(void* v_reader, void* v_key);
	void ignore_readerproxy(void* v_writer, void* v_key);
	void ignore_readerproxy_guid_prefix(void* v_writer, void* v_key);

	////////////////////////////

	void check_new_data_sample_status(Entity* p_entity, instanceset_t* p_instanceset, message_t* p_message);
	void check_new_data_instance_stautus(Entity* p_entity, instanceset_t* p_instanceset, message_t* p_message);
	void check_newData_viewStautus(Entity * p_entity, instanceset_t * p_instanceset, message_t *p_message);
	void check_instanceStatus_disposed(Entity *p_entity, instanceset_t *p_instanceset);
	void check_instanceStatus_unregister(Entity * p_entity, instanceset_t * p_instanceset);
	void read_instance_change_sample_status(DataReader* p_datareader, instanceset_t* p_instanceset);
	void read_instance_change_instance_status(DataReader* p_datareader, instanceset_t* p_instanceset);
	void read_change_sample_status(DataReader* p_datareader, int i);
	void read_change_instance_status(DataReader* p_datareader, int i);
	void take_remove_instance(DataReader * p_datareader, instanceset_t * p_instanceset);
	void take_instance_change_instance_status(DataReader* p_datareader, instanceset_t* p_instanceset);
	void take_change_instance_status(DataReader* p_datareader, int i);
	ReturnCode_t read_condition_set(DataReader* p_datareader, instanceset_t* p_instanceset);
	ReturnCode_t read_condition_set_from_add_instanceset(DataReader* p_datareader, instanceset_t* p_instanceset, message_t *p_message);
	ReturnCode_t check_all_samples_sample_status(instanceset_t* p_instanceset);
	void qosim_dcps_message_ordering_change(DataReader *p_datareader);


	///
	void take_change_instance_status_querycondition( DataReader* p_datareader, message_t *p_message, int i );


	///
	void set_global_domain_id(const DomainId_t domain_id);
	DomainId_t get_global_domain_id(void);

	uint32_t get_millisecond(const Duration_t duration);

#ifdef __cplusplus
}



#endif

#endif