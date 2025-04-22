package kr.re.etri.cps;

import kr.re.etri.cps.jni.*;

public class cps_reader extends Thread{
	
	
	FooDataReader2 datareader;
	FooTypeSupport2 fooTS1;
	
	FooSeq2 fseq = new FooSeq2();
	SampleInfoSeq2 sSeq = new SampleInfoSeq2();
	
	public static c1 callback = new c1();
	
	
	@Override
	public void run() {
		DomainParticipantFactory2 dpf = DomainParticipantFactory2.get_instance();
		int domain_id = 0;
		int mask = 0xffffffff;
		DomainParticipantQos domainparticipantqos = new DomainParticipantQos();
		DomainParticipantListener2 domainParticipantListener = new DomainParticipantListener2();
		SubscriberQos subscriberQos = new SubscriberQos();
		SubscriberListener2 subscriberListener = new SubscriberListener2();
		TopicQos topicQos = new TopicQos();
		TopicListener2 topicListener = new TopicListener2();
		DataReaderQos dataReaderQos = new DataReaderQos();
		DataReaderListener2 dataReaderListener= new DataReaderListener2();
		WaitSet2 waitset = new WaitSet2();



		dpf.get_default_participant_qos(domainparticipantqos);
		DomainParticipant2 participant = dpf.create_participant(domain_id, domainparticipantqos, domainParticipantListener, mask);

		participant = dpf.lookup_participant(domain_id);


		participant.get_default_subscriber_qos(subscriberQos);
		Subscriber2 subscriber = participant.create_subscriber(subscriberQos, subscriberListener, mask);

		//System.out.println("1 read");

		participant.get_default_topic_qos(topicQos);
		Topic2 topic = participant.create_topic("Square","ShapeType",topicQos,topicListener,mask);


		fooTS1 = new FooTypeSupport2();

		fooTS1.insert_parameter("color", SUPPORT_TYPE.DDS_STRING_TYPE, true);
		fooTS1.insert_parameter("byte", SUPPORT_TYPE.DDS_BYTE_TYPE);
		fooTS1.insert_parameter("bool", SUPPORT_TYPE.DDS_BOOLEAN_TYPE);
		fooTS1.insert_parameter("x8", SUPPORT_TYPE.DDS_INTEGER8_TYPE);
		fooTS1.insert_parameter("x16", SUPPORT_TYPE.DDS_INTEGER16_TYPE);
		fooTS1.insert_parameter("x32", SUPPORT_TYPE.DDS_INTEGER32_TYPE);
		fooTS1.insert_parameter("x64", SUPPORT_TYPE.DDS_INTEGER64_TYPE);
		
		fooTS1.insert_parameter("y8", SUPPORT_TYPE.DDS_UINTEGER8_TYPE);
		fooTS1.insert_parameter("y16", SUPPORT_TYPE.DDS_UINTEGER16_TYPE);
		fooTS1.insert_parameter("y32", SUPPORT_TYPE.DDS_UINTEGER32_TYPE);
		fooTS1.insert_parameter("y64", SUPPORT_TYPE.DDS_UINTEGER64_TYPE);
		
		fooTS1.insert_parameter("ShapeType", SUPPORT_TYPE.DDS_INTEGER32_TYPE);
		fooTS1.insert_parameter("testParam", SUPPORT_TYPE.DDS_STRING_TYPE);

		fooTS1.register_type(participant, "ShapeType");

		subscriber.get_default_datareader_qos(dataReaderQos);
		
		StatusCondition2 statuscodition1 = subscriber.get_statuscondition();
		statuscodition1.set_enabled_statuses(0xFFFF);
		waitset.attach_condition(statuscodition1);
		
		dataReaderListener.setCallback(callback);
		
		HistoryQosPolicy history = new HistoryQosPolicy();
		history.setDepth(10);
		history.setKind(HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS);
		dataReaderQos.setHistory(history);
		
		ReliabilityQosPolicy relia= new ReliabilityQosPolicy();
		relia.setKind(ReliabilityQosPolicyKind.BEST_EFFORT_RELIABILITY_QOS);
		dataReaderQos.setReliability(relia);
		datareader = subscriber.create_datareader(topic, dataReaderQos, dataReaderListener, mask);

		callback.datareader = datareader;
		callback.fooTS1 = fooTS1;

		
		while(true)
		{
			
			
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
