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
	TimebasedFilter QoS ���� ���� �ڵ�
	TimebasedFilter QoS ���� ���� �ð� ������ �ΰ� �����͸� �����ϴ� ���� Ȯ���Ѵ�. 

	2010-07-22 : ����
	...
	2013-11-22 : DataReader Listener �׽�Ʈ �ڵ� ���� �� ����, �����
	2013-12-05 : TimebasedFilter QoS ���� ���� �ڵ� ����, �����
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


//TopicType ����
typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
} ShapeType;


/************************************************************************/
/* ���ǵ� ShapeType Topic Type�� ���                                   */
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
/* DataReader Listener �Լ� ����                                        */
/************************************************************************/
void on_data_available_template(DataReader* p_datareader)
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


extern Duration_t time_subtraction_d(const Duration_t minuend_time, const Duration_t subtrahend_time);
/************************************************************************/
/* DataReader Listener �Լ� ����                                        */
/************************************************************************/
void on_data_available_1(DataReader* p_datareader)
{
	static b_first = true;
	static Duration_t first_time;
	if (b_first)
	{
		first_time = current_duration();
		b_first = false;
	}

	on_data_available_template(p_datareader);

	{
		Duration_t current_time = time_subtraction_d(current_duration(), first_time);
		printf("### %s:%d,\t TIME:[%d.%d]\r\n", __FUNCTION__, __LINE__, current_time.sec, current_time.nanosec);
	}
}


/************************************************************************/
/* DataReader Listener �Լ� ����                                        */
/************************************************************************/
void on_data_available_2(DataReader* p_datareader)
{
	static b_first = true;
	static Duration_t first_time;
	if (b_first)
	{
		first_time = current_duration();
		b_first = false;
	}

	on_data_available_template(p_datareader);

	{
		Duration_t current_time = time_subtraction_d(current_duration(), first_time);
		printf("### %s:%d,\t\t\t\t TIME:[%d.%d]\r\n", __FUNCTION__, __LINE__, current_time.sec, current_time.nanosec);
	}
}


/************************************************************************/
/* ��ǥ������ �����ϴ� �Լ�                                             */
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
/* Write ���� Thread                                                    */
/************************************************************************/
void write_thread(void* p_dummy)
{
	//���� ����
	DomainParticipantFactory* p_domainparticipant_factory;

	DomainId_t domain_id = 0;
	DomainParticipant* p_domainparticipant = NULL;

	PublisherQos publisher_qos;
	Publisher* p_publisher = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	Topic* p_topic;

	DataWriterQos writer_qos;
	FooDataWriter* p_datawriter = NULL;

	int32_t sleep_time = 50000;
	ShapeType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory ����
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant ����
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, NULL, NULL, ALL_ENABLE_STATUS);

	//Publisher QoS ����
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	//Publisher ����
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, NULL, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic ����
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "Square", "ShapeType", NULL ,NULL, ALL_ENABLE_STATUS);
	
	//DataWriter QoS ����
	p_publisher->get_default_datawriter_qos(&writer_qos);

	//DataWriter ����
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &writer_qos, NULL, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	data.shapesize = 30;
	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

		set_string(&data.color, "CYAN");

		for (i = 0; i < 100; i++)
		{
			point(&data, i, j); //point �Է� �Լ�
		
			msleep(sleep_time);

			if(is_quit()) break;

			p_datawriter->write(p_datawriter, p_foo, 0);
		}

		//set_string(&data.color, "BLUE");

		//for (i = 0; i < 100; i++)
		//{
		//	point(&data, i, j); //point �Է� �Լ�

		//	msleep(sleep_time);

		//	if(is_quit()) break;

		//	p_datawriter->write(p_datawriter, p_foo, 0);
		//}

		j++;
	}
}


/************************************************************************/
/* Read ���� Thread                                                     */
/************************************************************************/
void read_thread(void* p_dummy)
{
	//���� ����
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

	DataReaderQos datareader_qos_1;
	DataReaderListener* p_datareader_listener_1 = NULL;
	FooDataReader* p_datareader_1 = NULL;
	DataReaderQos datareader_qos_2;
	DataReaderListener* p_datareader_listener_2 = NULL;
	FooDataReader* p_datareader_2 = NULL;
	
	//DomainParticipantFactory ����
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant ����
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, NULL, ALL_ENABLE_STATUS);

	//Subscriber QoS ����
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber Listener ����
	//p_subscriber_listener = (SubscriberListener*)calloc(1, sizeof(SubscriberListener));
	//p_subscriber_listener->on_data_available = on_data_available;

	//Subscriber ����
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, p_subscriber_listener, DATA_AVAILABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic ����
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"Square", "ShapeType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS ����
	p_subscriber->get_default_datareader_qos(&datareader_qos_1);
	datareader_qos_1.history.depth = 10;
	datareader_qos_1.time_based_filter.minimum_separation.sec = 3;
	p_subscriber->get_default_datareader_qos(&datareader_qos_2);
	datareader_qos_2.history.depth = 10;
	datareader_qos_2.time_based_filter.minimum_separation.sec = 5;

	//DataReader Listener ����
	p_datareader_listener_1 = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener_1->on_data_available = on_data_available_1;
	p_datareader_listener_2 = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener_2->on_data_available = on_data_available_2;
	//DataReader ����
	p_datareader_1 = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos_1, p_datareader_listener_1, DATA_AVAILABLE_STATUS);
	p_datareader_2 = (FooDataReader*)p_subscriber->create_datareader(p_subscriber, p_topic, &datareader_qos_2, p_datareader_listener_2, DATA_AVAILABLE_STATUS);}


/************************************************************************/
/* main �Լ�                                                            */
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
