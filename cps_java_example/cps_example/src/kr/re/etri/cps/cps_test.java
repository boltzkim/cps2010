package kr.re.etri.cps;

import java.util.Map;

import kr.re.etri.cps.jni.*;

public class cps_test {

	public static void main(String[] args) {

		// DomainParticipantFactory2 dpf =
		// DomainParticipantFactory2.get_instance();

		cps_reader reader = new cps_reader();
		cps_writer writer = new cps_writer();

		writer.start();
		reader.start();

		while (true) {
			try {
				Thread.sleep(1);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}

}
