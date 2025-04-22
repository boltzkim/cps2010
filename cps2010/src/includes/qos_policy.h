/*******************************************************************************
 * Copyright (c) 2011 Electronics and Telecommunications Research Institute
 * (ETRI) All Rights Reserved.
 * 
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR in CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	QoS Policy 관련 헤더
	
	이력
	2012-11-06 : 시작

*/

#if !defined(__QOS_POLICY_H)
#define __QOS_POLICY_H


#ifndef RTPS_READER_T_DEFINE
	#define RTPS_READER_T_DEFINE 1
	typedef struct rtps_reader_t rtps_reader_t;
#endif

#ifndef SERIALIZED_PAYLOAD_FOR_READER_DEFINE
	#define SERIALIZED_PAYLOAD_FOR_READER_DEFINE 1
	typedef struct SerializedPayloadForReader SerializedPayloadForReader;
#endif

#ifndef GUID_T_DEFINE
	#define GUID_T_DEFINE 1
	typedef struct GUID_t GUID_t;
#endif

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

#ifndef PUBLISHER_QOS_DEFINE
	#define PUBLISHER_QOS_DEFINE 1
	typedef struct PublisherQos PublisherQos;
#endif

#ifndef SUBSCRIBER_QOS_DEFINE
	#define SUBSCRIBER_QOS_DEFINE 1
	typedef struct SubscriberQos SubscriberQos;
#endif

#ifndef TOPIC_QOS_DEFINE
	#define TOPIC_QOS_DEFINE 1
	typedef struct TopicQos TopicQos;
#endif

#ifndef DATA_WRITER_DEFINE
	#define DATA_WRITER_DEFINE 1
	typedef struct DataWriter DataWriter;
#endif

#ifndef DATA_WRITER_QOS_DEFINE
	#define DATA_WRITER_QOS_DEFINE 1
	typedef struct DataWriterQos DataWriterQos;
#endif

#ifndef DATA_READER_DEFINE
	#define DATA_READER_DEFINE 1
	typedef struct DataReader DataReader;
#endif

#ifndef DATA_READER_QOS_DEFINE
	#define DATA_READER_QOS_DEFINE 1
	typedef struct DataReaderQos DataReaderQos;
#endif

#ifndef WRITER_DATA_LIFECYCLE_QOS_POLICY_DEFINE
	#define WRITER_DATA_LIFECYCLE_QOS_POLICY_DEFINE 1
	typedef struct WriterDataLifecycleQosPolicy WriterDataLifecycleQosPolicy;
#endif

#ifndef ENTITY_FACTORY_QOS_POLICY_DEFINE
	#define ENTITY_FACTORY_QOS_POLICY_DEFINE 1
	typedef struct EntityFactoryQosPolicy EntityFactoryQosPolicy;
#endif

#ifndef HISTORY_QOS_POLICY_DEFINE
	#define HISTORY_QOS_POLICY_DEFINE 1
	typedef struct HistoryQosPolicy HistoryQosPolicy;
#endif

#ifndef RESOURCE_LIMITS_QOS_POLICY_DEFINE
	#define RESOURCE_LIMITS_QOS_POLICY_DEFINE 1
	typedef struct ResourceLimitsQosPolicy ResourceLimitsQosPolicy;
#endif

#ifndef RTPS_HISTORYCACHE_T_DEFINE
	#define RTPS_HISTORYCACHE_T_DEFINE 1
	typedef struct rtps_historycache_t rtps_historycache_t;
#endif

#ifndef RTPS_CACHECHANGE_T_DEFINE
	#define RTPS_CACHECHANGE_T_DEFINE 1
	typedef struct rtps_cachechange_t rtps_cachechange_t;
#endif

#ifndef INSTANCE_STATE_KIND_DEFINE
	#define INSTANCE_STATE_KIND_DEFINE 1
	typedef enum InstanceStateKind InstanceStateKind;
#endif

#ifndef RXO_QOS_DEFINE
	#define RXO_QOS_DEFINE 1
	typedef struct RxOQos RxOQos;
#endif

#ifndef TOPIC_BUILTIN_TOPIC_DATA_DEFINE
	#define TOPIC_BUILTIN_TOPIC_DATA_DEFINE 1
	typedef struct TopicBuiltinTopicData TopicBuiltinTopicData;
#endif

#ifndef PUBLICATION_BUILTIN_TOPIC_DATA_DEFINE
	#define PUBLICATION_BUILTIN_TOPIC_DATA_DEFINE 1
	typedef struct PublicationBuiltinTopicData PublicationBuiltinTopicData;
#endif

#ifndef SUBSCRIPTION_BUILTIN_TOPIC_DATA_DEFINE
	#define SUBSCRIPTION_BUILTIN_TOPIC_DATA_DEFINE 1
	typedef struct SubscriptionBuiltinTopicData SubscriptionBuiltinTopicData;
#endif

#ifndef USER_DATA_QOS_POLICY_DEFINE
	#define USER_DATA_QOS_POLICY_DEFINE 1
	typedef struct UserDataQosPolicy UserDataQosPolicy;
#endif

#ifndef DOMAIN_PARTICIPANT_DEFINE
	#define DOMAIN_PARTICIPANT_DEFINE 1
	typedef struct DomainParticipant DomainParticipant;
#endif

#ifndef DOMAIN_PARTICIPANT_QOS_DEFINE
	#define DOMAIN_PARTICIPANT_QOS_DEFINE 1
	typedef struct DomainParticipantQos DomainParticipantQos;
#endif

#ifndef TOPIC_DATA_QOS_POLICY_DEFINE
	#define TOPIC_DATA_QOS_POLICY_DEFINE 1
	typedef struct TopicDataQosPolicy TopicDataQosPolicy;
#endif

#ifndef GROUP_DATA_QOS_POLICY_DEFINE
	#define GROUP_DATA_QOS_POLICY_DEFINE 1
	typedef struct GroupDataQosPolicy GroupDataQosPolicy;
#endif

#ifndef PARTITION_QOS_POLICY_DEFINE
	#define PARTITION_QOS_POLICY_DEFINE 1
	typedef struct PartitionQosPolicy PartitionQosPolicy;
#endif

///////////////////////////추가///////////////////////////
#ifndef PUBLISHER_DEFINE
	#define PUBLISHER_DEFINE 1
	typedef struct Publisher Publisher;
#endif

#ifndef SUBSCRIBER_DEFINE
	#define SUBSCRIBER_DEFINE 1
	typedef struct Subscriber Subscriber;
#endif

#ifndef RTPS_STATEFULREADER_T_DEFINE
	#define RTPS_STATEFULREADER_T_DEFINE 1
	typedef struct rtps_statefulreader_t rtps_statefulreader_t;
#endif

#ifndef RTPS_WRITERPROXY_T_DEFINE
	#define RTPS_WRITERPROXY_T_DEFINE 1
	typedef struct rtps_writerproxy_t rtps_writerproxy_t;
#endif
/////////////////////////////////////////////////////////



typedef struct liveliness_writer_t
{
	DataWriter* p_datawriter;
	Duration_t	lease_duration;
} liveliness_writer_t;

struct qos_policy_t
{
	MODULE_COMMON_FIELD

	moduleload_t*	p_moduleload;

	ReturnCode_t (*check_immutable_policy_before_change_publisher_qos)(qos_policy_t* const p_this, const Publisher* const p_publisher, const PublisherQos* const p_new_qos);
	ReturnCode_t (*check_immutable_policy_before_change_subscriber_qos)(qos_policy_t* const p_this, const Subscriber* const p_subscriber, const SubscriberQos* const p_new_qos);
	ReturnCode_t (*check_immutable_policy_before_change_topic_qos)(qos_policy_t* const p_this, const Topic* const p_topic, const TopicQos* const p_new_qos);
	ReturnCode_t (*check_immutable_policy_before_change_datawriter_qos)(qos_policy_t* const p_this, const DataWriter* const p_datawriter, const DataWriterQos* const p_new_qos);
	ReturnCode_t (*check_immutable_policy_before_change_datareader_qos)(qos_policy_t* const p_this, const DataReader* const p_datareader, const DataReaderQos* const p_new_qos);
	ReturnCode_t (*check_inconsistent_policy_before_change_topic_qos)(qos_policy_t* const p_this, const TopicQos* const p_new_qos);
	ReturnCode_t (*check_inconsistent_policy_before_change_datawriter_qos)(qos_policy_t* const p_this, const DataWriterQos* const p_new_qos);
	ReturnCode_t (*check_inconsistent_policy_before_change_datareader_qos)(qos_policy_t* const p_this, const DataReaderQos* const p_new_qos);
	void (*send_publication_info_after_change_datawriter_qos)(qos_policy_t* const p_this, DataWriter* const p_datawriter);
	void (*send_subscription_info_after_change_datareader_qos)(qos_policy_t* const p_this, DataReader* const p_datareader);

	//////////////////////////////////////////////////////////////////////////
	// Entity Factory QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*is_enabled)(qos_policy_t* const p_this, const Entity* const p_entity);
	void (*create_ext_entity_factory_qos)(qos_policy_t* const p_this, Entity* const p_entity, const Entity* const p_factory, const EntityFactoryQosPolicy* const p_entity_factory_qos);
	ReturnCode_t (*enable_topic)(qos_policy_t* const p_this, Entity* p_entity);
	ReturnCode_t (*enable_datawriter)(qos_policy_t* const p_this, Entity* p_entity);
	ReturnCode_t (*enable_datareader)(qos_policy_t* const p_this, Entity* p_entity);
	ReturnCode_t (*enable_publisher)(qos_policy_t* const p_this, Entity* p_entity);
	ReturnCode_t (*enable_subscriber)(qos_policy_t* const p_this, Entity* p_entity);
	ReturnCode_t (*enable_domainparticipant)(qos_policy_t* const p_this, Entity* p_entity);

	//////////////////////////////////////////////////////////////////////////
	// History QoS Policy and Reliability QoS Policy
	//////////////////////////////////////////////////////////////////////////
	int32_t (*get_history_max_length)(qos_policy_t* const p_this, const HistoryQosPolicy* const p_history_qos, const ResourceLimitsQosPolicy* const p_resource_limits_qos);
	bool (*check_historycache_for_writer)(qos_policy_t* const p_this, rtps_writer_t* p_writer, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);
	bool (*check_historycache_for_reader)(qos_policy_t* const p_this, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);

	//////////////////////////////////////////////////////////////////////////
	// Liveliness QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool	b_operation_called;
	mutex_t	operation_called_lock;
	bool	b_operation_called_lock_initialize;
	liveliness_writer_t**	pp_automatic_liveliness_writers;
	int32_t					i_automatic_liveliness_writers;
	liveliness_writer_t**	pp_manual_liveliness_writers;
	int32_t					i_manual_liveliness_writers;
	uint32_t				sent_time;
	bool b_end_liveliness;
	void (*operation_called_for_liveliness_qos)(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void (*set_stop_liveliness)(qos_policy_t* const p_this, const bool state);
	void (*liveliness_thread)(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	void (*add_liveliness_writer)(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void (*send_heartbeat_for_topic_liveliness)(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void (*writerproxy_set_liveliness_kind_and_lease_duration)(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy, const RxOQos rxo_qos);
	void (*writer_set_liveliness_kind_and_lease_duration)(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	void (*participant_liveliness_set_alive)(qos_policy_t* const p_this, const SerializedPayloadForReader* const p_serialized);
	void (*liveliness_check_wakeup_time_for_reader)(qos_policy_t* const p_this, data_t* const p_job);
	void (*liveliness_check_wakeup_time_for_writer)(qos_policy_t* const p_this, data_t* const p_job);
	void (*manual_by_topic_liveliness_set_alive)(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy);
	void (*liveliness_writer_set_alive)(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	bool (*is_liveliness_job)(qos_policy_t* const p_this, const data_t* const p_job);
	data_t* (*add_liveliness_job_for_datareader)(qos_policy_t* const p_this, const DataReader* const p_datareader);
	data_t* (*add_liveliness_job_for_datawriter)(qos_policy_t* const p_this, const DataWriter* const p_datawriter);
	data_t* (*remove_liveliness_job_for_datareader)(qos_policy_t* const p_this, const DataReader* const p_datareader);
	data_t* (*remove_liveliness_job_for_datawriter)(qos_policy_t* const p_this, const DataWriter* const p_datawriter);

	//////////////////////////////////////////////////////////////////////////
	// Ownership QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*has_exclusive_ownership)(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader);
	void (*insert_exclusive_writer)(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, rtps_writerproxy_t* const p_rtps_writerproxy);
	int32_t (*find_exclusive_writer)(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);
	int32_t (*set_exclusive_writer_key)(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const SerializedPayloadForReader* const p_serialized);
	int (*change_owner_writer)(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const bool b_is_include);
	void (*remove_exclusive_writer)(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// Reader Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*remove_disposed_instance)(qos_policy_t* const p_this, DataReader* p_datareader);
	void (*rtps_reader_set_instance_state_of_cachechanges_with_guid)(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, InstanceStateKind instance_state, const GUID_t* const p_guid);

	//////////////////////////////////////////////////////////////////////////
	// Resource Limits QoS Policy
	//////////////////////////////////////////////////////////////////////////
	ReturnCode_t (*check_resource_limits)(qos_policy_t* const p_this, const ResourceLimitsQosPolicy* const p_resource_limits_qos/*, const Topic* const p_topic*/);

	//////////////////////////////////////////////////////////////////////////
	// Writer Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*is_autodispose_unregistered_instances)(qos_policy_t* const p_this, const WriterDataLifecycleQosPolicy* const p_writer_data_lifecycle_qos);

	//////////////////////////////////////////////////////////////////////////
	// Durability Service QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*durability_service_load)(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer);
	void (*durability_service_save)(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer);

	//////////////////////////////////////////////////////////////////////////
	// Deadline QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*compare_deadline)(qos_policy_t* const p_this, Duration_t receive_time, Duration_t* base_time, Duration_t deadline_period);

	//////////////////////////////////////////////////////////////////////////
	// Time Based Filter QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*compare_timebasedfilter)(qos_policy_t* const p_this, Duration_t receive_time, Duration_t* p_base_time, Duration_t minimum_separation);

	//////////////////////////////////////////////////////////////////////////
	// Lifespan QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*find_lifespan_qos_from_parameterlist)(qos_policy_t* const p_this, rtps_writerproxy_t* p_writerproxy, char* p_value, int32_t i_size);
	void (*input_lifespan)(qos_policy_t* const p_this, rtps_cachechange_t* p_rtps_cachechange, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// User Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*find_user_data_qos_from_parameterlist)(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data_qos, char* p_value, int32_t i_size);
	void (*init_user_data_qos)(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data);
	void (*copy_user_data_qos)(qos_policy_t* const p_this, UserDataQosPolicy* p_dst_user_data, UserDataQosPolicy* p_src_user_data);
	void (*set_qos_about_user_data_qos_from_datareader)(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos);
	void (*get_qos_about_user_data_qos_from_datareader)(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos);
	void (*set_qos_about_user_data_qos_from_datawriter)(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos);
	void (*get_qos_about_user_data_qos_from_datawriter)(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos);
	void (*set_qos_about_user_data_qos_from_domainparticipant)(qos_policy_t* const p_this, DomainParticipant* p_domainparticipant, DomainParticipantQos* p_qos);
	void (*print_user_data_qos)(qos_policy_t* const p_this, UserDataQosPolicy user_data_qos);
	int (*generate_user_data)(qos_policy_t* const p_this, char* p_serialized, UserDataQosPolicy a_user_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Topic Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*find_topic_data_qos_from_parameterlist)(qos_policy_t* const p_this, TopicDataQosPolicy* p_topic_data_qos, char* p_value, int32_t i_size);
	void (*print_topic_data_qos)(qos_policy_t* const p_this, TopicDataQosPolicy topic_data_qos);
	void (*init_topic_data_qos)(qos_policy_t* const p_this, TopicDataQosPolicy* p_topic_data);
	void (*copy_topic_data_qos)(qos_policy_t* const p_this, TopicDataQosPolicy* p_dst_topic_data, TopicDataQosPolicy* p_src_topic_data);
	void (*set_qos_about_topic_data_qos_from_topic)(qos_policy_t* const p_this, Topic* p_topic, TopicQos* p_qos);
	void (*get_qos_about_topic_data_qos_from_topic)(qos_policy_t* const p_this, Topic* p_topic, TopicQos* p_qos);
	int (*generate_topic_data)(qos_policy_t* const p_this, char* p_serialized, TopicDataQosPolicy a_topic_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Group Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void (*find_group_data_qos_from_parameterlist)(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data_qos, char* p_value, int32_t i_size);
	void (*print_group_data_qos)(qos_policy_t* const p_this, GroupDataQosPolicy group_data_qos);
	void (*init_group_data_qos)(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data);
	void (*copy_group_data_qos)(qos_policy_t* const p_this, GroupDataQosPolicy* p_dst_group_data, GroupDataQosPolicy* p_src_group_data);
	void (*set_qos_about_group_data_qos_from_publisher)(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos);
	void (*get_qos_about_group_data_qos_from_publisher)(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos);
	void (*set_qos_about_group_data_qos_from_subscriber)(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos* p_qos);
	void (*get_qos_about_group_data_qos_from_subscriber)(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos* p_qos);
	int (*generate_group_data)(qos_policy_t* const p_this, char* p_serialized, GroupDataQosPolicy a_group_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Partition Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool (*compare_partition)(qos_policy_t* const p_this, PartitionQosPolicy a_partition, PartitionQosPolicy b_partition);
	void (*find_partition_qos_from_parameterlist)(qos_policy_t* const p_this, PartitionQosPolicy *p_partition_qos, char* p_value, int32_t i_size);
	int (*generate_partition)(qos_policy_t* const p_this, char* p_serialized, PartitionQosPolicy a_partition_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Find Entity
	//////////////////////////////////////////////////////////////////////////
	void (*find_topic_qos_from_parameterlist)(qos_policy_t* const p_this, TopicBuiltinTopicData* p_topic_data, char* p_value, int32_t i_size);
	void (*find_publication_qos_from_parameterlist)(qos_policy_t* const p_this, PublicationBuiltinTopicData* p_publication_data, char* p_value, int32_t i_size);
	void (*find_subscription_qos_from_parameterlist)(qos_policy_t* const p_this, SubscriptionBuiltinTopicData* p_subscription_data, char* p_value, int32_t i_size);
};


#ifdef __cplusplus
extern "C" {
#endif

	int	launchQosPolicyModule(module_object_t* p_this);

	ReturnCode_t qos_check_immutable_policy_before_change_publisher_qos(const Publisher* const p_publisher, const PublisherQos* const p_new_qos);
	ReturnCode_t qos_check_immutable_policy_before_change_subscriber_qos(const Subscriber* const p_subscriber, const SubscriberQos* const p_new_qos);
	ReturnCode_t qos_check_immutable_policy_before_change_topic_qos(const Topic* const p_topic, const TopicQos* const p_new_qos);
	ReturnCode_t qos_check_immutable_policy_before_change_datawriter_qos(const DataWriter* const p_datawriter, const DataWriterQos* const p_new_qos);
	ReturnCode_t qos_check_immutable_policy_before_change_datareader_qos(const DataReader* const p_datareader, const DataReaderQos* const p_new_qos);
	ReturnCode_t qos_check_inconsistent_policy_before_change_topic_qos(const TopicQos* const p_new_qos);
	ReturnCode_t qos_check_inconsistent_policy_before_change_datawriter_qos(const DataWriterQos* const p_new_qos);
	ReturnCode_t qos_check_inconsistent_policy_before_change_datareader_qos(const DataReaderQos* const p_new_qos);
	void qos_send_publication_info_after_change_datawriter_qos(DataWriter* const p_datawriter);
	void qos_send_subscription_info_after_change_datareader_qos(DataReader* const p_datareader);

	//////////////////////////////////////////////////////////////////////////
	// Entity Factory QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_is_enabled(const Entity* const p_entity);
	void qos_create_ext_entity_factory_qos(Entity* const p_entity, const Entity* const p_factory, const EntityFactoryQosPolicy* const p_entity_factory_qos);
	ReturnCode_t qos_enable_topic(Entity *p_entity);
	ReturnCode_t qos_enable_datawriter(Entity *p_entity);
	ReturnCode_t qos_enable_datareader(Entity *p_entity);
	ReturnCode_t qos_enable_publisher(Entity *p_entity);
	ReturnCode_t qos_enable_subscriber(Entity *p_entity);
	ReturnCode_t qos_enable_domainparticipant(Entity *p_entity);

	//////////////////////////////////////////////////////////////////////////
	// History QoS Policy and Reliability QoS Policy
	//////////////////////////////////////////////////////////////////////////
	int32_t qos_get_history_max_length(const HistoryQosPolicy* const p_history_qos, const ResourceLimitsQosPolicy* const p_resource_limits_qos);
	bool qos_check_historycache_for_writer(rtps_writer_t *p_writer, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);
	bool qos_check_historycache_for_reader(rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);

	//////////////////////////////////////////////////////////////////////////
	// Liveliness QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_operation_called_for_liveliness_qos(const DataWriter* const p_data_writer);
	void qos_set_stop_liveliness(const bool state);
	void qos_liveliness_thread(rtps_writer_t* const p_rtps_writer);
	void qos_add_liveliness_writer(const DataWriter* const p_data_writer);
	void qos_send_heartbeat_for_topic_liveliness(const DataWriter* const p_data_writer);
	void qos_writerproxy_set_liveliness_kind_and_lease_duration(rtps_writerproxy_t* const p_writerproxy, const RxOQos rxo_qos);
	void qos_writer_set_liveliness_kind_and_lease_duration(rtps_writer_t* const p_rtps_writer);
	void qos_participant_liveliness_set_alive(const SerializedPayloadForReader* const p_serialized);
	void qos_liveliness_check_wakeup_time_for_reader(data_t* const p_job);
	void qos_liveliness_check_wakeup_time_for_writer(data_t* const p_job);
	void qos_manual_by_topic_liveliness_set_alive(rtps_writerproxy_t* const p_writerproxy);
	void qos_liveliness_writer_set_alive(rtps_writer_t* const p_rtps_writer);
	bool qos_is_liveliness_job(const data_t* const p_job);
	data_t* qos_add_liveliness_job_for_datareader(const DataReader* const p_datareader);
	data_t* qos_add_liveliness_job_for_datawriter(const DataWriter* const p_datawriter);
	data_t* qos_remove_liveliness_job_for_datareader(const DataReader* const p_datareader);
	data_t* qos_remove_liveliness_job_for_datawriter(const DataWriter* const p_datawriter);

	//////////////////////////////////////////////////////////////////////////
	// Ownership QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_has_exclusive_ownership(const rtps_reader_t* const p_rtpsReader);
	void qos_insert_exclusive_writer(rtps_reader_t* const p_rtps_reader, rtps_writerproxy_t* const p_rtps_writerproxy);
	int32_t qos_find_exclusive_writer(const rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);
	int32_t qos_set_exclusive_writer_key(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const SerializedPayloadForReader* const p_serialized);
	int qos_change_owner_writer(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const bool b_is_include);
	void qos_remove_exclusive_writer(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// Reader Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_remove_disposed_instance(DataReader* p_datareader);
	void qos_rtps_reader_set_instance_state_of_cachechanges_with_guid(const rtps_reader_t* const p_rtps_reader, InstanceStateKind instance_state, const GUID_t* const guid);

	//////////////////////////////////////////////////////////////////////////
	// Resource Limits QoS Policy
	//////////////////////////////////////////////////////////////////////////
	ReturnCode_t qos_check_resource_limits(const ResourceLimitsQosPolicy* const p_resource_limits_qos/*, const Topic* const p_topic*/);

	//////////////////////////////////////////////////////////////////////////
	// Writer Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_is_autodispose_unregistered_instances(const WriterDataLifecycleQosPolicy* const p_writer_data_lifecycle_qos);

	//////////////////////////////////////////////////////////////////////////
	// Durability Service QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_durability_service_load(const rtps_writer_t* const p_rtps_writer);
	void qos_durability_service_save(const rtps_writer_t* const p_rtps_writer);

	//////////////////////////////////////////////////////////////////////////
	// Deadline QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_compare_deadline(Duration_t receive_time, Duration_t* p_base_time, Duration_t deadline_period);

	//////////////////////////////////////////////////////////////////////////
	// Time Based Filter QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_compare_timebasedfilter(Duration_t receive_time, Duration_t* p_base_time, Duration_t minimum_separation);

	//////////////////////////////////////////////////////////////////////////
	// Lifespan QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_find_lifespan_qos_from_parameterlist(rtps_writerproxy_t* p_writerproxy, char* p_value, int32_t i_size);
	void qos_input_lifespan(rtps_cachechange_t* p_rtps_cachechange, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// User Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_find_user_data_qos_from_parameterlist(UserDataQosPolicy* p_user_data_qos, char* p_value, int32_t i_size);
	void qos_init_user_data_qos(UserDataQosPolicy* p_user_data);
	void qos_copy_user_data_qos(UserDataQosPolicy* p_dst_user_data, UserDataQosPolicy* p_src_user_data);
	void qos_set_qos_about_user_data_qos_from_datareader(DataReader* p_datareader, DataReaderQos* p_qos);
	void qos_get_qos_about_user_data_qos_from_datareader(DataReader* p_datareader, DataReaderQos* p_qos);
	void qos_set_qos_about_user_data_qos_from_datawriter(DataWriter* p_datawriter, DataWriterQos* p_qos);
	void qos_get_qos_about_user_data_qos_from_datawriter(DataWriter* p_datawriter, DataWriterQos* p_qos);
	void qos_set_qos_about_user_data_qos_from_domainparticipant(DomainParticipant* p_domainparticipant, DomainParticipantQos* p_qos);
	void qos_print_user_data_qos(UserDataQosPolicy user_data_qos);
	int qos_generate_user_data(char* p_serialized, UserDataQosPolicy a_user_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Topic Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_find_topic_data_qos_from_parameterlist(TopicDataQosPolicy* p_topic_data_qos, char* p_value, int32_t i_size);
	void qos_print_topic_data_qos(TopicDataQosPolicy topic_data_qos);
	void qos_init_topic_data_qos(TopicDataQosPolicy* p_topic_data);
	void qos_copy_topic_data_qos(TopicDataQosPolicy* p_dst_topic_data, TopicDataQosPolicy* p_src_topic_data);
	void qos_set_qos_about_topic_data_qos_from_topic(Topic* p_topic, TopicQos* p_qos);
	void qos_get_qos_about_topic_data_qos_from_topic(Topic* p_topic, TopicQos* p_qos);
	int qos_generate_topic_data(char* p_serialized, TopicDataQosPolicy a_topic_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Group Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qos_find_group_data_qos_from_parameterlist(GroupDataQosPolicy* p_group_data_qos, char* p_value, int32_t i_size);
	void qos_print_group_data_qos(GroupDataQosPolicy group_data_qos);
	void qos_init_group_data_qos(GroupDataQosPolicy* p_group_data);
	void qos_copy_group_data_qos(GroupDataQosPolicy* p_dst_group_data, GroupDataQosPolicy* p_src_group_data);
	void qos_set_qos_about_group_data_qos_from_publisher(Publisher* p_publisher, PublisherQos* p_qos);
	void qos_get_qos_about_group_data_qos_from_publisher(Publisher* p_publisher, PublisherQos* p_qos);
	void qos_set_qos_about_group_data_qos_from_subscriber(Subscriber* p_subscriber, SubscriberQos* p_qos);
	void qos_get_qos_about_group_data_qos_from_subscriber(Subscriber* p_subscriber, SubscriberQos *p_qos);
	int qos_generate_group_data(char* p_serialized, GroupDataQosPolicy a_group_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Partition Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qos_compare_partition(PartitionQosPolicy a_partition, PartitionQosPolicy b_partition);
	void qos_find_partition_qos_from_parameterlist(PartitionQosPolicy *p_partition_qos, char* p_value, int32_t i_size);
	int qos_generate_partition(char* p_serialized, PartitionQosPolicy a_partition_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Find Entity
	//////////////////////////////////////////////////////////////////////////
	void qos_find_topic_qos_from_parameterlist(TopicBuiltinTopicData* p_topic_data, char* p_value, int32_t i_size);
	void qos_find_publication_qos_from_parameterlist(PublicationBuiltinTopicData* p_publication_data, char* p_value, int32_t i_size);
	void qos_find_subscription_qos_from_parameterlist(SubscriptionBuiltinTopicData* p_subscription_data, char* p_value, int32_t i_size);

#ifdef __cplusplus
}
#endif


#endif
