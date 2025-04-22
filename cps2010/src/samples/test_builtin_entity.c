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
	Builtin Entity 정보를 얻어오는 시험 코드

	2010-07-22 : 시작
	2013-11-25 : Builtin Entity 정보를 얻어오는 시험 코드, 김경일
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
/* ShapeType Topic Type 등록 함수                                       */
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
	Publisher* p_publisher = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	Topic* p_topic;

	DataWriterQos datawriter_qos;
	FooDataWriter* p_datawriter = NULL;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, NULL, NULL, ALL_ENABLE_STATUS);

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
	datawriter_qos.deadline.period.sec = 10;

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, NULL, ALL_ENABLE_STATUS);
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

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, NULL, ALL_ENABLE_STATUS);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

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
	datareader_qos.deadline.period.sec = 1;

	//DataReader 생성
	p_datareader = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos, NULL, DATA_AVAILABLE_STATUS);
}


/************************************************************************/
/* TopicDescription 출력 함수                                           */
/************************************************************************/
void print_topic_description(const TopicDescription* const p_topic_description)
{
	if (p_topic_description)
	{
		printf("## topic_name: [%-16s], type_name: [%20s]\r\n", p_topic_description->topic_name, p_topic_description->type_name);
	}
	else
	{
		printf("## p_topic_description : (null)\r\n");
	}
}


/************************************************************************/
/* GUID 출력 함수                                                       */
/************************************************************************/
void print_guid(const int32_t i_seq, const uint8_t guid[16])
{
	printf("## [%d] GUID: 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x \n", i_seq, guid[0], guid[1], guid[2], guid[3], guid[4], guid[5], guid[6], guid[7], guid[8], guid[9], guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
}


/************************************************************************/
/* Subscriber Listener 함수 정의                                        */
/************************************************************************/
void builtin_subscriber_on_data_available(DataReader* p_datareader)
{
	TopicDescription* p_topic_description;

	FooDataReader* p_foo_datareader = (FooDataReader*)p_datareader;
	FooSeq foo_seq = INIT_FOOSEQ;
	SampleInfoSeq sample_info_seq = INIT_SAMPLEINFOSEQ;
	message_t* p_message = NULL;
	uint8_t guid[16];

	// Builtin Reader 에 대한 정보 (토픽 이름과 타입 이름) 출력
	printf("\n\r###################### [ BUILTIN_READER_INFO ] ######################\r\n");

	p_topic_description = p_datareader->get_topicdescription(p_datareader);
	print_topic_description(p_topic_description);

	p_foo_datareader->read(p_foo_datareader, &foo_seq, &sample_info_seq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	if (!strcmp(p_topic_description->topic_name, PARTICIPANTINFO_NAME) && !strcmp(p_topic_description->type_name, PARTICIPANTINFO_TYPE))
	{
		ParticipantBuiltinTopicData data;
		while (foo_seq.i_seq)
		{
			p_message = (message_t*)foo_seq.pp_foo[0];
			memcpy(&data, p_message->v_data, sizeof(ParticipantBuiltinTopicData));
			memcpy(guid, &(data.key), 16);

			print_guid(foo_seq.i_seq, guid);
			message_release(p_message);
			REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
		}
	}
	else if (!strcmp(p_topic_description->topic_name, TOPICINFO_NAME) && !strcmp(p_topic_description->type_name, TOPICINFO_TYPE))
	{
		TopicBuiltinTopicData data;
		while (foo_seq.i_seq)
		{
			p_message = (message_t*)foo_seq.pp_foo[0];
			memcpy(&data, p_message->v_data, sizeof(TopicBuiltinTopicData));
			memcpy(guid, &(data.key), 16);

			print_guid(foo_seq.i_seq, guid);
			//key 이외의 값(name, type, qos)은 저장하지 않고 있음
			message_release(p_message);
			REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
		}
	}
	else if (!strcmp(p_topic_description->topic_name, PUBLICATIONINFO_NAME) && !strcmp(p_topic_description->type_name, PUBLICATIONINFO_TYPE))
	{
		PublicationBuiltinTopicData data;
		while (foo_seq.i_seq)
		{
			p_message = (message_t*)foo_seq.pp_foo[0];
			memcpy(&data, p_message->v_data, sizeof(PublicationBuiltinTopicData));
			memcpy(guid, &(data.participant_key), 16);

			print_guid(foo_seq.i_seq, guid);
			message_release(p_message);
			REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
		}
	}
	else if (!strcmp(p_topic_description->topic_name, SUBSCRIPTIONINFO_NAME) && !strcmp(p_topic_description->type_name, SUBSCRIPTIONINFO_TYPE))
	{
		SubscriptionBuiltinTopicData data;
		while (foo_seq.i_seq)
		{
			p_message = (message_t*)foo_seq.pp_foo[0];
			memcpy(&data, p_message->v_data, sizeof(SubscriptionBuiltinTopicData));
			memcpy(guid, &(data.participant_key), 16);

			print_guid(foo_seq.i_seq, guid);
			message_release(p_message);
			REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);
		}
	}
	FREE(sample_info_seq.pp_sample_infos);
	FREE(foo_seq.pp_foo);

	printf("#####################################################################\r\n\n\r");
}


/************************************************************************/
/* Builtin Reader 들에 대한 정보 (토픽 이름과 타입 이름) 출력 함수      */
/************************************************************************/
void print_builtin_entity(void)
{
	DomainParticipantFactory* p_domainparticipant_factory;
	DomainParticipant* p_domainparticipant = NULL;
	Subscriber* p_subscriber = NULL;
	DataReader* p_datareader = NULL;
	TopicDescription* p_topic_description;
	SubscriberListener* p_subscriber_listener;
	
	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

//	p_domainparticipant = p_domainparticipant_factory->lookup_participant(BUILTIN_PARTICIPANT_DOMAIN_ID);//오타는 추후 수정 예정
	p_domainparticipant = p_domainparticipant_factory->create_participant(0, NULL, NULL, 0);

	p_subscriber = p_domainparticipant->get_builtin_subscriber(p_domainparticipant);
	////// get_builtin_publisher() 함수는 존재하지 않는다. (스펙에서도)

	// Builtin Reader 들에 대한 정보 (토픽 이름과 타입 이름) 출력
	printf("\n\r###################### [ BUILTIN_READER_INFO ] ######################\r\n");

	p_datareader = p_subscriber->lookup_datareader(p_subscriber, PARTICIPANTINFO_NAME);
	p_topic_description = p_datareader->get_topicdescription(p_datareader);
	print_topic_description(p_topic_description);

	p_datareader = p_subscriber->lookup_datareader(p_subscriber, TOPICINFO_NAME);
	p_topic_description = p_datareader->get_topicdescription(p_datareader);
	print_topic_description(p_topic_description);

	p_datareader = p_subscriber->lookup_datareader(p_subscriber, PUBLICATIONINFO_NAME);
	p_topic_description = p_datareader->get_topicdescription(p_datareader);
	print_topic_description(p_topic_description);

	p_datareader = p_subscriber->lookup_datareader(p_subscriber, SUBSCRIPTIONINFO_NAME);
	p_topic_description = p_datareader->get_topicdescription(p_datareader);
	print_topic_description(p_topic_description);

	printf("#####################################################################\r\n\n\r");

	/// Builtin Subscriber에 SubscriberListener 를 등록
	p_subscriber_listener = (SubscriberListener*)calloc(1, sizeof(SubscriberListener));
	p_subscriber_listener->on_data_available = builtin_subscriber_on_data_available;
	p_subscriber->set_listener(p_subscriber, p_subscriber_listener, DATA_AVAILABLE_STATUS);
}


/************************************************************************/
/* main 함수                                                            */
/************************************************************************/
void main()
{
#ifndef _MSC_VER
	pthread_t thread_id;
#endif

	print_builtin_entity();

#ifdef _MSC_VER
	_beginthread(write_thread, 0, NULL);
	_beginthread(read_thread, 0, NULL);
#else
	pthread_create(&thread_id, NULL, (void *)write_thread, NULL);
	pthread_create(&thread_id, NULL, (void *)read_thread, NULL);
#endif

	printf("\r\n\r\nIf you want to quit. please Enter any key.\r\n");
	getchar();

	quit();
}
