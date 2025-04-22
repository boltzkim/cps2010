package kr.re.etri.cps;

import java.math.BigInteger;

import kr.re.etri.cps.jni.*;

public class cps_writer extends Thread{
	


	@Override
	public void run() {
		
		int mask = 0xffffffff;
		DomainParticipantQos domainparticipantqos = new DomainParticipantQos();
		PublisherQos publisherQos = new PublisherQos();
		DomainParticipantListener2 domainParticipantListener = new DomainParticipantListener2();
		PublisherListener2 publisherListener = new PublisherListener2();
		TopicQos topicQos = new TopicQos();
		TopicListener2 topicListener = new TopicListener2();
		DataWriterQos dataWriterQos = new DataWriterQos();
		int domain_id = 0;

		DataWriterListener2 datawriterListener = new DataWriterListener2();

		DomainParticipantFactory2 dpf = DomainParticipantFactory2.get_instance();

		dpf.get_default_participant_qos(domainparticipantqos);

		DomainParticipant2 participant = dpf.create_participant(domain_id, domainparticipantqos, domainParticipantListener, mask);

		participant.get_default_publisher_qos(publisherQos);

		Publisher2 publisher = participant.create_publisher(publisherQos,publisherListener,mask);

		FooTypeSupport2 fooTS1 = new FooTypeSupport2();

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

		participant.get_default_topic_qos(topicQos);
		Topic2 topic = participant.create_topic("Square","ShapeType",topicQos,topicListener,mask);


		publisher.get_default_datawriter_qos(dataWriterQos);



		FooDataWriter2 datawriter = publisher.create_datawriter(topic,dataWriterQos,datawriterListener,0);



		Foo2 foo3 = new Foo2(fooTS1);
		

		String testParam = "testParam";
		foo3.setString("color", "test");
		int j=0;
		while(true)
		{
			
			int i=0;
			j++;


			foo3.setString("color", "CYAN");
			

			for(i=2; i < 100; i++){

				
				foo3.setByte("byte", (byte)99);
				foo3.setBoolean("bool", true);
				
				foo3.setInt8("x8", (byte)i);
				foo3.setInt16("x16", (short)(i^2));
				foo3.setInt32("x32", (int)(i^3));
				foo3.setInt64("x64", (long)(i^4));
				
				foo3.setUint8("y8", (short)j);
				foo3.setUint16("y16", (int)(j^2));
				foo3.setUint32("y32", (long)(j^3));
				BigInteger _big = new BigInteger(""+(j^4));
				foo3.setUint64("y64", _big);
				
				
				foo3.setInt32("ShapeType", 30);
				foo3.setString("testParam", testParam+i);


				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				//System.out.println("write..."+i);
				datawriter.write(foo3, 0);
			}
		}
	}

}
