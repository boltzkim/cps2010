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
	Domain ID 설정에 따른 동작 시험 코드

	2010-07-22 : 시작
	2013-11-18 : Domain ID 테스트 코드 삽입, 박제만
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


typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
} ShapeType;

#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter_key("color", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}

	I_PARAMETER(fooTS, "x", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(fooTS, "y", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(fooTS, "shapesize", DDS_INTEGER32_TYPE, sizeof(int32_t));
}
#else
static void insert_ShapeType_Parameters(FooTypeSupport *fooTS)
{
	{
		dds_parameter_t *p_para = get_new_parameter("color", DDS_STRING_TYPE, sizeof(dds_string));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("x", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("y", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}

	{
		dds_parameter_t *p_para = get_new_parameter("shapesize", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}

}
#endif

static bool b_is_quit = false;

void on_data_available(DataReader *p_datareader)
{
  //printf("Called on_data_available\r\n");
}


static InstanceHandle_t my_writer_instance = 0;

void point(ShapeType* data, int i, int j)
{
	switch(j % 5)
	{			
		case 0 :
			data->x = 250-i;
			data->y = i;
			break;
			
		case 1 :
			data->x = i;
			data->y = 250-i;
			break;
			
		case 2 :
			data->x = 250-i;
			data->y = 0;
			break;
			
		case 3 :
			data->x = i;
			data->y = i;
			break;
			
		case 4 :
			data->x = 225;
			data->y = 250-i;
			j = 0;
		break;
	}
}



static void on_publication_matched(DataWriter* the_writer, PublicationMatchedStatus* status)
{
	printf("Called on_publication_matched\r\n");
}


//status by jun
DataWriterListener datawriterListener;

void TopicWrite(void *dummy)
{
	//Attribute 정의
	StatusMask mask = 0xffffffff;
	DomainParticipant *p_Participant = NULL;
	Publisher *p_publisher;
	PublisherQos pqos;
	Topic *p_Topic;
	//jeman
	DomainId_t domain_id = get_global_domain_id();
	FooDataWriter *p_foodataWriter = NULL;
	DataWriterQos a_writerQos;
	int32_t sleep_time = 50000;
	FooTypeSupport *fooTS1 = get_new_foo_type_support();
	ShapeType data;
	ShapeType datakey;
	int32_t j=0;
	Foo *fFoo;
	InstanceHandle_t instance_handle1;
	InstanceHandle_t instance_handle2;


	//waitset by jun
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq  condseq;

	//DomainParticipantFactory 생성
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();

	//status by jun
	datawriterListener.on_offered_deadline_missed = NULL;
	datawriterListener.on_offered_incompatible_qos = NULL;
	datawriterListener.on_liveliness_lost = NULL;
	datawriterListener.on_publication_matched = on_publication_matched;

	//Participant 생성
	p_Participant = p_dpf->create_participant(domain_id, NULL, NULL, ALL_ENABLE_STATUS);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&pqos);

	//Publisher 생성
	p_publisher = p_Participant->create_publisher(p_Participant,&pqos,NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(fooTS1);
	fooTS1->i_size = sizeof(ShapeType);							
	fooTS1->register_type(fooTS1, p_Participant, "ShapeType");

	//Topic 생성
	p_Topic = p_Participant->create_topic(p_Participant,"Square","ShapeType",NULL,NULL, ALL_ENABLE_STATUS);	//Topic Name 수정
	
	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&a_writerQos);
	a_writerQos.history.depth = 1;
	a_writerQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;


	msleep(100000);
	//DataWriter 생성
	p_foodataWriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_Topic,&a_writerQos,NULL,0);

	p_foodataWriter->set_listener((DataWriter*)p_foodataWriter, &datawriterListener, PUBLICATION_MATCHED_STATUS);


	p_statusCondition = p_foodataWriter->get_statuscondition((Entity*)p_foodataWriter);
	p_statusCondition->set_enabled_statuses(p_statusCondition,REQUESTED_DEADLINE_MISSED_STATUS);

	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	condseq.i_seq = 0;
	condseq.pp_conditions = NULL;

	///////////////////

	memset(&data,0,sizeof(ShapeType));
	memset(&datakey,0,sizeof(ShapeType));

	set_string(&data.color, "CYAN");
	fFoo = (Foo *)&data;

	instance_handle1 = p_foodataWriter->register_instance(p_foodataWriter, fFoo);

	set_string(&data.color, "BLUE");
	instance_handle2 = p_foodataWriter->register_instance(p_foodataWriter, fFoo);
	fFoo = (Foo *)&data;


	p_foodataWriter->get_key_value(p_foodataWriter, (Foo *)&datakey, instance_handle1);
	printf("\r\nWrtier get_key_value : key : %s(%ld)\r\n", datakey.color.value, instance_handle1);

	memset(&datakey,0,sizeof(ShapeType));
	p_foodataWriter->get_key_value(p_foodataWriter, (Foo *)&datakey, instance_handle2);
	printf("\r\nWrtier get_key_value : key : %s(%ld)\r\n", datakey.color.value, instance_handle2);
	
	while (!b_is_quit && !is_quit())
	{
		int32_t i=0;
		ConditionSeq *p_temp_Condition = NULL;

		for (i = 0; i< condseq.i_seq; i++)
		{
			p_temp_Condition = (ConditionSeq *)condseq.pp_conditions[i];
			if (p_temp_Condition == (ConditionSeq *)p_statusCondition)
			{
				printf("condseq1->i_seq %d \n", condseq.i_seq);
			}
		}

		set_string(&data.color, "CYAN");

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point 입력 함수

			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (is_quit()) break;

			//DataWrite
			p_foodataWriter->write(p_foodataWriter,fFoo, 0);
		}

		//set_string(&data.color, "BLUE");

		//for (i = 0; i < 100; i++)
		//{
		//	point(&data, i, j); //point 입력 함수

		//	data.shapesize = 30;

		//	fFoo = (Foo *)&data;
		//
		//	msleep(sleep_time);

		//	if(is_quit()) break;

		//	//DataWrite
		//	p_foodataWriter->write(p_foodataWriter,fFoo, 0);
		//}

		j++;
	}


	set_string(&data.color, "CYAN");
	fFoo = (Foo *)&data;

	p_foodataWriter->dispose(p_foodataWriter, fFoo, instance_handle1);

	set_string(&data.color, "BLUE");
	fFoo = (Foo *)&data;

	p_foodataWriter->dispose(p_foodataWriter, fFoo, instance_handle2);


	set_string(&data.color, "CYAN");
	fFoo = (Foo *)&data;

	p_foodataWriter->unregister_instance(p_foodataWriter, fFoo, instance_handle1);


	set_string(&data.color, "BLUE");
	fFoo = (Foo *)&data;

	p_foodataWriter->unregister_instance(p_foodataWriter, fFoo, instance_handle2);
}


void TopicRead(void *dummy)
{
	//Attribute 정의
	//jeman
	DomainId_t domain_id = get_global_domain_id();
	StatusMask mask = 0xffffffff;
	DomainParticipant *p_Participant = NULL;
	Subscriber *p_subscriber = NULL;
	Topic *p_Topic;			

	FooDataReader *p_datareader;
	
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;

	DataReaderQos a_readerQos;

	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	SubscriberQos subscriberQos;
	SubscriberListener subscriberListener;

	TopicQos a_topicQos;
	
	FooTypeSupport *fooTS = get_new_foo_type_support(); 

	ShapeType shapeData;

	StringSeq seq;

	ShapeType data;
	ShapeType datakey;
	Foo *fFoo;
	InstanceHandle_t instance_handle;

	DomainParticipantQos p_dpq;
	
	//DomainParticipantFactory 생성
	DomainParticipantFactory *p_dpf= DomainParticipantFactory_get_instance();

	///////////////
	subscriberListener.on_data_available = on_data_available;
	subscriberListener.on_data_on_readers = NULL;
	subscriberListener.on_liveliness_changed = NULL;
	subscriberListener.on_requested_deadline_missed = NULL;
	subscriberListener.on_requested_incompatible_qos = NULL;
	subscriberListener.on_sample_lost = NULL;
	subscriberListener.on_sample_rejected = NULL;
	subscriberListener.on_subscription_matched = NULL;
	///////////////


	//////////////
	fseq.i_seq = 0;
	fseq.pp_foo = NULL;
	sSeq.i_seq = 0;
	sSeq.pp_sample_infos = NULL;
		
	condseq.i_seq = 0;
	condseq.pp_conditions = NULL;
	
	printf("called check key\r\n");

	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);


	//Participant 생성
	p_Participant = p_dpf->create_participant(domain_id, &p_dpq, NULL, ALL_ENABLE_STATUS);

	p_Participant = p_dpf->lookup_participant(domain_id);


	//Subscriber 생성
	p_subscriber = p_Participant->create_subscriber(p_Participant, &subscriberQos, NULL, ALL_ENABLE_STATUS);
	//listener by jun
	p_subscriber->set_listener(p_subscriber, &subscriberListener, REQUESTED_DEADLINE_MISSED_STATUS);


	//Topic 생성
	p_Topic = p_Participant->create_topic(p_Participant, "Square", "ShapeType", &a_topicQos, NULL, ALL_ENABLE_STATUS);	

	//Topic type register
	insert_ShapeType_Parameters(fooTS);
	fooTS->i_size = sizeof(ShapeType);								//Size 입력
	fooTS->register_type(fooTS, p_Participant, "ShapeType");		//Topic Name 입력

	seq.i_string = 0;
	seq.pp_string = NULL;
	
	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&a_readerQos);
	a_readerQos.history.depth = 10;
	a_readerQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;


	//DataReader 생성
	p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber, p_Topic, &a_readerQos, NULL, ALL_ENABLE_STATUS);

	///////////////

	memset(&data,0,sizeof(ShapeType));
	memset(&datakey,0,sizeof(ShapeType));

	set_string(&data.color, "BLUE");
	fFoo = (Foo *)&data;
	instance_handle = p_datareader->lookup_instance(p_datareader, fFoo);
	
	p_datareader->get_key_value(p_datareader, (Foo *)&datakey, instance_handle);

	printf("\r\nReader get_key_value : key : %s(%ld)\r\n", datakey.color.value, instance_handle);


	while (!b_is_quit && !is_quit())
	{
		int i = 0;
		Foo *data_value = NULL;
		SampleInfo *sample_info = NULL;
		ReturnCode_t ret = RETCODE_OK;
		InstanceHandle_t previous_handle = 0;
		
		//DataRead
		p_datareader->read(p_datareader, &fseq, &sSeq,  1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		

		if (fseq.i_seq)
		{
			message_t *p_message = NULL;
			int totla_size = fseq.i_seq;			
			
			i=0;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				memcpy(&shapeData, p_message->v_data, sizeof(ShapeType));		
				if (totla_size-1 == i) printf("\ncolor[%d:%d] : %s, x : %d, y : %d, shapesize : %d\n", 
					i, fseq.i_seq, shapeData.color.value, shapeData.x, shapeData.y, shapeData.shapesize);
				i++;
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}
		}
	}
}

void main()
{
	//jeman Domain ID 설정 전역 변수 g_domain_id
	set_global_domain_id(1);
#ifndef _MSC_VER
	pthread_t thread_id;
#endif
	
#ifdef _MSC_VER
	//Writer Thread
	_beginthread( TopicWrite, 0, NULL );

	//Reader Thread
	_beginthread( TopicRead, 0, NULL );
#else
	pthread_create( &thread_id, NULL, (void *)TopicWrite, NULL );
	pthread_create( &thread_id, NULL, (void *)TopicRead, NULL );
#endif

	printf("\r\n\r\nIf you want to quit. please Enter any key.33\r\n");
	getchar();

	quit();
}
