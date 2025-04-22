/* 
	Subscriber 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static mutex_t	default_qos_lock;

static SubscriberQos* p_defaultSubscriberQos = NULL;
static SubscriberQos defaultSubscriberQos;

static DataReader* create_datareader(Subscriber* p_subscriber, in_dds /*TopicDescription*/ Topic* p_topic, in_dds DataReaderQos* p_qos, in_dds DataReaderListener* p_listener, in_dds StatusMask mask)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	DataReader* p_datareader = NULL;
	FooTypeSupport* p_typeSupport = NULL;
	
	if (qos_check_inconsistent_policy_before_change_datareader_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return NULL; //by kki

	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_subscriber->get_participant(p_subscriber), p_topic->get_type_name(p_topic));
	

	p_datareader = malloc(sizeof(FooDataReader));
	memset(p_datareader, '\0', sizeof(FooDataReader));

	init_foo_datareader((FooDataReader *)p_datareader);
	


	if (!p_subscriber || !p_topic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create a DataReader. p_subscriber or p_topic is NULL.");
		FREE(p_datareader);
		return NULL;
	}

	p_datareader->p_topic = p_topic;
	p_datareader->p_subscriber = p_subscriber;
	p_datareader->set_listener(p_datareader,p_listener,mask);

	if (p_qos)
	{
		p_datareader->set_qos(p_datareader, p_qos);
		////printf("DataReader's HISTORY CACHE QoS : [%3d] [%3d] [%3d].\n", p_qos->history.depth, p_qos->history.kind, p_qos->resource_limits.max_samples);
	}
	

	mutex_lock(&p_subscriber->entity_lock);
	INSERT_ELEM(p_subscriber->pp_datareaders, p_subscriber->i_datareaders, p_subscriber->i_datareaders, p_datareader);
	mutex_unlock(&p_subscriber->entity_lock);

	qos_create_ext_entity_factory_qos((Entity*)p_datareader, (Entity*)p_subscriber, &p_subscriber->subscriber_qos.entity_factory);//by kki (entity factory)

	trace_msg(OBJECT(p_module), TRACE_DEBUG, "Create DataReader");

	monitoring__add_datareader(p_datareader);
	return p_datareader;
}

static ReturnCode_t delete_datareader(Subscriber *p_subscriber, in_dds DataReader *p_datareader)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_subscriber || !p_datareader)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a DataReader. p_subscriber or p_datareader is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_subscriber->entity_lock);
	for(i=0; i < p_subscriber->i_datareaders; i++)
	{
		if(p_subscriber->pp_datareaders[i] == p_datareader)
		{
			monitoring__delete_datareader(p_datareader);

			destroy_datareader(p_datareader);
			REMOVE_ELEM( p_subscriber->pp_datareaders, p_subscriber->i_datareaders, i);
			mutex_unlock(&p_subscriber->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete datawriter.");
			
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_subscriber->entity_lock);
	

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a DataReader. It's not exist.");

	return RETCODE_ERROR;
}


static ReturnCode_t delete_contained_entities(Subscriber *p_subscriber)
{
	int i_size, i;

	//할당된 DataReader들 삭제..
	mutex_lock(&p_subscriber->entity_lock);

	i_size = p_subscriber->i_datareaders;

	for(i = i_size-1; i >= 0; i--){

		destroy_datareader(p_subscriber->pp_datareaders[i]);
		REMOVE_ELEM(p_subscriber->pp_datareaders, p_subscriber->i_datareaders, i);
	}

	/*while(p_subscriber->i_datareaders)
	{
		destroy_datareader(p_subscriber->pp_datareaders[0]);
		REMOVE_ELEM( p_subscriber->pp_datareaders, p_subscriber->i_datareaders, 0);
	}*/
	mutex_unlock(&p_subscriber->entity_lock);

	FREE(p_subscriber->pp_datareaders);

	return RETCODE_OK;
}

static DataReader* lookup_datareader(Subscriber *p_subscriber, in_dds string topic_name)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	for(i = 0; i < p_subscriber->i_datareaders; i++)
	{
		if (p_subscriber->pp_datareaders[i]->p_topic && !strcmp(p_subscriber->pp_datareaders[i]->p_topic->topic_name, topic_name))
		{
			trace_msg(OBJECT(p_module),TRACE_TRACE,"Lookup a DataReader : %s", topic_name);

			return p_subscriber->pp_datareaders[i];
		}
	}

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't lookup a DataReader : %s", topic_name);
	
	return NULL;
}

static ReturnCode_t get_datareaders(Subscriber* p_subscriber, inout_dds DataReaderSeq* p_readers, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	/*
	This operation allows the application to access the DataReader objects that contain_dds samples with the specified
	sample_states, view_states, and instance_states.
	If the PRESENTATION QosPolicy of the Subscriber to which the DataReader belongs has the access_scope set to
	‘GROUP.’ This operation should only be invoked inside a begin_access/end_access block. Otherwise it will return the
	error PRECONDITION_NOT_MET.
	Depending on the setting of the PRESENTATION QoS policy (see Section 7.1.3.6, “PRESENTATION,” on page 110), the
	returned collection of DataReader objects may be a ‘set’ containing each DataReader at most once in_dds no specified order,
	or a ‘list’ containing each DataReader one or more times in_dds a specific order.
	1. If PRESENTATION access_scope is INSTANCE or TOPIC, the returned collection is a ‘set.’
	2. If PRESENTATION access_scope is GROUP and ordered_access is set to TRUE, then the returned collection is a
	‘list.’
	This difference is due to the fact that, in_dds the second situation it is required to access samples belonging to different
	DataReader objects in_dds a particular order. in_dds this case, the application should process each DataReader in_dds the same order
	it appears in_dds the ‘list’ and read or take exactly one sample from each DataReader. The patterns that an application should
	use to access data is fully described in_dds Section 7.1.2.5.1, “Access to the data,” on page 62.
	*/

	// 아직 SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states 에 대해서 하지는 않았음
	// 반드시 체크 해야 됨.
	if(p_subscriber && p_readers)
	{
		int i;

		for(i=0; i < p_subscriber->i_datareaders; i++)
		{
			mutex_lock(&p_subscriber->entity_lock);
			
			INSERT_ELEM( p_readers->pp_datareaders, p_readers->i_seq,
				p_readers->i_seq, p_subscriber->pp_datareaders[i] );


			mutex_unlock(&p_subscriber->entity_lock);
		}

	}

	return RETCODE_OK;
}

static ReturnCode_t notify_datareaders(Subscriber *p_subscriber)
{
	/*
	This operation invokes the operation on_data_available on the DataReaderListener objects attached to contained
	DataReader entities with a DATA_AVAILABLE status that is considered changed as described in_dds Section 7.1.4.2.2,
	“Changes in_dds Read Communication Statuses.”
	This operation is typically invoked from the on_data_on_readers operation in_dds the SubscriberListener. That way the
	SubscriberListener can delegate to the DataReaderListener objects the handling of the data.
	*/

	//not yet implemented

	DataReader *p_datareader = NULL;
	int i;
	DataReaderListener *p_datareaderListener = NULL;

	for( i = 0 ; i < p_subscriber->i_datareaders; i++)
	{
		p_datareader = p_subscriber->pp_datareaders[i];
		p_datareaderListener = p_datareader->get_listener(p_datareader);

		if(p_datareaderListener && p_datareaderListener->on_data_available)
		{
			p_datareaderListener->on_data_available(p_datareader);
			p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
		}
	}

	return RETCODE_ERROR;
}

static ReturnCode_t set_qos(Subscriber* p_subscriber, in_dds SubscriberQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_subscriber && p_qos)
	{
		int i;

		if (qos_check_immutable_policy_before_change_subscriber_qos(p_subscriber, p_qos) == RETCODE_IMMUTABLE_POLICY) return RETCODE_IMMUTABLE_POLICY; //by kki

		mutex_lock(&p_subscriber->entity_lock);
		
		p_subscriber->subscriber_qos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;

		//added by kyy (GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_init_group_data_qos(&p_subscriber->subscriber_qos.group_data);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_set_qos_about_group_data_qos_from_subscriber(p_subscriber, p_qos);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_subscriber->subscriber_qos.group_data.value);
//		if(p_qos->group_data.value)
//		{
//			p_subscriber->subscriber_qos.group_data.value = strdup(p_qos->group_data.value);
//		}

		//REMOVE_STRING_SEQ(p_subscriber->subscriber_qos.partition.name);
		p_subscriber->subscriber_qos.partition.name.i_string = 0;
		p_subscriber->subscriber_qos.partition.name.pp_string = NULL;
		for(i=0 ;i < p_qos->partition.name.i_string;i++)
		{
			string str = strdup(p_qos->partition.name.pp_string[i]);
			INSERT_ELEM( p_subscriber->subscriber_qos.partition.name.pp_string, p_subscriber->subscriber_qos.partition.name.i_string,
						 p_subscriber->subscriber_qos.partition.name.i_string, str );
		}
		
		p_subscriber->subscriber_qos.presentation.access_scope = p_qos->presentation.access_scope;
		p_subscriber->subscriber_qos.presentation.coherent_access = p_qos->presentation.coherent_access;
		p_subscriber->subscriber_qos.presentation.ordered_access = p_qos->presentation.ordered_access;

		mutex_unlock(&p_subscriber->entity_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Subscriber QoS is changed.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_qos(Subscriber *p_subscriber, inout_dds SubscriberQos *p_qos)
{
	if(p_subscriber && p_qos)
	{
		int i;
		mutex_lock(&p_subscriber->entity_lock);

		p_qos->entity_factory.autoenable_created_entities = p_subscriber->subscriber_qos.entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_init_group_data_qos(&p_subscriber->subscriber_qos.group_data);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_get_qos_about_group_data_qos_from_subscriber(p_subscriber, p_qos);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->group_data.value);
//		if(p_subscriber->subscriber_qos.group_data.value)
//		{
//			p_qos->group_data.value = strdup(p_subscriber->subscriber_qos.group_data.value);
//		}

		REMOVE_STRING_SEQ(p_qos->partition.name);
		
		for(i=0 ;i < p_subscriber->subscriber_qos.partition.name.i_string;i++)
		{
			string str = strdup(p_subscriber->subscriber_qos.partition.name.pp_string[i]);
			INSERT_ELEM( p_qos->partition.name.pp_string, p_qos->partition.name.i_string,
						 p_qos->partition.name.i_string, str );
		}
		
		p_qos->presentation.access_scope = p_subscriber->subscriber_qos.presentation.access_scope;
		p_qos->presentation.coherent_access = p_subscriber->subscriber_qos.presentation.coherent_access;
		p_qos->presentation.ordered_access = p_subscriber->subscriber_qos.presentation.ordered_access;

		mutex_unlock(&p_subscriber->entity_lock);
	}

	return RETCODE_OK;
}

static ReturnCode_t set_listener(Subscriber *p_subscriber, in_dds SubscriberListener *p_listener,in_dds StatusMask mask)
{
	if(p_subscriber)
	{
		if(p_listener) p_subscriber->p_subscriber_listener = p_listener;

		p_subscriber->l_status_mask = mask;

		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static SubscriberListener *get_listener(Subscriber *p_subscriber)
{
	if(p_subscriber) return p_subscriber->p_subscriber_listener;

	return NULL;
}


static ReturnCode_t begin_access(Subscriber *p_subscriber)
{
	/*
	This operation indicates that the application is about to access the data samples in_dds any of the DataReader objects attached
	to the Subscriber.
	The application is required to use this operation only if PRESENTATION QosPolicy of the Subscriber to which the
	DataReader belongs has the access_scope set to ‘GROUP.’
	in_dds the aforementioned case, the operation begin_access must be called prior to calling any of the sample-accessing
	operations, namely: get_datareaders on the Subscriber and read, take, read_w_condition, take_w_condition on any
	DataReader. Otherwise the sample-accessing operations will return the error PRECONDITION_NOT_MET. Once the
	application has finished accessing the data samples it must call end_access.
	It is not required for the application to call begin_access/end_access if the PRESENTATION QosPolicy has the
	access_scope set to something other than ‘GROUP.’ Calling begin_access/end_access in_dds this case is not considered an
	error and has no effect.
	The calls to begin_access/end_access may be nested. in_dds that case, the application must call end_access as many times as
	it called begin_access.
	Possible error codes returned in_dds addition to the standard ones: PRECONDITION_NOT_MET.
	*/

	//not yet implemented
	return RETCODE_PRECONDITION_NOT_MET;
}

static ReturnCode_t end_access(Subscriber *p_subscriber)
{
	/*
	Indicates that the application has finished accessing the data samples in_dds DataReader objects managed by the Subscriber.
	This operation must be used to ‘close’ a corresponding begin_access.
	After calling end_access the application should no longer access any of the Data or SampleInfo elements returned from
	the sample-accessing operations. This call must close a previous call to begin_access otherwise the operation will return
	the error PRECONDITION_NOT_MET.
	Possible error codes returned in_dds addition to the standard ones: PRECONDITION_NOT_MET.

	This operation allows the application to access the DataReader objects that contain_dds samples with the specified
	sample_states, view_states, and instance_states.
	If the PRESENTATION QosPolicy of the Subscriber to which the DataReader belongs has the access_scope set to
	‘GROUP.’ This operation should only be invoked inside a begin_access/end_access block. Otherwise it will return the
	error PRECONDITION_NOT_MET.
	Depending on the setting of the PRESENTATION QoS policy (see Section 7.1.3.6, “PRESENTATION,” on page 110), the
	returned collection of DataReader objects may be a ‘set’ containing each DataReader at most once in_dds no specified order,
	or a ‘list’ containing each DataReader one or more times in_dds a specific order.
	1. If PRESENTATION access_scope is INSTANCE or TOPIC, the returned collection is a ‘set.’
	2. If PRESENTATION access_scope is GROUP and ordered_access is set to TRUE, then the returned collection is a
	‘list.’
	This difference is due to the fact that, in_dds the second situation it is required to access samples belonging to different
	DataReader objects in_dds a particular order. in_dds this case, the application should process each DataReader in_dds the same order
	it appears in_dds the ‘list’ and read or take exactly one sample from each DataReader. The patterns that an application should
	use to access data is fully described in_dds Section 7.1.2.5.1, “Access to the data,” on page 62.
	*/

	//not yet implemented
	return RETCODE_ERROR;
}

static DomainParticipant *get_participant(Subscriber *p_subscriber)
{
	if(p_subscriber) return p_subscriber->p_domain_participant;
	return NULL;
}

static ReturnCode_t set_default_datareader_qos(in_dds DataReaderQos *p_qos)
{
	if(p_qos) static_set_default_datareader_qos(p_qos);
	return RETCODE_OK;
}

static ReturnCode_t get_default_datareader_qos(inout_dds DataReaderQos *p_qos)
{
	if(p_qos) static_get_default_datareader_qos(p_qos);
	return RETCODE_OK;
}

static ReturnCode_t copy_from_topic_qos(inout_dds DataReaderQos *p_datareader_qos,in_dds TopicQos *p_topic_qos)
{
	if(p_datareader_qos && p_topic_qos)
	{
		p_datareader_qos->deadline.period.sec = p_topic_qos->deadline.period.sec;
		p_datareader_qos->deadline.period.nanosec = p_topic_qos->deadline.period.nanosec;
		p_datareader_qos->destination_order.kind = p_topic_qos->destination_order.kind;
		p_datareader_qos->durability.kind = p_topic_qos->durability.kind;
		p_datareader_qos->history.depth = p_topic_qos->history.depth;
		p_datareader_qos->history.kind = p_topic_qos->history.kind;
		p_datareader_qos->latency_budget.duration.sec = p_topic_qos->latency_budget.duration.sec;
		p_datareader_qos->latency_budget.duration.nanosec = p_topic_qos->latency_budget.duration.nanosec;
		p_datareader_qos->liveliness.kind = p_topic_qos->liveliness.kind;
		p_datareader_qos->liveliness.lease_duration.sec = p_topic_qos->liveliness.lease_duration.sec;
		p_datareader_qos->liveliness.lease_duration.nanosec = p_topic_qos->liveliness.lease_duration.nanosec;
		p_datareader_qos->ownership.kind = p_topic_qos->ownership.kind;
		p_datareader_qos->reliability.kind = p_topic_qos->reliability.kind;
		p_datareader_qos->reliability.max_blocking_time.sec = p_topic_qos->reliability.max_blocking_time.sec;
		p_datareader_qos->reliability.max_blocking_time.nanosec = p_topic_qos->reliability.max_blocking_time.nanosec;
		p_datareader_qos->resource_limits.max_instances = p_topic_qos->resource_limits.max_instances;
		p_datareader_qos->resource_limits.max_samples = p_topic_qos->resource_limits.max_samples;
		p_datareader_qos->resource_limits.max_samples_per_instance = p_topic_qos->resource_limits.max_samples_per_instance;
	}

	return RETCODE_OK;
}


static void InitSubscriberQos()
{
	mutex_init(&default_qos_lock);
	//defaultSubscriberQos 초기화. 

	defaultSubscriberQos.entity_factory.autoenable_created_entities = true;

	//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_group_data_qos(&defaultSubscriberQos.group_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	defaultSubscriberQos.group_data.value = NULL;
	defaultSubscriberQos.partition.name.i_string = 0;
	defaultSubscriberQos.partition.name.pp_string = NULL;
	defaultSubscriberQos.presentation.access_scope = 0;
	defaultSubscriberQos.presentation.coherent_access = false;
	defaultSubscriberQos.presentation.ordered_access = false;

	p_defaultSubscriberQos = &defaultSubscriberQos;
}


void init_subscriber(Subscriber* p_subscriber)
{
	int i;

	if (p_defaultSubscriberQos == NULL)
	{
		InitSubscriberQos();
	}

	init_entity((Entity*)p_subscriber);
	p_subscriber->enable = qos_enable_subscriber;

	p_subscriber->create_datareader = create_datareader;
	p_subscriber->delete_datareader = delete_datareader;
	p_subscriber->delete_contained_entities = delete_contained_entities;
	p_subscriber->lookup_datareader = lookup_datareader;
	p_subscriber->get_datareaders = get_datareaders;
	p_subscriber->notify_datareaders = notify_datareaders;
	p_subscriber->set_qos = set_qos;
	p_subscriber->get_qos = get_qos;
	p_subscriber->set_listener = set_listener;
	p_subscriber->get_listener = get_listener;
	p_subscriber->begin_access = begin_access;
	p_subscriber->end_access = end_access;
	p_subscriber->get_participant = get_participant;
	p_subscriber->set_default_datareader_qos = set_default_datareader_qos;
	p_subscriber->get_default_datareader_qos = get_default_datareader_qos;
	p_subscriber->copy_from_topic_qos = copy_from_topic_qos;
	/////////////////

	mutex_lock(&default_qos_lock);

	p_subscriber->subscriber_qos.entity_factory.autoenable_created_entities = defaultSubscriberQos.entity_factory.autoenable_created_entities = true;

	//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_group_data_qos(&p_subscriber->subscriber_qos.group_data);
	if (defaultSubscriberQos.group_data.value.i_string > 0)
	{
		qos_copy_group_data_qos(&p_subscriber->subscriber_qos.group_data, &defaultSubscriberQos.group_data);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	p_subscriber->subscriber_qos.group_data.value = NULL;
//	if(defaultSubscriberQos.group_data.value)
//	{
//		p_subscriber->subscriber_qos.group_data.value = strdup(defaultSubscriberQos.group_data.value);
//	}

	p_subscriber->subscriber_qos.partition.name.pp_string = NULL;
	p_subscriber->subscriber_qos.partition.name.i_string = 0;

	if(defaultSubscriberQos.partition.name.i_string > 0)
	{
		for(i=0; i < defaultSubscriberQos.partition.name.i_string; i++)
		{
			string str = strdup(defaultSubscriberQos.partition.name.pp_string[i]);
			INSERT_ELEM( p_subscriber->subscriber_qos.partition.name.pp_string, p_subscriber->subscriber_qos.partition.name.i_string,
						 p_subscriber->subscriber_qos.partition.name.i_string, str );
		}
	}

	p_subscriber->subscriber_qos.presentation.access_scope = defaultSubscriberQos.presentation.access_scope;
	p_subscriber->subscriber_qos.presentation.coherent_access = defaultSubscriberQos.presentation.coherent_access;
	p_subscriber->subscriber_qos.presentation.ordered_access = defaultSubscriberQos.presentation.ordered_access;

	mutex_unlock(&default_qos_lock);

	/////////////////////////
	p_subscriber->i_datareaders = 0;
	p_subscriber->pp_datareaders = NULL;
	p_subscriber->p_subscriber_listener = NULL;
	p_subscriber->p_domain_participant = NULL;
	p_subscriber->i_entity_type = SUBSCRIBER_ENTITY;


	// status by jun
	init_subscriber_status(p_subscriber);
//	p_subscriber->status_changed_flag.b_DATA_ON_READERS_StatusChangedFlag = false;
}


void destroy_subscriber(Subscriber* p_subscriber)
{

	delete_contained_entities(p_subscriber);

	REMOVE_STRING_SEQ(defaultSubscriberQos.partition.name);

	//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	REMOVE_STRING_SEQ(defaultSubscriberQos.group_data.value);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	FREE(defaultSubscriberQos.group_data.value);

	REMOVE_STRING_SEQ(p_subscriber->subscriber_qos.partition.name);

	//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	REMOVE_STRING_SEQ(p_subscriber->subscriber_qos.group_data.value);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//FREE(p_subscriber->subscriber_qos.group_data.value);
	destroy_entity((Entity*)p_subscriber);
	FREE(p_subscriber);
}


ReturnCode_t static_set_default_subscriber_qos(in_dds SubscriberQos *p_qos)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

	if(p_qos)
	{
		int i;
		mutex_lock(&default_qos_lock);

		defaultSubscriberQos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		REMOVE_STRING_SEQ(defaultSubscriberQos.group_data.value);
		if (p_qos->group_data.value.i_string > 0)
		{
			qos_copy_group_data_qos(&defaultSubscriberQos.group_data, &p_qos->group_data);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(defaultSubscriberQos.group_data.value);
//		if(p_qos->group_data.value)
//		{
//			defaultSubscriberQos.group_data.value = strdup(p_qos->group_data.value);
//		}

		REMOVE_STRING_SEQ(defaultSubscriberQos.partition.name);
		
		for(i=0 ;i < p_qos->partition.name.i_string;i++)
		{
			string str = strdup(p_qos->partition.name.pp_string[i]);
			INSERT_ELEM( defaultSubscriberQos.partition.name.pp_string, defaultSubscriberQos.partition.name.i_string,
						 defaultSubscriberQos.partition.name.i_string, str );
		}
		
		defaultSubscriberQos.presentation.access_scope = p_qos->presentation.access_scope;
		defaultSubscriberQos.presentation.coherent_access = p_qos->presentation.coherent_access;
		defaultSubscriberQos.presentation.ordered_access = p_qos->presentation.ordered_access;

		mutex_unlock(&default_qos_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default Subscriber QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_subscriber_qos(inout_dds SubscriberQos *p_qos)
{
	if (p_defaultSubscriberQos == NULL)
	{
		InitSubscriberQos();
	}


	if(p_qos)
	{
		int i;
		mutex_lock(&default_qos_lock);

		p_qos->entity_factory.autoenable_created_entities = defaultSubscriberQos.entity_factory.autoenable_created_entities;

		//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		//p_qos->group_data.value.i_string = 0;
		//p_qos->group_data.value.pp_string = NULL;
		qos_init_group_data_qos(&p_qos->group_data);
		if (defaultSubscriberQos.group_data.value.i_string > 0)
		{
			qos_copy_group_data_qos(&p_qos->group_data, &defaultSubscriberQos.group_data);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//FREE(p_qos->group_data.value);
//		p_qos->group_data.value = NULL;
//		if(defaultSubscriberQos.group_data.value)
//		{
//			p_qos->group_data.value = strdup(defaultSubscriberQos.group_data.value);
//		}

		//REMOVE_STRING_SEQ(p_qos->partition.name);

		p_qos->partition.name.i_string = 0;
		p_qos->partition.name.pp_string = NULL;
		
		for(i=0 ;i < defaultSubscriberQos.partition.name.i_string;i++)
		{
			string str = strdup(defaultSubscriberQos.partition.name.pp_string[i]);
			INSERT_ELEM( p_qos->partition.name.pp_string, p_qos->partition.name.i_string,
						 p_qos->partition.name.i_string, str );
		}
		
		p_qos->presentation.access_scope = defaultSubscriberQos.presentation.access_scope;
		p_qos->presentation.coherent_access = defaultSubscriberQos.presentation.coherent_access;
		p_qos->presentation.ordered_access = defaultSubscriberQos.presentation.ordered_access;

		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}

