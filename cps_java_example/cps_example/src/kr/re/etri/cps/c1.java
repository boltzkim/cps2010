package kr.re.etri.cps;

import java.math.BigInteger;

import kr.re.etri.cps.jni.DataReader;
import kr.re.etri.cps.jni.DataReaderListenerCallBack;
import kr.re.etri.cps.jni.DataWriter;
import kr.re.etri.cps.jni.Foo2;
import kr.re.etri.cps.jni.FooDataReader2;
import kr.re.etri.cps.jni.FooSeq2;
import kr.re.etri.cps.jni.FooTypeSupport2;
import kr.re.etri.cps.jni.InconsistentTopicStatus;
import kr.re.etri.cps.jni.LivelinessChangedStatus;
import kr.re.etri.cps.jni.LivelinessLostStatus;
import kr.re.etri.cps.jni.Message2;
import kr.re.etri.cps.jni.OfferedDeadlineMissedStatus;
import kr.re.etri.cps.jni.OfferedIncompatibleQosStatus;
import kr.re.etri.cps.jni.PublicationMatchedStatus;
import kr.re.etri.cps.jni.RequestedDeadlineMissedStatus;
import kr.re.etri.cps.jni.RequestedIncompatibleQosStatus;
import kr.re.etri.cps.jni.SUPPORT_TYPE;
import kr.re.etri.cps.jni.SampleInfoSeq2;
import kr.re.etri.cps.jni.SampleLostStatus;
import kr.re.etri.cps.jni.SampleRejectedStatus;
import kr.re.etri.cps.jni.Subscriber;
import kr.re.etri.cps.jni.SubscriptionMatchedStatus;
import kr.re.etri.cps.jni.Topic;

public class c1 extends DataReaderListenerCallBack
{

	@Override
	public void on_inconsistent_topic(Topic the_topic,
			InconsistentTopicStatus status) {
		System.out.println("on_inconsistent_topic");
	}

	@Override
	public void on_offered_deadline_missed(DataWriter writer,
			OfferedDeadlineMissedStatus status) {
		System.out.println("on_offered_deadline_missed");
	}

	@Override
	public void on_offered_incompatible_qos(DataWriter writer,
			OfferedIncompatibleQosStatus status) {
		System.out.println("on_offered_incompatible_qos");
	}

	@Override
	public void on_liveliness_lost(DataWriter writer,
			LivelinessLostStatus status) {
		System.out.println("on_liveliness_lost");
	}

	@Override
	public void on_publication_matched(DataWriter writer,
			PublicationMatchedStatus status) {
		System.out.println("on_publication_matched");
	}

	@Override
	public void on_requested_deadline_missed(DataReader the_reader,
			RequestedDeadlineMissedStatus status) {
		System.out.println("on_requested_deadline_missed");
	}

	@Override
	public void on_requested_incompatible_qos(DataReader the_reader,
			RequestedIncompatibleQosStatus status) {
		System.out.println("on_requested_incompatible_qos");
	}

	@Override
	public void on_sample_rejected(DataReader the_reader,
			SampleRejectedStatus status) {
		System.out.println("on_sample_rejected");
	}

	@Override
	public void on_liveliness_changed(DataReader the_reader,
			LivelinessChangedStatus status) {
		System.out.println("on_liveliness_changed");
	}

	int count = 0;

	FooDataReader2 datareader;
	FooTypeSupport2 fooTS1;
	
	FooSeq2 fseq = new FooSeq2();
	SampleInfoSeq2 sSeq = new SampleInfoSeq2();
	
	
	@Override
	public void on_data_available(DataReader p_reader) {
		
		//System.out.println("["+count+"]on_data_available");
		
		datareader.read(fseq, sSeq, 1, 0xffff, 0xffff, 0xffff);
		
		if(fseq.has_data())
		{
		
			Message2 message = fseq.getMessage();
			Foo2 foo3 = new Foo2(fooTS1, message.getValue());

			String color = foo3.getString("color");

			// 자바에서는 unsigned를 지원 안하기 때문에, 크기가 더 큰 자료형을 사용한다. short형은 int형으로..
			byte byte1 = foo3.getByte("byte");
			boolean bool1 = foo3.getBoolean("bool");
			
			byte x8 = foo3.getInt8("x8");
			short x16 = foo3.getInt16("x16");
			int x32 = foo3.getInt32("x32");
			long x64 = foo3.getInt64("x64");
			
			short y8 = foo3.getUint8("y8");
			int y16 = foo3.getUint16("y16");
			long y32 = foo3.getUint32("y32");
			BigInteger y64 = foo3.getUint64("y64");
			
			int shapesize = foo3.getInt32("ShapeType");
			String testParam = foo3.getString("testParam");
			
			
			
			System.out.println("["+byte1+":"+bool1+"]"+"["+count+"]color : "+color+", x8 : "+x8+", x16 : "+x16+", x32 : "+x32+", x64 : "+x64+", y8 : "+y8+", y16 : "+y16+", y32 : "+y32+", y64 : "+y64+", shapetype : "+shapesize+", test : "+testParam);
			
			
			message.removeMessage();
			
		}
		count++;
		
	}

	@Override
	public void on_subscription_matched(DataReader the_reader,
			SubscriptionMatchedStatus status) {
		System.out.println("on_subscription_matched");
	}

	@Override
	public void on_sample_lost(DataReader the_reader,
			SampleLostStatus status) {
		System.out.println("on_sample_lost");
	}

	@Override
	public void on_data_on_readers(Subscriber p_subscriber) {
		System.out.println("on_data_on_readers");
	}
	
}