#include <core.hh>

using namespace cps;


typedef struct ShapeType
{
	dds_string color;
	int32_t x;
	int32_t y;
	int32_t shapesize;
}ShapeType;

static bool b_is_quit = false;

static DataWriterListener2 datawriterListener;
static DomainId_t domain_id = 0;

static void on_publication_matched(DataWriter* the_writer, PublicationMatchedStatus* status){
  printf("Called on_publication_matched\r\n");
}


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



int cccc = 0;

#include <mutex>


std::mutex m;


void TopicWrite( void *dummy )
{
	m.lock();
	int ii = *(int*)dummy;
	StatusMask mask = 0xffffffff;
	DomainParticipantQos domainparticipantqos;
	PublisherQos publisherQos;
	DomainParticipantListener2 domainParticipantListener;
	PublisherListener2 publisherListener;
	TopicQos topicQos;
	TopicListener2 topicListener;
	DataWriterQos dataWriterQos;
	WaitSet2 waitset;
	int32_t sleep_time = 50000 * 20;



	DomainParticipantFactory2 dpf = DomainParticipantFactory2::get_instance();

	datawriterListener.setOn_publication_matched(on_publication_matched);

	dpf.get_default_participant_qos(domainparticipantqos);

	DomainParticipant2 participant = dpf.create_participant(domain_id, domainparticipantqos, domainParticipantListener, mask);

	participant.get_default_publisher_qos(publisherQos);

	Publisher2 publisher = participant.create_publisher(publisherQos,publisherListener,mask);

	FooTypeSupport2 fooTS1;

	fooTS1.insert_parameter("color", DDS_STRING_TYPE);
	fooTS1.insert_parameter("x", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("y", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("shapesize", DDS_INTEGER32_TYPE);

	//fooTS1.insert_parameter("byte", DDS_BYTE_TYPE);
	//fooTS1.insert_parameter("bool", DDS_BOOLEAN_TYPE);
	//fooTS1.insert_parameter("x8", DDS_INTEGER8_TYPE);
	//fooTS1.insert_parameter("x16", DDS_INTEGER16_TYPE);
	//fooTS1.insert_parameter("x32", DDS_INTEGER32_TYPE);
	//fooTS1.insert_parameter("x64", DDS_INTEGER64_TYPE);
	//fooTS1.insert_parameter("y8", DDS_UINTEGER8_TYPE);
	//fooTS1.insert_parameter("y16", DDS_UINTEGER16_TYPE);
	//fooTS1.insert_parameter("y32", DDS_UINTEGER32_TYPE);
	//fooTS1.insert_parameter("y64", DDS_UINTEGER64_TYPE);
	//fooTS1.insert_parameter("ShapeType", DDS_INTEGER32_TYPE);
	//fooTS1.insert_parameter("testParam", DDS_STRING_TYPE);

	//fooTS1.setFootypeSize(sizeof(ShapeType));
	std::string type = "ShapeType" + ii;
	std::string topicname = "Square" + ii;
	fooTS1.register_type(participant, (char*)type.c_str());

	participant.get_default_topic_qos(topicQos);
	Topic2 topic = participant.create_topic((char*)topicname.c_str(), (char*)type.c_str(),topicQos,topicListener,mask);


	publisher.get_default_datawriter_qos(dataWriterQos);

	dataWriterQos.history.depth = 1;
	dataWriterQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;

	msleep(100000);

	FooDataWriter2 datawriter = publisher.create_datawriter(topic,dataWriterQos,datawriterListener,0);


	Foo2 foo3(fooTS1);

	int j=0;


	m.unlock();

	while(!b_is_quit && !is_quit())
	{
		
		j++;
		int32_t i=0;
		ConditionSeq *p_temp_Condition = NULL;
#ifdef START_IGNORE
		InstanceHandleSeq InstanceHandleSeq = INIT_INSTANCESEQ;
#endif



		foo3.setString("color", "CYAN");
		foo3.setInt32("shapesize",  20);

		for(i=2; i < 100; i++){

			foo3.setInt32("x", cccc);
			foo3.setInt32("y", ii);
			

			/*foo3.setBoolean("bool",true);

			foo3.setInt8("x8", i);
			foo3.setInt16("x16", i^2);
			foo3.setInt32("x32", i^3);
			foo3.setInt64("x64", i^4);

			foo3.setUint8("y8", j);
			foo3.setUint16("y16", j^2);
			foo3.setUint32("y32", j^3);
			foo3.setUint64("y64", j^4);

			foo3.setInt32("ShapeType", 30);
			foo3.setString("testParam", "testParam");*/

		
		
			msleep(sleep_time);

			if(is_quit()) break;

			//DataWrite
			datawriter.write(foo3, 0);
		}
	}

	foo3.removeFoo();

}



void on_data_available(in_dds DataReader *p_reader)
{

}
FooTypeSupport2 fooTS1;

class c1 : public DataReaderListenerCallBack
{
	virtual void on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)
	{
	}
	virtual void on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)
	{
	}
	virtual void on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)
	{
	}
	virtual void on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)
	{
	}
	virtual void on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)
	{
	}
	virtual void on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)
	{
	}
	virtual void on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)
	{
	}
	virtual void on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)
	{
	}
	virtual void on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)
	{
	}
	virtual void on_data_available(in_dds DataReader *p_reader)
	{

		FooSeq2 fseq;
		SampleInfoSeq2 sSeq;

		FooDataReader2 datareader((FooDataReader *)p_reader);
		datareader.read(fseq, sSeq,  1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if(fseq.has_data())
		{
			int i=0;
			int totla_size = fseq.getSize();

			while(fseq.has_data()){
				Message2 message = fseq.getMessage();
				Foo2 foo3(fooTS1, message.getValue());

				string color = foo3.getString("color");
				int32_t x = foo3.getInt32("x");
				int32_t y = foo3.getInt32("y");
				int32_t shapesize = foo3.getInt32("shapesize");

				//int8_t byte = foo3.getByte("byte");
				/*bool bool1 = foo3.getBoolean("bool");

				string color = foo3.getString("color");
				int8_t x8 = foo3.getInt8("x8");
				int16_t x16 = foo3.getInt16("x16");
				int32_t x32 = foo3.getInt32("x32");
				int64_t x64 = foo3.getInt64("x64");

				uint8_t y8 = foo3.getUint8("y8");
				uint16_t y16 = foo3.getUint16("y16");
				uint32_t y32 = foo3.getUint32("y32");
				uint64_t y64 = foo3.getUint64("y64");

				int shapesize = foo3.getInt32("ShapeType");
				string testParam = foo3.getString("testParam");*/

				//memcpy(&shapeData, message.getValue(), sizeof(ShapeType));

				if (totla_size-1 == i){
					printf("\ncolor[%d] : %s, x : %d, y: %d, shapesize : %d\n", i, color, x, y, shapesize);
					//printf("byte : %d\r\n", byte);
					//printf("\ncolor[%d] : %s, x8 : %d, x16 : %d, x32 : %d, x64 : %I64d, y8 : %d, y16 : %d, y32 : %d, y64 : %I64u, shapesize : %d, test:%s\n", i, color, x8, x16, x32, x64, y8, y16, y32, y64, shapesize, testParam);
				}
					i++;

					message.removeMessage();
			}
		}
	}
	virtual void on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)
	{
	}
	virtual void on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)
	{
	}
	virtual void on_data_on_readers(in_dds Subscriber *p_subscriber)
	{
	}
};



void TopicRead(void *dummy)
{

	m.lock();
	int ii = *(int*)dummy;
	DomainParticipantFactory2 dpf = DomainParticipantFactory2::get_instance();
	DomainId_t domain_id = 0;
	StatusMask mask = 0xffffffff;
	DomainParticipantQos domainparticipantqos;
	DomainParticipantListener2 domainParticipantListener;
	SubscriberQos subscriberQos;
	SubscriberListener2 subscriberListener;
	TopicQos topicQos;
	TopicListener2 topicListener;
	DataReaderQos dataReaderQos;
	DataReaderListener2 dataReaderListener;
	WaitSet2 waitset;
	ShapeType data;
	ShapeType datakey;


	dpf.get_default_participant_qos(domainparticipantqos);
	DomainParticipant2 participant = dpf.create_participant(domain_id, domainparticipantqos, domainParticipantListener, mask);

	participant = dpf.lookup_participant(domain_id);


	participant.get_default_subscriber_qos(subscriberQos);
	Subscriber2 subscriber = participant.create_subscriber(subscriberQos, subscriberListener, mask);

	subscriber.set_listener(subscriberListener, REQUESTED_DEADLINE_MISSED_STATUS);


	std::string type = "ShapeType" + ii;
	std::string topicname = "Square" + ii;

	participant.get_default_topic_qos(topicQos);
	Topic2 topic = participant.create_topic((char*)topicname.c_str(), (char*)type.c_str(),topicQos,topicListener,mask);


	
	fooTS1.insert_parameter("color", DDS_STRING_TYPE);
	fooTS1.insert_parameter("x", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("y", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("shapesize", DDS_INTEGER32_TYPE);

	 
	//fooTS1.insert_parameter("color", DDS_STRING_TYPE, true);
	//fooTS1.insert_parameter("byte", DDS_BYTE_TYPE);
	/*fooTS1.insert_parameter("bool", DDS_BOOLEAN_TYPE);
	fooTS1.insert_parameter("x8", DDS_INTEGER8_TYPE);
	fooTS1.insert_parameter("x16", DDS_INTEGER16_TYPE);
	fooTS1.insert_parameter("x32", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("x64", DDS_INTEGER64_TYPE);
	fooTS1.insert_parameter("y8", DDS_UINTEGER8_TYPE);
	fooTS1.insert_parameter("y16", DDS_UINTEGER16_TYPE);
	fooTS1.insert_parameter("y32", DDS_UINTEGER32_TYPE);
	fooTS1.insert_parameter("y64", DDS_UINTEGER64_TYPE);
	fooTS1.insert_parameter("ShapeType", DDS_INTEGER32_TYPE);
	fooTS1.insert_parameter("testParam", DDS_STRING_TYPE);*/

	//fooTS1.setFootypeSize(sizeof(ShapeType));
	fooTS1.register_type(participant, (char*)type.c_str());

	subscriber.get_default_datareader_qos(dataReaderQos);

	dataReaderQos.history.depth = 10;
	dataReaderQos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;

	dataReaderListener.setCallback(new c1());

	

	FooDataReader2 datareader = subscriber.create_datareader(topic, dataReaderQos, dataReaderListener, mask);


	//datareader = subscriber.create_datareader(topic, dataReaderQos, dataReaderListener, mask);

	ConditionSeq2 condseq;
	Duration_t waittime = {100,100};


	FooSeq2 fseq;
	SampleInfoSeq2 sSeq;
	ShapeType shapeData;


	m.unlock();

	while(!b_is_quit && !is_quit())
	{

		

		msleep(100);
	}

}

void main(int argc, char **argv)
{
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	if (argc >= 2)
	{
		cccc = atoi(argv[1]);
	}


	printf("cccc = %d\r\n", cccc);

	for (int i = 0; i < 100; i++)
	{
		_beginthread( TopicWrite, 0, (void*)&i);
		_beginthread( TopicRead, 0, (void*)&i);
	}


#ifdef _MSC_VER
	//Writer Thread
	//_beginthread( TopicWrite, 0, NULL );

	////Reader Thread
	//_beginthread( TopicRead, 0, NULL );
#else
	pthread_create( &thread_id, NULL, (void *)TopicWrite, NULL );
	pthread_create( &thread_id, NULL, (void *)TopicRead, NULL );
#endif

	getchar();
	printf("\r\n\r\nIf you want to quit. please Enter any key.33\r\n");
	getchar();


	quit();

	getchar();
	getchar();
}

