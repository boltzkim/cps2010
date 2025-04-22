using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace cps_csharp_example
{

    public class c1 : DataReaderListenerCallBack
    {
        public override void on_data_available(DataReader p_reader)
        {
            
        }
    }

    public partial class Form1 : Form
    {

        FooDataReader2 datareader;
        FooTypeSupport2 fooTS1;

        FooSeq2 fseq = new FooSeq2();
        SampleInfoSeq2 sSeq = new SampleInfoSeq2();
        DomainParticipantFactory2 dpf;
        DomainParticipantQos domainparticipantqos;

        DomainParticipantListener2 domainParticipantListener;
        SubscriberQos subscriberQos;
        SubscriberListener2 subscriberListener;
        TopicQos topicQos;
        TopicListener2 topicListener;
        DataReaderQos dataReaderQos;
        DataReaderListener2 dataReaderListener;
        WaitSet2 waitset;
        DomainParticipant2 participant;
        Subscriber2 subscriber;
        Topic2 topic;
        StatusCondition2 statuscodition1;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            uint mask = 0xffffffff;
            dpf = DomainParticipantFactory2.get_instance();
            int domain_id = 0;
            domainparticipantqos = new DomainParticipantQos();
            domainParticipantListener = new DomainParticipantListener2();
            subscriberQos = new SubscriberQos();
            subscriberListener = new SubscriberListener2();
            topicQos = new TopicQos();
            topicListener = new TopicListener2();
            dataReaderQos = new DataReaderQos();
            dataReaderListener = new DataReaderListener2();
            waitset = new WaitSet2();



            dpf.get_default_participant_qos(domainparticipantqos);
            participant = dpf.create_participant(domain_id, domainparticipantqos, domainParticipantListener, mask);

            participant = dpf.lookup_participant(domain_id);


            participant.get_default_subscriber_qos(subscriberQos);
            subscriber = participant.create_subscriber(subscriberQos, subscriberListener, mask);

            //System.out.println("1 read");

            participant.get_default_topic_qos(topicQos);
            topic = participant.create_topic("Square", "ShapeType", topicQos, topicListener, mask);


            fooTS1 = new FooTypeSupport2();

            fooTS1.insert_parameter("color", SUPPORT_TYPE.DDS_STRING_TYPE, true);
            fooTS1.insert_parameter("x", SUPPORT_TYPE.DDS_INTEGER32_TYPE);
            fooTS1.insert_parameter("y", SUPPORT_TYPE.DDS_INTEGER32_TYPE);
            fooTS1.insert_parameter("ShapeType", SUPPORT_TYPE.DDS_INTEGER32_TYPE);

            fooTS1.register_type(participant, "ShapeType");

            subscriber.get_default_datareader_qos(dataReaderQos);

            statuscodition1 = subscriber.get_statuscondition();
            statuscodition1.set_enabled_statuses(0xFFFF);
            waitset.attach_condition(statuscodition1);

            dataReaderListener.setCallback(new c1());

            datareader = subscriber.create_datareader(topic, dataReaderQos, dataReaderListener, mask);
        }
    }
}
