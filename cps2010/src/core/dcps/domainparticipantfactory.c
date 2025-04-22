/* 
	DomainParticipantFactory 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static mutex_t				structure_lock;
static bool structure_lock_initialize = false;

static bool is_INIT_DPF = false;
static DomainParticipantFactory* p_dpf = NULL;
static DomainParticipantFactoryQos domainParticipantFactoryQos;

static void create_builtin_participant_and_builtin_topic_and_builtin_publisher_and_builtin_datawriter(module_t* p_module);

//static bool isvisit_first_time = false;

static DomainParticipant* create_participant(in_dds DomainId_t domain_id, in_dds DomainParticipantQos* p_qos, in_dds DomainParticipantListener* p_listener, in_dds StatusMask mask)
{
	module_t* p_module = current_object(p_dpf->module_id);
	DomainParticipant* p_participant = NULL;
	int i;

	mutex_lock(&p_module->object_lock);
	for (i = 0; i < p_module->i_domain_participants; i++)
	{
		if (p_module->pp_domain_participants[i]->domain_id == domain_id)
		{
			mutex_unlock(&p_module->object_lock);
			return p_module->pp_domain_participants[i];
		}
	}
	mutex_unlock(&p_module->object_lock);

	if (p_module == NULL)
	{
		return NULL;
	}

	p_participant = malloc(sizeof(DomainParticipant));
	memset(p_participant, '\0', sizeof(DomainParticipant));

	init_domain_paticipant(p_participant);

	p_participant->domain_id = domain_id;
	p_participant->l_status_mask = mask;

	mutex_lock(&p_module->object_lock);
	INSERT_ELEM(p_module->pp_domain_participants, p_module->i_domain_participants, p_module->i_domain_participants, p_participant);
	mutex_unlock(&p_module->object_lock);

	if (p_qos)
	{
		p_participant->set_qos(p_participant, p_qos);
	}

	p_participant->p_domain_participant_listener = p_listener;
	
	//Participant 추가
	//add_entity_to_service((Entity*)p_participant);
//by kki...(entity factory)
	p_participant->b_enable = false;
	if (domainParticipantFactoryQos.entity_factory.autoenable_created_entities == true)
	{
		p_participant->enable((Entity*)p_participant);
	}

	trace_msg(OBJECT(p_module),TRACE_DEBUG,"Create participant : domain_id : %ld", domain_id);

	/*if(isvisit_first_time == false)
	{
		isvisit_first_time = true;
		msleep(1000000);
	}*/



	//by jun
	//하나의 Reader_Thread2를 생성
	p_participant->i_flag_reader_writer_thread = READER_THREAD;
    add_thread_to_service((Entity*)p_participant);

	//writer by jun
	//하나의 Writer_Thread2를 생성
	p_participant->i_flag_reader_writer_thread = WRITER_THREAD;
    add_thread_to_service((Entity*)p_participant);


	monitoring__add_participant(p_participant);
	
	return p_participant;
}

ReturnCode_t delete_participant(in_dds DomainParticipant* p_participant)
{
	module_t* p_module = current_object(p_dpf->module_id);
	int i;

	if (p_participant == NULL)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"p_participant is NULL");
		return RETCODE_ERROR;
	}

	mutex_lock(&p_module->object_lock);
	for (i = 0; i < p_module->i_domain_participants; i++)
	{
		if (p_module->pp_domain_participants[i] == p_participant)
		{
			monitoring__delete_participant(p_participant);
			destroy_domain_participant(p_participant,false);
			REMOVE_ELEM(p_module->pp_domain_participants, p_module->i_domain_participants, i);
			mutex_unlock(&p_module->object_lock);
			trace_msg(OBJECT(p_module),TRACE_DEBUG,"Delete participant");
			return RETCODE_OK;
		}
	}
	mutex_unlock(&p_module->object_lock);
	

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't delete a participant. It's not exist.");

	return RETCODE_NO_DATA;
}

static DomainParticipant *lookup_participant(in_dds DomainId_t domain_id)
{
	module_t *p_module = current_object( p_dpf->module_id );
	int i;

	for (i = 0; i < p_module->i_domain_participants; i++)
	{
		if (p_module->pp_domain_participants[i]->domain_id == domain_id)
		{
			trace_msg(OBJECT(p_module),TRACE_TRACE,"Lookup a participant : %ld",domain_id);

			return p_module->pp_domain_participants[i];
		}
	}

	trace_msg(OBJECT(p_module),TRACE_WARM,"Can't lookup a participant : %ld",domain_id);

	return NULL;
}

static ReturnCode_t set_default_participant_qos(in_dds DomainParticipantQos *p_qos)
{
	static_set_default_participant_qos(p_qos);
	return RETCODE_OK;
}

ReturnCode_t get_default_participant_qos(inout_dds DomainParticipantQos *p_qos)
{
	static_get_default_participant_qos(p_qos);
	return RETCODE_OK;
}


ReturnCode_t set_qos(in_dds DomainParticipantFactoryQos *p_qos)
{
	module_t* p_module = current_object(p_dpf->module_id);

	if (p_qos)
	{
		domainParticipantFactoryQos.entity_factory.autoenable_created_entities = p_qos->entity_factory.autoenable_created_entities;
		trace_msg(OBJECT(p_module), TRACE_TRACE, "ParticipantFactory QoS is changed.");
	}

	return RETCODE_OK;
}

ReturnCode_t get_qos(inout_dds DomainParticipantFactoryQos *p_qos)
{
	if (p_qos)
	{
		 p_qos->entity_factory.autoenable_created_entities = domainParticipantFactoryQos.entity_factory.autoenable_created_entities;
	}

	return RETCODE_OK;
}


static void initDomainParticipantFactory()
{
	p_dpf->create_participant = create_participant;
	p_dpf->delete_participant = delete_participant;
	p_dpf->lookup_participant = lookup_participant;

	p_dpf->set_default_participant_qos = set_default_participant_qos;
	p_dpf->get_default_participant_qos = get_default_participant_qos;

	p_dpf->set_qos = set_qos;
	p_dpf->get_qos = get_qos;
	////

	
}

#ifdef _MSC_VER

static BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		case CTRL_CLOSE_EVENT:
		case CTRL_C_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		default:
			break;
	}

	quit();
	msleep(1000000);

	return FALSE;
}

static void SetExitConsole()
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
}

#endif




static bool monitor_init = false;

static void Thread_Check_builtin(void* p_void)
{
	DomainParticipant* p_builtin_participant = NULL;
	Subscriber*	p_subscriber = NULL;
	Publisher*	p_publisher = NULL;
	DataReader*	p_datareader = NULL;
	DataWriter*	p_datawriter = NULL;
	int i, i_size;

	p_builtin_participant = create_participant(BUILTIN_PARTICIPANT_DOMAIN_ID, NULL, NULL, 0);

	while (monitor_init == false)
	{
		msleep(100*1000);
	}

	trace_msg(NULL, TRACE_LOG, "start Thread_Check_builtin");

	if (p_builtin_participant)
	{
		monitoring__add_participant(p_builtin_participant);
		p_subscriber = p_builtin_participant->get_builtin_subscriber(p_builtin_participant);

		if (p_subscriber)
		{
			monitoring__add_subscriber(p_builtin_participant->get_builtin_subscriber(p_builtin_participant));
			i_size = p_subscriber->i_datareaders;
			for (i = 0; i < i_size; i++)
			{
				monitoring__add_datareader(p_subscriber->pp_datareaders[i]);	
				p_datareader = p_subscriber->pp_datareaders[i];
				monitoring__add_writerproxy_for_builtin((rtps_reader_t *)p_datareader->p_related_rtps_reader);
			}
		}

		if (p_builtin_participant->i_publisher > 0)
		{
			p_publisher = p_builtin_participant->pp_publisher[0];
			if (p_publisher)
			{
				monitoring__add_publisher(p_publisher);
				i_size = p_publisher->i_datawriters;
				for (i = 0; i < i_size; i++)
				{
					monitoring__add_datawriter(p_publisher->pp_datawriters[i]);	
					p_datawriter = p_publisher->pp_datawriters[i];
					monitoring__add_readerproxy_for_builtin((rtps_writer_t *)p_datawriter->p_related_rtps_writer);
				}
			}
		}
	}

	trace_msg(NULL, TRACE_LOG, "end Thread_Check_builtin");
}

static bool structure_lock_initializing = false;
static bool is_checked = false;

static void check_lisense()
{

	int c_year = 2020, c_month = 8, c_day = 15;

	time_t timer_now, timer_expire;
	struct tm *t;


	if (is_checked == true) return;

	timer_now = time(NULL);

	t = localtime(&timer_now);

	t->tm_year = c_year - 1900;
	t->tm_mon = c_month - 1;
	t->tm_mday = c_day;

	timer_expire = mktime(t);

	if(timer_now > timer_expire)
	{
		printf("License expired : %04d-%02d-%02d\r\n", c_year, c_month, c_day);
		getch();
		exit(0);
	}else{
		printf("License : %04d-%02d-%02d\r\n", c_year, c_month, c_day);
	}

	is_checked = true;
}



DomainParticipantFactory* DomainParticipantFactory_get_instance()
{
	module_t* p_module = NULL;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif

#ifdef _MSC_VER
	SetExitConsole();
#endif

	check_lisense();

	//trace_msg(NULL,TRACE_LOG,"start DomainParticipantFactory_get_instance..");

	while (structure_lock_initializing == true)
	{
		printf("structure_lock_initializing.......\r\n");
		msleep(1000);
	}

	if (!structure_lock_initialize)
	{
		structure_lock_initializing = true;
		mutex_init(&structure_lock);
		structure_lock_initializing = false;
		structure_lock_initialize = true;
	}

	//trace_msg(NULL,TRACE_LOG,"start DomainParticipantFactory_get_instance..2");

	mutex_lock(&structure_lock);

	//trace_msg(NULL,TRACE_LOG,"start DomainParticipantFactory_get_instance..3");
	if (p_dpf == NULL)
	{
		trace_msg(NULL, TRACE_LOG, "EDDS r824. \r\n");

		p_dpf = malloc(sizeof(DomainParticipantFactory));

		memset(p_dpf, '\0', sizeof(DomainParticipantFactory));

		initDomainParticipantFactory(p_dpf);

		//trace_msg(NULL,TRACE_LOG,"p_dpf is null...");
		
		domainParticipantFactoryQos.entity_factory.autoenable_created_entities = true;

		p_dpf->module_id = modules_register();
		p_module = current_object( p_dpf->module_id );
		
		create_builtin_participant_and_builtin_topic_and_builtin_publisher_and_builtin_datawriter(p_module);

		//Monitoring 모듈 론치
		
		//msleep(5000000);
#ifdef _MSC_VER
		_beginthread(Thread_Check_builtin, 0, NULL);
#else
		pthread_create( &thread_id, NULL, (void *)Thread_Check_builtin, NULL );
#endif
	}
	//trace_msg(NULL,TRACE_LOG,"end DomainParticipantFactory_get_instance..1");
	
	//trace_msg(NULL,TRACE_LOG,"end DomainParticipantFactory_get_instance..2");


	if (is_INIT_DPF == false)
	{
		trace_msg(NULL,TRACE_LOG,"is_INIT_DPF           1");
		is_INIT_DPF = true;
		if (p_module)
		{
//			launch_monitoring_module(OBJECT(p_module));
			monitor_init = true;
		}
	}

	mutex_unlock(&structure_lock);


	return p_dpf;
}


int get_domain_participant_factory_module_id()
{
	if (p_dpf)
	{
		return p_dpf->module_id;
	}

	return -1;
}

void destroy_domain_participant_factory(DomainParticipantFactory* p_domain_participant_factory)
{
	int i, i_size;
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	p_module->b_end = true;

	//할당된 DomainParticipant들 삭제..
	mutex_lock(&p_module->object_lock);

	i_size = p_module->i_domain_participants;

	for (i = i_size-1; i >= 0; i--)
	{
		if (i == 0) msleep(1000000);
		if (i == 0) msleep(1000000);

		destroy_domain_participant(p_module->pp_domain_participants[i],false);
		REMOVE_ELEM(p_module->pp_domain_participants, p_module->i_domain_participants, i);
	}


	/*while(p_module->i_domain_participants)
	{
		destroy_domain_participant(p_module->pp_domain_participants[0], false);
		REMOVE_ELEM( p_module->pp_domain_participants, p_module->i_domain_participants, 0);	
	}*/
	mutex_unlock(&p_module->object_lock);
	//msleep(500000);

	FREE(p_module->pp_domain_participants);

	modules_unregister(get_domain_participant_factory_module_id());


	FREE(p_domain_participant_factory);
}


static DataReader* p_builtin_participant_reader = NULL;
static DataReader* p_builtin_topic_reader = NULL;
static DataReader* p_builtin_publisher_reader = NULL;
static DataReader* p_builtin_subscriber_reader = NULL;
static DataReader* p_builtin_live_reader = NULL;

DataReader* get_builtin_participant_reader()
{
	return p_builtin_participant_reader;
}

DataReader* get_builtin_topic_reader()
{
	return p_builtin_topic_reader;
}

DataReader* get_builtin_publisher_reader()
{
	return p_builtin_publisher_reader;
}

DataReader* get_builtin_subscriber_reader()
{
	return p_builtin_subscriber_reader;
}




static void create_builtin_participant_and_builtin_topic_and_builtin_publisher_and_builtin_datawriter(module_t* p_module)
{
	int i = 0;
	DomainParticipant* p_builtin_participant;
	
	TopicQos builtin_topic_qos;
	PublisherQos builtin_publisher_qos;
	DataWriterQos builtin_datawriter_qos;
	string	partition = NULL;
	Publisher* p_builtin_publisher;
	Topic*	p_builtin_topic_participant_info;
	//Topic*	p_builtin_topic_topic_info;
	Topic*	p_builtin_topic_publication_info;
	Topic*	p_builtin_topic_subscription_info;
	Topic*	p_builtin_topic_liveliness_info;
	DataWriter* p_builtin_writer_participant_info;
	DataWriter* p_builtin_writer_publication_info;
	DataWriter* p_builtin_writer_subscription_info;
	DataWriter* p_builtin_writer_live_info;

	DataReaderQos builtin_datareader_qos;
	SubscriberQos builtin_subscriber_qos;
	Subscriber* p_builtin_subscriber;

	p_builtin_participant = create_participant(BUILTIN_PARTICIPANT_DOMAIN_ID, NULL, NULL, 0);


	//added by kyy(TopicData QoS)
	builtin_topic_qos.topic_data.value.i_string = 0;
	builtin_topic_qos.topic_data.value.pp_string = NULL;
	//bultinTopicQos.topic_data.value = NULL;
	static_get_default_topic_qos(&builtin_topic_qos);
	
	
	builtin_topic_qos.durability.kind = TRANSIENT_DURABILITY_QOS;
	builtin_topic_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	builtin_topic_qos.durability_service.service_cleanup_delay = DURATIOIN_ZERO;

	p_builtin_topic_participant_info = p_builtin_participant->create_topic(p_builtin_participant, PARTICIPANTINFO_NAME, PARTICIPANTINFO_TYPE, &builtin_topic_qos, NULL, 0);


	//SKIP.... BUILTIN TOPIC
	//p_builtin_topic_topic_info = p_builtin_participant->create_topic(p_builtin_participant, TOPICINFO_NAME, TOPICINFO_TYPE, &builtin_topic_qos, NULL, 0);

	p_builtin_topic_publication_info = p_builtin_participant->create_topic(p_builtin_participant, PUBLICATIONINFO_NAME, PUBLICATIONINFO_TYPE, &builtin_topic_qos, NULL, 0);

	p_builtin_topic_subscription_info = p_builtin_participant->create_topic(p_builtin_participant, SUBSCRIPTIONINFO_NAME, SUBSCRIPTIONINFO_TYPE, &builtin_topic_qos, NULL, 0);
	
	p_builtin_topic_liveliness_info = p_builtin_participant->create_topic(p_builtin_participant, LIVELINESSP2P_NAME, LIVELINESSP2PINFO_NAME, &builtin_topic_qos, NULL, 0);
	

	///////////////////////////////////////////////////////////////////////////
	static_get_default_publisher_qos(&builtin_publisher_qos);

	
	builtin_publisher_qos.entity_factory.autoenable_created_entities = true;
	partition = BUILTIN_PUBLISHER_PARTITION_QOS;
	builtin_publisher_qos.partition.name.i_string = 0;
	builtin_publisher_qos.presentation.access_scope = TOPIC_PRESENTATION_QOS;
	builtin_publisher_qos.entity_factory.autoenable_created_entities = true;

	INSERT_ELEM(builtin_publisher_qos.partition.name.pp_string, builtin_publisher_qos.partition.name.i_string, builtin_publisher_qos.partition.name.i_string, partition);

	p_builtin_publisher = p_builtin_participant->create_publisher(p_builtin_participant, &builtin_publisher_qos, NULL, 0);
	FREE(builtin_publisher_qos.partition.name.pp_string);
	
	///////////////////////////////////////////////////////////////////////////
	static_get_default_datawriter_qos(&builtin_datawriter_qos);
	builtin_datawriter_qos.durability.kind = TRANSIENT_DURABILITY_QOS;
	builtin_datawriter_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	
	builtin_datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances = true;
	p_builtin_writer_participant_info = p_builtin_publisher->create_datawriter(p_builtin_publisher, p_builtin_topic_participant_info ,&builtin_datawriter_qos, NULL, 0);
	p_builtin_writer_participant_info->builtin_type = BUILTIN_PARTICIPANT_TYPE;

	builtin_datawriter_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;

	/*builtin_datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances = false;
	p_builtin_publisher->create_datawriter(p_builtin_publisher, p_builtin_topic_topic_info, &builtin_datawriter_qos, NULL, 0);
	p_builtin_publisher->builtin_type = BUILTIN_TOPIC_TYPE;
	*/

	builtin_datawriter_qos.writer_data_lifecycle.autodispose_unregistered_instances = true;
	p_builtin_writer_publication_info = p_builtin_publisher->create_datawriter(p_builtin_publisher, p_builtin_topic_publication_info, &builtin_datawriter_qos, NULL, 0);
	p_builtin_writer_publication_info->builtin_type = BUILTIN_PUBLICATION_TYPE;
	p_builtin_writer_subscription_info = p_builtin_publisher->create_datawriter(p_builtin_publisher, p_builtin_topic_subscription_info, &builtin_datawriter_qos, NULL, 0);
	p_builtin_writer_subscription_info->builtin_type = BUILTIN_SUBSCRIPTION_TYPE;


	builtin_datawriter_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	builtin_datawriter_qos.durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
	builtin_datawriter_qos.history.kind = KEEP_LAST_HISTORY_QOS;
	builtin_datawriter_qos.history.depth = INT_MAX; //256
	p_builtin_writer_live_info = p_builtin_publisher->create_datawriter(p_builtin_publisher, p_builtin_topic_liveliness_info, &builtin_datawriter_qos, NULL, 0);
	p_builtin_writer_live_info->builtin_type = BUILTIN_LIVELINESSP2P_TYPE;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static_get_default_subscriber_qos(&builtin_subscriber_qos);
	static_get_default_datareader_qos(&builtin_datareader_qos);

	builtin_datareader_qos.history.depth = INT_MAX;

	p_builtin_subscriber = p_builtin_participant->create_subscriber(p_builtin_participant,&builtin_subscriber_qos, NULL, 0);
	builtin_datareader_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	p_builtin_participant_reader = p_builtin_subscriber->create_datareader(p_builtin_subscriber, p_builtin_topic_participant_info, &builtin_datareader_qos, NULL, 0);
	p_builtin_participant_reader->builtin_type = BUILTIN_PARTICIPANT_TYPE;
	builtin_datareader_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	p_builtin_publisher_reader = p_builtin_subscriber->create_datareader(p_builtin_subscriber, p_builtin_topic_publication_info, &builtin_datareader_qos, NULL, 0);
	p_builtin_publisher_reader->builtin_type = BUILTIN_PUBLICATION_TYPE;
	p_builtin_subscriber_reader = p_builtin_subscriber->create_datareader(p_builtin_subscriber, p_builtin_topic_subscription_info, &builtin_datareader_qos, NULL, 0);
	p_builtin_subscriber_reader->builtin_type = BUILTIN_SUBSCRIPTION_TYPE;
	//p_builtin_topic_reader = p_builtin_subscriber->create_datareader(p_builtin_subscriber, p_builtin_topic_topic_info, &builtin_datareader_qos, NULL, 0);
	//p_builtin_topic_reader->builtin_type = BUILTIN_TOPIC_TYPE;
	builtin_datareader_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	builtin_datareader_qos.durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
	builtin_datareader_qos.history.kind = KEEP_LAST_HISTORY_QOS;
	builtin_datareader_qos.history.depth = INT_MAX;
	p_builtin_live_reader = p_builtin_subscriber->create_datareader(p_builtin_subscriber, p_builtin_topic_liveliness_info, &builtin_datareader_qos, NULL, 0);
	p_builtin_live_reader->builtin_type = BUILTIN_LIVELINESSP2P_TYPE;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	/*
		Bultin에 관련된 정보 전달 할 것...
		builtinCreateParticipantInfo
		builtinCreateTopicInfo
		builtinCreatePublicationInfo
	*/

	/*dataWriterWrite((DataWriter*)p_bultinWriterParticipantInfo,(void*)p_bultinWriterParticipantInfo, (InstanceHandle_t)&p_bultinWriterParticipantInfo);
	dataWriterWrite((DataWriter*)p_bultinWriterPublicationInfo,(void*)p_bultinWriterPublicationInfo, (InstanceHandle_t)&p_bultinWriterPublicationInfo);*/

}


static bool isquit = false;
static bool isTerminatedProgram = false;

bool is_quit()
{
	return isquit;
}

bool is_terminated()
{
	return isTerminatedProgram;
}

void monitoring_quit();

void quit()
{
	isquit = true;
	qos_set_stop_liveliness(true);
	monitoring_quit();
	msleep(1000000);
	if (p_dpf) destroy_domain_participant_factory(p_dpf);
	isTerminatedProgram = true;
}
