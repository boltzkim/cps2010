/* 
	Topic 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static mutex_t	default_qos_lock;

static TopicQos *p_defaulTopicQos = NULL;
static TopicQos defaulTopicQos;



static ReturnCode_t set_qos(Topic* p_topic, in_dds TopicQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_topic && p_qos)
	{
		if (qos_check_inconsistent_policy_before_change_topic_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return RETCODE_INCONSISTENT_POLICY; //by kki
		if (qos_check_immutable_policy_before_change_topic_qos(p_topic, p_qos) == RETCODE_IMMUTABLE_POLICY) return RETCODE_IMMUTABLE_POLICY; //by kki

		mutex_lock(&p_topic->entity_lock);
		p_topic->topic_qos.deadline.period.sec = p_qos->deadline.period.sec;
		p_topic->topic_qos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		p_topic->topic_qos.destination_order.kind = p_qos->destination_order.kind;
		p_topic->topic_qos.durability.kind = p_qos->durability.kind;
		p_topic->topic_qos.durability_service.history_depth = p_qos->durability_service.history_depth;
		p_topic->topic_qos.durability_service.history_kind = p_qos->durability_service.history_kind;
		p_topic->topic_qos.durability_service.max_instances = p_qos->durability_service.max_instances;
		p_topic->topic_qos.durability_service.max_samples = p_qos->durability_service.max_samples;
		p_topic->topic_qos.durability_service.max_samples_per_instance = p_qos->durability_service.max_samples_per_instance;
		p_topic->topic_qos.durability_service.service_cleanup_delay.sec = p_qos->durability_service.service_cleanup_delay.sec;
		p_topic->topic_qos.durability_service.service_cleanup_delay.nanosec = p_qos->durability_service.service_cleanup_delay.nanosec;
		p_topic->topic_qos.history.depth = p_qos->history.depth;
		p_topic->topic_qos.history.kind = p_qos->history.kind;
		p_topic->topic_qos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		p_topic->topic_qos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		p_topic->topic_qos.lifespan.duration.sec = p_qos->lifespan.duration.sec;
		p_topic->topic_qos.lifespan.duration.nanosec = p_qos->lifespan.duration.nanosec;
		p_topic->topic_qos.liveliness.kind = p_qos->liveliness.kind;
		p_topic->topic_qos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		p_topic->topic_qos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		p_topic->topic_qos.ownership.kind = p_qos->ownership.kind;
		p_topic->topic_qos.reliability.kind = p_qos->reliability.kind;
		p_topic->topic_qos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		p_topic->topic_qos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		p_topic->topic_qos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		p_topic->topic_qos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		p_topic->topic_qos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;

		//added by kyy (topic_data)/////////////////////////////////////////////////////////////////////////////////////////////////////
		qos_set_qos_about_topic_data_qos_from_topic(p_topic, p_qos);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_topic->topic_qos.topic_data.value);
//		if(p_qos->topic_data.value)
//			p_topic->topic_qos.topic_data.value = strdup(p_qos->topic_data.value);
		p_topic->topic_qos.transport_priority.value = p_qos->transport_priority.value;
		mutex_unlock(&p_topic->entity_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Topic QoS is changed.");
		return RETCODE_OK;
	}

	return RETCODE_INCONSISTENT_POLICY;
}

static ReturnCode_t get_qos(Topic* p_topic, inout_dds TopicQos *p_qos)
{
	if(p_topic && p_qos)
	{
		mutex_lock(&p_topic->entity_lock);
		p_qos->deadline.period.sec = p_topic->topic_qos.deadline.period.sec;
		p_qos->deadline.period.nanosec = p_topic->topic_qos.deadline.period.nanosec;
		p_qos->destination_order.kind = p_topic->topic_qos.destination_order.kind;
		p_qos->durability.kind = p_topic->topic_qos.durability.kind;
		p_qos->durability_service.history_depth = p_topic->topic_qos.durability_service.history_depth;
		p_qos->durability_service.history_kind = p_topic->topic_qos.durability_service.history_kind;
		p_qos->durability_service.max_instances = p_topic->topic_qos.durability_service.max_instances;
		p_qos->durability_service.max_samples = p_topic->topic_qos.durability_service.max_samples;
		p_qos->durability_service.max_samples_per_instance = p_topic->topic_qos.durability_service.max_samples_per_instance;
		p_qos->durability_service.service_cleanup_delay.sec = p_topic->topic_qos.durability_service.service_cleanup_delay.sec;
		p_qos->durability_service.service_cleanup_delay.nanosec = p_topic->topic_qos.durability_service.service_cleanup_delay.nanosec;
		p_qos->history.depth = p_topic->topic_qos.history.depth;
		p_qos->history.kind = p_topic->topic_qos.history.kind;
		p_qos->latency_budget.duration.sec = p_topic->topic_qos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = p_topic->topic_qos.latency_budget.duration.nanosec;
		p_qos->lifespan.duration.sec = p_topic->topic_qos.lifespan.duration.sec;
		p_qos->lifespan.duration.nanosec = p_topic->topic_qos.lifespan.duration.nanosec;
		p_qos->liveliness.kind = p_topic->topic_qos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = p_topic->topic_qos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = p_topic->topic_qos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = p_topic->topic_qos.ownership.kind;
		p_qos->reliability.kind = p_topic->topic_qos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = p_topic->topic_qos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = p_topic->topic_qos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = p_topic->topic_qos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = p_topic->topic_qos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = p_topic->topic_qos.resource_limits.max_samples_per_instance;

		//added by kyy (topic_data)/////////////////////////////////////////////////////////////////////////////////////////////////////
		qos_get_qos_about_topic_data_qos_from_topic(p_topic, p_qos);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->topic_data.value);
//		if(p_topic->topic_qos.topic_data.value)
//			p_qos->topic_data.value = strdup(p_topic->topic_qos.topic_data.value);
		p_qos->transport_priority.value = p_topic->topic_qos.transport_priority.value;

		mutex_unlock(&p_topic->entity_lock);
		return RETCODE_OK;
	}

	return RETCODE_INCONSISTENT_POLICY;
}

static ReturnCode_t set_listener(Topic* p_topic, in_dds TopicListener *p_listener,in_dds StatusMask mask)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_topic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't set a listener. p_topic is NULL.");
	}

	p_topic->p_topic_listener = p_listener;
	p_topic->l_status_mask = mask;

	trace_msg(OBJECT(p_module),TRACE_TRACE,"Topic set a listener");

	return RETCODE_OK;
}

static TopicListener *get_listener(Topic* p_topic)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_topic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't get a listener. p_topic is NULL.");
	}

	return p_topic->p_topic_listener;
}

static ReturnCode_t get_inconsistent_topic_status(Topic* p_topic, inout_dds InconsistentTopicStatus *a_status)
{
	/*
	This method allows the application to retrieve the INCONSISTENT_TOPIC status of the Topic.
	Each DomainEntity has a set of relevant communication statuses. A change of status causes the corresponding Listener
	to be invoked and can also be monitored by means of the associated StatusCondition.
	The complete list of communication status, their values, and the DomainEntities they apply to is provided in
	Section 7.1.4.1, “Communication Status,” on page 120.
	*/


	if(p_topic != NULL && a_status != NULL)
	{
		*a_status = p_topic->status.inconsistent_topic;

		monitoring__warning_condtion(2, "Inconsistent topic status", 0, ERROR1, 3001);
	}
	
	
	return RETCODE_OK;
}


static defaultTopicQosInit()
{
	mutex_init(&default_qos_lock);
	//defaulTopicQos 초기화. 
	defaulTopicQos.deadline.period.sec = 0;
	defaulTopicQos.deadline.period.nanosec = 0;
	defaulTopicQos.destination_order.kind = BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
	defaulTopicQos.durability.kind = VOLATILE_DURABILITY_QOS;
	defaulTopicQos.durability_service.history_depth = 0;
	defaulTopicQos.durability_service.history_kind = KEEP_LAST_HISTORY_QOS;

	defaulTopicQos.durability_service.max_instances = LENGTH_UNLIMITED;

	defaulTopicQos.durability_service.max_samples = 100;
	defaulTopicQos.durability_service.max_samples_per_instance = 100;
	defaulTopicQos.durability_service.service_cleanup_delay.sec = 0;
	defaulTopicQos.durability_service.service_cleanup_delay.nanosec = 0;
	defaulTopicQos.history.depth = 1;
	defaulTopicQos.history.kind = KEEP_LAST_HISTORY_QOS;
	defaulTopicQos.latency_budget.duration.sec = 0;
	defaulTopicQos.latency_budget.duration.nanosec = 0;
	defaulTopicQos.lifespan.duration.sec = 0;
	defaulTopicQos.lifespan.duration.nanosec = 0;
	defaulTopicQos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
	defaulTopicQos.liveliness.lease_duration.sec = 0;
	defaulTopicQos.liveliness.lease_duration.nanosec = 0;
	defaulTopicQos.ownership.kind = SHARED_OWNERSHIP_QOS;
	defaulTopicQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	defaulTopicQos.reliability.max_blocking_time.sec = 0;
	defaulTopicQos.reliability.max_blocking_time.nanosec = 0;
	defaulTopicQos.resource_limits.max_instances = LENGTH_UNLIMITED;
	defaulTopicQos.resource_limits.max_samples = LENGTH_UNLIMITED;
	defaulTopicQos.resource_limits.max_samples_per_instance = LENGTH_UNLIMITED;

	//added by kyy(Transport Priority)
	defaulTopicQos.transport_priority.value = 0;

	//added by kyy (topic_data)/////////////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_topic_data_qos(&defaulTopicQos.topic_data);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	defaulTopicQos.topic_data.value = NULL;
	defaulTopicQos.transport_priority.value = 0;
	p_defaulTopicQos = &defaulTopicQos;
}


extern ReturnCode_t enable_topic(Entity *p_entity);
void init_topic(Topic* p_topic)
{
	//Topic p_topic1;
	//int locate  = (int)&p_topic1.get_type_name - (int)&p_topic1.enable;
	//int size = sizeof(Entity);

	if(p_defaulTopicQos == NULL)
	{
		defaultTopicQosInit();
	}

	init_entity((Entity *)p_topic);
	p_topic->enable = qos_enable_topic;
	init_topic_description(WHERE_TOPICDESCRIPTION(p_topic));

	p_topic->set_qos = set_qos;
	p_topic->get_qos = get_qos;
	p_topic->set_listener = set_listener;
	p_topic->get_listener = get_listener;
	// Access the status
	p_topic->get_inconsistent_topic_status = get_inconsistent_topic_status;

	///////////////////////////////////

	mutex_lock(&default_qos_lock);

	p_topic->topic_qos.deadline.period.sec = defaulTopicQos.deadline.period.sec;
	p_topic->topic_qos.deadline.period.nanosec = defaulTopicQos.deadline.period.nanosec;
	p_topic->topic_qos.destination_order.kind = defaulTopicQos.destination_order.kind;
	p_topic->topic_qos.durability.kind = defaulTopicQos.durability.kind;
	p_topic->topic_qos.durability_service.history_depth = defaulTopicQos.durability_service.history_depth;
	p_topic->topic_qos.durability_service.history_kind = defaulTopicQos.durability_service.history_kind;
	p_topic->topic_qos.durability_service.max_instances = defaulTopicQos.durability_service.max_instances;
	p_topic->topic_qos.durability_service.max_samples = defaulTopicQos.durability_service.max_samples;
	p_topic->topic_qos.durability_service.max_samples_per_instance = defaulTopicQos.durability_service.max_samples_per_instance;
	p_topic->topic_qos.durability_service.service_cleanup_delay.sec = defaulTopicQos.durability_service.service_cleanup_delay.sec;
	p_topic->topic_qos.durability_service.service_cleanup_delay.nanosec = defaulTopicQos.durability_service.service_cleanup_delay.nanosec;
	p_topic->topic_qos.history.depth = defaulTopicQos.history.depth;
	p_topic->topic_qos.history.kind = defaulTopicQos.history.kind;
	p_topic->topic_qos.latency_budget.duration.sec = defaulTopicQos.latency_budget.duration.sec;
	p_topic->topic_qos.latency_budget.duration.nanosec = defaulTopicQos.latency_budget.duration.nanosec;
	p_topic->topic_qos.lifespan.duration.sec = defaulTopicQos.lifespan.duration.sec;
	p_topic->topic_qos.lifespan.duration.nanosec = defaulTopicQos.lifespan.duration.nanosec;
	p_topic->topic_qos.liveliness.kind = defaulTopicQos.liveliness.kind;
	p_topic->topic_qos.liveliness.lease_duration.sec = defaulTopicQos.liveliness.lease_duration.sec;
	p_topic->topic_qos.liveliness.lease_duration.nanosec = defaulTopicQos.liveliness.lease_duration.nanosec;
	p_topic->topic_qos.ownership.kind = defaulTopicQos.ownership.kind;
	p_topic->topic_qos.reliability.kind = defaulTopicQos.reliability.kind;
	p_topic->topic_qos.reliability.max_blocking_time.sec = defaulTopicQos.reliability.max_blocking_time.sec;
	p_topic->topic_qos.reliability.max_blocking_time.nanosec = defaulTopicQos.reliability.max_blocking_time.nanosec;
	p_topic->topic_qos.resource_limits.max_instances = defaulTopicQos.resource_limits.max_instances;
	p_topic->topic_qos.resource_limits.max_samples = defaulTopicQos.resource_limits.max_samples;
	p_topic->topic_qos.resource_limits.max_samples_per_instance = defaulTopicQos.resource_limits.max_samples_per_instance;

	//added by kyy (topic_data)/////////////////////////////////////////////////////////////////////////////////////////////////////
	qos_set_qos_about_topic_data_qos_from_topic(p_topic, &defaulTopicQos);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	p_topic->topic_qos.topic_data.value = NULL;
//	if(defaulTopicQos.topic_data.value)
//		p_topic->topic_qos.topic_data.value = strdup(defaulTopicQos.topic_data.value);
	p_topic->topic_qos.transport_priority.value = defaulTopicQos.transport_priority.value;

	mutex_unlock(&default_qos_lock);

	p_topic->p_topic_listener = NULL;
	
	p_topic->i_entity_type = TOPIC_ENTITY;
	p_topic->p_rtps_topic = NULL;
	

	// status by jun
	init_topic_status(p_topic);
//	p_topic->StatusChangedFlag.b_INCONSISTENT_TOPIC_StatusChangedFlag = false;

}


void destroy_topic(Topic* p_topic)
{
	
	destroy_topic_description(WHERE_TOPICDESCRIPTION(p_topic));
	
	destroy_entity((Entity*)p_topic);

	//added by kyy (user_data)/////////////////////////////////////////////////////////////////////////////////////////////////////
	FREE(p_topic->topic_qos.topic_data.value.pp_string);
	FREE(defaulTopicQos.topic_data.value.pp_string);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	FREE(p_topic->topic_qos.topic_data.value);
//	FREE(defaulTopicQos.topic_data.value);
	FREE(p_topic);
}


ReturnCode_t static_set_default_topic_qos(in_dds TopicQos *p_qos)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

	if(p_defaulTopicQos == NULL)
	{
		defaultTopicQosInit();
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

		defaulTopicQos.deadline.period.sec = p_qos->deadline.period.sec;
		defaulTopicQos.deadline.period.nanosec = p_qos->deadline.period.nanosec;
		defaulTopicQos.destination_order.kind = p_qos->destination_order.kind;
		defaulTopicQos.durability.kind = p_qos->durability.kind;
		defaulTopicQos.durability_service.history_depth = p_qos->durability_service.history_depth;
		defaulTopicQos.durability_service.history_kind = p_qos->durability_service.history_kind;
		defaulTopicQos.durability_service.max_instances = p_qos->durability_service.max_instances;
		defaulTopicQos.durability_service.max_samples = p_qos->durability_service.max_samples;
		defaulTopicQos.durability_service.max_samples_per_instance = p_qos->durability_service.max_samples_per_instance;
		defaulTopicQos.durability_service.service_cleanup_delay.sec = p_qos->durability_service.service_cleanup_delay.sec;
		defaulTopicQos.durability_service.service_cleanup_delay.nanosec = p_qos->durability_service.service_cleanup_delay.nanosec;
		defaulTopicQos.history.depth = p_qos->history.depth;
		defaulTopicQos.history.kind = p_qos->history.kind;
		defaulTopicQos.latency_budget.duration.sec = p_qos->latency_budget.duration.sec;
		defaulTopicQos.latency_budget.duration.nanosec = p_qos->latency_budget.duration.nanosec;
		defaulTopicQos.lifespan.duration.sec = p_qos->lifespan.duration.sec;
		defaulTopicQos.lifespan.duration.nanosec = p_qos->lifespan.duration.nanosec;
		defaulTopicQos.liveliness.kind = p_qos->liveliness.kind;
		defaulTopicQos.liveliness.lease_duration.sec = p_qos->liveliness.lease_duration.sec;
		defaulTopicQos.liveliness.lease_duration.nanosec = p_qos->liveliness.lease_duration.nanosec;
		defaulTopicQos.ownership.kind = p_qos->ownership.kind;
		defaulTopicQos.reliability.kind = p_qos->reliability.kind;
		defaulTopicQos.reliability.max_blocking_time.sec = p_qos->reliability.max_blocking_time.sec;
		defaulTopicQos.reliability.max_blocking_time.nanosec = p_qos->reliability.max_blocking_time.nanosec;
		defaulTopicQos.resource_limits.max_instances = p_qos->resource_limits.max_instances;
		defaulTopicQos.resource_limits.max_samples = p_qos->resource_limits.max_samples;
		defaulTopicQos.resource_limits.max_samples_per_instance = p_qos->resource_limits.max_samples_per_instance;

		//added by kyy(TopicData QoS)
		qos_copy_topic_data_qos(&defaulTopicQos.topic_data, &p_qos->topic_data);
		////////////////////////////////////////////////////////////////////////////

//		FREE(defaulTopicQos.topic_data.value);
//		if(p_qos->topic_data.value)
//			defaulTopicQos.topic_data.value = strdup(p_qos->topic_data.value);
		defaulTopicQos.transport_priority.value = p_qos->transport_priority.value;

		mutex_unlock(&default_qos_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default Topic QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_topic_qos(inout_dds TopicQos *p_qos)
{
	if(p_defaulTopicQos == NULL)
	{
		defaultTopicQosInit();
	}

	if(p_qos)
	{
		mutex_lock(&default_qos_lock);

		p_qos->deadline.period.sec = defaulTopicQos.deadline.period.sec;
		p_qos->deadline.period.nanosec = defaulTopicQos.deadline.period.nanosec;
		p_qos->destination_order.kind = defaulTopicQos.destination_order.kind;
		p_qos->durability.kind = defaulTopicQos.durability.kind;
		p_qos->durability_service.history_depth = defaulTopicQos.durability_service.history_depth;
		p_qos->durability_service.history_kind = defaulTopicQos.durability_service.history_kind;
		p_qos->durability_service.max_instances = defaulTopicQos.durability_service.max_instances;
		p_qos->durability_service.max_samples = defaulTopicQos.durability_service.max_samples;
		p_qos->durability_service.max_samples_per_instance = defaulTopicQos.durability_service.max_samples_per_instance;
		p_qos->durability_service.service_cleanup_delay.sec = defaulTopicQos.durability_service.service_cleanup_delay.sec;
		p_qos->durability_service.service_cleanup_delay.nanosec = defaulTopicQos.durability_service.service_cleanup_delay.nanosec;
		p_qos->history.depth = defaulTopicQos.history.depth;
		p_qos->history.kind = defaulTopicQos.history.kind;
		p_qos->latency_budget.duration.sec = defaulTopicQos.latency_budget.duration.sec;
		p_qos->latency_budget.duration.nanosec = defaulTopicQos.latency_budget.duration.nanosec;
		p_qos->lifespan.duration.sec = defaulTopicQos.lifespan.duration.sec;
		p_qos->lifespan.duration.nanosec = defaulTopicQos.lifespan.duration.nanosec;
		p_qos->liveliness.kind = defaulTopicQos.liveliness.kind;
		p_qos->liveliness.lease_duration.sec = defaulTopicQos.liveliness.lease_duration.sec;
		p_qos->liveliness.lease_duration.nanosec = defaulTopicQos.liveliness.lease_duration.nanosec;
		p_qos->ownership.kind = defaulTopicQos.ownership.kind;
		p_qos->reliability.kind = defaulTopicQos.reliability.kind;
		p_qos->reliability.max_blocking_time.sec = defaulTopicQos.reliability.max_blocking_time.sec;
		p_qos->reliability.max_blocking_time.nanosec = defaulTopicQos.reliability.max_blocking_time.nanosec;
		p_qos->resource_limits.max_instances = defaulTopicQos.resource_limits.max_instances;
		p_qos->resource_limits.max_samples = defaulTopicQos.resource_limits.max_samples;
		p_qos->resource_limits.max_samples_per_instance = defaulTopicQos.resource_limits.max_samples_per_instance;

		//added by kyy(TopicData QoS)
		qos_copy_topic_data_qos(&p_qos->topic_data, &defaulTopicQos.topic_data);
		//////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->topic_data.value);
//		if(defaulTopicQos.topic_data.value)
//			p_qos->topic_data.value = strdup(defaulTopicQos.topic_data.value);
		p_qos->transport_priority.value = defaulTopicQos.transport_priority.value;

		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}