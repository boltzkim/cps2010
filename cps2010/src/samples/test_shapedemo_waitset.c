//
//	이파일은 default 설정으로 shapedemo 토픽을 교환하는 예제이다.
//  QoS,listener 등은 모두 default 설정이며
//  Reader가 sample을 수신하기 위해  waitset 만을 사용하였다.
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
	datawriter_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	//datawriter_qos.reliability = RELIABLE_RELIABILITY_QOS;
	//datawriter_qos.history.kind = KEEP_LAST_HISTORY_QOS;
	//datawriter_qos.history.depth = 5;

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, NULL, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	data.shapesize = 30;
	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

		set_string(&data.color, "CYAN");

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point 입력 함수

			msleep(sleep_time);

			if(is_quit()) break;

			p_datawriter->write(p_datawriter, p_foo, 0);
		}

		//set_string(&data.color, "BLUE");

		//for (i = 0; i < 100; i++)
		//{
		//	point(&data, i, j); //point 입력 함수

		//	msleep(sleep_time);

		//	if(is_quit()) break;

		//	p_datawriter->write(p_datawriter, p_foo, 0);
		//}

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

	ShapeType shapeData;

	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;

	WaitSet * p_waitset = waitset_new();
	StatusCondition *p_statusCondition;
	ReadCondition *p_readCondition;
	Duration_t waittime = {100,0};
	ConditionSeq condseq = INIT_CONDITIONSEQ;

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
	datareader_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
	//datareader_qos.reliability = RELIABLE_RELIABILITY_QOS;
	//datareader_qos.history.kind = KEEP_LAST_HISTORY_QOS;
	//datareader_qos.history.depth = 5;

	//DataReader 생성
	//p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, p_datareader_listener, ALL_ENABLE_STATUS);
	p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, NULL, ALL_ENABLE_STATUS);

	//statusCondition 설정
	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,ALL_ENABLE_STATUS);

	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);


	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	//DataReader의 WaitSet 등록
	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);


	while(!b_is_quit && !is_quit())
	{
		int i=0;
		Foo *data_value = NULL;
		SampleInfo *sample_info = NULL;
		ReturnCode_t ret = RETCODE_OK;
		InstanceHandle_t previous_handle=0;

		p_waitset->wait(p_waitset, &condseq, waittime);

		//msleep(3000000);

		p_datareader->read(p_datareader, &fseq, &sSeq,  0, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

		if (fseq.i_seq)
		{
			message_t *p_message = NULL;
			int totla_size = fseq.i_seq;

			i = 0;
			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				memcpy(&shapeData, p_message->v_data, sizeof(ShapeType));		
				//if(totla_size-1 == i) {
				printf("\ncolor[%d:%d] : %s, x : %d, y : %d, shapesize : %d\n", 
					i, fseq.i_seq, shapeData.color.value, shapeData.x, shapeData.y, shapeData.shapesize);
				//}
				i++;
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);

			}
			FREE(sample_info_seq.pp_sample_infos);
			FREE(foo_seq.pp_foo);
		}
	}
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
