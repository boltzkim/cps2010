/*******************************************************************************
 * Copyright (c) 2011 Electronics and Telecommunications Research Institute
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
	Ownership QoS Test Code
	이 파일은 Ownership QoS 테스트를 위한 파일이다.

	2011-12-08 : 수정
	2014-07-15 : 수정
	2014-07-24 : 수정
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
}ShapeType;

/************************************************************************/
/* 정의된 ShapeType Topic Type의 등록                                   */
/************************************************************************/
#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter_key("color", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(p_foo_typesupport->pp_parameters, p_foo_typesupport->i_parameters, p_foo_typesupport->i_parameters, p_para);
	}

	I_PARAMETER(p_foo_typesupport, "x", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "y", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(p_foo_typesupport, "ShapeType", DDS_INTEGER32_TYPE, sizeof(int32_t));
}
#else
static void insert_ShapeType_Parameters(FooTypeSupport *p_foo_typesupport)
{
	{
		dds_parameter_t *p_para = get_new_parameter("color", DDS_STRING_TYPE, sizeof(dds_string));
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
		dds_parameter_t *p_para = get_new_parameter("ShapeType", DDS_INTEGER32_TYPE, sizeof(int32_t));
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
			printf("\ncolor[%d] : %s, x : %d, y : %d, shapesize : %d\n", foo_seq.i_seq, shape_data.color.value, shape_data.x, shape_data.y, shape_data.shapesize);
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

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);
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
	//p_domainparticipant_listener = (DomainParticipantListener*)calloc(1, sizeof(DomainParticipantListener));
	//p_domainparticipant_listener->on_data_available = on_data_available;

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

	DomainId_t domain_id = *(DomainId_t*)p_dummy;
	DomainParticipant* p_domainparticipant = NULL;

	PublisherQos publisher_qos;
	Publisher* p_publisher = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	Topic* p_topic;

	DataWriterQos datawriter_qos;
	DataWriterQos datawriter_qos_2;
	FooDataWriter* p_datawriter = NULL;
	FooDataWriter* p_datawriter_2 = NULL;

	int32_t sleep_time = 50000;
	ShapeType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	//Publisher 생성
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "Square", "ShapeType", NULL ,NULL, ALL_ENABLE_STATUS);

	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&datawriter_qos);
	datawriter_qos.history.depth = 10;
//	datawriter_qos.ownership.kind = SHARED_OWNERSHIP_QOS;
	datawriter_qos.ownership.kind = EXCLUSIVE_OWNERSHIP_QOS;
	datawriter_qos.ownership_strength.value = 15;

	p_publisher->get_default_datawriter_qos(&datawriter_qos_2);
	datawriter_qos_2.history.depth = 10;
	//	datawriter_qos_2.ownership.kind = SHARED_OWNERSHIP_QOS;
	datawriter_qos_2.ownership.kind = EXCLUSIVE_OWNERSHIP_QOS;
	datawriter_qos_2.ownership_strength.value = 10;


	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, NULL, ALL_ENABLE_STATUS);
	p_datawriter_2 = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos_2, NULL, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	p_foo = (Foo *)&data;

	set_string(&data.color, "BLUE");

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point 입력 함수

			msleep(sleep_time);

			if(is_quit()) break;

			data.shapesize = datawriter_qos.ownership_strength.value;
			p_datawriter->write(p_datawriter, p_foo, 0);
			data.shapesize = datawriter_qos_2.ownership_strength.value;
			p_datawriter_2->write(p_datawriter_2, p_foo, 0);
		}

		j++;
		if (j == 1)
		{
			datawriter_qos.ownership_strength.value = 12;
			p_datawriter->set_qos((DataWriter*)p_datawriter, &datawriter_qos);
		}
		else if (j == 2)
		{
			datawriter_qos.ownership_strength.value = 7;
			p_datawriter->set_qos((DataWriter*)p_datawriter, &datawriter_qos);
		}
	}
}


/************************************************************************/
/* Read 동작 Thread                                                     */
/************************************************************************/
void read_thread(void* p_dummy)
{
	//변수 선언
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainId_t domain_id = *(DomainId_t*)p_dummy;
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

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->lookup_participant(domain_id);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber Listener 설정
	//p_subscriber_listener = (SubscriberListener*)calloc(1, sizeof(SubscriberListener));
	//p_subscriber_listener->on_data_available = on_data_available;

	//Subscriber 생성
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, p_subscriber_listener, DATA_AVAILABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"Square", "ShapeType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&datareader_qos);
	datareader_qos.history.depth = 10;
//	datareader_qos.ownership.kind = SHARED_OWNERSHIP_QOS;
	datareader_qos.ownership.kind = EXCLUSIVE_OWNERSHIP_QOS;


	//DataReader Listener 설정
	p_datareader_listener = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener->on_data_available = on_data_available;

	//DataReader 생성
	p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, p_datareader_listener, DATA_AVAILABLE_STATUS);
}


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
	if (strcmp(p_local_address, "192.168.72.129") != 0)
	{
#ifdef _MSC_VER
		_beginthread(write_thread, 0, &domain_id);
#else
		pthread_create(&thread_id, NULL, (void *)write_thread, &domain_id);
#endif
	}
	else
	{
#ifdef _MSC_VER
		_beginthread(read_thread, 0, &domain_id);
#else
		pthread_create(&thread_id, NULL, (void *)read_thread, &domain_id);
#endif
	}

	printf("\r\n\r\nIf you want to quit. please Enter any key.\r\n");
	getchar();

	quit();
}
