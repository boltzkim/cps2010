/* 
	Datawriter 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


#include "../../../src/modules/rtps/rtps.h"


static mutex_t	default_qos_lock;

static DataWriterQos *p_defaultDataWriterQos = NULL;
static DataWriterQos defaultDataWriterQos;


static ReturnCode_t set_qos(DataWriter* p_datawriter, in_dds DataWriterQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_datawriter && p_qos)
	{
		if (qos_check_inconsistent_policy_before_change_datawriter_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return RETCODE_INCONSISTENT_POLICY; //by kki
		if (qos_check_immutable_policy_before_change_datawriter_qos(p_datawriter, p_qos) == RETCODE_IMMUTABLE_POLICY) return RETCODE_IMMUTABLE_POLICY; //by kki

		//mutex_lock(&p_datawriter->entity_lock);

		p_datawriter->datawriter_qos.deadline.period.sec = p_qos->deadline.period.sec;
		p_datawriter->datawriter_qos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		p_datawriter->datawriter_qos.destination_order.kind = p_qos->destination_order.kind;
		p_datawriter->datawriter_qos.durability.kind = p_qos->durability.kind;
		p_datawriter->datawriter_qos.durability_service.history_depth = p_qos->durability_service.history_depth;
		p_datawriter->datawriter_qos.durability_service.history_kind = p_qos->durability_service.history_kind;
		p_datawriter->datawriter_qos.durability_service.max_instances = p_qos->durability_service.max_instances;
		p_datawriter->datawriter_qos.durability_service.max_samples = p_qos->durability_service.max_samples;
		p_datawriter->datawriter_qos.durability_service.max_samples_per_instance = p_qos->durability_service.max_samples_per_instance;
		p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.sec = p_qos->durability_service.service_cleanup_delay.sec;
		p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.nanosec = p_qos->durability_service.service_cleanup_delay.nanosec;
		p_datawriter->datawriter_qos.history.depth = p_qos->history.depth;
		p_datawriter->datawriter_qos.history.kind = p_qos->history.kind;
//by kki...(history && reliability)
		if (p_datawriter->p_related_rtps_writer)
		{
			rtps_historycache_t* p_writer_cache = ((rtps_writer_t*)p_datawriter->p_related_rtps_writer)->p_writer_cache;
			p_writer_cache->history_kind       = p_datawriter->datawriter_qos.history.kind;
			p_writer_cache->history_depth      = p_datawriter->datawriter_qos.history.depth;
			p_writer_cache->history_max_length = qos_get_history_max_length(&p_datawriter->datawriter_qos.history, &p_datawriter->datawriter_qos.resource_limits);//by kki
			p_writer_cache->reliability_kind   = p_datawriter->datawriter_qos.reliability.kind;
		}
		p_datawriter->datawriter_qos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		p_datawriter->datawriter_qos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		p_datawriter->datawriter_qos.lifespan.duration.sec = p_qos->lifespan.duration.sec;
		p_datawriter->datawriter_qos.lifespan.duration.nanosec = p_qos->lifespan.duration.nanosec;
		p_datawriter->datawriter_qos.liveliness.kind = p_qos->liveliness.kind;
		p_datawriter->datawriter_qos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		p_datawriter->datawriter_qos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		p_datawriter->datawriter_qos.ownership.kind = p_qos->ownership.kind;
		p_datawriter->datawriter_qos.ownership_strength.value = p_qos->ownership_strength.value;
		p_datawriter->datawriter_qos.reliability.kind = p_qos->reliability.kind;
		p_datawriter->datawriter_qos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		p_datawriter->datawriter_qos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		p_datawriter->datawriter_qos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		p_datawriter->datawriter_qos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		p_datawriter->datawriter_qos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;
		p_datawriter->datawriter_qos.transport_priority.value = p_qos->transport_priority.value;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_set_qos_about_user_data_qos_from_datawriter(p_datawriter, p_qos);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//FREE(p_datawriter->dataWriterQos.user_data.value);
//		p_datawriter->dataWriterQos.user_data.value = NULL;
//		if(p_qos->user_data.value)
//			p_datawriter->dataWriterQos.user_data.value = strdup(p_qos->user_data.value);
		p_datawriter->datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances = p_qos->writer_data_lifecycle.autodispose_unregistered_instances;

		//mutex_unlock(&p_datawriter->entity_lock);

		//by kki (엔티티 생성이후 QoS 를 수정하여 RxO 가 가능해진 경우를 처리하기 위한 코드)
		qos_send_publication_info_after_change_datawriter_qos(p_datawriter);

		trace_msg(OBJECT(p_module), TRACE_TRACE, "DataWriter QoS is changed.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_qos(DataWriter* p_datawriter, inout_dds DataWriterQos *p_qos)
{
	if(p_datawriter && p_qos)
	{
		//mutex_lock(&p_datawriter->entity_lock);

		p_qos->deadline.period.sec = p_datawriter->datawriter_qos.deadline.period.sec;
		p_qos->deadline.period.nanosec = p_datawriter->datawriter_qos.deadline.period.nanosec;
		p_qos->destination_order.kind = p_datawriter->datawriter_qos.destination_order.kind;
		p_qos->durability.kind = p_datawriter->datawriter_qos.durability.kind;
		p_qos->durability_service.history_depth = p_datawriter->datawriter_qos.durability_service.history_depth;
		p_qos->durability_service.history_kind = p_datawriter->datawriter_qos.durability_service.history_kind;
		p_qos->durability_service.max_instances = p_datawriter->datawriter_qos.durability_service.max_instances;
		p_qos->durability_service.max_samples = p_datawriter->datawriter_qos.durability_service.max_samples;
		p_qos->durability_service.max_samples_per_instance = p_datawriter->datawriter_qos.durability_service.max_samples_per_instance;
		p_qos->durability_service.service_cleanup_delay.sec = p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.sec;
		p_qos->durability_service.service_cleanup_delay.nanosec = p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.nanosec;
		p_qos->history.depth = p_datawriter->datawriter_qos.history.depth;
		p_qos->history.kind = p_datawriter->datawriter_qos.history.kind;
		p_qos->latency_budget.duration.sec = p_datawriter->datawriter_qos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = p_datawriter->datawriter_qos.latency_budget.duration.nanosec;
		p_qos->lifespan.duration.sec = p_datawriter->datawriter_qos.lifespan.duration.sec;
		p_qos->lifespan.duration.nanosec = p_datawriter->datawriter_qos.lifespan.duration.nanosec;
		p_qos->liveliness.kind = p_datawriter->datawriter_qos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = p_datawriter->datawriter_qos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = p_datawriter->datawriter_qos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = p_datawriter->datawriter_qos.ownership.kind;
		p_qos->ownership_strength.value = p_datawriter->datawriter_qos.ownership_strength.value;
		p_qos->reliability.kind = p_datawriter->datawriter_qos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = p_datawriter->datawriter_qos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = p_datawriter->datawriter_qos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = p_datawriter->datawriter_qos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = p_datawriter->datawriter_qos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = p_datawriter->datawriter_qos.resource_limits.max_samples_per_instance;
		p_qos->transport_priority.value = p_datawriter->datawriter_qos.transport_priority.value;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_get_qos_about_user_data_qos_from_datawriter(p_datawriter, p_qos);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->user_data.value);
//		if(p_datawriter->dataWriterQos.user_data.value)
//			p_qos->user_data.value = strdup(p_datawriter->dataWriterQos.user_data.value);
		p_qos->writer_data_lifecycle.autodispose_unregistered_instances = p_datawriter->datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances;

		//mutex_unlock(&p_datawriter->entity_lock);
	}
	return RETCODE_OK;
}


static ReturnCode_t set_listener(DataWriter* p_datawriter, in_dds DataWriterListener *p_listener,in_dds StatusMask mask)
{
	if(p_datawriter)
	{
		if(p_listener) {
			p_datawriter->p_datawriter_listener = p_listener;
		}

		p_datawriter->l_status_mask = mask;

		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}
	
static DataWriterListener *get_listener(DataWriter* p_datawriter)
{
	if (p_datawriter) return p_datawriter->p_datawriter_listener;
	return NULL;
}

static Topic *get_topic(DataWriter* p_datawriter)
{
	if(p_datawriter) return p_datawriter->p_topic;
	return NULL;
}

static Publisher *get_publisher(DataWriter* p_datawriter)
{
	if(p_datawriter) return p_datawriter->p_publisher;
	return NULL;
}

static ReturnCode_t wait_for_acknowledgments(DataWriter* p_datawriter, in_dds Duration_t max_wait)
{
	/*
	This operation is intended to be used only if the DataWriter has RELIABILITY QoS kind set to RELIABLE. Otherwise
	the operation will return immediately with RETCODE_OK.

	The operation wait_for_acknowledgments blocks the calling thread until either all data written by the DataWriter is
	acknowledged by all matched DataReader entities that have RELIABILITY QoS kind RELIABLE, or else the duration
	specified by the max_wait parameter elapses, whichever happens first. A return value of OK indicates that all the samples
	written have been acknowledged by all reliable matched data readers; a return value of TIMEOUT indicates that
	max_wait elapsed before all the data was acknowledged.
	*/
	DataWriterQos qos;
	mutex_lock(&p_datawriter->entity_lock);

	memset(&qos, 0, sizeof(DataWriterQos));

	p_datawriter->get_qos(p_datawriter, &qos);

	if(qos.reliability.kind == RELIABLE_RELIABILITY_QOS)
	{

		uint32_t waittime = get_millisecond(max_wait);
		cond_waittimed(&p_datawriter->entity_wait, &p_datawriter->entity_lock, waittime);

		printf("wait_for_ack ok\n");


	/*
		((FooDataWriter*)p_datawriter)->b_write_blocked = true;
		qos.reliability.max_blocking_time = max_wait;

		p_datawriter->set_qos(p_datawriter, &qos);
	*/
	}
	mutex_unlock(&p_datawriter->entity_lock);

	return RETCODE_ERROR;
}

static ReturnCode_t get_liveliness_lost_status(DataWriter* p_datawriter, inout_dds LivelinessLostStatus *status)
{
	/*
	This operation allows access to the LIVELINESS_LOST communication status. Communication statuses are described in
	Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->total_count = p_datawriter->status.liveliness_lost.total_count;
	status->total_count_change = p_datawriter->status.liveliness_lost.total_count_change;

	p_datawriter->status.liveliness_lost.total_count_change = 0;
	
	p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
	p_datawriter->l_status_changes -= LIVELINESS_LOST_STATUS;

	monitoring__warning_condtion(1, "Liveliness lost", 1, WARNING1, 2004);
	
	return RETCODE_ERROR;
}

static ReturnCode_t get_offered_deadline_missed_status(DataWriter* p_datawriter, inout_dds OfferedDeadlineMissedStatus *status)
{
	/*
	This operation allows access to the OFFERED_DEADLINE_MISSED communication status. Communication statuses are
	described in_dds section 7.1.4.1.
	*/

	status->total_count = p_datawriter->status.offered_deadline_missed.total_count;
	status->total_count_change = p_datawriter->status.offered_deadline_missed.total_count_change;
	status->last_instance_handle = p_datawriter->status.offered_deadline_missed.last_instance_handle;

	p_datawriter->status.offered_deadline_missed.total_count_change = 0;
	
	p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
	p_datawriter->l_status_changes -= OFFERED_DEADLINE_MISSED_STATUS;

	monitoring__warning_condtion(1, "Deadlines missed", 1, WARNING2, 2005);
	
	return RETCODE_ERROR;
}

static ReturnCode_t get_offered_incompatible_qos_status(DataWriter* p_datawriter, inout_dds OfferedIncompatibleQosStatus *status)
{
	/*
	This operation allows access to the OFFERED_INCOMPATIBLE_QOS communication status. Communication statuses
	are described in_dds section 7.1.4.1.
	*/

	status->total_count = p_datawriter->status.offered_incompatible_qos.total_count;
	status->total_count_change = p_datawriter->status.offered_incompatible_qos.total_count_change;
	status->last_policy_id = p_datawriter->status.offered_incompatible_qos.last_policy_id;
// check by jun
	status->policies = p_datawriter->status.offered_incompatible_qos.policies;

	p_datawriter->status.offered_incompatible_qos.total_count_change = 0;
	
	p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
	p_datawriter->l_status_changes -= OFFERED_INCOMPATIBLE_QOS_STATUS;

	monitoring__warning_condtion(1, "Incompatible QoS", 0, ERROR2, 2002);
	
	return RETCODE_ERROR;
}

static ReturnCode_t get_publication_matched_status(DataWriter* p_datawriter, inout_dds PublicationMatchedStatus *status)
{
	/*
	This operation allows access to the PUBLICATION_MATCHED communication status. Communication statuses are
	described in_dds section 7.1.4.1.
	*/

	status->total_count = p_datawriter->status.publication_matched.total_count;
	status->total_count_change = p_datawriter->status.publication_matched.total_count_change;
	status->current_count = p_datawriter->status.publication_matched.current_count;
	status->current_count_change = p_datawriter->status.publication_matched.current_count_change;
	status->last_subscription_handle = p_datawriter->status.publication_matched.last_subscription_handle;

	p_datawriter->status.publication_matched.total_count_change = 0;
	p_datawriter->status.publication_matched.current_count_change = 0;
	
	p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
	p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;
	
	return RETCODE_ERROR;
}

static ReturnCode_t assert_liveliness(DataWriter* p_datawriter)
{
	/*
	This operation manually asserts the liveliness of the DataWriter. This is used in_dds combination with the LIVELINESS QoS
	policy (see Section 7.1.3, “Supported QoS,” on page 96) to indicate to the Service that the entity remains active.
	This operation need only be used if the LIVELINESS setting is either MANUAL_BY_PARTICIPANT or
	MANUAL_BY_TOPIC. Otherwise, it has no effect.
	Note - Writing data via the write operation on a DataWriter asserts liveliness on the DataWriter itself and its DomainParticipant.
	Consequently the use of assert_liveliness is only needed if the application is not writing data regularly.
	Complete details are provided in_dds Section 7.1.3.11, “LIVELINESS,” on page 113.
	*/

	//[KKI] only be used if the LIVELINESS setting is either MANUAL_BY_PARTICIPANT or MANUAL_BY_TOPIC. Otherwise, it has no effect.

	//by kki...(liveliness)
	if (p_datawriter->datawriter_qos.liveliness.kind == MANUAL_BY_PARTICIPANT_LIVELINESS_QOS)
	{
		qos_operation_called_for_liveliness_qos(p_datawriter);
	}
	else if (p_datawriter->datawriter_qos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
	{
		//[KKI] send a Heartbeat Message with final flag and liveliness flag set
		qos_send_heartbeat_for_topic_liveliness(p_datawriter);
	}


	return RETCODE_ERROR;
}

static ReturnCode_t get_matched_subscriptions(DataWriter* p_datawriter, inout_dds InstanceHandleSeq *subscription_handles)
{
	/*
	This operation retrieves the list of subscriptions currently “associated” with the DataWriter; that is, subscriptions that
	have a matching Topic and compatible QoS that the application has not indicated should be “ignored” by means of the
	DomainParticipant ignore_subscription operation.
	The handles returned in_dds the ‘subscription_handles’ list are the ones that are used by the DDS implementation to locally
	identify the corresponding matched DataReader entities. These handles match the ones that appear in_dds the
	‘instance_handle’ field of the SampleInfo when reading the “DCPSSubscriptions” builtin_dds topic.
	The operation may fail if the infrastructure does not locally maintain_dds the connectivity information.
	*/

	if(subscription_handles == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_subscriber_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				message_t *p_message = (message_t *)fseq.pp_foo[i];
				InstanceHandle_t *p_handle;

				if(is_matched_remote_subscriber(p_datawriter->p_topic->topic_name, p_datawriter->p_topic->type_name, p_message->v_related_cachechange))
				{
					
					p_handle = malloc(sizeof(InstanceHandle_t));
					memset(p_handle, '\0', sizeof(InstanceHandle_t));

					*p_handle = sSeq.pp_sample_infos[i]->instance_handle;

					INSERT_ELEM(subscription_handles->pp_instancehandle, subscription_handles->i_seq , subscription_handles->i_seq , p_handle);
				}
			}

			while(fseq.i_seq){
				message_t *p_message = (message_t *)fseq.pp_foo[0];
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
				
			}
		}
	}

	return RETCODE_OK;
}

static ReturnCode_t get_matched_subscription_data(DataWriter* p_datawriter, inout_dds SubscriptionBuiltinTopicData *subscription_data,in_dds InstanceHandle_t subscription_handle)
{
	/*
	This operation retrieves information on a subscription that is currently “associated” with the DataWriter; that is, a
	subscription with a matching Topic and compatible QoS that the application has not indicated should be “ignored” by
	means of the DomainParticipant ignore_subscription operation.
	The subscription_handle must correspond to a subscription currently associated with the DataWriter, otherwise the
	operation will fail and return BAD_PARAMETER. The operation get_matched_subscriptions can be used to find the
	subscriptions that are currently matched with the DataWriter.

	The operation may also fail if the infrastructure does not hold the information necessary to fill in_dds the subscription_data.
	in_dds this case the operation will return UNSUPPORTED.
	*/

	if(subscription_data == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_subscriber_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == subscription_handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];
					memcpy(subscription_data, p_message->v_data, sizeof(SubscriptionBuiltinTopicData));
					get_subscription_builtin_topic_data(subscription_data, p_message->v_related_cachechange);
				}
			}
		}
	}

	return RETCODE_OK;
}


static defaultDataWriterQosInit()
{
	mutex_init(&default_qos_lock);
	//defaultDataWriterQos 초기화.
	memset(&defaultDataWriterQos, '\0', sizeof(defaultDataWriterQos));
	defaultDataWriterQos.deadline.period = TIME_INFINITE;
	defaultDataWriterQos.destination_order.kind = BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
	defaultDataWriterQos.durability.kind = VOLATILE_DURABILITY_QOS;
	defaultDataWriterQos.durability_service.history_depth = 0;
	defaultDataWriterQos.durability_service.history_kind = KEEP_LAST_HISTORY_QOS;

	defaultDataWriterQos.durability_service.max_instances = LENGTH_UNLIMITED;

	defaultDataWriterQos.durability_service.max_samples = LENGTH_UNLIMITED;
	defaultDataWriterQos.durability_service.max_samples_per_instance = LENGTH_UNLIMITED;
	defaultDataWriterQos.durability_service.service_cleanup_delay.sec = 0;
	defaultDataWriterQos.durability_service.service_cleanup_delay.nanosec = 0;
	defaultDataWriterQos.history.depth = 1;
	defaultDataWriterQos.history.kind = KEEP_LAST_HISTORY_QOS;
	defaultDataWriterQos.latency_budget.duration.sec = 0;
	defaultDataWriterQos.latency_budget.duration.nanosec = 0;
	defaultDataWriterQos.lifespan.duration = TIME_INFINITE;
	defaultDataWriterQos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
	defaultDataWriterQos.liveliness.lease_duration = TIME_INFINITE;
	defaultDataWriterQos.ownership.kind = SHARED_OWNERSHIP_QOS;
	defaultDataWriterQos.ownership_strength.value = 0;
	defaultDataWriterQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	defaultDataWriterQos.reliability.max_blocking_time.sec = 0;
	defaultDataWriterQos.reliability.max_blocking_time.nanosec = 0;
	defaultDataWriterQos.resource_limits.max_instances = LENGTH_UNLIMITED;
	defaultDataWriterQos.resource_limits.max_samples = LENGTH_UNLIMITED;
	defaultDataWriterQos.resource_limits.max_samples_per_instance = LENGTH_UNLIMITED;
	defaultDataWriterQos.transport_priority.value = 0;
	//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_user_data_qos(&defaultDataWriterQos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	defaultDataWriterQos.user_data.value = NULL;
	defaultDataWriterQos.writer_data_lifecycle.autodispose_unregistered_instances = true;

	p_defaultDataWriterQos = &defaultDataWriterQos;
}


extern void init_datawriter_status(DataWriter* p_datawriter);
void init_datawriter(DataWriter* p_datawriter)
{
	if(p_defaultDataWriterQos == NULL)
	{
		defaultDataWriterQosInit();
	}

	init_entity((Entity *)p_datawriter);
	p_datawriter->enable = qos_enable_datawriter;

	p_datawriter->set_qos = set_qos;
	p_datawriter->get_qos = get_qos;
	p_datawriter->set_listener = set_listener;
	p_datawriter->get_listener = get_listener;
	p_datawriter->get_topic = get_topic;
	p_datawriter->get_publisher = get_publisher;
	p_datawriter->wait_for_acknowledgments = wait_for_acknowledgments;

	p_datawriter->get_liveliness_lost_status = get_liveliness_lost_status;
	p_datawriter->get_offered_deadline_missed_status = get_offered_deadline_missed_status;
	p_datawriter->get_offered_incompatible_qos_status = get_offered_incompatible_qos_status;
	p_datawriter->get_publication_matched_status = get_publication_matched_status;
	p_datawriter->assert_liveliness = assert_liveliness;
	p_datawriter->get_matched_subscriptions = get_matched_subscriptions;
	p_datawriter->get_matched_subscription_data = get_matched_subscription_data;

	///////////////////////////

	mutex_lock(&default_qos_lock);

	p_datawriter->datawriter_qos.deadline.period.sec = defaultDataWriterQos.deadline.period.sec;
	p_datawriter->datawriter_qos.deadline.period.nanosec = defaultDataWriterQos.deadline.period.nanosec;
	p_datawriter->datawriter_qos.destination_order.kind = defaultDataWriterQos.destination_order.kind;
	p_datawriter->datawriter_qos.durability.kind = defaultDataWriterQos.durability.kind;
	p_datawriter->datawriter_qos.durability_service.history_depth = defaultDataWriterQos.durability_service.history_depth;
	p_datawriter->datawriter_qos.durability_service.history_kind = defaultDataWriterQos.durability_service.history_kind;
	p_datawriter->datawriter_qos.durability_service.max_instances = defaultDataWriterQos.durability_service.max_instances;
	p_datawriter->datawriter_qos.durability_service.max_samples = defaultDataWriterQos.durability_service.max_samples;
	p_datawriter->datawriter_qos.durability_service.max_samples_per_instance = defaultDataWriterQos.durability_service.max_samples_per_instance;
	p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.sec = defaultDataWriterQos.durability_service.service_cleanup_delay.sec;
	p_datawriter->datawriter_qos.durability_service.service_cleanup_delay.nanosec = defaultDataWriterQos.durability_service.service_cleanup_delay.nanosec;
	p_datawriter->datawriter_qos.history.depth = defaultDataWriterQos.history.depth;
	p_datawriter->datawriter_qos.history.kind = defaultDataWriterQos.history.kind;
	p_datawriter->datawriter_qos.latency_budget.duration.sec = defaultDataWriterQos.latency_budget.duration.sec;
	p_datawriter->datawriter_qos.latency_budget.duration.nanosec = defaultDataWriterQos.latency_budget.duration.nanosec;
	p_datawriter->datawriter_qos.lifespan.duration.sec = defaultDataWriterQos.lifespan.duration.sec;
	p_datawriter->datawriter_qos.lifespan.duration.nanosec = defaultDataWriterQos.lifespan.duration.nanosec;
	p_datawriter->datawriter_qos.liveliness.kind = defaultDataWriterQos.liveliness.kind;
	p_datawriter->datawriter_qos.liveliness.lease_duration.sec = defaultDataWriterQos.liveliness.lease_duration.sec;
	p_datawriter->datawriter_qos.liveliness.lease_duration.nanosec = defaultDataWriterQos.liveliness.lease_duration.nanosec;
	p_datawriter->datawriter_qos.ownership.kind = defaultDataWriterQos.ownership.kind;
	p_datawriter->datawriter_qos.ownership_strength.value = defaultDataWriterQos.ownership_strength.value;
	p_datawriter->datawriter_qos.reliability.kind = defaultDataWriterQos.reliability.kind;
	p_datawriter->datawriter_qos.reliability.max_blocking_time.sec = defaultDataWriterQos.reliability.max_blocking_time.sec;
	p_datawriter->datawriter_qos.reliability.max_blocking_time.nanosec = defaultDataWriterQos.reliability.max_blocking_time.nanosec;
	p_datawriter->datawriter_qos.resource_limits.max_instances = defaultDataWriterQos.resource_limits.max_instances;
	p_datawriter->datawriter_qos.resource_limits.max_samples = defaultDataWriterQos.resource_limits.max_samples;
	p_datawriter->datawriter_qos.resource_limits.max_samples_per_instance = defaultDataWriterQos.resource_limits.max_samples_per_instance;
	p_datawriter->datawriter_qos.transport_priority.value = defaultDataWriterQos.transport_priority.value;

	//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_copy_user_data_qos(&p_datawriter->datawriter_qos.user_data, &defaultDataWriterQos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	p_datawriter->dataWriterQos.user_data.value = NULL;
//	if(defaultDataWriterQos.user_data.value)
//		p_datawriter->dataWriterQos.user_data.value = strdup(defaultDataWriterQos.user_data.value);
	p_datawriter->datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances = defaultDataWriterQos.writer_data_lifecycle.autodispose_unregistered_instances;

	mutex_unlock(&default_qos_lock);

	///////////////////////////

	p_datawriter->p_datawriter_listener = NULL;
	p_datawriter->p_topic = NULL;
	p_datawriter->p_publisher = NULL;

	p_datawriter->i_entity_type = DATAWRITER_ENTITY;

	init_datawriter_status(p_datawriter);
/*
	p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
*/
	p_datawriter->i_instanceset = 0;
	p_datawriter->pp_instanceset = NULL;
	p_datawriter->i_message_order = 0;
	p_datawriter->pp_message_order = NULL;

	p_datawriter->p_related_rtps_writer = NULL;

	p_datawriter->builtin_type = NORMAL_TYPE;
}


void destroy_datawriter(DataWriter* p_datawriter)
{
	while(p_datawriter->i_instanceset)
	{
		int i_cachesize = p_datawriter->pp_instanceset[0]->i_messages;
		while(p_datawriter->pp_instanceset[0]->i_messages){
			rtps_cachechange_ref((rtps_cachechange_t *)p_datawriter->pp_instanceset[0]->pp_messages[0]->v_related_cachechange, false, false);
			rtps_cachechange_destory((rtps_cachechange_t *)p_datawriter->pp_instanceset[0]->pp_messages[0]->v_related_cachechange);
			REMOVE_ELEM( p_datawriter->pp_instanceset[0]->pp_messages, p_datawriter->pp_instanceset[0]->i_messages, 0);
		}

		if(p_datawriter->pp_instanceset[0]->i_messages == 0)
		{
			instanceset_remove((Entity*)p_datawriter, p_datawriter->pp_instanceset[0]);
			REMOVE_ELEM( p_datawriter->pp_instanceset, p_datawriter->i_instanceset, 0);
		}
	}

	remove_entity_to_service((Entity *)p_datawriter);

	destroy_entity((Entity*)p_datawriter);

	//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_user_data_qos(&defaultDataWriterQos.user_data);
	qos_init_user_data_qos(&p_datawriter->datawriter_qos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	FREE(defaultDataWriterQos.user_data.value);
//	FREE(p_datawriter->dataWriterQos.user_data.value);
	FREE(p_datawriter);
}


ReturnCode_t static_set_default_datawriter_qos(in_dds DataWriterQos *p_qos)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

	if(p_defaultDataWriterQos == NULL)
	{
		defaultDataWriterQosInit();
	}

	if(p_qos)
	{
//by kki...(resource limits)
		{
			ReturnCode_t ret;
			if ((ret = qos_check_resource_limits(&p_qos->resource_limits)) != RETCODE_OK)//by kki
			{
				return ret;
			}
		}

		mutex_lock(&default_qos_lock);
		
		defaultDataWriterQos.deadline.period.sec = p_qos->deadline.period.sec;
		defaultDataWriterQos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		defaultDataWriterQos.destination_order.kind = p_qos->destination_order.kind;
		defaultDataWriterQos.durability.kind = p_qos->durability.kind;
		defaultDataWriterQos.durability_service.history_depth = p_qos->durability_service.history_depth;
		defaultDataWriterQos.durability_service.history_kind = p_qos->durability_service.history_kind;
		defaultDataWriterQos.durability_service.max_instances = p_qos->durability_service.max_instances;
		defaultDataWriterQos.durability_service.max_samples = p_qos->durability_service.max_samples;
		defaultDataWriterQos.durability_service.max_samples_per_instance = p_qos->durability_service.max_samples_per_instance;
		defaultDataWriterQos.durability_service.service_cleanup_delay.sec = p_qos->durability_service.service_cleanup_delay.sec;
		defaultDataWriterQos.durability_service.service_cleanup_delay.nanosec = p_qos->durability_service.service_cleanup_delay.nanosec;
		defaultDataWriterQos.history.depth = p_qos->history.depth;
		defaultDataWriterQos.history.kind = p_qos->history.kind;
		defaultDataWriterQos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		defaultDataWriterQos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		defaultDataWriterQos.lifespan.duration.sec = p_qos->lifespan.duration.sec;
		defaultDataWriterQos.lifespan.duration.nanosec = p_qos->lifespan.duration.nanosec;
		defaultDataWriterQos.liveliness.kind = p_qos->liveliness.kind;
		defaultDataWriterQos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		defaultDataWriterQos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		defaultDataWriterQos.ownership.kind = p_qos->ownership.kind;
		defaultDataWriterQos.ownership_strength.value = p_qos->ownership_strength.value;
		defaultDataWriterQos.reliability.kind = p_qos->reliability.kind;
		defaultDataWriterQos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		defaultDataWriterQos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		defaultDataWriterQos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		defaultDataWriterQos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		defaultDataWriterQos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;
		defaultDataWriterQos.transport_priority.value = p_qos->transport_priority.value;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_copy_user_data_qos(&defaultDataWriterQos.user_data, &p_qos->user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(defaultDataWriterQos.user_data.value);
//		if(p_qos->user_data.value)
//			defaultDataWriterQos.user_data.value = strdup(p_qos->user_data.value);
		defaultDataWriterQos.writer_data_lifecycle.autodispose_unregistered_instances = p_qos->writer_data_lifecycle.autodispose_unregistered_instances;


		mutex_unlock(&default_qos_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default DataWriter QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_datawriter_qos(inout_dds DataWriterQos *p_qos)
{
	if(p_defaultDataWriterQos == NULL)
	{
		defaultDataWriterQosInit();
	}

	if(p_qos)
	{
		mutex_lock(&default_qos_lock);

		p_qos->deadline.period.sec = defaultDataWriterQos.deadline.period.sec;
		p_qos->deadline.period.nanosec = defaultDataWriterQos.deadline.period.nanosec;
		p_qos->destination_order.kind = defaultDataWriterQos.destination_order.kind;
		p_qos->durability.kind = defaultDataWriterQos.durability.kind;
		p_qos->durability_service.history_depth = defaultDataWriterQos.durability_service.history_depth;
		p_qos->durability_service.history_kind = defaultDataWriterQos.durability_service.history_kind;
		p_qos->durability_service.max_instances = defaultDataWriterQos.durability_service.max_instances;
		p_qos->durability_service.max_samples = defaultDataWriterQos.durability_service.max_samples;
		p_qos->durability_service.max_samples_per_instance = defaultDataWriterQos.durability_service.max_samples_per_instance;
		p_qos->durability_service.service_cleanup_delay.sec = defaultDataWriterQos.durability_service.service_cleanup_delay.sec;
		p_qos->durability_service.service_cleanup_delay.nanosec = defaultDataWriterQos.durability_service.service_cleanup_delay.nanosec;
		p_qos->history.depth = defaultDataWriterQos.history.depth;
		p_qos->history.kind = defaultDataWriterQos.history.kind;
		p_qos->latency_budget.duration.sec = defaultDataWriterQos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = defaultDataWriterQos.latency_budget.duration.nanosec;
		p_qos->lifespan.duration.sec = defaultDataWriterQos.lifespan.duration.sec;
		p_qos->lifespan.duration.nanosec = defaultDataWriterQos.lifespan.duration.nanosec;
		p_qos->liveliness.kind = defaultDataWriterQos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = defaultDataWriterQos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = defaultDataWriterQos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = defaultDataWriterQos.ownership.kind;
		p_qos->ownership_strength.value = defaultDataWriterQos.ownership_strength.value;
		p_qos->reliability.kind = defaultDataWriterQos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = defaultDataWriterQos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = defaultDataWriterQos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = defaultDataWriterQos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = defaultDataWriterQos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = defaultDataWriterQos.resource_limits.max_samples_per_instance;
		p_qos->transport_priority.value = defaultDataWriterQos.transport_priority.value;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_copy_user_data_qos(&p_qos->user_data, &defaultDataWriterQos.user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		p_qos->user_data.value = NULL;
//		if(defaultDataWriterQos.user_data.value)
//			p_qos->user_data.value = strdup(defaultDataWriterQos.user_data.value);
		p_qos->writer_data_lifecycle.autodispose_unregistered_instances = defaultDataWriterQos.writer_data_lifecycle.autodispose_unregistered_instances;

		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}
