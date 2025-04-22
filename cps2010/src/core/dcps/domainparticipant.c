/* 
	DomainParticipant 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static mutex_t	default_qos_lock;
static DomainParticipantQos* p_default_domain_participant_qos = NULL;
static DomainParticipantQos default_domain_participant_qos;


static Publisher *create_publisher(DomainParticipant *p_domainparticipant, in_dds PublisherQos *p_qos,in_dds PublisherListener *p_listener,in_dds StatusMask mask)
{
	module_t *p_module=NULL;
	Publisher *p_publisher = malloc(sizeof(Publisher));
	memset(p_publisher, '\0', sizeof(Publisher));

	init_publisher(p_publisher);

	p_module = current_object(get_domain_participant_factory_module_id());

	if(p_module == NULL){
		FREE(p_publisher);
		return NULL;
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	INSERT_ELEM( p_domainparticipant->pp_publisher, p_domainparticipant->i_publisher,
                 p_domainparticipant->i_publisher, p_publisher );
	mutex_unlock(&p_domainparticipant->entity_lock);


	if(p_qos)
	{
		p_publisher->set_qos(p_publisher, p_qos);
	}

	p_publisher->set_listener(p_publisher,p_listener,mask);

	p_publisher->p_domain_participant = p_domainparticipant;


	if(p_qos)
	{
		int i;
		mutex_lock(&p_domainparticipant->entity_lock);
		for(i = 0;  i < p_qos->partition.name.i_string; i++)
		{
			if(!strcmp(p_qos->partition.name.pp_string[i],BUILTIN_PUBLISHER_PARTITION_QOS))
			{
				p_domainparticipant->p_builtin_publisher = p_publisher;
				break;
			}
		}
		mutex_unlock(&p_domainparticipant->entity_lock);
	}

	qos_create_ext_entity_factory_qos((Entity*)p_publisher, (Entity*)p_domainparticipant, &p_domainparticipant->qos.entity_factory);//by kki (entity factory)

	trace_msg(OBJECT(p_module),TRACE_DEBUG,"Create publisher : domain_id : %ld",p_domainparticipant->domain_id);

	monitoring__add_publisher(p_publisher);

	return p_publisher;
}

static ReturnCode_t delete_publisher(DomainParticipant *p_domainparticipant, in_dds Publisher *p_publisher)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_domainparticipant || !p_publisher)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a publisher. p_domainparticipant or p_publisher is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_publisher; i++)
	{
		if(p_domainparticipant->pp_publisher[i] == p_publisher)
		{
			monitoring__delete_publisher(p_publisher);
			destroy_publisher(p_publisher);
			REMOVE_ELEM( p_domainparticipant->pp_publisher, p_domainparticipant->i_publisher, i);
			mutex_unlock(&p_domainparticipant->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete publisher.");
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);
	

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a publisher. It's not exist.");

	

	return RETCODE_ERROR;
}


static Subscriber* create_subscriber(DomainParticipant* p_domainparticipant, in_dds SubscriberQos* p_qos, in_dds SubscriberListener* p_listener, in_dds StatusMask mask)
{
	module_t* p_module = NULL;
	Subscriber* p_subscriber = malloc(sizeof(Subscriber));
	memset(p_subscriber, '\0', sizeof(Subscriber));

	init_subscriber(p_subscriber);

	p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module == NULL)
	{
		FREE(p_subscriber);
		return NULL;
	}

	if (p_qos)
	{
		p_subscriber->set_qos(p_subscriber, p_qos);
	}

	p_subscriber->set_listener(p_subscriber, p_listener, mask);
	p_subscriber->p_domain_participant = p_domainparticipant;

	mutex_lock(&p_domainparticipant->entity_lock);
	INSERT_ELEM(p_domainparticipant->pp_subscriber, p_domainparticipant->i_subscriber, p_domainparticipant->i_subscriber, p_subscriber);
	mutex_unlock(&p_domainparticipant->entity_lock);

	qos_create_ext_entity_factory_qos((Entity*)p_subscriber, (Entity*)p_domainparticipant, &p_domainparticipant->qos.entity_factory);//by kki (entity factory)

	trace_msg(OBJECT(p_module), TRACE_DEBUG, "Create Subscriber : domain_id : %ld", p_domainparticipant->domain_id);

	monitoring__add_subscriber(p_subscriber);
	return p_subscriber;
}

static ReturnCode_t delete_subscriber(DomainParticipant *p_domainparticipant, in_dds Subscriber *p_subscriber)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_domainparticipant || !p_subscriber)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a subscriber. p_domainparticipant or p_subscriber is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_subscriber; i++)
	{
		if(p_domainparticipant->pp_subscriber[i] == p_subscriber)
		{
			monitoring__delete_subscriber(p_subscriber);
			destroy_subscriber(p_subscriber);
			REMOVE_ELEM( p_domainparticipant->pp_subscriber, p_domainparticipant->i_subscriber, i);
			mutex_unlock(&p_domainparticipant->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete subscriber.");
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a subscriber. It's not exist.");

	return RETCODE_ERROR;
}

static Subscriber *get_builtin_subscriber(DomainParticipant *p_domainparticipant)
{
	Subscriber *p_subscriber = NULL;

	// mutex_lock() 이전에 수행되어야 한다.
	if (p_domainparticipant->domain_id != BUILTIN_PARTICIPANT_DOMAIN_ID)
	{
		p_domainparticipant = DomainParticipantFactory_get_instance()->lookup_participant(BUILTIN_PARTICIPANT_DOMAIN_ID);
	}

	mutex_lock(&p_domainparticipant->entity_lock);

	if( p_domainparticipant->i_subscriber > 0){
		p_subscriber = p_domainparticipant->pp_subscriber[0];
	//	break;

	}
	mutex_unlock(&p_domainparticipant->entity_lock);
	return p_subscriber;
}

static bool CheckBultinTopic(string topic_name, string type_name)
{
	if(!topic_name || !type_name)
	{
		return false;
	}

	if(!strcmp(topic_name,PARTICIPANTINFO_NAME) && !strcmp(type_name,PARTICIPANTINFO_TYPE))
	{
		return true;
	}else if(!strcmp(topic_name,TOPICINFO_NAME) && !strcmp(type_name,TOPICINFO_TYPE))
	{
		return true;
	}else if(!strcmp(topic_name,PUBLICATIONINFO_NAME) && !strcmp(type_name,PUBLICATIONINFO_TYPE))
	{
		return true;
	}else if(!strcmp(topic_name,SUBSCRIPTIONINFO_NAME) && !strcmp(type_name,SUBSCRIPTIONINFO_TYPE))
	{
		return true;
	}
	
	return false;
}

static Topic *create_topic(DomainParticipant *p_domainparticipant, in_dds string topic_name,in_dds string type_name,in_dds TopicQos *p_qos,in_dds TopicListener *p_listener,in_dds StatusMask mask)
{
	module_t *p_module=NULL;
	int i;
	bool b_finded = false;
	Topic *p_topic = NULL;
	
	if (qos_check_inconsistent_policy_before_change_topic_qos(p_qos) == RETCODE_INCONSISTENT_POLICY) return NULL; //by kki

	p_module = current_object(get_domain_participant_factory_module_id());

	if(p_module == NULL){
		return NULL;
	}

	if(topic_name == NULL){
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create Topic. topic_name is NULL");
		return NULL;
	}

	if(type_name == NULL){
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create Topic. type_name is NULL");
		return NULL;
	}

	

	mutex_lock(&p_domainparticipant->entity_lock);

	for(i=0; i < p_domainparticipant->i_topic; i++)
	{
		if(!strcmp(p_domainparticipant->pp_topic[i]->topic_name, topic_name))
		{
			b_finded = true;
			p_topic = p_domainparticipant->pp_topic[i];
			break;
		}
	}

	if(b_finded == false)
	{
		p_topic = malloc(sizeof(Topic));
		memset(p_topic, '\0', sizeof(Topic));

		init_topic(p_topic);

		p_topic->topic_name = strdup(topic_name);
		p_topic->type_name = strdup(type_name);
		p_topic->l_status_mask = mask;

		p_topic->p_domain_participant = p_domainparticipant;

		if(p_qos)
		{
			p_topic->set_qos(p_topic, p_qos);
		}

		p_topic->set_listener(p_topic, p_listener, mask);


		INSERT_ELEM( p_domainparticipant->pp_topic, p_domainparticipant->i_topic,
					 p_domainparticipant->i_topic, p_topic );
	}else
	{
		trace_msg(OBJECT(p_module),TRACE_DEBUG,"Already Exist Topic : domain_id : %ld",p_domainparticipant->domain_id);
	}

	mutex_unlock(&p_domainparticipant->entity_lock);
	
	if(b_finded == false)
	{

		if(CheckBultinTopic(topic_name, type_name))
		{
			mutex_lock(&p_domainparticipant->entity_lock);
			INSERT_ELEM( p_domainparticipant->pp_builtin_topics, p_domainparticipant->i_builtin_topics,
					 p_domainparticipant->i_builtin_topics, p_topic );
			mutex_unlock(&p_domainparticipant->entity_lock);
		}

		trace_msg(OBJECT(p_module),TRACE_DEBUG,"Create Topic : domain_id : %ld",p_domainparticipant->domain_id);


		p_topic->topic_type = DEFAULT_TOPIC_TYPE;

		monitoring__add_topic(p_topic);
		add_entity_to_service((Entity *)p_topic);
	}

	return p_topic;
}

static ReturnCode_t delete_topic(DomainParticipant *p_domainparticipant, in_dds Topic *p_topic)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_domainparticipant || !p_topic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a topic. p_domainparticipant or p_topic is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_topic; i++)
	{
		if(p_domainparticipant->pp_topic[i] == p_topic)
		{
			remove_entity_to_service((Entity *)p_topic);
			monitoring__delete_topic(p_topic);
			destroy_topic(p_topic);
			REMOVE_ELEM( p_domainparticipant->pp_topic, p_domainparticipant->i_topic, i);
			mutex_unlock(&p_domainparticipant->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete topic.");
			
			
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);
	

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a topic. It's not exist.");

	return RETCODE_ERROR;
}


static Topic *find_topic(DomainParticipant *p_domainparticipant, in_dds string topic_name,in_dds Duration_t timeout)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	for(i=0; i < p_domainparticipant->i_topic; i++)
	{
		if( !strcmp( p_domainparticipant->pp_topic[i]->topic_name, topic_name ) )
        {
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Finded the topic [%s].",topic_name);
            return p_domainparticipant->pp_topic[i];
        }
	}
	
	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't fined the topic [%s].",topic_name);

	return NULL;
}

static TopicDescription *lookup_topicdescription(DomainParticipant *p_domainparticipant, in_dds string name)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	for(i=0; i < p_domainparticipant->i_topic; i++)
	{
		if( !strcmp( p_domainparticipant->pp_topic[i]->topic_name, name ) )
        {
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Finded the topic description[%s].",name);
            return WHERE_TOPICDESCRIPTION(p_domainparticipant->pp_topic[i]);
        }
	}
	
	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't fined the topic description[%s].",name);

	return NULL;
}


static ContentFilteredTopic *create_contentfilteredtopic(DomainParticipant *p_domainparticipant, in_dds string name,in_dds Topic *p_related_topic,in_dds string filter_expression,in_dds StringSeq expression_parameters)
{
	int i;
	module_t *p_module=NULL;
	expression_t *p_expr = NULL;
	ContentFilteredTopic *p_contentFilteredTopic = malloc(sizeof(ContentFilteredTopic));
	memset(p_contentFilteredTopic, '\0', sizeof(ContentFilteredTopic));

	init_content_filtered_topic(p_contentFilteredTopic);

	p_module = current_object(get_domain_participant_factory_module_id());

	if(p_module == NULL){
		destroy_content_filtered_topic(p_contentFilteredTopic);
		return NULL;
	}

	if(name == NULL){
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create ContentFilteredTopic. name is NULL");
		destroy_content_filtered_topic(p_contentFilteredTopic);
		return NULL;
	}

	if(p_related_topic == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create the ContentFilteredTopic. p_related_topic is NULL");
		destroy_content_filtered_topic(p_contentFilteredTopic);
		return NULL;
	}

	if(filter_expression == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create the ContentFilteredTopic. filter_expression is NULL");
		destroy_content_filtered_topic(p_contentFilteredTopic);
		return NULL;
	}


	p_expr = expression_parse(filter_expression);
	if(p_expr == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create the ContentFilteredTopic. filter_expression is wrong.");
		destroy_content_filtered_topic(p_contentFilteredTopic);
		return NULL;
	}

	p_contentFilteredTopic->p_expr = p_expr;

	///
	{
		FooTypeSupport *p_typeSupport = NULL;
		p_typeSupport = domain_participant_find_support_type(OBJECT(p_module),p_domainparticipant, p_related_topic->get_type_name(p_related_topic));

		expression_check_field(p_expr, p_typeSupport);
	}


	p_contentFilteredTopic->topic_name = strdup(name);
	p_contentFilteredTopic->p_related_topic = p_related_topic;
	p_contentFilteredTopic->get_type_name = p_related_topic->get_type_name;
	p_contentFilteredTopic->type_name = strdup(p_related_topic->type_name);

	if(filter_expression)p_contentFilteredTopic->filter_expression = strdup(filter_expression);

	p_contentFilteredTopic->p_domain_participant = p_domainparticipant;
	
	for(i = 0; i < expression_parameters.i_string; i++)
	{
		string str = strdup(expression_parameters.pp_string[i]);
		INSERT_ELEM(p_contentFilteredTopic->expression_parameters.pp_string, p_contentFilteredTopic->expression_parameters.i_string, 
			p_contentFilteredTopic->expression_parameters.i_string, str);
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	INSERT_ELEM( p_domainparticipant->pp_content_filtered_topics, p_domainparticipant->i_content_filtered_topics,
                 p_domainparticipant->i_content_filtered_topics, p_contentFilteredTopic);
	mutex_unlock(&p_domainparticipant->entity_lock);

	trace_msg(OBJECT(p_module),TRACE_DEBUG,"Create ContentFilteredTopic : domain_id : %ld",p_domainparticipant->domain_id);
	
	p_contentFilteredTopic->topic_type = CONTENTFILTER_TOPIC_TYPE;

	monitoring__add_topic((Topic*)p_contentFilteredTopic);

	return p_contentFilteredTopic;
}

static ReturnCode_t delete_contentfilteredtopic(DomainParticipant *p_domainparticipant, in_dds ContentFilteredTopic *p_contentfilteredtopic)
{

	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_domainparticipant || !p_contentfilteredtopic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a contentfilteredtopic. p_domainparticipant or p_contentfilteredtopic is NULL.");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_content_filtered_topics; i++)
	{
		if(p_domainparticipant->pp_content_filtered_topics[i] == p_contentfilteredtopic)
		{
			monitoring__delete_topic((Topic*)p_contentfilteredtopic);
			destroy_content_filtered_topic(p_contentfilteredtopic);
			REMOVE_ELEM( p_domainparticipant->pp_content_filtered_topics, p_domainparticipant->i_content_filtered_topics, i);
			mutex_unlock(&p_domainparticipant->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete contentFilteredTopic.");
			
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a contentFilteredTopic. It's not exist.");

	return RETCODE_ERROR;
}


static MultiTopic *create_multitopic(DomainParticipant *p_domainparticipant, in_dds string name,in_dds string type_name,in_dds string subscription_expression,in_dds StringSeq expression_parameters)
{
	int i;
	module_t *p_module=NULL;
	MultiTopic *p_multiTopic = malloc(sizeof(MultiTopic));
	memset(p_multiTopic, '\0', sizeof(MultiTopic));

	init_multi_topic(p_multiTopic);

	p_module = current_object(get_domain_participant_factory_module_id());

	if(p_module == NULL){
		FREE(p_multiTopic);
		return NULL;
	}

	if(name == NULL){
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create MultiTopic. name is NULL");
		FREE(p_multiTopic);
		return NULL;
	}

	if(type_name == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't create the MultiTopic. type_name is NULL");
		FREE(p_multiTopic);
		return NULL;
	}

	p_multiTopic->topic_name = strdup(name);
	p_multiTopic->type_name = strdup(type_name);


	if(subscription_expression)p_multiTopic->subscription_expression = strdup(subscription_expression);

	p_multiTopic->p_domain_participant = p_domainparticipant;

	for(i = 0; i < expression_parameters.i_string; i++)
	{
		string str = strdup(expression_parameters.pp_string[i]);
		INSERT_ELEM(p_multiTopic->expression_parameters.pp_string, p_multiTopic->expression_parameters.i_string, 
			p_multiTopic->expression_parameters.i_string, str);
	}

	mutex_lock(&p_domainparticipant->entity_lock);
	INSERT_ELEM( p_domainparticipant->pp_multi_topics, p_domainparticipant->i_multi_topics,
                 p_domainparticipant->i_multi_topics, p_multiTopic);
	mutex_unlock(&p_domainparticipant->entity_lock);

	trace_msg(OBJECT(p_module),TRACE_DEBUG,"Create MultiTopic : domain_id : %ld",p_domainparticipant->domain_id);

	p_multiTopic->topic_type = MULTITOPIC_TYPE;

	monitoring__add_topic((Topic*)p_multiTopic);
	return p_multiTopic;

}

static ReturnCode_t delete_multitopic(DomainParticipant *p_domainparticipant, in_dds MultiTopic *p_multitopic)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	int i;

	if(!p_domainparticipant || !p_multitopic)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't delete a multitopic. p_multitopic or p_contentfilteredtopic is NULL.");
		return RETCODE_ERROR;
	}


	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_multi_topics; i++)
	{
		if(p_domainparticipant->pp_multi_topics[i] == p_multitopic)
		{
			monitoring__delete_topic((Topic*)p_multitopic);

			destroy_multi_topic(p_multitopic);
			REMOVE_ELEM( p_domainparticipant->pp_multi_topics, p_domainparticipant->i_multi_topics, i);
			mutex_unlock(&p_domainparticipant->entity_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete multitopic.");
			
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);
	

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a multitopic. It's not exist.");

	return RETCODE_ERROR;
}

static ReturnCode_t delete_contained_entities(DomainParticipant *p_domainparticipant)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	destroy_domain_participant(p_domainparticipant,true);
	trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete All Entities.");
	return RETCODE_OK;
}


static ReturnCode_t set_qos(DomainParticipant *p_domainparticipant, in_dds DomainParticipantQos *p_qos)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	if(p_qos)
	{
		p_domainparticipant->qos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_set_qos_about_user_data_qos_from_domainparticipant(p_domainparticipant,p_qos);
//		qos_copy_user_data_qos(&p_domainparticipant->qos.user_data, &p_qos->user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_domainparticipant->qos.user_data.value);
//		if(p_qos->user_data.value)
//		{
//			p_domainparticipant->qos.user_data.value = strdup(p_qos->user_data.value);
//		}


		//domainParticipantFactoryQos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;
		trace_msg(OBJECT(p_module),TRACE_TRACE,"Participant QoS is changed.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_qos(DomainParticipant *p_domainparticipant, inout_dds DomainParticipantQos *p_qos)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	if(p_qos)
	{
		p_qos->entity_factory.autoenable_created_entities = p_domainparticipant->qos.entity_factory.autoenable_created_entities;

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_copy_user_data_qos(&p_qos->user_data, &p_domainparticipant->qos.user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(p_qos->user_data.value);
//		if(p_domainparticipant->qos.user_data.value)
//		{
//			p_qos->user_data.value = strdup(p_domainparticipant->qos.user_data.value);
//		}

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Participant QoS is got.");

	}else
	{
		trace_msg(OBJECT(p_module),TRACE_WARM,"Participant QoS is not changed. because p_qos is NULL");
		return RETCODE_ERROR;
	}

	return RETCODE_OK;
}


ReturnCode_t set_listener(DomainParticipant *p_domainparticipant, in_dds DomainParticipantListener *p_listener,in_dds StatusMask mask)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_domainparticipant || !p_listener)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't set a listener. p_domainparticipant or p_listener is NULL.");
	}

	p_domainparticipant->p_domain_participant_listener = p_listener;
	p_domainparticipant->l_status_mask = mask;

	trace_msg(OBJECT(p_module),TRACE_TRACE,"DomainParticipant set a listener");

	return RETCODE_OK;
}

static DomainParticipantListener *get_listener(DomainParticipant *p_domainparticipant)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_domainparticipant)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't get a listener. p_domainparticipant is NULL.");
	}

	return p_domainparticipant->p_domain_participant_listener;
}


void ignoreRemoteParticipant(BuiltinTopicKey_t key);
void ignoreTopic(DomainParticipant *p_domainparticipant, BuiltinTopicKey_t key, char *topic_name, char *type_name);

static ReturnCode_t ignore_participant(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle)
{
	/* 
	This operation allows an application to instruct the Service to locally ignore a remote domain_dds participant. From that point
	onwards the Service will locally behave as if the remote participant did not exist. This means it will ignore any Topic,
	publication, or subscription that originates on that domain_dds participant.
	This operation can be used, in_dds conjunction with the discovery of remote participants offered by means of the
	“DCPSParticipant” built-in_dds Topic, to provide, for example, access control. Application data can be associated with a
	DomainParticipant by means of the USER_DATA QoS policy. This application data is propagated as a field in_dds the builtin
	topic and can be used by an application to implement its own access control policy. See Section 7.1.5, “Built-in
	Topics,” on page 134 for more details on the built-in_dds topics.
	The domain_dds participant to ignore is identified by the handle argument. This handle is the one that appears in_dds the
	SampleInfo retrieved when reading the data-samples available for the built-in_dds DataReader to the “DCPSParticipant”
	topic. The built-in_dds DataReader is read with the same read/take operations used for any DataReader. These data-accessing
	operations are described in_dds Section 7.1.2.5, “Subscription Module,” on page 61.
	The ignore_participant operation is not required to be reversible. The Service offers no means to reverse it.
	Possible error codes returned in_dds addition to the standard ones: OUT_OF_RESOURCES. 
	*/

	
	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_participant_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];
					ParticipantBuiltinTopicData participant_data;

					memset(&participant_data, 0, sizeof(ParticipantBuiltinTopicData));

					memcpy(&participant_data, p_message->v_data, sizeof(ParticipantBuiltinTopicData));
					get_builtin_participant_data(&participant_data, p_message->v_related_cachechange);
					ignoreRemoteParticipant(participant_data.key);
					
				}
			}

			while(fseq.i_seq){
				message_t *p_message = (message_t *)fseq.pp_foo[0];
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
				
			}
		}
	}


	return RETCODE_OUT_OF_RESOURCES;
}


static ReturnCode_t ignore_topic(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle)
{

	/*
	This operation allows an application to instruct the Service to locally ignore a Topic. This means it will locally ignore any
	publication or subscription to the Topic.
	This operation can be used to save local resources when the application knows that it will never publish or subscribe to
	data under certain_dds topics.
	The Topic to ignore is identified by the handle argument. This handle is the one that appears in_dds the SampleInfo retrieved
	when reading the data-samples from the built-in_dds DataReader to the “DCPSTopic” topic.
	The ignore_topic operation is not required to be reversible. The Service offers no means to reverse it.
	Possible error codes returned in_dds addition to the standard ones: OUT_OF_RESOURCES.
	*/


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_topic_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];

					TopicBuiltinTopicData topic_data;

					memset(&topic_data,0,sizeof(TopicBuiltinTopicData));

					memcpy(&topic_data, p_message->v_data, sizeof(TopicBuiltinTopicData));

					get_builtin_topic_data(&topic_data, p_message->v_related_cachechange);

					ignoreTopic(p_domainparticipant, topic_data.key, topic_data.name, topic_data.type_name);


					FREE(topic_data.name);
					FREE(topic_data.type_name);
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

static ReturnCode_t ignore_publication(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle)
{
	/*
	This operation allows an application to instruct the Service to locally ignore a remote publication; a publication is defined
	by the association of a topic name, and user data and partition set on the Publisher (see the “DCPSPublication” built-in
	Topic in_dds Section 7.1.5, “Built-in_dds Topics,” on page 134). After this call, any data written related to that publication will be
	ignored.
	The DataWriter to ignore is identified by the handle argument. This handle is the one that appears in_dds the SampleInfo
	retrieved when reading the data-samples from the built-in_dds DataReader to the “DCPSPublication” topic.
	The ignore_publication operation is not required to be reversible. The Service offers no means to reverse it.
	Possible error codes returned in_dds addition to the standard ones: OUT_OF_RESOURCES.
	*/

	

	
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
			if(sSeq.pp_sample_infos[i]->instance_handle == handle)
			{
				message_t *p_message = (message_t *)fseq.pp_foo[i];

				PublicationBuiltinTopicData publication_data;

				
				memset(&publication_data, 0, sizeof(PublicationBuiltinTopicData));
				memcpy(&publication_data, p_message->v_data, sizeof(PublicationBuiltinTopicData));

				get_publication_builtin_topic_data(&publication_data, p_message->v_related_cachechange);

				if(publication_data.topic_name)
				{
					
					//ignore writeproxy;
					//publication_data.key
					mutex_lock(&p_domainparticipant->entity_lock);
					for(i=0; i < p_domainparticipant->i_subscriber; i++)
					{
						Subscriber *p_subscriber = p_domainparticipant->pp_subscriber[i];
						int j;

						
						for(j=0; j < p_subscriber->i_datareaders; j++)
						{
							DataReader* p_datareader = p_subscriber->pp_datareaders[j];
							ignore_writerproxy(p_datareader->p_related_rtps_reader, &publication_data.key);
						}
						
					}
					mutex_unlock(&p_domainparticipant->entity_lock);

				}


				FREE(publication_data.topic_name);
				FREE(publication_data.type_name);
			}
		}

		while(fseq.i_seq){
			message_t *p_message = (message_t *)fseq.pp_foo[0];
			message_release(p_message);
			REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			
		}
	}
	



	return RETCODE_OK;
}

static ReturnCode_t ignore_subscription(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t handle)
{
	/*
	This operation allows an application to instruct the Service to locally ignore a remote subscription; a subscription is
	defined by the association of a topic name, and user data and partition set on the Subscriber (see the “DCPSSubscription”
	built-in_dds Topic in_dds Section 7.1.5, “Built-in_dds Topics,” on page 134). After this call, any data received related to that
	subscription will be ignored.
	The DataReader to ignore is identified by the handle argument. This handle is the one that appears in_dds the SampleInfo
	retrieved when reading the data-samples from the built-in_dds DataReader to the “DCPSSubscription” topic.
	The ignore_subscription operation is not required to be reversible. The Service offers no means to reverse it.
	Possible error codes returned in_dds addition to the standard ones: OUT_OF_RESOURCES.
	*/

	int i_size;
	int i;
	FooDataReader *p_databuiltinreader = (FooDataReader*)get_builtin_subscriber_reader();
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


	if(p_databuiltinreader != NULL)
	{
		p_databuiltinreader->read(p_databuiltinreader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

		i_size = sSeq.i_seq;

		for(i=0; i < sSeq.i_seq; i++)
		{
			if(sSeq.pp_sample_infos[i]->instance_handle == handle)
			{
				message_t *p_message = (message_t *)fseq.pp_foo[i];

				SubscriptionBuiltinTopicData subscription_data;

				
				memset(&subscription_data, 0, sizeof(SubscriptionBuiltinTopicData));
				memcpy(&subscription_data, p_message->v_data, sizeof(SubscriptionBuiltinTopicData));

				get_subscription_builtin_topic_data(&subscription_data, p_message->v_related_cachechange);

				if(subscription_data.topic_name)
				{
					
					//ignore readerproxy;
					//subscription_data.key
					mutex_lock(&p_domainparticipant->entity_lock);
					for(i=0; i < p_domainparticipant->i_publisher; i++)
					{
						Publisher *p_publisher = p_domainparticipant->pp_publisher[i];
						int j;
					

						for(j=0; j < p_publisher->i_datawriters; j++)
						{
							DataWriter* p_dataWriter = p_publisher->pp_datawriters[j];
							ignore_readerproxy(p_dataWriter->p_related_rtps_writer, &subscription_data.key);
						}
						
					}
					mutex_unlock(&p_domainparticipant->entity_lock);

				}


				FREE(subscription_data.topic_name);
				FREE(subscription_data.type_name);
			}
		}

		while(fseq.i_seq){
			message_t *p_message = (message_t *)fseq.pp_foo[0];
			message_release(p_message);
			REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			
		}
	}
	



	return RETCODE_OK;
}

static DomainId_t get_domain_id(DomainParticipant *p_domainparticipant)
{
	return p_domainparticipant->domain_id;
}

static ReturnCode_t assert_liveliness(DomainParticipant *p_domainparticipant)
{
	/*
	This operation manually asserts the liveliness of the DomainParticipant. This is used in_dds combination with the
	LIVELINESS QoS policy (cf. Section 7.1.3, “Supported QoS,” on page 96) to indicate to the Service that the entity
	remains active.
	This operation needs to only be used if the DomainParticipant contains DataWriter entities with the LIVELINESS set to
	MANUAL_BY_PARTICIPANT and it only affects the liveliness of those DataWriter entities. Otherwise, it has no effect.
	Note - Writing data via the write operation on a DataWriter asserts liveliness on the DataWriter itself and its DomainParticipant.
	Consequently the use of assert_liveliness is only needed if the application is not writing data regularly.
	Complete details are provided in_dds Section 7.1.3.11, “LIVELINESS,” on page 113.
	*/

	int i_size_writer;
	int i, j, i_size;
	Publisher *p_publisher;
	DataWriterQos writer_qos;

	
	mutex_lock(&p_domainparticipant->entity_lock);

	i_size = p_domainparticipant->i_publisher;

	for (i = i_size-1; i >= 0; i--)
	{
		p_publisher = p_domainparticipant->pp_publisher[i];
		i_size_writer = p_publisher->i_datawriters;
		for (j = 0; j < i_size_writer; j++)
		{
			memset(&writer_qos, 0, sizeof(DataWriterQos));
			p_publisher->pp_datawriters[j]->get_qos(p_publisher->pp_datawriters[j], &writer_qos);

			if (writer_qos.liveliness.kind == MANUAL_BY_PARTICIPANT_LIVELINESS_QOS)
			{
				qos_operation_called_for_liveliness_qos(p_publisher->pp_datawriters[j]);
			}
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	return RETCODE_OK;
}


static ReturnCode_t set_default_publisher_qos(in_dds PublisherQos *p_qos)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_qos)
	{
		static_set_default_publisher_qos(p_qos);
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't set a default publisher qos. p_qos is NULL.");
	}

	return RETCODE_OK;
}


static ReturnCode_t get_default_publisher_qos(inout_dds PublisherQos *p_qos)
{
	static_get_default_publisher_qos(p_qos);
	return RETCODE_OK;
}


static ReturnCode_t set_default_subscriber_qos(in_dds SubscriberQos *p_qos)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_qos)
	{
		static_set_default_subscriber_qos(p_qos);
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't set a default subscriber qos. p_qos is NULL.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_default_subscriber_qos(inout_dds SubscriberQos *p_qos)
{
	static_get_default_subscriber_qos(p_qos);
	return RETCODE_OK;
}

static ReturnCode_t set_default_topic_qos(in_dds TopicQos *p_qos)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(!p_qos)
	{
		static_set_default_topic_qos(p_qos);
		trace_msg(OBJECT(p_module),TRACE_ERROR,"Can't set a default topic qos. p_qos is NULL.");
	}

	return RETCODE_OK;
}

static ReturnCode_t get_default_topic_qos(inout_dds TopicQos *p_qos)
{
	static_get_default_topic_qos(p_qos);
	return RETCODE_OK;
}


static ReturnCode_t get_discovered_participants(DomainParticipant *p_domainparticipant, inout_dds InstanceHandleSeq *participant_handles)
{
	/*
	This operation retrieves the list of DomainParticipants that have been discovered in_dds the domain_dds and that the application
	has not indicated should be “ignored” by means of the DomainParticipant ignore_participant operation.
	The operation may fail if the infrastructure does not locally maintain_dds the connectivity information. in_dds this case the
	operation will return UNSUPPORTED.
	*/

	if(participant_handles == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_participant_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				InstanceHandle_t *p_handle = malloc(sizeof(InstanceHandle_t));
				memset(p_handle, '\0', sizeof(InstanceHandle_t));

				*p_handle = sSeq.pp_sample_infos[i]->instance_handle;
				INSERT_ELEM(participant_handles->pp_instancehandle, participant_handles->i_seq , participant_handles->i_seq , p_handle);
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

static ReturnCode_t get_discovered_participant_data(DomainParticipant *p_domainparticipant, inout_dds ParticipantBuiltinTopicData *p_participant_data,in_dds InstanceHandle_t participant_handle)
{
	/*
	This operation retrieves information on a DomainParticipant that has been discovered on the network. The participant
	must be in_dds the same domain_dds as the participant on which this operation is invoked and must not have been “ignored” by
	means of the DomainParticipant ignore_participant operation.
	The participant_handle must correspond to such a DomainParticipant. Otherwise, the operation will fail and return
	PRECONDITION_NOT_MET.
	Use the operation get_discovered_participants to find the DomainParticipants that are currently discovered.
	The operation may also fail if the infrastructure does not hold the information necessary to fill in_dds the participant_data. In
	this case the operation will return UNSUPPORTED.
	*/

	if(p_participant_data == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_participant_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == participant_handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];

					memcpy(p_participant_data, p_message->v_data, sizeof(ParticipantBuiltinTopicData));
					get_builtin_participant_data(p_participant_data, p_message->v_related_cachechange);
					
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

static ReturnCode_t get_discovered_topics(DomainParticipant *p_domainparticipant, inout_dds InstanceHandleSeq *topic_handles)
{
	/*
	This operation retrieves the list of Topics that have been discovered in_dds the domain_dds and that the application has not
	indicated should be “ignored” by means of the DomainParticipant ignore_topic operation.
	*/



	if(topic_handles == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_topic_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				InstanceHandle_t *p_handle = malloc(sizeof(InstanceHandle_t));
				memset(p_handle, '\0', sizeof(InstanceHandle_t));
				*p_handle = sSeq.pp_sample_infos[i]->instance_handle;

				INSERT_ELEM(topic_handles->pp_instancehandle, topic_handles->i_seq , topic_handles->i_seq , p_handle);
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

static ReturnCode_t get_discovered_topic_data(DomainParticipant *p_domainparticipant, inout_dds TopicBuiltinTopicData *p_topic_data,in_dds InstanceHandle_t topic_handle)
{

	/*
	This operation retrieves information on a Topic that has been discovered on the network. The topic must have been
	created by a participant in_dds the same domain_dds as the participant on which this operation is invoked and must not have been
	“ignored” by means of the DomainParticipant ignore_topic operation.
	The topic_handle must correspond to such a topic. Otherwise, the operation will fail and return
	PRECONDITION_NOT_MET.
	Use the operation get_discovered_topics to find the topics that are currently discovered.
	The operation may also fail if the infrastructure does not hold the information necessary to fill in_dds the topic_data. in_dds this
	case the operation will return UNSUPPORTED.
	The operation may fail if the infrastructure does not locally maintain_dds the connectivity information. in_dds this case the
	operation will return UNSUPPORTED.
	*/

	if(p_topic_data == NULL) return RETCODE_ERROR;


	{
		int i_size;
		int i;
		FooDataReader *p_datareader = (FooDataReader*)get_builtin_topic_reader();
		FooSeq fseq = INIT_FOOSEQ;
		SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;


		if(p_datareader != NULL)
		{
			p_datareader->read(p_datareader, &fseq, &sSeq,  LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

			i_size = sSeq.i_seq;

			for(i=0; i < sSeq.i_seq; i++)
			{
				if(sSeq.pp_sample_infos[i]->instance_handle == topic_handle)
				{
					message_t *p_message = (message_t *)fseq.pp_foo[i];

					memcpy(p_topic_data, p_message->v_data, sizeof(TopicBuiltinTopicData));

					get_builtin_topic_data(p_topic_data, p_message->v_related_cachechange);
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

static bool contains_entity(DomainParticipant *p_domainparticipant, in_dds InstanceHandle_t a_handle)
{
	int i, j, i_size;
	bool isfound = false;
	Publisher *p_publisher;
	Subscriber *p_subscriber;


	mutex_lock(&p_domainparticipant->entity_lock);

	i_size = p_domainparticipant->i_publisher;

	for(i = i_size-1; i >= 0; i--){
		p_publisher = p_domainparticipant->pp_publisher[i];

		if(p_publisher->get_instance_handle((Entity*) p_publisher)
			== a_handle)
		{
			isfound = true;
			break;
		}else{
			int i_size_writer = p_publisher->i_datawriters;
			for(j = 0; j < i_size_writer; j++){
				if(p_publisher->pp_datawriters[j]->get_instance_handle((Entity*)p_publisher->pp_datawriters[j])
					== a_handle)
				{
					isfound = true;
					break;
				}

			}
		}

		if(isfound)
		{
			break;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);


	if(isfound)
	{
		return true;
	}

	mutex_lock(&p_domainparticipant->entity_lock);

	i_size = p_domainparticipant->i_subscriber;

	for(i = i_size-1; i >= 0; i--){
		p_subscriber = p_domainparticipant->pp_subscriber[i];

		if(p_subscriber->get_instance_handle((Entity*) p_subscriber)
			== a_handle)
		{
			isfound = true;
			break;
		}else{
			int i_size_reader = p_subscriber->i_datareaders;

			for(j = 0; j < i_size_reader; j++){
				if(p_subscriber->pp_datareaders[j]->get_instance_handle((Entity*)p_subscriber->pp_datareaders[j])
					== a_handle)
				{
					isfound = true;
					break;
				}

			}
		}

		if(isfound)
		{
			break;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	if(isfound)
	{
		return true;
	}


	

	mutex_lock(&p_domainparticipant->entity_lock);
	i_size = p_domainparticipant->i_topic;
	for(i = i_size-1; i >= 0; i--)
	{
		if(p_domainparticipant->pp_topic[i]->get_instance_handle((Entity*) p_domainparticipant->pp_topic[i])
			== a_handle)
		{
			isfound = true;
			break;
		}
	}

	mutex_unlock(&p_domainparticipant->entity_lock);

	if(isfound)
	{
		return true;
	}



	mutex_lock(&p_domainparticipant->entity_lock);
	i_size = p_domainparticipant->i_content_filtered_topics;
	for(i = i_size-1; i >= 0; i--)
	{
		if(p_domainparticipant->pp_content_filtered_topics[i]->get_instance_handle((Entity*) p_domainparticipant->pp_content_filtered_topics[i])
			== a_handle)
		{
			isfound = true;
			break;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	if(isfound)
	{
		return true;
	}



	mutex_lock(&p_domainparticipant->entity_lock);
	i_size = p_domainparticipant->i_multi_topics;
	for(i = i_size-1; i >= 0; i--)
	{
		if(p_domainparticipant->pp_multi_topics[i]->get_instance_handle((Entity*) p_domainparticipant->pp_multi_topics[i])
			== a_handle)
		{
			isfound = true;
			break;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	if(isfound)
	{
		return true;
	}


	mutex_lock(&p_domainparticipant->entity_lock);
	i_size = p_domainparticipant->i_builtin_topics;
	for(i = i_size-1; i >= 0; i--)
	{
		if(p_domainparticipant->pp_builtin_topics[i]->get_instance_handle((Entity*) p_domainparticipant->pp_builtin_topics[i])
			== a_handle)
		{
			isfound = true;
			break;
		}
	}
	mutex_unlock(&p_domainparticipant->entity_lock);

	if(isfound)
	{
		return true;
	}


	return false;
}
	
static ReturnCode_t get_current_time(DomainParticipant *p_domainparticipant, inout_dds Time_t *p_current_time)
{

	if(p_current_time)
	{
		(*p_current_time).sec = current_duration().sec;
		(*p_current_time).nanosec = current_duration().nanosec;

		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}


extern ReturnCode_t enable_domainparticipant(Entity *p_entity);
void init_domain_paticipant(DomainParticipant* p_domain_participant)
{
	if (p_default_domain_participant_qos == NULL)
	{
		mutex_init(&default_qos_lock);
		//default_domain_participant_qos 초기화. 
//		default_domain_participant_qos.user_data.value = NULL;
		default_domain_participant_qos.entity_factory.autoenable_created_entities = true;
		p_default_domain_participant_qos = &default_domain_participant_qos;
	}

	init_entity((Entity *)p_domain_participant);
	p_domain_participant->enable = qos_enable_domainparticipant;

	p_domain_participant->create_publisher = create_publisher;
	p_domain_participant->delete_publisher = delete_publisher;
	p_domain_participant->create_subscriber = create_subscriber;
	p_domain_participant->delete_subscriber = delete_subscriber;
	p_domain_participant->get_builtin_subscriber = get_builtin_subscriber;
	p_domain_participant->create_topic = create_topic;
	p_domain_participant->delete_topic = delete_topic;

	p_domain_participant->find_topic = find_topic;
	p_domain_participant->lookup_topicdescription = lookup_topicdescription;
	p_domain_participant->create_contentfilteredtopic = create_contentfilteredtopic;
	p_domain_participant->delete_contentfilteredtopic = delete_contentfilteredtopic;
	p_domain_participant->create_multitopic = create_multitopic;
	p_domain_participant->delete_multitopic = delete_multitopic;
	p_domain_participant->delete_contained_entities = delete_contained_entities;
	p_domain_participant->set_qos = set_qos;
	p_domain_participant->get_qos = get_qos;
	p_domain_participant->set_listener = set_listener;
	p_domain_participant->get_listener = get_listener;
	p_domain_participant->ignore_participant = ignore_participant;
	p_domain_participant->ignore_topic = ignore_topic;
	p_domain_participant->ignore_publication = ignore_publication;
	p_domain_participant->ignore_subscription = ignore_subscription;
	p_domain_participant->get_domain_id = get_domain_id;
	p_domain_participant->assert_liveliness = assert_liveliness;
	p_domain_participant->set_default_publisher_qos = set_default_publisher_qos;
	p_domain_participant->get_default_publisher_qos = get_default_publisher_qos;
	p_domain_participant->set_default_subscriber_qos = set_default_subscriber_qos;
	p_domain_participant->get_default_subscriber_qos = get_default_subscriber_qos;
	p_domain_participant->set_default_topic_qos = set_default_topic_qos;
	p_domain_participant->get_default_topic_qos = get_default_topic_qos;

	p_domain_participant->get_discovered_participants = get_discovered_participants;
	p_domain_participant->get_discovered_participant_data = get_discovered_participant_data;
	p_domain_participant->get_discovered_topics = get_discovered_topics;
	p_domain_participant->get_discovered_topic_data = get_discovered_topic_data;
	p_domain_participant->contains_entity = contains_entity;
	p_domain_participant->get_current_time = get_current_time;

	///
	p_domain_participant->i_publisher = 0;
	p_domain_participant->pp_publisher = NULL;

	p_domain_participant->i_subscriber = 0;
	p_domain_participant->pp_subscriber = NULL;

	p_domain_participant->i_topic = 0;
	p_domain_participant->pp_topic = NULL;

	p_domain_participant->i_content_filtered_topics = 0;
	p_domain_participant->pp_content_filtered_topics = NULL;

	p_domain_participant->i_multi_topics = 0;
	p_domain_participant->pp_multi_topics = NULL;

	mutex_lock(&default_qos_lock);
	p_domain_participant->qos.entity_factory.autoenable_created_entities = default_domain_participant_qos.entity_factory.autoenable_created_entities;

	//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
	qos_init_user_data_qos(&p_domain_participant->qos.user_data);
	if (p_domain_participant->qos.user_data.value.i_string > 0)
		qos_copy_user_data_qos(&p_domain_participant->qos.user_data, &default_domain_participant_qos.user_data);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	p_domainparticipant->qos.user_data.value = NULL;

//	if(default_domain_participant_qos.user_data.value)
//	{
//		p_domainparticipant->qos.user_data.value = strdup(default_domain_participant_qos.user_data.value);
//	}
	mutex_unlock(&default_qos_lock);

	p_domain_participant->p_domain_participant_listener = NULL;


	p_domain_participant->i_builtin_topics = 0;
	p_domain_participant->pp_builtin_topics = NULL;

	p_domain_participant->p_builtin_publisher = NULL;

	p_domain_participant->i_waitsets = 0;
	p_domain_participant->pp_waitsets = NULL;

	p_domain_participant->i_entity_type = DOMAINPARTICIPANT_ENTITY;

	p_domain_participant->p_rtps_participant = NULL;


	//by jun
	//여러개의 Reader_Thread를 하나로 통합하기 위해서 사용되는 데이터 queue
	p_domain_participant->p_reader_thread_fifo = data_fifo_new();

	// writer by jun
	// 여러개의 Writer_Thread를 하나로 통합하기 위해서 사용되는 데이터 queue
	p_domain_participant->p_writer_thread_fifo = data_fifo_new();

	// writer by jun
	// Writer_Thread2에 이벤트 전달을 위해서 사용 
	cond_init(&p_domain_participant->writer_thread_wait);
	mutex_init(&p_domain_participant->writer_thread_lock );


	p_domain_participant->b_end = false;
	p_domain_participant->b_end2 = false;
	p_domain_participant->thread_id = 0;
	p_domain_participant->thread_id2 = 0;
	p_domain_participant->i_flag_reader_writer_thread = 0;
}


ReturnCode_t static_set_default_participant_qos(in_dds DomainParticipantQos* p_qos)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_default_domain_participant_qos == NULL)
	{
		mutex_init(&default_qos_lock);
		//default_domain_participant_qos 초기화. 
//		default_domain_participant_qos.user_data.value = NULL;
		default_domain_participant_qos.entity_factory.autoenable_created_entities = true;
		p_default_domain_participant_qos = &default_domain_participant_qos;
	}

	if (p_qos)
	{
		mutex_lock(&default_qos_lock);

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		qos_copy_user_data_qos(&default_domain_participant_qos.user_data, &p_qos->user_data);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		FREE(default_domain_participant_qos.user_data.value);

//		if(p_qos->user_data.value){
//			default_domain_participant_qos.user_data.value = strdup(p_qos->user_data.value);
//		}

		default_domain_participant_qos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;
		mutex_unlock(&default_qos_lock);

		trace_msg(OBJECT(p_module),TRACE_TRACE,"Default Participant QoS is changed.");
	}

	return RETCODE_OK;
}


ReturnCode_t static_get_default_participant_qos(inout_dds DomainParticipantQos* p_qos)
{
	if (p_default_domain_participant_qos == NULL)
	{
		mutex_init(&default_qos_lock);
		//default_domain_participant_qos 초기화. 
//		default_domain_participant_qos.user_data.value = NULL;
		default_domain_participant_qos.entity_factory.autoenable_created_entities = true;
		p_default_domain_participant_qos = &default_domain_participant_qos;
	}

	if (p_qos)
	{
		mutex_lock(&default_qos_lock);

		qos_init_user_data_qos(&p_qos->user_data);
		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////////////////////////////////
		if (default_domain_participant_qos.user_data.value.i_string > 0)
		{
			qos_copy_user_data_qos(&p_qos->user_data, &default_domain_participant_qos.user_data);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		if(default_domain_participant_qos.user_data.value)
//		{
//			FREE(p_qos->user_data.value);
//			p_qos->user_data.value = default_domain_participant_qos.user_data.value;
//		}

		p_qos->entity_factory.autoenable_created_entities = default_domain_participant_qos.entity_factory.autoenable_created_entities;
		mutex_unlock(&default_qos_lock);
	}

	return RETCODE_OK;
}


void destroy_domain_participant(DomainParticipant* p_domain_participant, bool b_except_domain_paticipant)
{
	int i_size, i;

	//FREE(default_domain_participant_qos.user_data.value);

	//할당된 publisher들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);

	i_size = p_domain_participant->i_publisher;

	for(i = i_size-1; i >= 0; i--){

		destroy_publisher(p_domain_participant->pp_publisher[i]);
		REMOVE_ELEM( p_domain_participant->pp_publisher, p_domain_participant->i_publisher, i);
	}

	/*while(p_domainparticipant->i_publisher)
	{
		destroy_publisher(p_domainparticipant->pp_publisher[0]);
		REMOVE_ELEM( p_domainparticipant->pp_publisher, p_domainparticipant->i_publisher, 0);
	}*/
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_publisher);


	//할당된 subscriber들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);

	i_size = p_domain_participant->i_subscriber;

	for(i = i_size-1; i >= 0; i--){

		destroy_subscriber(p_domain_participant->pp_subscriber[i]);
		REMOVE_ELEM( p_domain_participant->pp_subscriber, p_domain_participant->i_subscriber, i);
	}


	/*while(p_domainparticipant->i_subscriber)
	{
		destroy_subscriber(p_domainparticipant->pp_subscriber[0]);
		REMOVE_ELEM( p_domainparticipant->pp_subscriber, p_domainparticipant->i_subscriber, 0);
	}*/
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_subscriber);


	//할당된 topic들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);
	while(p_domain_participant->i_topic)
	{
		destroy_topic(p_domain_participant->pp_topic[0]);
		REMOVE_ELEM( p_domain_participant->pp_topic, p_domain_participant->i_topic, 0);
	}
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_topic);


	//할당된 contentFilteredTopic들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);
	while(p_domain_participant->i_content_filtered_topics)
	{
		destroy_content_filtered_topic(p_domain_participant->pp_content_filtered_topics[0]);
		REMOVE_ELEM( p_domain_participant->pp_content_filtered_topics, p_domain_participant->i_content_filtered_topics, 0);
	}
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_content_filtered_topics);


	//할당된 multiTopic들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);
	while(p_domain_participant->i_multi_topics)
	{
		destroy_multi_topic(p_domain_participant->pp_multi_topics[0]);
		REMOVE_ELEM( p_domain_participant->pp_multi_topics, p_domain_participant->i_multi_topics, 0);
	}
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_multi_topics);


	//할당된 bultinTopic들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);
	while(p_domain_participant->i_builtin_topics)
	{
		REMOVE_ELEM(p_domain_participant->pp_builtin_topics, p_domain_participant->i_builtin_topics, 0);
	}
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_builtin_topics);


	//할당된 WaitSet들 삭제..
	mutex_lock(&p_domain_participant->entity_lock);
	while(p_domain_participant->i_waitsets)
	{	
		destroy_waitset(p_domain_participant->pp_waitsets[0]);
		REMOVE_ELEM( p_domain_participant->pp_waitsets, p_domain_participant->i_waitsets, 0);
	}
	mutex_unlock(&p_domain_participant->entity_lock);

	FREE(p_domain_participant->pp_waitsets);

//	if(p_domain_participant->p_writer_thread_fifo)data_fifo_release(p_domain_participant->p_writer_thread_fifo);
//	if(p_domain_participant->p_reader_thread_fifo)data_fifo_release(p_domain_participant->p_reader_thread_fifo);

	remove_entity_to_service((Entity *)p_domain_participant);



	{
		//by jun
		// 각 도메인 파티시펀트에 할당된 reader 스레드 및 fifo queue 를 삭제
		//현재 각 reader 스레드를 통합해서 하나의 reader 스레드로 동작 시킴 
		data_t *p_data;
		data_t *p_data2;
		p_domain_participant->b_end = true;
		p_data = malloc(sizeof(data_t));

		memset(p_data,'\0',sizeof(data_t));
		data_fifo_put(p_domain_participant->p_reader_thread_fifo, p_data);

		thread_join2( p_domain_participant->thread_id );
		if(p_domain_participant->p_reader_thread_fifo)data_fifo_release(p_domain_participant->p_reader_thread_fifo);

		//writer by jun
		//통합된 하나의 writer 스레드를 삭제
		//job queue 및 job을 free
		p_domain_participant->b_end2 = true;
		p_data2 = malloc(sizeof(data_t));

		memset(p_data2,'\0',sizeof(data_t));
		data_fifo_put(p_domain_participant->p_writer_thread_fifo, p_data2);

		//thread_join2( p_domain_participant->thread_id2 );
		if(p_domain_participant->p_writer_thread_fifo)data_fifo_release(p_domain_participant->p_writer_thread_fifo);

	}

	destroy_entity((Entity*)p_domain_participant);
	if(!b_except_domain_paticipant)
		FREE(p_domain_participant);
}
