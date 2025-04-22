//
//	이파일은
//  get_current_time(),
//  get_discovered_participants(), get_discovered_participant_data(),
//  get_discovered_topics(), get_discovered_topic_data관련 API를 테스트하기 위한 파일이다.
//  
//  writer에서 CYAN sample을 전송 후 그동안 발견된 Participant, Topic 들의 정보를 프린트
//
//	작성자 : jeman
//
//	이력
//	2014-04-10 작성
//
//


#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


#ifdef _MSC_VER
#include <windows.h>
#include <process.h>
#include <conio.h>
#else
#include <curses.h>
#endif


//TopicType 정의
typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
} ShapeType;


/************************************************************************/
/* 정의된 ShapeType Topic Type의 등록                                   */
/************************************************************************/
#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport* p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter_key("color", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters, p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	I_PARAMETER(p_foo_typesupport, "x", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "y", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "shapesize", DDS_INTEGER32_TYPE, sizeof(int32_t));
}
#else
static void insert_ShapeType_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter_key("color", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("x", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("y", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("shapesize", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}
}
#endif


static bool b_is_quit = false;


/************************************************************************/
/* DataReader Listener 함수 정의                                        */
/************************************************************************/
void on_data_available(DataReader* p_datareader)
{
	FooDataReader* p_foo_datareader = (FooDataReader*)p_datareader;
	FooSeq foo_seq = INIT_FOOSEQ;
	SampleInfoSeq sample_info_seq = INIT_SAMPLEINFOSEQ;
	ShapeType shape_data;
	message_t* p_message = NULL;

	p_foo_datareader->read(p_foo_datareader, &foo_seq, &sample_info_seq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	while (foo_seq.i_seq)
	{
		p_message = (message_t*)foo_seq.pp_foo[0];
		memcpy(&shape_data, p_message->v_data, sizeof(ShapeType));
		printf("\ncolor[%d] : %s, x : %d, y : %d, shapesize : %d\n", foo_seq.i_seq, shape_data.color.value, shape_data.x, shape_data.y, shape_data.shapesize);
		message_release(p_message);
		REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
	}
	FREE(sample_info_seq.pp_sample_infos);
	FREE(foo_seq.pp_foo);
}


/************************************************************************/
/* 좌표정보를 변경하는 함수                                             */
/************************************************************************/
void point(ShapeType* p_shape_data, int i, int j)
{
	switch (j % 5)
	{			
	case 0 :
		p_shape_data->x = 250-i;
		p_shape_data->y = i;
		break;

	case 1 :
		p_shape_data->x = i;
		p_shape_data->y = 250-i;
		break;

	case 2 :
		p_shape_data->x = 250-i;
		p_shape_data->y = 0;
		break;

	case 3 :
		p_shape_data->x = i;
		p_shape_data->y = i;
		break;

	case 4 :
		p_shape_data->x = 225;
		p_shape_data->y = 250-i;
		j = 0;
		break;
	}
}


/************************************************************************/
/* Write 동작 Thread                                                    */
/************************************************************************/
void write_thread(void* p_dummy)
{
	//변수 선언
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainId_t domain_id = 0;
	DomainParticipant* p_domainparticipant = NULL;
	DomainParticipantQos domainparticipant_qos;

	PublisherQos publisher_qos;
	Publisher* p_publisher = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	TopicQos topic_qos;
	Topic* p_topic;

	DataWriterQos datawriter_qos;
	FooDataWriter* p_datawriter = NULL;

	int32_t sleep_time = 50000;
	ShapeType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant QoS 설정
	p_domainparticipant_factory->get_default_participant_qos(&domainparticipant_qos);

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, NULL, ALL_ENABLE_STATUS);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	//Publisher 생성
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic QoS 설정
	p_domainparticipant->get_default_topic_qos(&topic_qos);

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "Square", "ShapeType", &topic_qos ,NULL, ALL_ENABLE_STATUS);

	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&datawriter_qos);

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, NULL, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	data.shapesize = 30;
	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;


		InstanceHandleSeq InstanceHandleSeq = INIT_INSTANCESEQ;
		Time_t p_current_time = {0,0};
		time_t time1;

		char timeline[26];
		errno_t err;


		set_string(&data.color, "CYAN");

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point 입력 함수

			msleep(sleep_time);

			if(is_quit()) break;

			p_datawriter->write(p_datawriter, p_foo, 0);
		}

		////////////////////////get current time//////////////////////////////////
		p_domainparticipant->get_current_time(p_domainparticipant, &p_current_time);

		time1 = p_current_time.sec;


		err = ctime_s( timeline, _countof(timeline),  &(time1) );
		if (err)
		{
			printf("\n////////////////////////get current time//////////////////////////////////\n");
			printf("Invalid argument to ctime_s.\n");
		}
		printf( "Current time is %.19s.%hu %s \n", timeline, p_current_time.nanosec, &timeline[20] );

		////////////////////////get current time//////////////////////////////////


		////////////////////////get discovered participant //////////////////////////////////
		p_domainparticipant->get_discovered_participants(p_domainparticipant, &InstanceHandleSeq);

		if(InstanceHandleSeq.i_seq)
		{
			for(i = 0; i < InstanceHandleSeq.i_seq; i++){
				InstanceHandle_t handle = *InstanceHandleSeq.pp_instancehandle[i];
				if(handle > 0)
				{
					ParticipantBuiltinTopicData participant_data;
					ReturnCode_t ret;

					memset(&participant_data, 0, sizeof(ParticipantBuiltinTopicData));
					ret = p_domainparticipant->get_discovered_participant_data(p_domainparticipant, &participant_data, handle);

					if(ret == RETCODE_OK)
					{
						printf("\n////////////////////////get discovered participant data//////////////////////////////////\n");
						printf("Participant key value is %d.%d.%d.%d \n\n",participant_data.key.value[0],participant_data.key.value[1],participant_data.key.value[2],participant_data.key.value[3]);
					}
				}
				FREE(InstanceHandleSeq.pp_instancehandle[i]);
			}
			while(InstanceHandleSeq.i_seq)
			{
				REMOVE_ELEM(InstanceHandleSeq.pp_instancehandle, InstanceHandleSeq.i_seq, 0);
			}
		}

		////////////////////////get discovered participant //////////////////////////////////



		////////////////////////get discovered topic //////////////////////////////////
		p_domainparticipant->get_discovered_topics(p_domainparticipant, &InstanceHandleSeq);

		if(InstanceHandleSeq.i_seq)
		{
			for(i = 0; i < InstanceHandleSeq.i_seq; i++){
				InstanceHandle_t handle = *InstanceHandleSeq.pp_instancehandle[i];
				if(handle > 0)
				{
					TopicBuiltinTopicData topic_data;
					ReturnCode_t ret;

					memset(&topic_data, 0, sizeof(TopicBuiltinTopicData));
					ret = p_domainparticipant->get_discovered_topic_data(p_domainparticipant, &topic_data, handle);

					if(ret == RETCODE_OK)
					{
						printf("\n////////////////////////get discovered topic data //////////////////////////////////\n");
						printf("Topic name is %s \n",topic_data.name);
						printf("Type name is %s \n",topic_data.type_name);

						if(topic_data.reliability.kind == RELIABLE_RELIABILITY_QOS){
							printf("Qos.reliability.kind value is RELIABLE_RELIABILITY_QOS \n");
						}
						else if (topic_data.reliability.kind == BEST_EFFORT_RELIABILITY_QOS){
							printf("Qos.reliability.kind value is BEST_EFFORT_RELIABILITY_QOS \n");
						}
						printf("Qos.reliability.max_blocking_time value is %d,%d \n", topic_data.reliability.max_blocking_time.sec, topic_data.reliability.max_blocking_time.nanosec);


						if(topic_data.history.kind == KEEP_ALL_HISTORY_QOS){
							printf("Qos.history.kind value is KEEP_ALL_HISTORY_QOS \n");
						}
						else if (topic_data.history.kind == KEEP_LAST_HISTORY_QOS){
							printf("Qos.history.kind value is KEEP_LAST_HISTORY_QOS \n");
						}
						printf("Qos.history.depth value. is %d \n",topic_data.history.depth);

						printf("Qos.resource_limits.max_samples value is %d \n",topic_data.resource_limits.max_samples);
						printf("Qos.resource_limits.max_instances value is %d \n",topic_data.resource_limits.max_instances);
						printf("Qos.resource_limits.max_samples_per_instance value is %d \n",topic_data.resource_limits.max_samples_per_instance);
					
					}

					FREE(topic_data.name);
					FREE(topic_data.type_name);
				}

				FREE(InstanceHandleSeq.pp_instancehandle[i]);
			}
			while(InstanceHandleSeq.i_seq)
			{
				REMOVE_ELEM(InstanceHandleSeq.pp_instancehandle, InstanceHandleSeq.i_seq, 0);
			}
		}
		////////////////////////get discovered topic //////////////////////////////////





		set_string(&data.color, "BLUE");

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point 입력 함수

			msleep(sleep_time);

			if(is_quit()) break;

			p_datawriter->write(p_datawriter, p_foo, 0);
		}

		j++;
	}
}


/************************************************************************/
/* Read 동작 Thread                                                     */
/************************************************************************/
void read_thread(void* p_dummy)
{
	//변수 선언
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainId_t domain_id = 0;
	DomainParticipantQos domainparticipant_qos;
	DomainParticipant* p_domainparticipant = NULL;

	SubscriberQos subscriber_qos;
	SubscriberListener* p_subscriber_listener = NULL;
	Subscriber* p_subscriber = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	TopicQos topic_qos;
	Topic* p_topic;

	DataReaderQos datareader_qos;
	DataReaderListener* p_datareader_listener = NULL;
	FooDataReader* p_datareader = NULL;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant QoS 설정
	p_domainparticipant_factory->get_default_participant_qos(&domainparticipant_qos);

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, NULL, ALL_ENABLE_STATUS);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber 생성
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic QoS 설정
	p_domainparticipant->get_default_topic_qos(&topic_qos);

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"Square", "ShapeType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&datareader_qos);


	//DataReader Listener 설정
	p_datareader_listener = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener->on_data_available = on_data_available;

	//DataReader 생성
	p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, p_datareader_listener, ALL_ENABLE_STATUS);
}


/************************************************************************/
/* main 함수                                                            */
/************************************************************************/
void main()
{
#ifdef _MSC_VER
	_beginthread(write_thread, 0, NULL);
	_beginthread(read_thread, 0, NULL);
#else
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, (void *)write_thread, NULL);
	pthread_create(&thread_id, NULL, (void *)read_thread, NULL);
#endif

	printf("\r\n\r\nIf you want to quit. please Enter any key.\r\n");
	getchar();

	quit();
}
