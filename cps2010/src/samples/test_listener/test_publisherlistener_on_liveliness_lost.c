/*******************************************************************************
 * Copyright (c) 2013 Electronics and Telecommunications Research Institute
 * (ETRI) All Rights Reserved.
 * 
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	Publisher Listener 동작 시험 코드
	Publisher 에 설정한 PublisherListener 함수를 통하여 데이터 수신을 확인한다. 

	2010-07-22 : 시작
	...
	2013-11-22 : DataReader Listener 테스트 코드 삽입 및 정리, 김경일
	2013-12-05 : on_data_available 함수의 출력 코드 수정, 김경일
	2014-03-06 : on_liveliness_lost 함수 추가, 김경일
*/

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
/* DataWriter Listener 함수 정의                                        */
/************************************************************************/
void on_liveliness_lost(DataWriter* p_datawriter, LivelinessLostStatus* p_status)
{
	printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);

	printf("### %s:%d\t ########## \r\n", __FUNCTION__, __LINE__);
}


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


	// ANY_SAMPLE_STATE
	// READ_SAMPLE_STATE
	// NOT_READ_SAMPLE_STATE

	// ANY_VIEW_STATE
	// NEW_VIEW_STATE
	// NOT_NEW_VIEW_STATE

	// ANY_INSTANCE_STATE
	// ALIVE_INSTANCE_STATE
	// NOT_ALIVE_DISPOSED_INSTANCE_STATE
	// NOT_ALIVE_NO_WRITERS_INSTANCE_STATE
	// NOT_ALIVE_INSTANCE_STATE
	p_foo_datareader->read(p_foo_datareader, &foo_seq, &sample_info_seq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	if (foo_seq.i_seq)
	{
		while (foo_seq.i_seq)
		{
			p_message = (message_t*)foo_seq.pp_foo[0];
			memcpy(&shape_data, p_message->v_data, sizeof(ShapeType));
			printf("color[%d] : %s, x : %d, y : %d, shapesize : %d\n", foo_seq.i_seq, shape_data.color.value, shape_data.x, shape_data.y, shape_data.shapesize);
			message_release(p_message);
			REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
		}
		FREE(sample_info_seq.pp_sample_infos);
		FREE(foo_seq.pp_foo);
	}
	else
	{
		printf("********** on_data_available -> no data.\r\n");
	}
}


/************************************************************************/
/* domainparticipant 생성하는 함수                                      */
/************************************************************************/
void create_domainparticipant(void)
{
	//변수 선언
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainId_t domain_id = 0;
	DomainParticipantQos domainparticipant_qos;
	DomainParticipantListener* p_domainparticipant_listener = NULL;
	DomainParticipant* p_domainparticipant = NULL;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	////DomainParticipant Listener 설정
	//p_domainparticipant_listener = (DomainParticipantListener*)calloc(1, sizeof(DomainParticipantListener));
	//p_domainparticipant_listener->on_liveliness_lost = on_liveliness_lost;

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, p_domainparticipant_listener, ALL_ENABLE_STATUS);
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

	PublisherQos publisher_qos;
	PublisherListener* p_publisher_listener = NULL;
	Publisher* p_publisher = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	Topic* p_topic;

	DataWriterQos datawriter_qos;
	DataWriterListener* p_datawriter_listener = NULL;
	FooDataWriter* p_datawriter = NULL;

	int32_t sleep_time = 50000;
	ShapeType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant 검색
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	//Publisher Listener 설정
	p_publisher_listener = (PublisherListener*)calloc(1, sizeof(PublisherListener));
	p_publisher_listener->on_liveliness_lost = on_liveliness_lost;

	//Publisher 생성
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, p_publisher_listener, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "Square", "ShapeType", NULL ,NULL, ALL_ENABLE_STATUS);
	
	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&datawriter_qos);
//	datawriter_qos.liveliness.kind = AUTOMATIC_LIVELINESS_QOS;
	datawriter_qos.liveliness.kind = MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
//	datawriter_qos.liveliness.kind = MANUAL_BY_TOPIC_LIVELINESS_QOS;
	datawriter_qos.liveliness.lease_duration.sec = 2;
	datawriter_qos.liveliness.lease_duration.nanosec = 0;

	////DataWriter Listener 설정
	//p_datawriter_listener = (DataWriterListener*)calloc(1, sizeof(DataWriterListener));
	//p_datawriter_listener->on_liveliness_lost = on_liveliness_lost;

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, p_datawriter_listener, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	data.shapesize = 30;
	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

		set_string(&data.color, "CYAN");

		for (i = 0; i < 3; i++)
		{
			point(&data, i, j); //point 입력 함수
		
			msleep(sleep_time);

			if(is_quit()) break;

			p_datawriter->write(p_datawriter, p_foo, 0);
		}
		msleep(5000000);

		//set_string(&data.color, "BLUE");

		//for (i = 0; i < 3; i++)
		//{
		//	point(&data, i, j); //point 입력 함수

		//	msleep(sleep_time);

		//	if(is_quit()) break;

		//	p_datawriter->write(p_datawriter, p_foo, 0);
		//}
		//msleep(5000000);

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
	DomainParticipant* p_domainparticipant = NULL;

	SubscriberQos subscriber_qos;
	Subscriber* p_subscriber = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	TopicQos topic_qos;
	Topic* p_topic;

	DataReaderQos datareader_qos;
	DataReaderListener* p_datareader_listener = NULL;
	FooDataReader* p_datareader = NULL;
	
	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant 검색
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber 생성
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, NULL, DATA_AVAILABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"Square", "ShapeType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&datareader_qos);
	datareader_qos.history.depth = 10;

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
	create_domainparticipant();

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
