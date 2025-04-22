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
	QoS Policy 테스트 프레임.

	이력
	2012-02-10
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
#include <stddef.h>
#include <stdlib.h>
#include "..\modules\rtps\rtps.h"



typedef struct dds_string
{
	int32_t size;
	char value[128];
}dds_string;


typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
}ShapeType;


#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport *fooTS)
{
	I_PARAMETER(fooTS, "color", DDS_STRING_TYPE, sizeof(dds_string));
	I_PARAMETER(fooTS, "x", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(fooTS, "y", DDS_INTEGER32_TYPE, sizeof(int32_t));
	I_PARAMETER(fooTS, "ShapeType", DDS_INTEGER32_TYPE, sizeof(int32_t));
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
		dds_parameter_t *p_para = get_new_parameter("ShapeType", DDS_INTEGER32_TYPE, sizeof(int32_t));
		INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
	}
}
#endif


static bool b_is_quit = false;


void on_data_available(DataReader *p_datareader)
{
  //printf("Called on_data_available\r\n");
}


int get_dds_string(dds_string *p_string, char *p_data, int string_size, int i_all_data_size)
{
	int i_string = 0;

	//*p_string = *((dds_string*)p_data);

	memset(p_string,0, sizeof(dds_string));
	memcpy(p_string, p_data, i_all_data_size);
	
	if(p_string->size <= 128)
	{
		
		//memset(p_string->value+p_string->size,0,128-p_string->size);
		i_string = p_string->size / 4;
		if((p_string->size % 4)!=0)
		{
			i_string++;
		}
		i_string *= 4;
	}

	i_string += sizeof(int32_t); // i_size

	return i_string;
}


void get_ShapeData(ShapeType *p_shapedata, char *p_data, int32_t i_all_data_size)
{
	int i_string_num = 0;
	i_string_num += get_dds_string(&p_shapedata->color, p_data, 128, i_all_data_size);

	p_shapedata->x = *((int32_t*)&p_data[i_string_num]);
	i_string_num += sizeof(int32_t);
	p_shapedata->y = *((int32_t*)&p_data[i_string_num]);
	i_string_num += sizeof(int32_t);
	p_shapedata->shapesize = *((int32_t*)&p_data[i_string_num]);
}


void set_string( dds_string* p_string, char* p_value )
{
	p_string->size = strlen(p_value)+1;
	memcpy(p_string->value,p_value,strlen(p_value));
	p_string->value[strlen(p_value)] = '\0';
}


DataWriterQos writer_qos_1;
DataWriterQos writer_qos_2;
bool b_active_writer_1 = false;
bool b_active_writer_2 = false;

extern void set_writer_qos(void);


void writer_thread(void* dummy)
{
    DomainParticipantFactory* p_dpf = DomainParticipantFactory_get_instance();
	DomainId_t domain_id = 0;
	StatusMask mask = 0xffffffff;
	DomainParticipant* p_participant = NULL;
	Publisher* p_publisher = NULL;
	PublisherQos publisher_qos;
	Topic* p_topic;
	FooDataWriter* p_foo_data_writer_1 = NULL;
	FooDataWriter* p_foo_data_writer_2 = NULL;
	int32_t sleep_time = 1000;


	ShapeType data;

	FooTypeSupport *fooTS = get_new_foo_type_support(); 
	////////////////////////////


	p_domainparticipant->get_default_publisher_qos(&publisher_qos);
	p_participant = p_dpf->create_participant(domain_id, NULL, NULL, mask);

	insert_ShapeType_Parameters(fooTS);
	fooTS->i_size = sizeof(ShapeType);
	fooTS->register_type(fooTS, p_participant, "ShapeType");

	p_publisher = p_participant->create_publisher(p_participant, &publisher_qos, NULL, mask);


	set_writer_qos();


	////////// writer 1 ////////
	if (b_active_writer_1)
	{
		p_topic = p_participant->create_topic(p_participant, "Square", "ShapeType", NULL, NULL, mask);	//Topic Name 수정
		p_foo_data_writer_1 = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &writer_qos_1, NULL, 0);
	}

	////////// writer 2 ////////
	if (b_active_writer_2)
	{
		p_topic = p_participant->create_topic(p_participant, "Circle", "ShapeType", NULL, NULL, mask);	//Topic Name 수정
		p_foo_data_writer_2 = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &writer_qos_2, NULL, 0);
	}

	////////////////

	set_string(&data.color, "CYAN");

	while (!b_is_quit && !is_quit())
	{
		Foo* fFoo;
		int32_t i = 0;
		int32_t j = 0;

		for (i = 0; i < 250; i++)
		{
			data.x = 250-i;
			data.y = i;
			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (p_foo_data_writer_1)
			{
				p_foo_data_writer_1->write(p_foo_data_writer_1, fFoo, 0);
			}
			if (p_foo_data_writer_2)
			{
				p_foo_data_writer_2->write(p_foo_data_writer_2, fFoo, 0);
			}
		}

		for (i = 0; i < 250; i++)
		{
			data.x = i;
			data.y = 250-i;
			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (p_foo_data_writer_1)
			{
				if (p_foo_data_writer_1->dataWriterQos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
				{
					p_foo_data_writer_1->assert_liveliness((DataWriter*)p_foo_data_writer_1);
				}
				else
				{
					p_foo_data_writer_1->write(p_foo_data_writer_1, fFoo, 0);
				}
			}
			if (p_foo_data_writer_2)
			{
				if (p_foo_data_writer_2->dataWriterQos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
				{
					p_foo_data_writer_2->assert_liveliness((DataWriter*)p_foo_data_writer_2);
				}
				else
				{
					p_foo_data_writer_2->write(p_foo_data_writer_2, fFoo, 0);
				}
			}
		}

		for (i = 0; i < 250; i++)
		{
			data.x = 250-i;
			data.y = 0;
			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (p_foo_data_writer_1)
			{
				p_foo_data_writer_1->write(p_foo_data_writer_1, fFoo, 0);
			}
			if (p_foo_data_writer_2)
			{
				p_foo_data_writer_2->write(p_foo_data_writer_2, fFoo, 0);
			}
		}

		for (i = 0; i < 250; i++)
		{
			data.x = i;
			data.y = i;
			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (p_foo_data_writer_1)
			{
				if (p_foo_data_writer_1->dataWriterQos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
				{
					p_foo_data_writer_1->assert_liveliness((DataWriter*)p_foo_data_writer_1);
				}
				else
				{
					p_foo_data_writer_1->write(p_foo_data_writer_1, fFoo, 0);
				}
			}
			if (p_foo_data_writer_2) 
			{
				if (p_foo_data_writer_2->dataWriterQos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
				{
					p_foo_data_writer_2->assert_liveliness((DataWriter*)p_foo_data_writer_2);
				}
				else
				{
					p_foo_data_writer_2->write(p_foo_data_writer_2, fFoo, 0);
				}
			}

		}

		for (i = 0; i < 250; i++)
		{
			data.x = 230;
			data.y = 250-i;
			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if (p_foo_data_writer_1)
			{
				p_foo_data_writer_1->write(p_foo_data_writer_1, fFoo, 0);
			}
			if (p_foo_data_writer_2)
			{
				p_foo_data_writer_2->write(p_foo_data_writer_2, fFoo, 0);
			}
		}
	}
}


DataReaderQos reader_qos_1;


extern void set_reader_qos(void);


void print_historycache(DataReader* const p_data_reader)
{
	rtps_reader_t* p_rtps_reader = (rtps_reader_t*)p_data_reader->p_related_rtps_reader;
	rtps_historycache_t* p_historycache = p_rtps_reader->reader_cache;
	int i;
	rtps_cachechange_t* p_rtps_cachechange;
	SerializedPayloadForReader* p_serializedForReader;
	message_t* p_message;
	ShapeType shapeData;

	mutex_lock(&p_historycache->object_lock);
	printf("\t* HistoryCache.size : %d\n", p_historycache->i_changes);
	for (i = 0; i < p_historycache->i_changes; i++)
	{
		p_rtps_cachechange = p_historycache->pp_changes[i];
		printf("\t%02d, ", i+1);
		p_serializedForReader = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
		p_message = message_new_for_builtin_reader(p_data_reader, &p_serializedForReader->p_value[4], p_serializedForReader->i_size - 4);
		memcpy(&shapeData, p_message->v_data, sizeof(ShapeType));
		printf("color : %s, x : %3d, y : %3d, shapesize : %d, ", 
			shapeData.color.value, shapeData.x, shapeData.y, shapeData.shapesize);
		printf("seq_num : %d\n", p_rtps_cachechange->sequence_number.low);
	}
	mutex_unlock(&p_historycache->object_lock);
}


void reader_thread(void* dummy)
{
	DomainParticipantFactory* p_dpf = DomainParticipantFactory_get_instance();
	DomainId_t domain_id = 0;
	StatusMask mask = 0xffffffff;
	DomainParticipant* p_participant = NULL;
	Subscriber* p_subscriber = NULL;
	SubscriberQos subscriber_qos;
	Topic* p_topic;
	FooDataReader* p_foo_data_reader;
	

	FooSeq fseq;
	SampleInfoSeq sSeq;
	ConditionSeq condseq;

	StatusCondition* p_statusCondition;
	ReadCondition* p_readCondition;

	int32_t count = 1;
	////////////////////////////////////
	
	WaitSet* p_waitset = waitset_new();
	
	Duration_t waittime = {100,100};
	//module_t* p_module = current_object(getDomainParticipantFactoryModuleId());


	SubscriberListener subscriberListener;
	
	ShapeType shapeData;

	FooTypeSupport *fooTS = get_new_foo_type_support(); 

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
	fseq.i_seq = 0;
	fseq.pp_foo = NULL;
	sSeq.i_seq = 0;
	sSeq.pp_sample_infos = NULL;
		
	condseq.i_seq = 0;
	condseq.pp_conditions = NULL;
	
	printf("called reader_thread\r\n");
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);
	p_participant = p_dpf->create_participant(domain_id, NULL, NULL, mask);
	p_participant = p_dpf->lookup_participant(domain_id);
	
	insert_ShapeType_Parameters(fooTS);
	fooTS->i_size = sizeof(ShapeType);
	fooTS->register_type(fooTS, p_participant, "ShapeType");


	set_reader_qos();


	p_subscriber = p_participant->create_subscriber(p_participant, &subscriber_qos, NULL, mask);

	p_topic = p_participant->create_topic(p_participant, "Square", "ShapeType", NULL, NULL, mask);
	p_foo_data_reader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber, p_topic, &reader_qos_1, NULL, mask);

	p_statusCondition = p_foo_data_reader->get_statuscondition((Entity*)p_foo_data_reader);
	p_statusCondition->set_enabled_statuses(p_statusCondition, DATA_AVAILABLE_STATUS);

	p_readCondition = p_foo_data_reader->create_readcondition((DataReader *)p_foo_data_reader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);
	//////

	
	////////////////////////////

	while (!b_is_quit && !is_quit())
	{
		int i;
		
//		p_waitset->wait(p_waitset, &condseq, waittime);
//		p_foo_data_reader->read(p_foo_data_reader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		p_foo_data_reader->take(p_foo_data_reader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;
			
			i = 0;
			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				memcpy(&shapeData, p_message->v_data, sizeof(ShapeType));
				//get_ShapeData(&shapeData, (char*)p_message->v_data, p_message->i_datasize);
				printf("[%4d]\tcolor[%2d] : %s, x : %3d, y : %3d, shapesize : %d\n", 
					count++, i++, shapeData.color.value, shapeData.x, shapeData.y, shapeData.shapesize);
				
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

//			print_historycache((DataReader*) p_foo_data_reader);
		}
	}

	p_participant->delete_subscriber(p_participant, p_subscriber);
	printf("\r\n\r\nremove publisher \r\n");
}


void main()
{
#ifndef _MSC_VER
	pthread_t writer_thread_id;
	pthread_t reader_thread_id;
#endif

	DomainParticipantFactory* p_dpf = DomainParticipantFactory_get_instance();
	const char* p_local_address = get_default_local_address();

	//////////////////////////////////////////////////////////////////////////
	// 내 로컬 IP 주소가 192.168.226.128 이면, reader_thread를 실행하고,
	// 아니면, writer_thread를 실행한다.
	// 192.168.226.128 은 그때그때 상황에 맞게 수정해서 사용한다.
	//////////////////////////////////////////////////////////////////////////
	// Pub 컴퓨터와 Sub 컴퓨터 양쪽에서 접근할 수 있는 공유폴더에 소스를 놓고 
	// 각각 컴퓨터에서 Visual Studio로 컴파일하여 실행할 수 있도록 함.
	// 공유폴더이기 때문에 한쪽 컴퓨터에서 수정하면, 다른쪽도 똑같이 반영된다.
	//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
	if (strcmp(p_local_address, "129.254.174.213") == 0)
	{
		_beginthread(reader_thread, 0, NULL);
	}
	else
	{
		_beginthread(writer_thread, 0, NULL);
	}
#else
	if (strcmp(p_local_address, "192.168.226.128") == 0)
	{
		pthread_create(&reader_thread_id, NULL, (void *)reader_thread, NULL);
	}
	else
	{
		pthread_create(&writer_thread_id, NULL, (void *)writer_thread, NULL);
	}
#endif

	printf("\r\n\r\nIf you want to quit. please Enter any key.\r\n");
	getch();
}
