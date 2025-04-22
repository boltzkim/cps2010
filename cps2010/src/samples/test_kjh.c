/*
	이파일은 단지 테스트를 위한 파일이며, 실제 아무의미는 없다.

	작성자 : 

	이력
	2010-07-22 : 시작

	2012-06-18 : 주석 시작 

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


//#define START_IGNORE 1

/*
typedef struct HelloWorld
{
  int a;
  int b;
}HelloWorld;
*/

/*
typedef struct dds_string
{
	int32_t size;
	char value[128];
}dds_string;
*/

typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
}ShapeType;

#ifdef _MSC_VER
static void insert_ShapeType_Parameters(FooTypeSupport *p_foo_typesupport)
{
	p_foo_typesupport->insert_parameter(p_foo_typesupport, "color", DDS_STRING_TYPE, true);
	p_foo_typesupport->insert_parameter(p_foo_typesupport, "x", DDS_INTEGER32_TYPE, false);
	p_foo_typesupport->insert_parameter(p_foo_typesupport, "y", DDS_INTEGER32_TYPE, false);
	p_foo_typesupport->insert_parameter(p_foo_typesupport, "shapesize", DDS_INTEGER32_TYPE, false);
	//I_PARAMETER(fooTS, "x", DDS_INTEGER32_TYPE, sizeof(int32_t));
	//I_PARAMETER(fooTS, "y", DDS_INTEGER32_TYPE, sizeof(int32_t));
	//I_PARAMETER(fooTS, "shapesize", DDS_INTEGER32_TYPE, sizeof(int32_t));
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



static void on_publication_matched(DataWriter* the_writer, PublicationMatchedStatus* status){
  //printf("Called on_publication_matched\r\n");
}


int ccc = 1;
int kkk = 0;
//status by jun
DataWriterListener datawriterListener ;

void TopicWrite( void *dummy )
{
    
	//Attribute 정의
	StatusMask mask = 0xffffffff;
	DomainParticipant *p_Participant = NULL;
	Publisher *p_publisher;
	PublisherQos pqos;
	Topic *p_Topic;
	DomainId_t domain_id = 0;
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

	int num = ccc;
	int num2 = kkk;
	char *topic_name;
	int i_send = 0;




	//DomainParticipantFactory 생성
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();

	//status by jun
	datawriterListener.on_offered_deadline_missed = NULL;
	datawriterListener.on_offered_incompatible_qos = NULL;
	datawriterListener.on_liveliness_lost = NULL;
	datawriterListener.on_publication_matched = on_publication_matched;

	//Participant 생성
	p_Participant = p_dpf->create_participant(domain_id, NULL, NULL, mask);

	//Publisher QoS 설정
	p_Participant->get_default_publisher_qos(&pqos);

	//Publisher 생성
	p_publisher = p_Participant->create_publisher(p_Participant,&pqos,NULL,mask);

	//Topic type register
	insert_ShapeType_Parameters(fooTS1);
	fooTS1->i_size = sizeof(ShapeType);							
	fooTS1->register_type(fooTS1, p_Participant, "ShapeType");

	
	asprintf(&topic_name, "Square%d", ccc);

	//Topic 생성
	p_Topic = p_Participant->create_topic(p_Participant, topic_name,"ShapeType",NULL,NULL,mask);	//Topic Name 수정
	
	//DataWriter QoS 설정
	p_publisher->get_default_datawriter_qos(&a_writerQos);
	a_writerQos.history.depth = 100;
	a_writerQos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	a_writerQos.ownership.kind = SHARED_OWNERSHIP_QOS;

	a_writerQos.durability_service.service_cleanup_delay.sec = 1;
	a_writerQos.durability_service.service_cleanup_delay.nanosec = 1;
	a_writerQos.durability_service.history_kind = KEEP_ALL_HISTORY_QOS;
	a_writerQos.durability_service.history_depth = 2;
	a_writerQos.durability_service.max_samples = 3;
	a_writerQos.durability_service.max_instances = 4;
	a_writerQos.durability_service.max_samples_per_instance = 5;

	//User Data QoS 설정(DataWriter)
//	a_writerQos.user_data.value.i_string = 0;
//	a_writerQos.user_data.value.pp_string = NULL;
//	INSERT_ELEM(a_writerQos.user_data.value.pp_string, a_writerQos.user_data.value.i_string,a_writerQos.user_data.value.i_string, strdup("ABC") );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	//printf("\r\nWrtier get_key_value : key : %s(%ld)\r\n", datakey.color.value, instance_handle1);

	memset(&datakey,0,sizeof(ShapeType));
	p_foodataWriter->get_key_value(p_foodataWriter, (Foo *)&datakey, instance_handle2);
	//printf("\r\nWrtier get_key_value : key : %s(%ld)\r\n", datakey.color.value, instance_handle2);
	

	
	//msleep(3000000);

	while(!b_is_quit && !is_quit())
	{
		
		int32_t i=0;
		ConditionSeq *p_temp_Condition = NULL;
#ifdef START_IGNORE
		InstanceHandleSeq InstanceHandleSeq = INIT_INSTANCESEQ;



		for(i=0; i< condseq.i_seq; i++){

			p_temp_Condition = (ConditionSeq *)condseq.pp_conditions[i];
			if(p_temp_Condition == (ConditionSeq *)p_statusCondition){
				printf("condseq1->i_seq %d \n", condseq.i_seq);

			}
		}




		set_string(&data.color, "CYAN");

		for(i=0; i < 100; i++){

			point(&data, i, j); //point 입력 함수

			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if(is_quit()) break;

			//DataWrite
			p_foodataWriter->write(p_foodataWriter,fFoo, 0);
		}

#endif
		set_string(&data.color, "CYAN");
		data.shapesize = 30;
		//point(&data, ccc, 0); //point 입력 함수

		i_send++;
		data.x = num;
		data.y = i_send;
		p_foodataWriter->write(p_foodataWriter, fFoo, 0);
		//printf("send : %d\r\n", i_send);
		if (is_quit()) break;

		//Sleep(1);
		if (i_send > 2000) Sleep(1);

		if (i_send % 100000 == 0) printf("send data..... %d\r\n", i_send);
		


#ifdef START_IGNORE
		monitoring__warning_condtion(2, "Test Inconsistent topic status", 0, ERROR1, 3001);
		msleep(sleep_time);
		monitoring__warning_condtion(0, "Test Type conflicts (equality comparison)", 0, ERROR1, 1001);
		msleep(sleep_time);
		monitoring__warning_condtion(0, "Test Deadlines missed", 1, WARNING2, 1005);
		msleep(sleep_time);
		monitoring__warning_condtion(1, "Test Inactivated DataReaders", 0, ERROR3, 2003);
		msleep(sleep_time);
		monitoring__warning_condtion(1, "Test Liveliness lost", 1, WARNING1, 2004);
		msleep(sleep_time);


		//////////////////////////////////////////////////////
/*
		p_Participant->get_discovered_participants(p_Participant, &InstanceHandleSeq);

		if(InstanceHandleSeq.i_seq)
		{
			InstanceHandle_t handle = *InstanceHandleSeq.pp_instancehandle[0];
			if(handle > 0)
			{
				ParticipantBuiltinTopicData participant_data;
				ReturnCode_t ret;

				memset(&participant_data, 0, sizeof(ParticipantBuiltinTopicData));
				ret = p_Participant->get_discovered_participant_data(p_Participant, &participant_data, handle);

				if(ret == RETCODE_OK)
				{

				}

				p_Participant->ignore_participant(p_Participant, handle);

			}
		}


		while(InstanceHandleSeq.i_seq)
		{
			FREE(InstanceHandleSeq.pp_instancehandle[0]);
			REMOVE_ELEM(InstanceHandleSeq.pp_instancehandle, InstanceHandleSeq.i_seq, 0);
		}



		//////////////////////////////////////////

		p_Participant->get_discovered_topics(p_Participant, &InstanceHandleSeq);

		if(InstanceHandleSeq.i_seq)
		{
			InstanceHandle_t handle = *InstanceHandleSeq.pp_instancehandle[0];
			if(handle > 0)
			{
				TopicBuiltinTopicData topic_data;
				ReturnCode_t ret;

				memset(&topic_data, 0, sizeof(TopicBuiltinTopicData));
				ret = p_Participant->get_discovered_topic_data(p_Participant, &topic_data, handle);

				if(ret == RETCODE_OK)
				{

				}
				FREE(topic_data.name);
				FREE(topic_data.type_name);

				p_Participant->ignore_topic(p_Participant, handle);

			}
		}


		while(InstanceHandleSeq.i_seq)
		{
			FREE(InstanceHandleSeq.pp_instancehandle[0]);
			REMOVE_ELEM(InstanceHandleSeq.pp_instancehandle, InstanceHandleSeq.i_seq, 0);
		}


		////////////////////////////////////////////
*/
		msleep(3000000);
		p_foodataWriter->get_matched_subscriptions((DataWriter*)p_foodataWriter, &InstanceHandleSeq);

		if(InstanceHandleSeq.i_seq)
		{
			InstanceHandle_t handle = *InstanceHandleSeq.pp_instancehandle[0];
			if(handle > 0)
			{
				SubscriptionBuiltinTopicData subscription_data;
				ReturnCode_t ret;

				memset(&subscription_data, 0, sizeof(SubscriptionBuiltinTopicData));
				ret = p_foodataWriter->get_matched_subscription_data((DataWriter*)p_foodataWriter, &subscription_data, handle);

				if(ret == RETCODE_OK)
				{

				}

				FREE(subscription_data.topic_name);
				FREE(subscription_data.type_name);


//				p_Participant->ignore_subscription(p_Participant, handle);

			}

			
		}


		while(InstanceHandleSeq.i_seq)
		{
			FREE(InstanceHandleSeq.pp_instancehandle[0]);
			REMOVE_ELEM(InstanceHandleSeq.pp_instancehandle, InstanceHandleSeq.i_seq, 0);
		}


		set_string(&data.color, "BLUE");

		for(i=0; i < 100; i++){

			point(&data, i, j); //point 입력 함수

			data.shapesize = 30;

			fFoo = (Foo *)&data;
		
			msleep(sleep_time);

			if(is_quit()) break;

			//DataWrite
			p_foodataWriter->write(p_foodataWriter,fFoo, 0);
		}

		j++;
#endif
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


	//exit(1);

//	p_Participant->delete_publisher(p_Participant,p_publisher);

}

void on_data_available(DataReader* p_datareader);

void TopicRead(void *dummy)
{

	//DomainParticipantFactory 생성
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();

	//Attribute 정의
	DomainId_t domain_id = 0;
	StatusMask mask = 0xffffffff;
	DomainParticipant *p_Participant = NULL;
	Subscriber *p_subscriber = NULL;
	Topic *p_Topic;			

#ifndef _MSC_VER
	pthread_t thread_id;
#endif
	
	FooDataReader *p_datareader;
	
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	StringSeq query_parameters;


// waitset by jun
	StatusCondition *p_statusCondition;
	StatusCondition *p_statusCondition2;

	ReadCondition *p_readCondition;
	QueryCondition *p_queryConditon;
	////////////////////////////////////
	DataReaderQos a_readerQos;

	WaitSet * p_waitset = waitset_new();
	
	Duration_t waittime = {100,100};

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
	char *topic_name;
	DataReaderListener* p_datareader_listener = NULL;
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
	
	//printf("called check key\r\n");

	

	//UserData QoS 설정(DomainParticipant)
	p_dpf->get_default_participant_qos(&p_dpq);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Participant 생성
//	p_Participant = p_dpf->create_participant(domain_id, NULL, NULL, mask);
	p_Participant = p_dpf->create_participant(domain_id, &p_dpq, NULL, mask);

	p_Participant = p_dpf->lookup_participant(domain_id);

	p_Participant->get_default_subscriber_qos(&subscriberQos);

	//Partition QoS 설정
//	INSERT_ELEM(subscriberQos.partition.name.pp_string, subscriberQos.partition.name.i_string, subscriberQos.partition.name.i_string, strdup("ABBsfasf") );
	//Group Data QoS 설정(Subscriber)///////////////////
//	INSERT_ELEM(subscriberQos.group_data.value.pp_string, subscriberQos.group_data.value.i_string, subscriberQos.group_data.value.i_string, strdup("ABBsfasf") );
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Subscriber 생성
	p_subscriber = p_Participant->create_subscriber(p_Participant, &subscriberQos, NULL, mask);
	//listener by jun
	p_subscriber->set_listener(p_subscriber, &subscriberListener, REQUESTED_DEADLINE_MISSED_STATUS);

	//Topic Data QoS 설정(Topic)
	p_Participant->get_default_topic_qos(&a_topicQos);
//	a_topicQos.topic_data.value.i_string = 0;
//	a_topicQos.topic_data.value.pp_string = NULL;		
//	INSERT_ELEM(a_topicQos.topic_data.value.pp_string, a_topicQos.topic_data.value.i_string,a_topicQos.topic_data.value.i_string, strdup("ABC") );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	
	asprintf(&topic_name, "Square%d", ccc);

	//Topic 생성
	//p_Topic = p_Participant->create_topic(p_Participant,"Square","ShapeType",NULL,NULL,mask);
	p_Topic = p_Participant->create_topic(p_Participant, topic_name,"ShapeType",&a_topicQos,NULL,mask);

	//Topic type register
	insert_ShapeType_Parameters(fooTS);
	fooTS->i_size = sizeof(ShapeType);								//Size 입력
	fooTS->register_type(fooTS, p_Participant, "ShapeType");		//Topic Name 입력

	seq.i_string = 0;
	seq.pp_string = NULL;
	
	//DataReader QoS 설정
	p_subscriber->get_default_datareader_qos(&a_readerQos);
	a_readerQos.history.depth = 10;
	a_readerQos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	//a_readerQos.time_based_filter.minimum_separation.sec = 2;


	//User Data QoS 설정(DataReader)
//	a_readerQos.user_data.value.i_string = 0;
//	a_readerQos.user_data.value.pp_string = NULL;		
//	INSERT_ELEM(a_readerQos.user_data.value.pp_string, a_readerQos.user_data.value.i_string,a_readerQos.user_data.value.i_string, strdup("ABC") );
//	INSERT_ELEM(a_readerQos.user_data.value.pp_string, a_readerQos.user_data.value.i_string,a_readerQos.user_data.value.i_string, strdup("DDD") );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// deadline.period.sec가 -1이면 rti하고 통신 못함
	// 체크 부탁...
	//a_readerQos.deadline.period.sec = -1;
	
	p_datareader_listener = (DataReaderListener*)calloc(1, sizeof(DataReaderListener));
	p_datareader_listener->on_data_available = on_data_available;

	//DataReader 생성
	p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_Topic,&a_readerQos, p_datareader_listener,mask);



}


static int i_rec = 0;

void on_data_available(DataReader* p_datareader)
{
	FooDataReader* p_foo_datareader = (FooDataReader*)p_datareader;
	FooSeq foo_seq = INIT_FOOSEQ;
	SampleInfoSeq sample_info_seq = INIT_SAMPLEINFOSEQ;
	ShapeType shape_data;
	message_t* p_message = NULL;

	p_foo_datareader->read(p_foo_datareader, &foo_seq, &sample_info_seq, -1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	while (foo_seq.i_seq)
	{
		p_message = (message_t*)foo_seq.pp_foo[0];
		memcpy(&shape_data, p_message->v_data, sizeof(ShapeType));
		//fprintf(stderr, "\ncolor[%d] : %s, x : %d, y : %d, shapesize : %d\n", foo_seq.i_seq, shape_data.color.value, shape_data.x, shape_data.y, shape_data.shapesize);
		message_release(p_message);
		REMOVE_ELEM(foo_seq.pp_foo, foo_seq.i_seq, 0);

		if (i_rec % 10000 == 0) printf("rec data..... %d\r\n", i_rec);

		i_rec++;
	}
	FREE(sample_info_seq.pp_sample_infos);
	FREE(foo_seq.pp_foo);
}



void main(int agc, char **argv)
{
#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CrtMemState s1; 
	_CrtMemCheckpoint(&s1);
#endif


	if (agc >= 2)
	{
		kkk = atoi(argv[1]);
	}


	kkk = rand();


	printf("kkk = %d\r\n", kkk);

	for (ccc = 0 ; ccc < 1; ccc++)
	{

	
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

	Sleep(500);

	}

	printf("\r\n\r\nIf you want to quit. please Enter any key.33\r\n");
	getchar();

	
	quit();
	
	getchar();
	getchar();
#ifdef _CRTDBG_MAP_ALLOC
	_CrtMemDumpStatistics(&s1);
	_CrtDumpMemoryLeaks();
#endif
	getchar();
}
