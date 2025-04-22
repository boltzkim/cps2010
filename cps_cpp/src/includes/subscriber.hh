#if !defined(__SUBSCRIBER_HH__)
#define __SUBSCRIBER_HH__

namespace cps
{

class DomainParticipant2;

class Subscriber2 : public Entity2
{
public:
	Subscriber2(Subscriber *subscriber);
	Subscriber *getSubscriber();
//////////////////////////////////////


	FooDataReader2 create_datareader(in_dds /*TopicDescription*/Topic2 topic,in_dds DataReaderQos qos,in_dds DataReaderListener2 &listener,in_dds StatusMask mask);
	ReturnCode_t delete_datareader(in_dds DataReader2 datareader);
	ReturnCode_t delete_contained_entities();
	FooDataReader2 lookup_datareader(in_dds string topic_name);
	ReturnCode_t get_datareaders(inout_dds DataReaderSeq &readers,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t notify_datareaders();
	ReturnCode_t set_qos(in_dds SubscriberQos qos);
	ReturnCode_t get_qos(inout_dds SubscriberQos &qos);
	ReturnCode_t set_listener(in_dds SubscriberListener2 &listener,in_dds StatusMask mask);
	SubscriberListener2 get_listener();
	ReturnCode_t begin_access();
	ReturnCode_t end_access();
	DomainParticipant2 get_participant();
	ReturnCode_t set_default_datareader_qos(in_dds DataReaderQos qos);
	ReturnCode_t get_default_datareader_qos(inout_dds DataReaderQos &qos);
	ReturnCode_t copy_from_topic_qos(inout_dds DataReaderQos &datareader_qos,in_dds TopicQos topic_qos);


//////////////////////////////////////

	
private:
	Subscriber *p_subscriber;

};


} // namespace cps end




#endif