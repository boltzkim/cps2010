/* 
	Publisher 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static mutex_t	default_qos_lock;

static PublisherQos* p_defaultPublisherQos = NULL;
static PublisherQos defaultPublisherQos;

static DataWriter* create_datawriter(Publisher* p_publisher, in_dds Topic* p_topic, in_dds DataWriterQos* p_qos, in_dds DataWriterListener* p_listener, in_dds StatusMask mask)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	DataWriter* p_datawriter = NULL;
	FooTypeSupport* p_typeSupport = NULL;

	if (qos_check_inconsistent_policy_before_change_datawriter_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return NULL; //by kki

	//trace_msg(OBJECT(p_module),TRACE_LOG,"create_datawriter 1");
	
	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module),p_publisher->get_participant(p_publisher),p_topic->get_type_name(p_topic));

	//trace_msg(OBJECT(p_module),TRACE_LOG,"create_datawriter 2");
	
	if (p_typeSupport)
	{
		p_datawriter = malloc(sizeof(FooDataWriter));
		memset(p_datawriter, '\0', sizeof(FooDataWriter));

		init_foo_datawriter((FooDataWriter*)p_datawriter);
	}
	else
	{
		p_datawriter = malloc(sizeof(DataWriter));
		memset(p_datawriter, '\0', sizeof(DataWriter));

		init_datawriter(p_datawriter);
	}


	//trace_msg(OBJECT(p_module),TRACE_LOG,"create_datawriter 3");

	
	if (!p_publisher || !p_topic)
	{
		trace_msg(OBJECT(p_module), TRACE_ERROR, "Can't create a datawriter. p_publisher or p_topic is NULL.");
		FREE(p_datawriter);
		return NULL;
	}

	p_datawriter->p_topic = p_topic;
	p_datawriter->p_publisher = p_publisher;
	p_datawriter->set_listener(p_datawriter,p_listener,mask);

	if (p_qos)
	{
		p_datawriter->set_qos(p_datawriter, p_qos);
		////printf("DataWriter's HISTORY CACHE QoS : [%3d] [%3d] [%3d].\n", p_qos->history.depth, p_qos->history.kind, p_qos->resource_limits.max_samples);
	}

	//trace_msg(OBJECT(p_module),TRACE_LOG,"create_datawriter 4");
	

	mutex_lock(&p_publisher->entity_lock);
	INSERT_ELEM(p_publisher->pp_datawriters, p_publisher->i_datawriters, p_publisher->i_datawriters, p_datawriter);
	mutex_unlock(&p_publisher->entity_lock);

	//trace_msg(OBJECT(p_module),TRACE_LOG,"add_entity_to_service start");

	qos_create_ext_entity_factory_qos((Entity*)p_datawriter, (Entity*)p_publisher, &p_publisher->publisher_qos.entity_factory);//by kki (entity factory)


	//trace_msg(OBJECT(p_module),TRACE_LOG,"add_entity_to_service end");

	trace_msg(OBJECT(p_module), TRACE_DEBUG, "Create datawriter");

	monitoring__add_datawriter(p_datawriter);
	return p_datawriter;
}


static ReturnCode_t delete_datawriter(Publisher *p_publisher, in_dds DataWriter *p_datawriter)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if (!p_publisher || !p_datawriter)
	{
		trace_msg(OBJECT(p_module), TRACE_ERROR, "Can't delete a DataWriter. p_publisher or p_a_datawriter is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_publisher->entity_lock);
	for (i = 0; i < p_publisher->i_datawriters; i++)
	{
		if (p_publisher->pp_datawriters[i] == p_datawriter)
		{
			monitoring__delete_datawriter(p_datawriter);

			destroy_datawriter(p_datawriter);
			REMOVE_ELEM(p_publisher->pp_datawriters, p_publisher->i_datawriters, i);
			mutex_unlock(&p_publisher->entity_lock);
			trace_msg(OBJECT(p_module), TRACE_DEBUG, "Delete datawriter.");
			
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_publisher->entity_lock);

	trace_msg(OBJECT(p_module), TRACE_WARM, "Can't delete a DataWriter. It's not exist.");

	return RETCODE_ERROR;
}


static DataWriter* lookup_datawriter(Publisher* p_publisher, in_dds string topic_name)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	for (i = 0; i < p_publisher->i_datawriters; i++)
	{
		if (p_publisher->pp_datawriters[i]->p_topic && !strcmp(p_publisher->pp_datawriters[i]->p_topic->topic_name, topic_name))
		{
			trace_msg(OBJECT(p_module), TRACE_TRACE, "Lookup a DataWriter : %s", topic_name);

			return p_publisher->pp_datawriters[i];
		}
	}

	trace_msg(OBJECT(p_module), TRACE_WARM, "Can't lookup a DataWriter : %s", topic_name);
	
	return NULL;
}

static ReturnCode_t delete_contained_entities(Publisher* p_publisher)
{
	int i_size;
	int i;

	//할당된 DataWriter들 삭제..
	mutex_lock(&p_publisher->entity_lock);

	i_size = p_publisher->i_datawriters;

	for (i = i_size-1; i >= 0; i--)
  {
		destroy_datawriter(p_publisher->pp_datawriters[i]);
		REMOVE_ELEM(p_publisher->pp_datawriters, p_publisher->i_datawriters, i);
	}

	/*while(p_publisher->i_datawriters)
	{
		destroy_datawriter(p_publisher->pp_datawriters[0]);
		REMOVE_ELEM( p_publisher->pp_datawriters, p_publisher->i_datawriters, 0);
	}*/

	mutex_unlock(&p_publisher->entity_lock);

	FREE(p_publisher->pp_datawriters);

	return RETCODE_OK;
}

static ReturnCode_t set_qos(Publisher* p_publisher, in_dds PublisherQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_publisher && p_qos)
	{
		int i;

		if (qos_check_immutable_policy_before_change_publisher_qos(p_publisher, p_qos) == RETCODE_IMMUTABLE_POLICY) return RETCODE_IMMUTABLE_POLICY; //by kki

		mutex_lock(&p_publisher->entity_lock);
		p_publisher->publisher_qos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;

		//added by kyy (GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_init_group_data_qos(&p_publisher->publisher_qos.group_data);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_set_qos_about_group_data_qos_from_publisher(p_publisher, p_qos);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_publisher->publisher_qos.group_data.value);

//		if(p_qos->group_data.value)
//		{
//			p_publisher->publisher_qos.group_data.value = strdup(p_qos->group_data.value);
//		}

		//REMOVE_STRING_SEQ(p_publisher->publisher_qos.partition.name);
		p_publisher->publisher_qos.partition.name.i_string = 0;
		p_publisher->publisher_qos.partition.name.pp_string = NULL;

		for (i = 0 ;i < p_qos->partition.name.i_string;i++)
		{
			string str = strdup(p_qos->partition.name.pp_string[i]);
			INSERT_ELEM(p_publisher->publisher_qos.partition.name.pp_string, p_publisher->publisher_qos.partition.name.i_string, p_publisher->publisher_qos.partition.name.i_string, str);
		}
		
		p_publisher->publisher_qos.presentation.access_scope = p_qos->presentation.access_scope;
		p_publisher->publisher_qos.presentation.coherent_access = p_qos->presentation.coherent_access;
		p_publisher->publisher_qos.presentation.ordered_access = p_qos->presentation.ordered_access;

		mutex_unlock(&p_publisher->entity_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Publisher QoS is changed.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_qos(Publisher *p_publisher, inout_dds PublisherQos *p_qos)
{
	if (p_publisher && p_qos)
	{
		int i;
		mutex_lock(&p_publisher->entity_lock);

		p_qos->entity_factory.autoenable_created_entities = p_publisher->publisher_qos.entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_init_group_data_qos(&p_publisher->publisher_qos.group_data);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_get_qos_about_group_data_qos_from_publisher(p_publisher, p_qos);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->group_data.value);

//		if(p_publisher->publisher_qos.group_data.value)
//		{
//			p_qos->group_data.value = strdup(p_publisher->publisher_qos.group_data.value);
//		}

		REMOVE_STRING_SEQ(p_qos->partition.name);
		
		for (i = 0 ;i < p_publisher->publisher_qos.partition.name.i_string;i++)
		{
			string str = strdup(p_publisher->publisher_qos.partition.name.pp_string[i]);
			INSERT_ELEM(p_qos->partition.name.pp_string, p_qos->partition.name.i_string, p_qos->partition.name.i_string, str);
		}
		
		p_qos->presentation.access_scope = p_publisher->publisher_qos.presentation.access_scope;
		p_qos->presentation.coherent_access = p_publisher->publisher_qos.presentation.coherent_access;
		p_qos->presentation.ordered_access = p_publisher->publisher_qos.presentation.ordered_access;

		mutex_unlock(&p_publisher->entity_lock);
	}

	return RETCODE_OK;
}

static ReturnCode_t set_listener(Publisher *p_publisher, in_dds PublisherListener *p_listener,in_dds StatusMask mask)
{
	if (p_publisher)
	{
		if (p_listener) p_publisher->p_publisher_listener = p_listener;

		p_publisher->l_status_mask = mask;

		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static PublisherListener *get_listener(Publisher *p_publisher)
{
	if (p_publisher) return p_publisher->p_publisher_listener;

	return NULL;
}

static ReturnCode_t suspend_publications(Publisher *p_publisher)
{
	/*
	This operation indicates to the Service that the application is about to make multiple modifications using DataWriter
	objects belonging to the Publisher.
	It is a hint to the Service so it can optimize its performance by e.g., holding the dissemination of the modifications and
	then batching them.
	It is not required that the Service use this hint in_dds any way.
	The use of this operation must be matched by a corresponding call to resume_publications indicating that the set of
	modifications has completed. If the Publisher is deleted before resume_publications is called, any suspended updates yet
	to be published will be discarded.
	*/

	p_publisher->is_suspend = true;
	return RETCODE_OK;
}

static ReturnCode_t resume_publications(Publisher *p_publisher)
{
	/*
	This operation indicates to the Service that the application has completed the multiple changes initiated by the previous
	suspend_publications. This is a hint to the Service that can be used by a Service implementation to e.g., batch all the
	modifications made since the suspend_publications.
	The call to resume_publications must match a previous call to suspend_publications. Otherwise the operation will return
	the error PRECONDITION_NOT_MET.
	Possible error codes returned in_dds addition to the standard ones: PRECONDITION_NOT_MET.
	*/

	p_publisher->is_suspend = false;
	return RETCODE_OK;
}

static ReturnCode_t begin_coherent_changes(Publisher *p_publisher)
{
	/*
	This operation requests that the application will begin_dds a ‘coherent set’ of modifications using DataWriter objects attached
	to the Publisher. The ‘coherent set’ will be completed by a matching call to end_coherent_changes.
	A ‘coherent set’ is a set of modifications that must be propagated in_dds such a way that they are interpreted at the receivers’
	side as a consistent set of modifications; that is, the receiver will only be able to access the data after all the modifications
	in_dds the set are available at the receiver end12.
	A connectivity change may occur in_dds the middle of a set of coherent changes; for example, the set of partitions used by the
	Publisher or one of its Subscribers may change, a late-joining DataReader may appear on the network, or a
	communication failure may occur. in_dds the event that such a change prevents an entity from receiving the entire set of
	coherent changes, that entity must behave as if it had received none of the set.
	These calls can be nested. in_dds that case, the coherent set terminates only with the last call to end_coherent_ changes.
	The support for ‘coherent changes’ enables a publishing application to change the value of several data-instances that
	could belong to the same or different topics and have those changes be seen ‘atomically’ by the readers. This is useful in
	cases where the values are inter-related. For example, if there are two data-instances representing the ‘altitude’ and
	‘velocity vector’ of the same aircraft and both are changed, it may be useful to communicate those values in_dds a way the
	reader can see both together; otherwise, it may e.g., erroneously interpret that the aircraft is on a collision course.
	*/

	p_publisher->begin_coherent = true;

	return RETCODE_OK;
}

static ReturnCode_t end_coherent_changes(Publisher *p_publisher)
{

	Time_t message_time = currenTime();
	/*
	This operation terminates the ‘coherent set’ initiated by the matching call to begin_coherent_ changes. If there is no
	matching call to begin_coherent_ changes, the operation will return the error PRECONDITION_NOT_MET.
	Possible error codes returned in_dds addition to the standard ones: PRECONDITION_NOT_MET.
	*/
	int i;

	p_publisher->begin_coherent = false;

	//added by kyy(Presentation QoS Writer Side)
	//Publisher가 가진 모든 Writer에게 NULL Data를 보내어 원격의 Reader에게 coherent set이 끝남을 알림
	for (i = 0; i < p_publisher->i_datawriters; i++)
	{
		datawriter_write(p_publisher->pp_datawriters[i],NULL,0,message_time);
	}

	return RETCODE_OK;
	//return RETCODE_PRECONDITION_NOT_MET;
}


static ReturnCode_t wait_for_acknowledgments(Publisher *p_publisher, in_dds Duration_t max_wait)
{
	/*
	This operation blocks the calling thread until either all data written by the reliable DataWriter entities is acknowledged by
	all matched reliable DataReader entities, or else the duration specified by the max_wait parameter elapses, whichever
	happens first. A return value of OK indicates that all the samples written have been acknowledged by all reliable matched
	data readers; a return value of TIMEOUT indicates that max_wait elapsed before all the data was acknowledged.
	*/


	int i_size = p_publisher->i_datawriters;
	int i;

	for (i = 0; i < i_size; i++)
	{
		p_publisher->pp_datawriters[i]->wait_for_acknowledgments(p_publisher->pp_datawriters[i], max_wait);
	}
	
	return RETCODE_OK;
}

static DomainParticipant *get_participant(Publisher *p_publisher)
{
	if (p_publisher)
	{
		return p_publisher->p_domain_participant;
	}

	return NULL;
}

ReturnCode_t set_default_datawriter_qos(in_dds DataWriterQos *p_qos)
{
	if (p_qos) static_set_default_datawriter_qos(p_qos);
	return RETCODE_OK;
}

ReturnCode_t get_default_datawriter_qos(inout_dds DataWriterQos *p_qos)
{
	if (p_qos) static_get_default_datawriter_qos(p_qos);
	return RETCODE_OK;
}

ReturnCode_t copy_from_topic_qos(inout_dds DataWriterQos *p_datawriter_qos,in_dds TopicQos *p_topic_qos)
{
	if (p_datawriter_qos && p_datawriter_qos)
	{
		p_datawriter_qos->deadline.period.sec = p_topic_qos->deadline.period.sec;
		p_datawriter_qos->deadline.period.nanosec = p_topic_qos->deadline.period.nanosec;
		p_datawriter_qos->destination_order.kind = p_topic_qos->destination_order.kind;
		p_datawriter_qos->durability_service.history_depth = p_topic_qos->durability_service.history_depth;
		p_datawriter_qos->durability_service.history_kind = p_topic_qos->durability_service.history_kind;
		p_datawriter_qos->durability_service.max_instances = p_topic_qos->durability_service.max_instances;
		p_datawriter_qos->durability_service.max_samples = p_topic_qos->durability_service.max_samples;
		p_datawriter_qos->durability_service.max_samples_per_instance = p_topic_qos->durability_service.max_samples_per_instance;
		p_datawriter_qos->durability_service.service_cleanup_delay.sec = p_topic_qos->durability_service.service_cleanup_delay.sec;
		p_datawriter_qos->durability_service.service_cleanup_delay.nanosec = p_topic_qos->durability_service.service_cleanup_delay.nanosec;
		p_datawriter_qos->history.depth = p_topic_qos->history.depth;
		p_datawriter_qos->history.kind = p_topic_qos->history.kind;
		p_datawriter_qos->latency_budget.duration.sec = p_topic_qos->latency_budget.duration.sec;
		p_datawriter_qos->latency_budget.duration.nanosec = p_topic_qos->latency_budget.duration.nanosec;
		p_datawriter_qos->lifespan.duration.sec = p_topic_qos->lifespan.duration.sec;
		p_datawriter_qos->lifespan.duration.nanosec = p_topic_qos->lifespan.duration.nanosec;
		p_datawriter_qos->liveliness.kind = p_topic_qos->liveliness.kind;
		p_datawriter_qos->liveliness.lease_duration.sec = p_topic_qos->liveliness.lease_duration.sec;
		p_datawriter_qos->liveliness.lease_duration.nanosec = p_topic_qos->liveliness.lease_duration.nanosec;
		p_datawriter_qos->ownership.kind = p_topic_qos->ownership.kind;
		p_datawriter_qos->reliability.kind = p_topic_qos->reliability.kind;
		p_datawriter_qos->reliability.max_blocking_time.sec = p_topic_qos->reliability.max_blocking_time.sec;
		p_datawriter_qos->reliability.max_blocking_time.nanosec = p_topic_qos->reliability.max_blocking_time.nanosec;
		p_datawriter_qos->resource_limits.max_instances = p_topic_qos->resource_limits.max_instances;
		p_datawriter_qos->resource_limits.max_samples = p_topic_qos->resource_limits.max_samples;
		p_datawriter_qos->resource_limits.max_samples_per_instance = p_topic_qos->resource_limits.max_samples_per_instance;
		p_datawriter_qos->transport_priority.value = p_topic_qos->transport_priority.value;
	}

	return RETCODE_OK;
}


static defaultPublisherQosInit()
{
	mutex_init(&default_qos_lock);
	//defaultPublisherQos 초기화. 
	defaultPublisherQos.entity_factory.autoenable_created_entities = true;

	//added by kyy(GroupData QoS)/////////////////////////////////////
	qos_init_group_data_qos(&defaultPublisherQos.group_data);
	//////////////////////////////////////////////////////////////

//	defaultPublisherQos.group_data.value = NULL;
	defaultPublisherQos.partition.name.i_string = 0;
	defaultPublisherQos.partition.name.pp_string = NULL;
	defaultPublisherQos.presentation.access_scope = 0;
	defaultPublisherQos.presentation.coherent_access = false;
	defaultPublisherQos.presentation.ordered_access = false;

	p_defaultPublisherQos = &defaultPublisherQos;
}

extern ReturnCode_t enable_publisher(Entity *p_entity);
void init_publisher(Publisher* p_publisher)
{
	int i;

	if (p_defaultPublisherQos == NULL)
	{
		defaultPublisherQosInit();
	}

	init_entity((Entity *)p_publisher);
	p_publisher->enable = qos_enable_publisher;

	p_publisher->create_datawriter = create_datawriter;
	p_publisher->delete_datawriter = delete_datawriter;
	p_publisher->lookup_datawriter = lookup_datawriter;
	p_publisher->delete_contained_entities = delete_contained_entities;
	p_publisher->set_qos = set_qos;
	p_publisher->get_qos = get_qos;
	p_publisher->set_listener = set_listener;
	p_publisher->get_listener = get_listener;
	p_publisher->suspend_publications = suspend_publications;
	p_publisher->resume_publications = resume_publications;
	p_publisher->begin_coherent_changes = begin_coherent_changes;
	p_publisher->end_coherent_changes = end_coherent_changes;
	p_publisher->wait_for_acknowledgments = wait_for_acknowledgments;
	p_publisher->get_participant = get_participant;
	p_publisher->set_default_datawriter_qos = set_default_datawriter_qos;
	p_publisher->get_default_datawriter_qos = get_default_datawriter_qos;
	p_publisher->copy_from_topic_qos = copy_from_topic_qos;

	///////

	mutex_lock(&default_qos_lock);

	p_publisher->publisher_qos.entity_factory.autoenable_created_entities = defaultPublisherQos.entity_factory.autoenable_created_entities = true;

	//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_group_data_qos(&p_publisher->publisher_qos.group_data);
	if (defaultPublisherQos.group_data.value.i_string > 0)
	{
		qos_copy_group_data_qos(&p_publisher->publisher_qos.group_data, &defaultPublisherQos.group_data);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	p_publisher->publisher_qos.group_data.value = NULL;

//	if(defaultPublisherQos.group_data.value)
//	{
//		p_publisher->publisher_qos.group_data.value = strdup(defaultPublisherQos.group_data.value);
//	}

	if (defaultPublisherQos.partition.name.i_string > 0)
	{
		for (i = 0; i < defaultPublisherQos.partition.name.i_string; i++)
		{
			string str = strdup(defaultPublisherQos.partition.name.pp_string[i]);
			INSERT_ELEM(p_publisher->publisher_qos.partition.name.pp_string, p_publisher->publisher_qos.partition.name.i_string, p_publisher->publisher_qos.partition.name.i_string, str);
		}
	}

	p_publisher->publisher_qos.presentation.access_scope = defaultPublisherQos.presentation.access_scope;
	
	p_publisher->publisher_qos.presentation.coherent_access = defaultPublisherQos.presentation.coherent_access;
	p_publisher->publisher_qos.presentation.ordered_access = defaultPublisherQos.presentation.ordered_access;

	mutex_unlock(&default_qos_lock);

	p_publisher->p_publisher_listener = NULL;
	p_publisher->i_datawriters = 0;
	p_publisher->pp_datawriters = NULL;

	p_publisher->p_domain_participant = NULL;
	p_publisher->i_entity_type = PUBLISHER_ENTITY;

	p_publisher->is_suspend = false;
	p_publisher->begin_coherent = false;
}


void destroy_publisher(Publisher* p_publisher)
{
	delete_contained_entities(p_publisher);

	REMOVE_STRING_SEQ(p_publisher->publisher_qos.partition.name);
	REMOVE_STRING_SEQ(defaultPublisherQos.partition.name);

	destroy_entity((Entity*)p_publisher);

	//added by kyy(GroupData QoS)
	FREE(p_publisher->publisher_qos.group_data.value.pp_string);
	//FREE(p_publisher->publisher_qos.group_data.value);
	FREE(p_publisher);
}



ReturnCode_t static_set_default_publisher_qos(in_dds PublisherQos *p_qos)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

	if(p_defaultPublisherQos == NULL)
	{
		defaultPublisherQosInit();
	}

	if(p_qos)
	{
		int i;
		mutex_lock(&default_qos_lock);

		defaultPublisherQos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_init_group_data_qos(&defaultPublisherQos.group_data);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_copy_group_data_qos(&defaultPublisherQos.group_data, &p_qos->group_data);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(defaultPublisherQos.group_data.value);
//		if(p_qos->group_data.value)
//		{
//			defaultPublisherQos.group_data.value = strdup(p_qos->group_data.value);
//		}

		REMOVE_STRING_SEQ(defaultPublisherQos.partition.name);
		
		for(i=0 ;i < p_qos->partition.name.i_string;i++)
		{
			string str = strdup(p_qos->partition.name.pp_string[i]);
			INSERT_ELEM( defaultPublisherQos.partition.name.pp_string, defaultPublisherQos.partition.name.i_string,
						 defaultPublisherQos.partition.name.i_string, str );
		}
		
		defaultPublisherQos.presentation.access_scope = p_qos->presentation.access_scope;
		defaultPublisherQos.presentation.coherent_access = p_qos->presentation.coherent_access;
		defaultPublisherQos.presentation.ordered_access = p_qos->presentation.ordered_access;

		mutex_unlock(&default_qos_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default Publisher QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_publisher_qos(inout_dds PublisherQos *p_qos)
{
	if(p_defaultPublisherQos == NULL)
	{
		defaultPublisherQosInit();
	}

	if(p_qos)
	{
		int i;
		mutex_lock(&default_qos_lock);

		p_qos->entity_factory.autoenable_created_entities = defaultPublisherQos.entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)//////////////////////////////////////////////////////////
		qos_init_group_data_qos(&p_qos->group_data);
		if (defaultPublisherQos.group_data.value.i_string > 0)
		{
			qos_copy_group_data_qos(&p_qos->group_data, &defaultPublisherQos.group_data);
		}
		else
		{
			p_qos->group_data.value.i_string = 0;
			p_qos->group_data.value.pp_string = NULL;
		}
		/////////////////////////////////////////////////////////////////////////////////////


//		FREE(p_qos->group_data.value);
//		if(defaultPublisherQos.group_data.value)
//		{
//			p_qos->group_data.value = strdup(defaultPublisherQos.group_data.value);
//		}else{
//			p_qos->group_data.value = NULL;
//		}

		/*if(p_qos->partition.name.i_string) {
			REMOVE_STRING_SEQ(p_qos->partition.name);
		}*/
		p_qos->partition.name.i_string = 0;
		p_qos->partition.name.pp_string = NULL;
		
		for(i=0 ;i < defaultPublisherQos.partition.name.i_string;i++)
		{
			string str = strdup(defaultPublisherQos.partition.name.pp_string[i]);
			INSERT_ELEM( p_qos->partition.name.pp_string, p_qos->partition.name.i_string,
						 p_qos->partition.name.i_string, str );
		}
		
		p_qos->presentation.access_scope = defaultPublisherQos.presentation.access_scope;
		p_qos->presentation.coherent_access = defaultPublisherQos.presentation.coherent_access;
		p_qos->presentation.ordered_access = defaultPublisherQos.presentation.ordered_access;

		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}