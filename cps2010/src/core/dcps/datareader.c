/* 
	DataReader 관련 부분 구현
	작성자 : 
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include "../../../src/modules/rtps/rtps.h"

static mutex_t	default_qos_lock;

static DataReaderQos *p_defaultDataReaderQos = NULL;
static DataReaderQos defaultDataReaderQos;

static ReadCondition *create_readcondition(DataReader* p_datareader, in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	ReadCondition *p_readCondition = malloc(sizeof(ReadCondition));
	memset(p_readCondition, '\0', sizeof(ReadCondition));

	init_read_condition(p_readCondition);

	if(!p_datareader)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create a ReadCondition. p_datareader is NULL.");
		FREE(p_readCondition);
		return NULL;
	}
	
	p_readCondition->l_sampleStateMask = sample_states;
	p_readCondition->l_viewStateMask = view_states;
	p_readCondition->l_instanceStateMask = instance_states;
	p_readCondition->p_datareader = p_datareader;


	mutex_lock(&p_datareader->entity_lock);
	INSERT_ELEM( p_datareader->pp_read_conditions, p_datareader->i_read_conditions,
                 p_datareader->i_read_conditions, p_readCondition );
	mutex_unlock(&p_datareader->entity_lock);

	return p_readCondition;
}

static QueryCondition *create_querycondition(DataReader* p_datareader, in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states,in_dds string query_expression,in_dds StringSeq *p_query_parameters)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	expression_t *p_expr = NULL;
	QueryCondition *p_queryCondition = malloc(sizeof(QueryCondition));
	memset(p_queryCondition, '\0', sizeof(QueryCondition));

	init_query_condition(p_queryCondition);

	if(!p_queryCondition)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create a QueryCondition. p_datareader is NULL.");
		FREE(p_queryCondition);
		return NULL;
	}

	


	p_queryCondition->l_sampleStateMask = sample_states;
	p_queryCondition->l_viewStateMask = view_states;
	p_queryCondition->l_instanceStateMask = instance_states;
	p_queryCondition->p_datareader = p_datareader;

	if(query_expression){
		p_queryCondition->query_expression = strdup(query_expression);
	}else{
		destroy_query_condition(p_queryCondition);
		return NULL;
	}

	p_expr = expression_parse(p_queryCondition->query_expression);

	if(p_expr == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create the create_querycondition. query_expression is wrong.");
		destroy_query_condition(p_queryCondition);
		return NULL;
	}

	p_queryCondition->p_expr = p_expr;

//	expression_print(p_expr,0);

	if(p_query_parameters)
	{
		int i;
		for(i = 0; i < p_query_parameters->i_string; i++)
		{
			string str = strdup(p_query_parameters->pp_string[i]);
			INSERT_ELEM( p_queryCondition->query_parameters.pp_string, p_queryCondition->query_parameters.i_string,
                 p_queryCondition->query_parameters.i_string, str );
		}
	}


	mutex_lock(&p_datareader->entity_lock);
	INSERT_ELEM( p_datareader->pp_query_conditions, p_datareader->i_query_conditions,
                 p_datareader->i_query_conditions, p_queryCondition );
	mutex_unlock(&p_datareader->entity_lock);

	p_queryCondition->p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_datareader->p_subscriber->p_domain_participant, p_datareader->p_topic->type_name);

	assert(p_queryCondition->p_typeSupport != NULL);

	return p_queryCondition;
}

static ReturnCode_t delete_readcondition(DataReader* p_datareader, in_dds ReadCondition *p_condition)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(p_datareader && p_condition)
	{	
		int i;

		mutex_lock(&p_datareader->entity_lock);
		for (i = 0; i < p_datareader->i_read_conditions; i++)
		{
			if (p_datareader->pp_read_conditions[i] == p_condition)
			{	
				destroy_read_condition(p_condition);
				REMOVE_ELEM( p_datareader->pp_read_conditions, p_datareader->i_read_conditions, i);
				trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete ReadCondition.");
				mutex_unlock(&p_datareader->entity_lock);
				return RETCODE_OK;
			}
		}
		mutex_unlock(&p_datareader->entity_lock);

		mutex_lock(&p_datareader->entity_lock);
		for (i = 0; i < p_datareader->i_query_conditions; i++)
		{
			if (p_datareader->pp_query_conditions[i] == (QueryCondition *)p_condition)
			{
				destroy_query_condition((QueryCondition *)p_condition);
				REMOVE_ELEM( p_datareader->pp_query_conditions, p_datareader->i_query_conditions, i);
				mutex_unlock(&p_datareader->entity_lock);
				trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete ReadCondition.");
				return RETCODE_OK;
			}
		}
		mutex_unlock(&p_datareader->entity_lock);
	}

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a ReadCondition. It's not exist.");
	return RETCODE_ERROR;
}

static ReturnCode_t delete_contained_entities(DataReader* p_datareader)
{
	//할당된 ReadCondition들 삭제..
	mutex_lock(&p_datareader->entity_lock);
	while (p_datareader->i_read_conditions)
	{
		destroy_read_condition(p_datareader->pp_read_conditions[0]);
		REMOVE_ELEM( p_datareader->pp_read_conditions, p_datareader->i_read_conditions, 0);
	}
	mutex_unlock(&p_datareader->entity_lock);

	FREE(p_datareader->pp_read_conditions);

	//할당된 QueryCondition들 삭제..
	mutex_lock(&p_datareader->entity_lock);
	while (p_datareader->i_query_conditions)
	{
		destroy_query_condition(p_datareader->pp_query_conditions[0]);
		REMOVE_ELEM( p_datareader->pp_query_conditions, p_datareader->i_query_conditions, 0);
	}
	mutex_unlock(&p_datareader->entity_lock);

	FREE(p_datareader->pp_query_conditions);

	return RETCODE_OK;
}


static ReturnCode_t set_qos(DataReader* p_datareader, in_dds DataReaderQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_datareader && p_qos)
	{
		if (qos_check_inconsistent_policy_before_change_datareader_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return RETCODE_INCONSISTENT_POLICY; //by kki
		if (qos_check_immutable_policy_before_change_datareader_qos(p_datareader, p_qos) == RETCODE_IMMUTABLE_POLICY) return RETCODE_IMMUTABLE_POLICY; //by kki

		//mutex_lock(&p_datareader->entity_lock);
		p_datareader->datareader_qos.deadline.period.sec = p_qos->deadline.period.sec;
		p_datareader->datareader_qos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		p_datareader->datareader_qos.destination_order.kind = p_qos->destination_order.kind;
		p_datareader->datareader_qos.durability.kind = p_qos->durability.kind;
		p_datareader->datareader_qos.history.depth = p_qos->history.depth;
		p_datareader->datareader_qos.history.kind = p_qos->history.kind;
//by kki...(history)
		if (p_datareader->p_related_rtps_reader)
		{
			rtps_historycache_t* p_reader_cache = ((rtps_reader_t*)p_datareader->p_related_rtps_reader)->p_reader_cache;
			p_reader_cache->history_kind = p_datareader->datareader_qos.history.kind;
			p_reader_cache->history_depth = p_datareader->datareader_qos.history.depth;
			p_reader_cache->history_max_length = qos_get_history_max_length(&p_datareader->datareader_qos.history, &p_datareader->datareader_qos.resource_limits);//by kki
			p_reader_cache->reliability_kind = p_datareader->datareader_qos.reliability.kind;
		}
		p_datareader->datareader_qos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		p_datareader->datareader_qos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		p_datareader->datareader_qos.liveliness.kind = p_qos->liveliness.kind;
		p_datareader->datareader_qos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		p_datareader->datareader_qos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		p_datareader->datareader_qos.ownership.kind = p_qos->ownership.kind;
		p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec = p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.sec;
		p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec;
		p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.sec;
		p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec;
		p_datareader->datareader_qos.reliability.kind = p_qos->reliability.kind;
		p_datareader->datareader_qos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		p_datareader->datareader_qos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		p_datareader->datareader_qos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		p_datareader->datareader_qos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		p_datareader->datareader_qos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;
		p_datareader->datareader_qos.time_based_filter.minimum_separation.sec = p_qos->time_based_filter.minimum_separation.sec;
		p_datareader->datareader_qos.time_based_filter.minimum_separation.nanosec = p_qos->time_based_filter.minimum_separation.nanosec;

		//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
		qos_set_qos_about_user_data_qos_from_datareader(p_datareader, p_qos);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*		FREE(p_datareader->dataReaderQos.user_data.value);
		if(p_qos->user_data.value)
			p_datareader->dataReaderQos.user_data.value = strdup(p_qos->user_data.value);
*/		//mutex_unlock(&p_datareader->entity_lock);

		//by kki (엔티티 생성이후 QoS 를 수정하여 RxO 가 가능해진 경우를 처리하기 위한 코드)
		qos_send_subscription_info_after_change_datareader_qos(p_datareader);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"DataReader QoS is changed.");

		//일단 depth 만큼
		p_datareader->max_len = qos_get_history_max_length(&p_datareader->datareader_qos.history, &p_datareader->datareader_qos.resource_limits);//by kki//p_datareader->datareader_qos.history.depth;
	}

	return RETCODE_OK;
}

static ReturnCode_t get_qos(DataReader* p_datareader, inout_dds DataReaderQos* p_qos)
{
	if(p_datareader && p_qos)
	{
		//mutex_lock(&p_datareader->entity_lock);
		p_qos->deadline.period.sec = p_datareader->datareader_qos.deadline.period.sec;
		p_qos->deadline.period.nanosec = p_datareader->datareader_qos.deadline.period.nanosec;
		p_qos->destination_order.kind = p_datareader->datareader_qos.destination_order.kind;
		p_qos->durability.kind = p_datareader->datareader_qos.durability.kind;
		p_qos->history.depth = p_datareader->datareader_qos.history.depth;
		p_qos->history.kind = p_datareader->datareader_qos.history.kind;
		p_qos->latency_budget.duration.sec = p_datareader->datareader_qos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = p_datareader->datareader_qos.latency_budget.duration.nanosec;
		p_qos->liveliness.kind = p_datareader->datareader_qos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = p_datareader->datareader_qos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = p_datareader->datareader_qos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = p_datareader->datareader_qos.ownership.kind;
		p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.sec = p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec;
		p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec;
		p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec;
		p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec;
		p_qos->reliability.kind = p_datareader->datareader_qos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = p_datareader->datareader_qos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = p_datareader->datareader_qos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = p_datareader->datareader_qos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = p_datareader->datareader_qos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = p_datareader->datareader_qos.resource_limits.max_samples_per_instance;
		p_qos->time_based_filter.minimum_separation.sec = p_datareader->datareader_qos.time_based_filter.minimum_separation.sec;
		p_qos->time_based_filter.minimum_separation.nanosec = p_datareader->datareader_qos.time_based_filter.minimum_separation.nanosec;

		//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
		qos_get_qos_about_user_data_qos_from_datareader(p_datareader, p_qos);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*		FREE(p_qos->user_data.value);
		if(p_datareader->dataReaderQos.user_data.value)
			p_qos->user_data.value = strdup(p_datareader->dataReaderQos.user_data.value);
*/		//mutex_unlock(&p_datareader->entity_lock);
	}

	return RETCODE_OK;
}

static ReturnCode_t set_listener(DataReader* p_datareader, in_dds DataReaderListener *p_listener,in_dds StatusMask mask)
{
	if(p_datareader && p_listener)
	{
		p_datareader->p_datareader_listener = p_listener;
		p_datareader->l_status_mask = mask;
		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static DataReaderListener *get_listener(DataReader* p_datareader)
{
	if (p_datareader) return p_datareader->p_datareader_listener;
	return NULL;
}

static TopicDescription *get_topicdescription(DataReader* p_datareader)
{
	if(p_datareader)
	{
		return WHERE_TOPICDESCRIPTION(p_datareader->p_topic);
	}

	return NULL;
}

static Subscriber *get_subscriber(DataReader* p_datareader)
{
	if(p_datareader)
	{
		return p_datareader->p_subscriber;
	}

	return NULL;
}

static ReturnCode_t get_sample_rejected_status(DataReader* p_datareader, inout_dds SampleRejectedStatus *status)
{
	/*
	This operation allows access to the SAMPLE_REJECTED communication status. Communication statuses are described
	in_dds Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->total_count = p_datareader->status.sample_rejected.total_count;
	status->total_count_change = p_datareader->status.sample_rejected.total_count_change;
	status->last_reason = p_datareader->status.sample_rejected.last_reason;
	status->last_instance_handle = p_datareader->status.sample_rejected.last_instance_handle;

	p_datareader->status.sample_rejected.total_count_change = 0;
	p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
	p_datareader->l_status_changes -= SAMPLE_REJECTED_STATUS;

	monitoring__warning_condtion(0, "samples rejected", 0, ERROR3, 1003);

	return RETCODE_ERROR;
}

static ReturnCode_t get_liveliness_changed_status(DataReader* p_datareader, inout_dds LivelinessChangedStatus *status)
{
	/*
	This operation allows access to the LIVELINESS_CHANGED communication status. Communication statuses are
	described in_dds Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->alive_count = p_datareader->status.liveliness_changed.alive_count;
	status->not_alive_count = p_datareader->status.liveliness_changed.not_alive_count;
	status->alive_count_change = p_datareader->status.liveliness_changed.alive_count_change;
	status->not_alive_count_change = p_datareader->status.liveliness_changed.not_alive_count_change;
	status->last_publication_handle = p_datareader->status.liveliness_changed.last_publication_handle;

	p_datareader->status.liveliness_changed.alive_count_change = 0;
	p_datareader->status.liveliness_changed.not_alive_count_change = 0;
	
	p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
	p_datareader->l_status_changes -= LIVELINESS_CHANGED_STATUS;
	return RETCODE_ERROR;
}

static ReturnCode_t get_requested_deadline_missed_status(DataReader* p_datareader, inout_dds RequestedDeadlineMissedStatus *status)
{
	/*
	This operation allows access to the REQUESTED_DEADLINE_MISSED communication status. Communication statuses
	are described in_dds Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->total_count = p_datareader->status.requested_deadline_missed.total_count;
	status->total_count_change = p_datareader->status.requested_deadline_missed.total_count_change;
	status->last_instance_handle = p_datareader->status.requested_deadline_missed.last_instance_handle;

	p_datareader->status.requested_deadline_missed.total_count_change = 0;
	p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
	p_datareader->l_status_changes -= REQUESTED_DEADLINE_MISSED_STATUS;

	monitoring__warning_condtion(0, "Deadlines missed", 1, WARNING2, 1005);

	return RETCODE_ERROR;
}

static ReturnCode_t get_requested_incompatible_qos_status(DataReader* p_datareader, inout_dds RequestedIncompatibleQosStatus *status)
{
	/*
	This operation allows access to the REQUESTED_INCOMPATIBLE_QOS communication status. Communication
	statuses are described in_dds Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->total_count = p_datareader->status.requested_incompatible_qos.total_count;
	status->total_count_change = p_datareader->status.requested_incompatible_qos.total_count_change;
	status->last_policy_id = p_datareader->status.requested_incompatible_qos.last_policy_id;
//need checking
	status->policies = p_datareader->status.requested_incompatible_qos.policies;

	p_datareader->status.requested_incompatible_qos.total_count_change = 0;
	p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
	p_datareader->l_status_changes -= REQUESTED_INCOMPATIBLE_QOS_STATUS;	

	monitoring__warning_condtion(0, "Incompatible QoS", 0, ERROR2, 1002);
	
	return RETCODE_ERROR;
}

static ReturnCode_t get_subscription_matched_status(DataReader* p_datareader, inout_dds SubscriptionMatchedStatus *status)
{
	/*
	This operation allows access to the SUBSCRIPTION_MATCHED communication status. Communication statuses are
	described in_dds Section 7.1.4.1, “Communication Status,” on page 120.
	*/

	status->total_count = p_datareader->status.subscriber_matched.total_count;
	status->total_count_change = p_datareader->status.subscriber_matched.total_count_change;
	status->current_count = p_datareader->status.subscriber_matched.current_count;
	status->current_count_change = p_datareader->status.subscriber_matched.current_count_change;
	status->last_publication_handle = p_datareader->status.subscriber_matched.last_publication_handle;

	p_datareader->status.subscriber_matched.total_count_change = 0;
	p_datareader->status.subscriber_matched.current_count_change = 0;
	
	p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
	p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;	
	
	return RETCODE_ERROR;
}

static ReturnCode_t get_sample_lost_status(DataReader* p_datareader, inout_dds SampleLostStatus *status)
{
	/*
	This operation allows access to the SAMPLE_LOST communication status. Communication statuses are described in
	Section 7.1.4.1, “Communication Status,” on page 120.”
	*/

	status->total_count = p_datareader->status.sample_lost.total_count;
	status->total_count_change = p_datareader->status.sample_lost.total_count_change;

	p_datareader->status.sample_lost.total_count_change = 0;
	
	p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
	p_datareader->l_status_changes -= SAMPLE_LOST_STATUS;


	monitoring__warning_condtion(0, "Samples lost", 1, WARNING3, 1006);
	
	return RETCODE_ERROR;
}

static ReturnCode_t wait_for_historical_data(DataReader* p_datareader, in_dds Duration_t max_wait)
{
	/*
	This operation is intended only for DataReader entities that have a non-VOLATILE PERSISTENCE QoS kind.
	As soon as an application enables a non-VOLATILE DataReader it will start receiving both “historical” data, i.e., the
	data that was written prior to the time the DataReader joined the domain, as well as any new data written by the
	DataWriter entities. There are situations where the application logic may require the application to wait until all
	“historical” data is received. This is the purpose of the wait_for_historical_data operation.
	The operation wait_for_historical_data blocks the calling thread until either all “historical” data is received, or else the
	duration specified by the max_wait parameter elapses, whichever happens first. A return value of OK indicates that all the
	“historical” data was received; a return value of TIMEOUT indicates that max_wait elapsed before all the data was
	received.
	*/

	//not yet implemented;
	return RETCODE_ERROR;
}

static ReturnCode_t get_matched_publications(DataReader* p_datareader, inout_dds InstanceHandleSeq *publication_handles)
{
	/*
	This operation retrieves the list of publications currently “associated” with the DataReader; that is, publications that have
	a matching Topic and compatible QoS that the application has not indicated should be “ignored” by means of the
	DomainParticipant ignore_ publication operation.
	The handles returned in_dds the 'publication_handles' list are the ones that are used by the DDS implementation to locally
	identify the corresponding matched DataWriter entities. These handles match the ones that appear in_dds the 'instance_handle'
	field of the SampleInfo when reading the “DCPSPublications” builtin_dds topic.
	The operation may fail if the infrastructure does not locally maintain_dds the connectivity information.
	*/

	if(publication_handles == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_databuiltinreader = (FooDataReader*)get_builtin_publisher_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;

		if(p_databuiltinreader != NULL)
		{
			p_databuiltinreader->read(p_databuiltinreader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				message_t *p_message = (message_t *)fseq.pp_foo[i];
				InstanceHandle_t *p_handle;

				if(is_matched_remote_publisher(p_datareader->p_topic->topic_name, p_datareader->p_topic->type_name, p_message->v_related_cachechange)){
					p_handle = malloc(sizeof(InstanceHandle_t));
					memset(p_handle, '\0', sizeof(InstanceHandle_t));

					*p_handle = sSeq.pp_sample_infos[i]->instance_handle;
					INSERT_ELEM(publication_handles->pp_instancehandle, publication_handles->i_seq , publication_handles->i_seq , p_handle);
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

static ReturnCode_t get_matched_publication_data(DataReader* p_datareader, inout_dds PublicationBuiltinTopicData *publication_data,in_dds InstanceHandle_t publication_handle)
{
	/*
	This operation retrieves information on a publication that is currently “associated” with the DataReader; that is, a
	publication with a matching Topic and compatible QoS that the application has not indicated should be “ignored” by
	means of the DomainParticipant ignore_publication operation.
	The publication_handle must correspond to a publication currently associated with the DataReader otherwise the
	operation will fail and return BAD_PARAMETER. The operation get_matched_publications can be used to find the
	publications that are currently matched with the DataReader.
	The operation may also fail if the infrastructure does not hold the information necessary to fill in_dds the publication_data. In
	this case the operation will return UNSUPPORTED.
	*/

	if(publication_data == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_databuiltinreader = (FooDataReader*)get_builtin_publisher_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_databuiltinreader != NULL)
		{
			p_databuiltinreader->read(p_databuiltinreader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == publication_handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];

					memcpy(publication_data, p_message->v_data, sizeof(PublicationBuiltinTopicData));
					get_publication_builtin_topic_data(publication_data, p_message->v_related_cachechange);
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

static void InitDataReaderQos()
{

	mutex_init(&default_qos_lock);
	//defaultDataReaderQos 초기화.
	defaultDataReaderQos.deadline.period = TIME_INFINITE;
	defaultDataReaderQos.destination_order.kind = BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
	defaultDataReaderQos.durability.kind = VOLATILE_DURABILITY_QOS;

	defaultDataReaderQos.history.depth = 1;
	defaultDataReaderQos.history.kind = KEEP_LAST_HISTORY_QOS;
	defaultDataReaderQos.latency_budget.duration.sec = 0;
	defaultDataReaderQos.latency_budget.duration.nanosec = 0;
	defaultDataReaderQos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
	defaultDataReaderQos.liveliness.lease_duration =  TIME_INFINITE;
	defaultDataReaderQos.ownership.kind = SHARED_OWNERSHIP_QOS;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec = 0;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = 0;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay = TIME_INFINITE;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = 0;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = 0;
	defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay = TIME_INFINITE;
	defaultDataReaderQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	defaultDataReaderQos.reliability.max_blocking_time.sec = 0;
	defaultDataReaderQos.reliability.max_blocking_time.nanosec = 0;
	defaultDataReaderQos.resource_limits.max_instances = LENGTH_UNLIMITED;
	defaultDataReaderQos.resource_limits.max_samples = LENGTH_UNLIMITED;
	defaultDataReaderQos.resource_limits.max_samples_per_instance = LENGTH_UNLIMITED;
	defaultDataReaderQos.time_based_filter.minimum_separation.sec = 0;
	defaultDataReaderQos.time_based_filter.minimum_separation.nanosec = 0;

	//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_user_data_qos(&defaultDataReaderQos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	defaultDataReaderQos.user_data.value = NULL;

	p_defaultDataReaderQos = &defaultDataReaderQos;
}


extern void init_datareader_status(DataReader* p_datareader);
void init_datareader(DataReader* p_datareader)
{
	if(p_defaultDataReaderQos == NULL)
	{
		InitDataReaderQos();
	}

	init_entity((Entity *)p_datareader);
	p_datareader->enable = qos_enable_datareader; //by kki

	p_datareader->create_readcondition = create_readcondition;
	p_datareader->create_querycondition = create_querycondition;
	p_datareader->delete_readcondition = delete_readcondition;
	p_datareader->delete_contained_entities = delete_contained_entities;
	p_datareader->set_qos = set_qos;
	p_datareader->get_qos = get_qos;
	p_datareader->set_listener = set_listener;
	p_datareader->get_listener = get_listener;
	p_datareader->get_topicdescription = get_topicdescription;
	p_datareader->get_subscriber = get_subscriber;
	p_datareader->get_sample_rejected_status = get_sample_rejected_status;
	p_datareader->get_liveliness_changed_status = get_liveliness_changed_status;
	p_datareader->get_requested_deadline_missed_status = get_requested_deadline_missed_status;
	p_datareader->get_requested_incompatible_qos_status = get_requested_incompatible_qos_status;
	p_datareader->get_subscription_matched_status = get_subscription_matched_status;
	p_datareader->get_sample_lost_status = get_sample_lost_status;
	p_datareader->wait_for_historical_data = wait_for_historical_data;
	p_datareader->get_matched_publications = get_matched_publications;
	p_datareader->get_matched_publication_data = get_matched_publication_data;

	///////////////////

	mutex_lock(&default_qos_lock);

	p_datareader->datareader_qos.deadline.period.sec = defaultDataReaderQos.deadline.period.sec;
	p_datareader->datareader_qos.deadline.period.nanosec = defaultDataReaderQos.deadline.period.nanosec;
	p_datareader->datareader_qos.destination_order.kind = defaultDataReaderQos.destination_order.kind;
	p_datareader->datareader_qos.durability.kind = defaultDataReaderQos.durability.kind;
	p_datareader->datareader_qos.history.depth = defaultDataReaderQos.history.depth;
	p_datareader->datareader_qos.history.kind = defaultDataReaderQos.history.kind;
	p_datareader->datareader_qos.latency_budget.duration.sec = defaultDataReaderQos.latency_budget.duration.sec;
	p_datareader->datareader_qos.latency_budget.duration.nanosec = defaultDataReaderQos.latency_budget.duration.nanosec;
	p_datareader->datareader_qos.liveliness.kind = defaultDataReaderQos.liveliness.kind;
	p_datareader->datareader_qos.liveliness.lease_duration.sec = defaultDataReaderQos.liveliness.lease_duration.sec;
	p_datareader->datareader_qos.liveliness.lease_duration.nanosec = defaultDataReaderQos.liveliness.lease_duration.nanosec;
	p_datareader->datareader_qos.ownership.kind = defaultDataReaderQos.ownership.kind;
	p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec = defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec;
	p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec;
	p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec;
	p_datareader->datareader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec;
	p_datareader->datareader_qos.reliability.kind = defaultDataReaderQos.reliability.kind;
	p_datareader->datareader_qos.reliability.max_blocking_time.sec = defaultDataReaderQos.reliability.max_blocking_time.sec;
	p_datareader->datareader_qos.reliability.max_blocking_time.nanosec = defaultDataReaderQos.reliability.max_blocking_time.nanosec;
	p_datareader->datareader_qos.resource_limits.max_instances = defaultDataReaderQos.resource_limits.max_instances;
	p_datareader->datareader_qos.resource_limits.max_samples = defaultDataReaderQos.resource_limits.max_samples;
	p_datareader->datareader_qos.resource_limits.max_samples_per_instance = defaultDataReaderQos.resource_limits.max_samples_per_instance;
	p_datareader->datareader_qos.time_based_filter.minimum_separation.sec = defaultDataReaderQos.time_based_filter.minimum_separation.sec;
	p_datareader->datareader_qos.time_based_filter.minimum_separation.nanosec = defaultDataReaderQos.time_based_filter.minimum_separation.nanosec;

	//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
	//qos_init_user_data_qos(p_datareader->dataReaderQos.user_data);
	if (defaultDataReaderQos.user_data.value.i_string > 0)
		qos_copy_user_data_qos(&p_datareader->datareader_qos.user_data,  &defaultDataReaderQos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*	p_datareader->dataReaderQos.user_data.value = NULL;
	if(defaultDataReaderQos.user_data.value)
		p_datareader->dataReaderQos.user_data.value = strdup(defaultDataReaderQos.user_data.value);
*/	mutex_unlock(&default_qos_lock);
	///////////////////
	p_datareader->p_subscriber = NULL;
	p_datareader->p_topic = NULL;


	p_datareader->i_read_conditions = 0;
	p_datareader->pp_read_conditions = NULL;

	p_datareader->i_query_conditions = 0;
	p_datareader->pp_query_conditions = NULL;
	p_datareader->p_datareader_listener = NULL;

	p_datareader->i_entity_type = DATAREADER_ENTITY;


//by jun 

	init_datareader_status(p_datareader);
/*	p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
	p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
	p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
	p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
	p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
	p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
	p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
*/

	p_datareader->i_instanceset = 0;
	p_datareader->pp_instanceset = NULL;


	p_datareader->i_message_order = 0;
	p_datareader->pp_message_order = NULL;

	p_datareader->len = 0;
	//일단 depth만큼...
	p_datareader->max_len = qos_get_history_max_length(&p_datareader->datareader_qos.history, &p_datareader->datareader_qos.resource_limits);//by kki//p_datareader->datareader_qos.history.depth;
	p_datareader->owned = false;

	//added by kyy(Presentation QoS)
	p_datareader->first_coh_number = 0;
	p_datareader->last_coh_number = 0;
	p_datareader->is_coherent_set_end = false;

	p_datareader->p_related_rtps_reader = NULL;
	p_datareader->builtin_type = NORMAL_TYPE;
	
}

void destroy_datareader(DataReader* p_datareader)
{	

	//if (is_quit()) return;

	while (p_datareader->i_instanceset && is_quit() == false)
	{
		int i_cachesize = p_datareader->pp_instanceset[0]->i_messages;
		while(p_datareader->pp_instanceset[0]->i_messages){
			rtps_cachechange_ref((rtps_cachechange_t *)p_datareader->pp_instanceset[0]->pp_messages[0]->v_related_cachechange, false, false);
			rtps_cachechange_destory((rtps_cachechange_t *)p_datareader->pp_instanceset[0]->pp_messages[0]->v_related_cachechange);
			REMOVE_ELEM( p_datareader->pp_instanceset[0]->pp_messages, p_datareader->pp_instanceset[0]->i_messages, 0);
		}

		if(p_datareader->pp_instanceset[0]->i_messages == 0)
		{
			instanceset_remove((Entity*)p_datareader, p_datareader->pp_instanceset[0]);
			REMOVE_ELEM( p_datareader->pp_instanceset, p_datareader->i_instanceset, 0);
		}
	}

	FREE(p_datareader->pp_message_order);

	delete_contained_entities(p_datareader);

		
	//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
	FREE(defaultDataReaderQos.user_data.value.pp_string);
	FREE(p_datareader->datareader_qos.user_data.value.pp_string);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	FREE(defaultDataReaderQos.user_data.value);
//	FREE(p_datareader->dataReaderQos.user_data.value);

	remove_entity_to_service((Entity *)p_datareader);
	destroy_entity((Entity*)p_datareader);
	FREE(p_datareader);
}


ReturnCode_t static_set_default_datareader_qos(in_dds DataReaderQos *p_qos)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

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

		defaultDataReaderQos.deadline.period.sec = p_qos->deadline.period.sec;
		defaultDataReaderQos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		defaultDataReaderQos.destination_order.kind = p_qos->destination_order.kind;
		defaultDataReaderQos.durability.kind = p_qos->durability.kind;
		defaultDataReaderQos.history.depth = p_qos->history.depth;
		defaultDataReaderQos.history.kind = p_qos->history.kind;
		defaultDataReaderQos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		defaultDataReaderQos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		defaultDataReaderQos.liveliness.kind = p_qos->liveliness.kind;
		defaultDataReaderQos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		defaultDataReaderQos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		defaultDataReaderQos.ownership.kind = p_qos->ownership.kind;
		defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec = p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.sec;
		defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec;
		defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.sec;
		defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec;
		defaultDataReaderQos.reliability.kind = p_qos->reliability.kind;
		defaultDataReaderQos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		defaultDataReaderQos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		defaultDataReaderQos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		defaultDataReaderQos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		defaultDataReaderQos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;
		defaultDataReaderQos.time_based_filter.minimum_separation.sec = p_qos->time_based_filter.minimum_separation.sec;
		defaultDataReaderQos.time_based_filter.minimum_separation.nanosec = p_qos->time_based_filter.minimum_separation.nanosec;

		//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
//		FREE(defaultDataReaderQos.user_data.value.pp_string);
		if (p_qos->user_data.value.i_string > 0)
			qos_copy_user_data_qos(&defaultDataReaderQos.user_data, &p_qos->user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(defaultDataReaderQos.user_data.value);
//		if(p_qos->user_data.value)
//			defaultDataReaderQos.user_data.value = strdup(p_qos->user_data.value);

		mutex_unlock(&default_qos_lock);


		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default DataReader QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_datareader_qos(inout_dds DataReaderQos *p_qos)
{
	if (p_defaultDataReaderQos == NULL)
	{
		InitDataReaderQos();
	}

	if (p_qos)
	{
		mutex_lock(&default_qos_lock);
		p_qos->deadline.period.sec = defaultDataReaderQos.deadline.period.sec;
		p_qos->deadline.period.nanosec = defaultDataReaderQos.deadline.period.nanosec;
		p_qos->destination_order.kind = defaultDataReaderQos.destination_order.kind;
		p_qos->durability.kind = defaultDataReaderQos.durability.kind;
		p_qos->history.depth = defaultDataReaderQos.history.depth;
		p_qos->history.kind = defaultDataReaderQos.history.kind;
		p_qos->latency_budget.duration.sec = defaultDataReaderQos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = defaultDataReaderQos.latency_budget.duration.nanosec;
		p_qos->liveliness.kind = defaultDataReaderQos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = defaultDataReaderQos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = defaultDataReaderQos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = defaultDataReaderQos.ownership.kind;
		p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.sec = defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec;
		p_qos->reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec = defaultDataReaderQos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec;
		p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.sec = defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec;
		p_qos->reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec = defaultDataReaderQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec;
		p_qos->reliability.kind = defaultDataReaderQos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = defaultDataReaderQos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = defaultDataReaderQos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = defaultDataReaderQos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = defaultDataReaderQos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = defaultDataReaderQos.resource_limits.max_samples_per_instance;
		p_qos->time_based_filter.minimum_separation.sec = defaultDataReaderQos.time_based_filter.minimum_separation.sec;
		p_qos->time_based_filter.minimum_separation.nanosec = defaultDataReaderQos.time_based_filter.minimum_separation.nanosec;

		//added by kyy (UserData QoS)/////////////////////////////////////////////////////////////////////////////////////////////////////
		if (defaultDataReaderQos.user_data.value.i_string > 0)
		{
			qos_copy_user_data_qos(&p_qos->user_data, &defaultDataReaderQos.user_data);
		}
		else
		{
			qos_init_user_data_qos(&p_qos->user_data);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->user_data.value);
//		p_qos->user_data.value = NULL;
//		if(defaultDataReaderQos.user_data.value)
//			p_qos->user_data.value = strdup(defaultDataReaderQos.user_data.value);
		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}