//
//	이파일은 default 설정으로 shapedemo 토픽을 교환하는 예제이다.
//  QoS, waitset, listener 등은 모두 default 설정이며
//  Reader가 sample을 수신하기 위해 on_data_available 만을 사용하였다.
//
//	작성자 : jeman
//
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
typedef struct TopicType
{
	dds_string	string_1;
	int32_t		integer_1;
	dds_string	string_2;
	char		char_1;
	double		float_1;
	double		float_2;
	int32_t		integer_2;
	int32_t		integer_3;
	char		char_2;
	char		char_3;
} TopicType;


/************************************************************************/
/* 정의된 TopicType Topic Type의 등록                                   */
/************************************************************************/
#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport* p_foo_typesupport)
{
	I_PARAMETER(p_foo_typesupport, "string_1",	DDS_STRING_TYPE,	sizeof(dds_string));
	I_PARAMETER(p_foo_typesupport, "integer_1",	DDS_INTEGER_TYPE,	sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "string_2",	DDS_STRING_TYPE,	sizeof(dds_string));
	I_PARAMETER(p_foo_typesupport, "char_1",	DDS_CHARACTER_TYPE,	sizeof(char));
	I_PARAMETER(p_foo_typesupport, "float_1",	DDS_FLOAT_TYPE,		sizeof(double));
	I_PARAMETER(p_foo_typesupport, "float_2",	DDS_FLOAT_TYPE,		sizeof(double));
	I_PARAMETER(p_foo_typesupport, "integer_2",	DDS_INTEGER_TYPE,	sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "integer_3",	DDS_INTEGER_TYPE,	sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "char_2",	DDS_CHARACTER_TYPE,	sizeof(char));
	I_PARAMETER(p_foo_typesupport, "char_3",	DDS_CHARACTER_TYPE,	sizeof(char));
}
#else
static void insert_ShapeType_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter_key("string_1", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("integer_2", DDS_INTEGER_TYPE, sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("integer_3", DDS_INTEGER_TYPE, sizeof(int32_t));
		INSERT_PARAM(p_foo_typesupport->pp_parameters,p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("char_2", DDS_INTEGER_TYPE, sizeof(int32_t));
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
	TopicType data;
	message_t* p_message = NULL;

	p_foo_datareader->read(p_foo_datareader, &foo_seq, &sample_info_seq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	while (foo_seq.i_seq)
	{
		p_message = (message_t*)foo_seq.pp_foo[0];
		memcpy(&data, p_message->v_data, sizeof(TopicType));
		printf("\n");
		printf("[%s]", data.string_1.value);
		printf("[%s]", data.string_2.value);
		printf("[%d]", data.integer_1);
		printf("[%d]", data.integer_2);
		printf("[%d]", data.integer_3);
		printf("[%c]", data.char_1);
		printf("[%c]", data.char_2);
		printf("[%c]", data.char_3);
		printf("[%f]", data.float_1);
		printf("[%f]", data.float_2);
		printf("\n");
		REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
	}
}


/************************************************************************/
/* domainparticipant 생성하는 함수                                      */
/************************************************************************/
void create_domainparticipant(DomainId_t domain_id)
{
	//변수 선언
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainParticipantQos domainparticipant_qos;
	DomainParticipantListener* p_domainparticipant_listener = NULL;
	DomainParticipant* p_domainparticipant = NULL;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//DomainParticipant QoS 설정
	p_domainparticipant_factory->get_default_participant_qos(&domainparticipant_qos);

	////DomainParticipant Listener 설정
	//p_domainparticipant_listener = (DomainParticipantListener*)malloc(sizeof(DomainParticipantListener));
	//p_domainparticipant_listener->on_liveliness_changed = on_liveliness_changed;

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, p_domainparticipant_listener, ALL_ENABLE_STATUS);
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

	int32_t sleep_time = 500000;
	TopicType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant QoS 설정
	static_get_default_participant_qos(&domainparticipant_qos);

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	//Publisher 생성
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(TopicType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "TopicType");

	//Topic QoS 설정
	p_domainparticipant->get_default_topic_qos(&topic_qos);

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "TestTopic1", "TopicType", &topic_qos ,NULL, ALL_ENABLE_STATUS);

	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&datawriter_qos);

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, NULL, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	set_string(&data.string_1, "string_1");
	set_string(&data.string_2, "string_2");
	data.char_1 = 'A';
	data.integer_1 = 1;
	data.float_1 = 1.0f;
	data.float_2 = 2.0f;
	data.integer_2 = 2;
	data.char_2 = 'B';
	data.char_3 = 'C';
	data.integer_3 = 3;

	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

		for (i = 0; i < 100; i++)
		{
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
	static_get_default_participant_qos(&domainparticipant_qos);

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber 생성
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(TopicType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "TopicType");

	//Topic QoS 설정
	p_domainparticipant->get_default_topic_qos(&topic_qos);

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"TestTopic1", "TopicType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&datareader_qos);


	//DataReader Listener 설정
	p_datareader_listener = (DataReaderListener*)malloc(sizeof(DataReaderListener));
	p_datareader_listener->on_data_available = on_data_available;

	//DataReader 생성
	p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, p_datareader_listener, ALL_ENABLE_STATUS);
}


struct member1
{
	char a[6];
	int b;
	char c[6];
	char d;
	double e;
};


struct member2
{
	char a;
	float b;
	int c;
	char d;
	int e;
};


/************************************************************************/
/* main 함수                                                            */
/************************************************************************/
void main()
{
#ifndef _MSC_VER
	pthread_t thread_id;
#endif
	DomainId_t domain_id = 0;
	char* p_local_address;

	struct member1 ex1;
	printf("member1's size : %d bytes \n", sizeof(ex1));
	printf("member1.a : (%d bytes) %d \n", sizeof(ex1.a), (int)&ex1.a-(int)&ex1.a);
	printf("member1.b : (%d bytes) %d \n", sizeof(ex1.b), (int)&ex1.b-(int)&ex1.a);
	printf("member1.c : (%d bytes) %d \n", sizeof(ex1.c), (int)&ex1.c-(int)&ex1.a);
	printf("member1.d : (%d bytes) %d \n", sizeof(ex1.d), (int)&ex1.d-(int)&ex1.a);
	printf("member1.e : (%d bytes) %d \n", sizeof(ex1.e), (int)&ex1.e-(int)&ex1.a);
	getchar();


	create_domainparticipant(domain_id);

	p_local_address = get_default_local_address();

	//////////////////////////////////////////////////////////////////////////
	// 내 로컬 IP 주소가 192.168.72.141 이면, reader_thread를 실행하고,
	// 아니면, writer_thread를 실행한다.
	// 로컬 IP 주소는 그때그때 상황에 맞게 수정해서 사용한다.
	//////////////////////////////////////////////////////////////////////////
	// Pub 컴퓨터와 Sub 컴퓨터 양쪽에서 접근할 수 있는 공유폴더에 실행파일을 놓고 
	// 각각 컴퓨터에서 실행함.
	//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
	if (strcmp(p_local_address, "192.168.72.141") == 0)
	{
		_beginthread(write_thread, 0, &domain_id);
	}
	else
	{
		_beginthread(read_thread, 0, &domain_id);
	}
#else
	if (strcmp(p_local_address, "192.168.72.141") == 0)
	{
		pthread_create(&thread_id, NULL, (void *)write_thread, &domain_id);
	}
	else
	{
		pthread_create(&thread_id, NULL, (void *)read_thread, &domain_id);
	}
#endif

	printf("\r\n\r\nIf you want to quit. please Enter any key.\r\n");
	getchar();

	quit();
}
