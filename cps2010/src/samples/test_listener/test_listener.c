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
/* Topic Listener 함수 정의                                             */
/************************************************************************/
//발동 조건 : Topic EDP를 수신했을 때 자신이 가진 Topic과 일치하지 않을 경우
void on_inconsistent_topic(Topic *p_topic, InconsistentTopicStatus *p_status)
{
	p_topic->get_inconsistent_topic_status(p_topic,p_status);
	if(p_topic->p_topic_listener)
		printf("Inconsistent Topic\n");
	if(p_topic->p_domain_participant->p_domain_participant_listener)
		printf("Inconsistent Topic for DomainParticipant\n");

}

/************************************************************************/
/* DataWriter Listener 함수 정의                                        */
/************************************************************************/
void on_offered_deadline_missed(DataWriter* p_datawriter, OfferedDeadlineMissedStatus* p_status)
{
	OfferedDeadlineMissedStatus* temp_status;

	temp_status = (OfferedDeadlineMissedStatus*)calloc(1, sizeof(OfferedDeadlineMissedStatus));

	if(p_datawriter->p_datawriter_listener)
		printf("Offered Deadline missed\n");
	if(p_datawriter->p_publisher->p_publisher_listener)
		printf("Offered Deadline missed for Publisher\n");
	if(p_datawriter->p_publisher->p_domain_participant->p_domain_participant_listener)
		printf("Offered Deadline missed for DomainParticipant\n");

	p_datawriter->get_offered_deadline_missed_status(p_datawriter,temp_status);

	printf("Total count = %d\n",temp_status->total_count);
	printf("Last Instance handle = %d\n",temp_status->last_instance_handle);
	printf("Total count change = %d\n",temp_status->total_count_change);

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);

	FREE(temp_status);
}
void on_offered_incompatible_qos(in_dds DataWriter* p_datawriter,in_dds OfferedIncompatibleQosStatus* p_status)
{
	OfferedIncompatibleQosStatus* temp_status;

	temp_status = (OfferedIncompatibleQosStatus*)calloc(1, sizeof(OfferedIncompatibleQosStatus));

	if(p_datawriter->p_datawriter_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);
	if(p_datawriter->p_publisher->p_publisher_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);
	if(p_datawriter->p_publisher->p_domain_participant->p_domain_participant_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);

	p_datawriter->get_offered_incompatible_qos_status(p_datawriter, temp_status);

	printf("Total count = %d\n",temp_status->total_count);
	printf("Last Instance handle = %d\n",temp_status->last_policy_id);
	printf("Total count change = %d\n",temp_status->total_count_change);
	printf("Policy id of policies = %d\n",temp_status->policies->policy_id);
	printf("Count of policies = %d\n",temp_status->policies->count);

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);

	FREE(temp_status);
}
void on_liveliness_lost(DataWriter* p_datawriter, LivelinessLostStatus* p_status)
{
	if(p_datawriter->p_datawriter_listener)
		printf("Offered Liveliness Lost missed\n");
	if(p_datawriter->p_publisher->p_publisher_listener)
		printf("Offered Liveliness Lost for Publisher\n");
	if(p_datawriter->p_publisher->p_domain_participant->p_domain_participant_listener)
		printf("Offered Liveliness Lost for DomainParticipant\n");

	printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);

	printf("### %s:%d\t ########## \r\n", __FUNCTION__, __LINE__);
}

void on_publication_matched(DataWriter *p_datawriter,PublicationMatchedStatus *p_status)
{
	if(p_datawriter->p_datawriter_listener)
		printf("Offered Publication Matched\n");
	if(p_datawriter->p_publisher->p_publisher_listener)
		printf("Offered Publication Matched for Publisher\n");
	if(p_datawriter->p_publisher->p_domain_participant->p_domain_participant_listener)
		printf("Offered Publication Matched for DomainParticipant\n");

	printf("### total_count[%3d (%+d) (%+d) (%+d) (%+d)] \r\n", p_status->current_count, p_status->current_count_change, p_status->last_subscription_handle, p_status->total_count, p_status->total_count_change);

	printf("### %s:%d\t ########## \r\n", __FUNCTION__, __LINE__);
}

/************************************************************************/
/* Subscirber Listener 함수 정의                                        */
/************************************************************************/
void on_data_on_readers(Subscriber *p_subscriber)
{
	int i = 0;

	for(i=0;i<p_subscriber->i_datareaders;i++)
	{
		FooDataReader* p_foo_datareader = (FooDataReader*)p_subscriber->pp_datareaders[i];
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
	}

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);
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

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);
}
void on_requested_deadline_missed(DataReader* p_datareader, RequestedDeadlineMissedStatus* status)
{
	RequestedDeadlineMissedStatus* temp_status;

	temp_status = (RequestedDeadlineMissedStatus*)calloc(1, sizeof(RequestedDeadlineMissedStatus));

	if(p_datareader->p_subscriber->p_domain_participant->p_domain_participant_listener)
		printf("Requested Deadline missed for Participant\n");

	if(p_datareader->p_datareader_listener)
		printf("Requested Deadline missed\n");

	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("Requested Deadline missed for Subscriber\n");

	p_datareader->get_requested_deadline_missed_status(p_datareader, temp_status);

	printf("Total count = %d\n",temp_status->total_count);
	printf("Last Instance handle = %d\n",temp_status->last_instance_handle);
	printf("Total count change = %d\n",temp_status->total_count_change);

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);

	FREE(temp_status);
}
void on_requested_incompatible_qos(in_dds DataReader* p_datareader, in_dds RequestedIncompatibleQosStatus* p_status)
{
	RequestedIncompatibleQosStatus* temp_status;

	temp_status = (RequestedIncompatibleQosStatus*)calloc(1, sizeof(RequestedIncompatibleQosStatus));

	if(p_datareader->p_datareader_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("\r\n########## %s:%d, event triggered ...[%d]\r\n\r\n", __FUNCTION__, __LINE__, p_status->last_policy_id);

	p_datareader->get_requested_incompatible_qos_status(p_datareader, temp_status);

	printf("Total count = %d\n",temp_status->total_count);
	printf("Last Instance handle = %d\n",temp_status->last_policy_id);
	printf("Total count change = %d\n",temp_status->total_count_change);
	printf("Policy id of policies = %d\n",temp_status->policies->policy_id);
	printf("Count of policies = %d\n",temp_status->policies->count);

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);

	FREE(temp_status);
}

void on_liveliness_changed(DataReader* p_datareader, LivelinessChangedStatus* p_status)
{
	if(p_datareader->p_datareader_listener)
		printf("### alive_count[%3d (%+d)], not_alive_count[%3d (%+d)], last_handle[%d] \r\n", p_status->alive_count, p_status->alive_count_change, p_status->not_alive_count, p_status->not_alive_count_change, p_status->last_publication_handle);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### alive_count[%3d (%+d)], not_alive_count[%3d (%+d)], last_handle[%d] \r\n", p_status->alive_count, p_status->alive_count_change, p_status->not_alive_count, p_status->not_alive_count_change, p_status->last_publication_handle);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### alive_count[%3d (%+d)], not_alive_count[%3d (%+d)], last_handle[%d] \r\n", p_status->alive_count, p_status->alive_count_change, p_status->not_alive_count, p_status->not_alive_count_change, p_status->last_publication_handle);

//	printf("### alive_count[%3d (%+d)], not_alive_count[%3d (%+d)], last_handle[%d] \r\n", p_status->alive_count, p_status->alive_count_change, p_status->not_alive_count, p_status->not_alive_count_change, p_status->last_publication_handle);

	printf("### %s:%d\t ########## \r\n", __FUNCTION__, __LINE__);
}

void on_sample_lost(DataReader* p_datareader, SampleLostStatus* p_status)
{
	if(p_datareader->p_datareader_listener)
		printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);

//	printf("### total_count[%3d (%+d)] \r\n", p_status->total_count, p_status->total_count_change);

	printf("### %s:%d\t########## \r\n", __FUNCTION__, __LINE__);
}

void on_sample_rejected(DataReader* p_datareader, SampleRejectedStatus* p_status)
{
	if(p_datareader->p_datareader_listener)
		printf("### total_count[%3d (%+d)], reaseon:[%2d], last_handle:[%d] \r\n", p_status->total_count, p_status->total_count_change, p_status->last_reason, p_status->last_instance_handle);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d)], reaseon:[%2d], last_handle:[%d] \r\n", p_status->total_count, p_status->total_count_change, p_status->last_reason, p_status->last_instance_handle);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d)], reaseon:[%2d], last_handle:[%d] \r\n", p_status->total_count, p_status->total_count_change, p_status->last_reason, p_status->last_instance_handle);

//	printf("### total_count[%3d (%+d)], reaseon:[%2d], last_handle:[%d] \r\n", p_status->total_count, p_status->total_count_change, p_status->last_reason, p_status->last_instance_handle);

	printf("### %s:%d\t \r\n", __FUNCTION__, __LINE__);
}

void on_subscription_matched(DataReader *p_datareader,SubscriptionMatchedStatus *p_status)
{
	if(p_datareader->p_datareader_listener)
		printf("### total_count[%3d (%+d) (%+d) (%+d) (%+d)] \r\n", p_status->current_count, p_status->current_count_change, p_status->last_publication_handle, p_status->total_count, p_status->total_count_change);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d) (%+d) (%+d) (%+d)] \r\n", p_status->current_count, p_status->current_count_change, p_status->last_publication_handle, p_status->total_count, p_status->total_count_change);
	if(p_datareader->p_subscriber->p_subscriber_listener)
		printf("### total_count[%3d (%+d) (%+d) (%+d) (%+d)] \r\n", p_status->current_count, p_status->current_count_change, p_status->last_publication_handle, p_status->total_count, p_status->total_count_change);

//	printf("### total_count[%3d (%+d) (%+d) (%+d) (%+d)] \r\n", p_status->current_count, p_status->current_count_change, p_status->last_publication_handle, p_status->total_count, p_status->total_count_change);

	printf("### %s:%d\t ########## \r\n", __FUNCTION__, __LINE__);
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
	Publisher* p_publisher = NULL;
	PublisherListener* p_publisher_listener = NULL;

	FooTypeSupport* p_foo_typesupport = get_new_foo_type_support();

	Topic* p_topic;
	TopicListener* p_topic_listener = NULL;

	DataWriterQos datawriter_qos;
	FooDataWriter* p_datawriter = NULL;
	DataWriterListener* p_datawriter_listener = NULL;

	//전송 time
	int32_t sleep_time = 1000000;

	ShapeType data;
	int32_t j = 0;
	Foo* p_foo;

	//DomainParticipantFactory 생성
	p_domainparticipant_factory = DomainParticipantFactory_get_instance();

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, NULL, NULL, ALL_ENABLE_STATUS);

	//Publisher QoS 설정
	p_domainparticipant->get_default_publisher_qos(&publisher_qos);

	p_publisher_listener = (PublisherListener*)calloc(1, sizeof(PublisherListener));
	p_publisher_listener->on_offered_deadline_missed = on_offered_deadline_missed;
	p_publisher_listener->on_offered_incompatible_qos = on_offered_incompatible_qos;
	p_publisher_listener->on_liveliness_lost = on_liveliness_lost;
	p_publisher_listener->on_publication_matched = on_publication_matched;

	//Publisher 생성
	p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &publisher_qos, p_publisher_listener, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);							
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant, "Square", "ShapeType", NULL ,NULL, ALL_ENABLE_STATUS);

	p_topic_listener = (TopicListener*)calloc(1, sizeof(TopicListener));
	p_topic_listener->on_inconsistent_topic = on_inconsistent_topic;
	
	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&datawriter_qos);

	datawriter_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	datawriter_qos.deadline.period.sec = 1;

	p_datawriter_listener = (DataWriterListener*)calloc(1, sizeof(DataWriterListener));
	p_datawriter_listener->on_offered_deadline_missed = on_offered_deadline_missed;
	p_datawriter_listener->on_offered_incompatible_qos = on_offered_incompatible_qos;
	p_datawriter_listener->on_liveliness_lost = on_liveliness_lost;
	p_datawriter_listener->on_publication_matched = on_publication_matched;

	//DataWriter 생성
	p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &datawriter_qos, p_datawriter_listener, ALL_ENABLE_STATUS);

	///////////////////////////////////////////////////////////////////////////

	data.shapesize = 30;
	p_foo = (Foo *)&data;

	while (!b_is_quit && !is_quit())
	{
		int32_t i = 0;

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
	DomainParticipantListener* p_domainparticipant_listener = NULL;

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

	p_domainparticipant_listener = (DomainParticipantListener*)calloc(1, sizeof(DomainParticipantListener));
	p_domainparticipant_listener->on_requested_deadline_missed = on_requested_deadline_missed;

	//Domain Participant 생성
	p_domainparticipant = p_domainparticipant_factory->create_participant(domain_id, &domainparticipant_qos, p_domainparticipant_listener, ALL_ENABLE_STATUS);

	//Subscriber QoS 설정
	p_domainparticipant->get_default_subscriber_qos(&subscriber_qos);

	//Subscriber Listener 설정
	p_subscriber_listener = (SubscriberListener*)calloc(1, sizeof(SubscriberListener));
	p_subscriber_listener->on_data_available = on_data_available;
	p_subscriber_listener->on_liveliness_changed = on_liveliness_changed;
	p_subscriber_listener->on_requested_deadline_missed = on_requested_deadline_missed;
	p_subscriber_listener->on_requested_incompatible_qos = on_requested_incompatible_qos;
	p_subscriber_listener->on_sample_lost = on_sample_lost;
	p_subscriber_listener->on_sample_rejected = on_sample_rejected;
	p_subscriber_listener->on_subscription_matched = on_subscription_matched;
	p_subscriber_listener->on_data_on_readers = on_data_on_readers;


	//Subscriber 생성
//	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, p_subscriber_listener, DATA_AVAILABLE_STATUS);
//	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, p_subscriber_listener, DATA_ON_READERS_STATUS);
	p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &subscriber_qos, p_subscriber_listener, ALL_ENABLE_STATUS);

	//Topic type register
	insert_ShapeType_Parameters(p_foo_typesupport);
	p_foo_typesupport->i_size = sizeof(ShapeType);
	p_foo_typesupport->register_type(p_foo_typesupport, p_domainparticipant, "ShapeType");

	//Topic 생성
	p_topic = p_domainparticipant->create_topic(p_domainparticipant,"Square", "ShapeType", &topic_qos, NULL, ALL_ENABLE_STATUS);

	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&datareader_qos);

	datareader_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	datareader_qos.deadline.period.sec = 1;

	//DataReader Listener 설정
	p_datareader_listener = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener->on_data_available = on_data_available;
	p_datareader_listener->on_liveliness_changed = on_liveliness_changed;
	p_datareader_listener->on_requested_deadline_missed = on_requested_deadline_missed;
	p_datareader_listener->on_requested_incompatible_qos = on_requested_incompatible_qos;
	p_datareader_listener->on_sample_lost = on_sample_lost;
	p_datareader_listener->on_sample_rejected = on_sample_rejected;
	p_datareader_listener->on_subscription_matched = on_subscription_matched;
	///////////////

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
