#include <core.hh>


namespace cps
{

Subscriber2::Subscriber2(Subscriber *subscriber) : Entity2((Entity*)subscriber)
{
	p_subscriber = subscriber;
}

Subscriber *Subscriber2::getSubscriber()
{
	return p_subscriber;
}

///////////////////////////////////////////////////////////



FooDataReader2 Subscriber2::create_datareader(in_dds Topic2 topic,in_dds DataReaderQos qos,in_dds DataReaderListener2 &listener,in_dds StatusMask mask)
{
	FooDataReader *p_dataReader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber, topic.getTopic(), &qos, listener.getListenerPtr(), mask);
	FooDataReader2 dataReader(p_dataReader);
	return dataReader;
}


ReturnCode_t Subscriber2::delete_datareader(in_dds DataReader2 datareader)
{
	return p_subscriber->delete_datareader(p_subscriber, datareader.getDataReader());
}

ReturnCode_t Subscriber2::delete_contained_entities()
{
	return p_subscriber->delete_contained_entities(p_subscriber);
}

FooDataReader2 Subscriber2::lookup_datareader(in_dds string topic_name)
{
	return FooDataReader2((FooDataReader *)p_subscriber->lookup_datareader(p_subscriber, topic_name));
}

ReturnCode_t Subscriber2::get_datareaders(inout_dds DataReaderSeq &readers,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	return p_subscriber->get_datareaders(p_subscriber, &readers, sample_states, view_states, instance_states);
}

ReturnCode_t Subscriber2::notify_datareaders()
{
	return p_subscriber->notify_datareaders(p_subscriber);
}

ReturnCode_t Subscriber2::set_qos(in_dds SubscriberQos qos)
{
	return p_subscriber->set_qos(p_subscriber, &qos);
}

ReturnCode_t Subscriber2::get_qos(inout_dds SubscriberQos &qos)
{
	return p_subscriber->get_qos(p_subscriber, &qos);
}

ReturnCode_t Subscriber2::set_listener(in_dds SubscriberListener2 &listener,in_dds StatusMask mask)
{
	return p_subscriber->set_listener(p_subscriber, listener.getListenerPtr(), mask);
}

SubscriberListener2 Subscriber2::get_listener()
{
	SubscriberListener2 listener(p_subscriber->get_listener(p_subscriber));
	return listener;
}

ReturnCode_t Subscriber2::begin_access()
{
	return p_subscriber->begin_access(p_subscriber);
}

ReturnCode_t Subscriber2::end_access()
{
	return p_subscriber->end_access(p_subscriber);
}

DomainParticipant2 Subscriber2::get_participant()
{
	DomainParticipant *p_domainParticipant = p_subscriber->get_participant(p_subscriber);
	DomainParticipant2 domainParticipant(p_domainParticipant);
	return domainParticipant;
}

ReturnCode_t Subscriber2::set_default_datareader_qos(in_dds DataReaderQos qos)
{
	return p_subscriber->set_default_datareader_qos(&qos);
}

ReturnCode_t Subscriber2::get_default_datareader_qos(inout_dds DataReaderQos &qos)
{
	return p_subscriber->get_default_datareader_qos(&qos);
}

ReturnCode_t Subscriber2::copy_from_topic_qos(inout_dds DataReaderQos &datareader_qos,in_dds TopicQos topic_qos)
{
	return p_subscriber->copy_from_topic_qos(&datareader_qos, &topic_qos);
}



///////////////////////////////////////////////////////////

}