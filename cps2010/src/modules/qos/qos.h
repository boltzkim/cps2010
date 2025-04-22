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
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	QoS Policy 관련 헤더
	작성자 : 
	이력
	2012-11-06 : 시작

*/

#if !defined(__QOS_H)
#define __QOS_H


#define QOS_PARTITION_STRING_SIZE 128

typedef struct dds_partition_string
{
	int32_t size;
	char value[QOS_PARTITION_STRING_SIZE];
}dds_partition_string;


#ifdef __cplusplus
extern "C" {
#endif
	ReturnCode_t qosim_check_immutable_policy_before_change_publisher_qos(qos_policy_t* const p_this, const Publisher* const p_publisher, const PublisherQos* const p_qos);
	ReturnCode_t qosim_check_immutable_policy_before_change_subscriber_qos(qos_policy_t* const p_this, const Subscriber* const p_subscriber, const SubscriberQos* const p_qos);
	ReturnCode_t qosim_check_immutable_policy_before_change_topic_qos(qos_policy_t* const p_this, const Topic* const p_topic, const TopicQos* const p_qos);
	ReturnCode_t qosim_check_immutable_policy_before_change_datawriter_qos(qos_policy_t* const p_this, const DataWriter* const p_datawriter, const DataWriterQos* const p_qos);
	ReturnCode_t qosim_check_immutable_policy_before_change_datareader_qos(qos_policy_t* const p_this, const DataReader* const p_datareader, const DataReaderQos* const p_qos);
	ReturnCode_t qosim_check_inconsistent_policy_before_change_topic_qos(qos_policy_t* const p_this, const TopicQos* const p_new_qos);
	ReturnCode_t qosim_check_inconsistent_policy_before_change_datawriter_qos(qos_policy_t* const p_this, const DataWriterQos* const p_new_qos);
	ReturnCode_t qosim_check_inconsistent_policy_before_change_datareader_qos(qos_policy_t* const p_this, const DataReaderQos* const p_new_qos);
	void qosim_send_publication_info_after_change_datawriter_qos(qos_policy_t* const p_this, DataWriter* const p_datawriter);
	void qosim_send_subscription_info_after_change_datareader_qos(qos_policy_t* const p_this, DataReader* const p_datareader);

	//////////////////////////////////////////////////////////////////////////
	// Entity Factory QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_is_enabled(qos_policy_t* const p_this, const Entity* const p_entity);
	void qosim_create_ext_entity_factory_qos(qos_policy_t* const p_this, Entity* const p_entity, const Entity* const p_factory, const EntityFactoryQosPolicy* const p_entity_factory_qos);
	ReturnCode_t qosim_enable_topic(qos_policy_t* const p_this, Entity *p_entity);
	ReturnCode_t qosim_enable_datawriter(qos_policy_t* const p_this, Entity *p_entity);
	ReturnCode_t qosim_enable_datareader(qos_policy_t* const p_this, Entity *p_entity);
	ReturnCode_t qosim_enable_publisher(qos_policy_t* const p_this, Entity *p_entity);
	ReturnCode_t qosim_enable_subscriber(qos_policy_t* const p_this, Entity *p_entity);
	ReturnCode_t qosim_enable_domainparticipant(qos_policy_t* const p_this, Entity *p_entity);

	//////////////////////////////////////////////////////////////////////////
	// History QoS Policy and Reliability QoS Policy
	//////////////////////////////////////////////////////////////////////////
	int32_t qosim_get_history_max_length(qos_policy_t* const p_this, const HistoryQosPolicy* const p_history_qos, const ResourceLimitsQosPolicy* const p_resource_limits_qos);
	bool qosim_check_historycache_for_writer(qos_policy_t* const p_this, rtps_writer_t *p_writer, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);
	bool qosim_check_historycache_for_reader(qos_policy_t* const p_this, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange);

	//////////////////////////////////////////////////////////////////////////
	// Liveliness QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_operation_called_for_liveliness_qos(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void qosim_set_stop_liveliness(qos_policy_t* const p_this, const bool state);
	void qosim_liveliness_thread(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	void qosim_add_liveliness_writer(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void qosim_send_heartbeat_for_topic_liveliness(qos_policy_t* const p_this, const DataWriter* const p_data_writer);
	void qosim_writerproxy_set_liveliness_kind_and_lease_duration(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy, const RxOQos rxo_qos);
	void qosim_writer_set_liveliness_kind_and_lease_duration(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	void qosim_participant_liveliness_set_alive(qos_policy_t* const p_this, const SerializedPayloadForReader* const p_serialized);
	void qosim_liveliness_check_wakeup_time_for_reader(qos_policy_t* const p_this, data_t* const p_job);
	void qosim_liveliness_check_wakeup_time_for_writer(qos_policy_t* const p_this, data_t* const p_job);
	void qosim_manual_by_topic_liveliness_set_alive(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy);
	void qosim_liveliness_writer_set_alive(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer);
	bool qosim_is_liveliness_job(qos_policy_t* const p_this, const data_t* const p_job);
	data_t* qosim_add_liveliness_job_for_datareader(qos_policy_t* const p_this, const DataReader* const p_datareader);
	data_t* qosim_add_liveliness_job_for_datawriter(qos_policy_t* const p_this, const DataWriter* const p_datawriter);
	data_t* qosim_remove_liveliness_job_for_datareader(qos_policy_t* const p_this, const DataReader* const p_datareader);
	data_t* qosim_remove_liveliness_job_for_datawriter(qos_policy_t* const p_this, const DataWriter* const p_datawriter);

	//////////////////////////////////////////////////////////////////////////
	// Ownership QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_has_exclusive_ownership(qos_policy_t* const p_this, const rtps_reader_t* const p_rtpsReader);
	void qosim_insert_exclusive_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, rtps_writerproxy_t* const p_rtps_writerproxy);
	//int32_t qosim_find_exclusive_writer(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, const GUID_t* const p_writer_guid);
	int32_t qosim_find_exclusive_writer(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);
	int32_t qosim_set_exclusive_writer_key(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const SerializedPayloadForReader* const p_serialized);
	int qosim_change_owner_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const bool b_is_include);
	void qosim_remove_exclusive_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// Reader Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_remove_disposed_instance(qos_policy_t* const p_this, DataReader* p_datareader);
	void qosim_rtps_reader_set_instance_state_of_cachechanges_with_guid(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, InstanceStateKind instance_state, const GUID_t* const p_guid);

	//////////////////////////////////////////////////////////////////////////
	// Resource Limits QoS Policy
	//////////////////////////////////////////////////////////////////////////
	ReturnCode_t qosim_check_resource_limits(qos_policy_t* const p_this, const ResourceLimitsQosPolicy* const p_resource_limits_qos/*, const Topic* const p_topic*/);

	//////////////////////////////////////////////////////////////////////////
	// Writer Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_is_autodispose_unregistered_instances(qos_policy_t* const p_this, const WriterDataLifecycleQosPolicy* const p_writer_data_lifecycle_qos);

	//////////////////////////////////////////////////////////////////////////
	// Durability Service QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_durability_service_load(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer);
	void qosim_durability_service_save(qos_policy_t* const p_this, const rtps_writer_t* const p_rtps_writer);

	//////////////////////////////////////////////////////////////////////////
	// Deadline QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_compare_deadline(qos_policy_t* const p_this, Duration_t receive_time, Duration_t* base_time, Duration_t deadline_period);

	//////////////////////////////////////////////////////////////////////////
	// Time Based Filter QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_compare_timebasedfilter(qos_policy_t* const p_this, Duration_t receive_time, Duration_t* p_base_time, Duration_t minimum_separation);

	//////////////////////////////////////////////////////////////////////////
	// Lifespan QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_find_lifespan_qos_from_parameterlist(qos_policy_t* const p_this, rtps_writerproxy_t* p_writerproxy, char* p_value, int32_t i_size);
	void qosim_input_lifespan(qos_policy_t* const p_this, rtps_cachechange_t* p_rtps_cachechange, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);

	//////////////////////////////////////////////////////////////////////////
	// User Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_find_user_data_qos_from_parameterlist(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data_qos, char* p_value, int32_t i_size);
	void qosim_init_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data);
	void qosim_copy_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy* p_dst_user_data, UserDataQosPolicy* p_src_user_data);
	void qosim_set_qos_about_user_data_qos_from_datareader(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos);
	void qosim_get_qos_about_user_data_qos_from_datareader(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos);
	void qosim_set_qos_about_user_data_qos_from_datawriter(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos);
	void qosim_get_qos_about_user_data_qos_from_datawriter(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos);
	void qosim_set_qos_about_user_data_qos_from_domainparticipant(qos_policy_t* const p_this, DomainParticipant* p_domainparticipant, DomainParticipantQos* p_qos);
	void qosim_print_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy user_data_qos);
	int qosim_generate_user_data(qos_policy_t* const p_this, char* p_serialized, UserDataQosPolicy a_user_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Topic Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_find_topic_data_qos_from_parameterlist(qos_policy_t* const p_this, TopicDataQosPolicy* p_topic_data_qos, char* p_value, int32_t i_size);
	void qosim_print_topic_data_qos(qos_policy_t* const p_this, TopicDataQosPolicy topic_data_qos);
	void qosim_init_topic_data_qos(qos_policy_t* const p_this, TopicDataQosPolicy* p_topic_data);
	void qosim_copy_topic_data_qos(qos_policy_t* const p_this, TopicDataQosPolicy* p_dst_topic_data, TopicDataQosPolicy* p_src_topic_data);
	void qosim_set_qos_about_topic_data_qos_from_topic(qos_policy_t* const p_this, Topic* p_topic, TopicQos* p_qos);
	void qosim_get_qos_about_topic_data_qos_from_topic(qos_policy_t* const p_this, Topic* p_topic, TopicQos* p_qos);
	int qosim_generate_topic_data(qos_policy_t* const p_this, char* p_serialized, TopicDataQosPolicy a_topic_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Group Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	void qosim_find_group_data_qos_from_parameterlist(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data_qos, char* p_value, int32_t i_size);
	void qosim_print_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy group_data_qos);
	void qosim_init_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data);
	void qosim_copy_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy* p_dst_group_data, GroupDataQosPolicy* p_src_group_data);
	void qosim_set_qos_about_group_data_qos_from_publisher(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos);
	void qosim_get_qos_about_group_data_qos_from_publisher(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos);
	void qosim_set_qos_about_group_data_qos_from_subscriber(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos* p_qos);
	void qosim_get_qos_about_group_data_qos_from_subscriber(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos *p_qos);
	int qosim_generate_group_data(qos_policy_t* const p_this, char* p_serialized, GroupDataQosPolicy a_group_data_qos, int i_size);

	//////////////////////////////////////////////////////////////////////////
	// Partition Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	bool qosim_compare_partition(qos_policy_t* const p_this, PartitionQosPolicy a_partition, PartitionQosPolicy b_partition);
	void qosim_find_partition_qos_from_parameterlist(qos_policy_t* const p_this, PartitionQosPolicy *p_partition_qos, char* p_value, int32_t i_size);
	int qosim_generate_partition(qos_policy_t* const p_this, char* p_serialized, PartitionQosPolicy a_partition_qos, int i_size);


	//////////////////////////////////////////////////////////////////////////
	// Find Entity
	//////////////////////////////////////////////////////////////////////////
	void qosim_find_topic_qos_from_parameterlist(qos_policy_t* const p_this, TopicBuiltinTopicData* p_topic_data, char* p_value, int32_t i_size);
	void qosim_find_publication_qos_from_parameterlist(qos_policy_t* const p_this, PublicationBuiltinTopicData* p_publication_data, char* p_value, int32_t i_size);
	void qosim_find_subscription_qos_from_parameterlist(qos_policy_t* const p_this, SubscriptionBuiltinTopicData* p_subscription_data, char* p_value, int32_t i_size);

	////
	///
	///

	int get_dds_partition_string(dds_partition_string* p_string, char* p_data, int string_size, int i_all_data_size);

#ifdef __cplusplus
}
#endif


#endif
