#include <core.hh>


namespace cps
{

Publisher2::Publisher2(Publisher *publisher) : Entity2((Entity*)publisher)
{
	p_publisher = publisher;
}

Publisher *Publisher2::getPublisher()
{
	return p_publisher;
}
///////////////////////////////////////



FooDataWriter2 Publisher2::create_datawriter(in_dds Topic2 topic,in_dds DataWriterQos qos,in_dds DataWriterListener2 &listener,in_dds StatusMask mask)
{
	FooDataWriter *p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, topic.getTopic(), &qos, listener.getListenerPtr(), mask);
	FooDataWriter2 datawriter(p_datawriter);
	return datawriter;
}


ReturnCode_t Publisher2::delete_datawriter(in_dds DataWriter2 datawriter)
{
	return p_publisher->delete_datawriter(p_publisher, datawriter.getDataWriter());
}


FooDataWriter2 Publisher2::lookup_datawriter(in_dds string topic_name)
{
	FooDataWriter *p_datawriter = (FooDataWriter *)p_publisher->lookup_datawriter(p_publisher,topic_name);
	FooDataWriter2 datawriter(p_datawriter);
	return datawriter;
}


ReturnCode_t Publisher2::delete_contained_entities()
{
	return p_publisher->delete_contained_entities(p_publisher);
}


ReturnCode_t Publisher2::set_qos(in_dds PublisherQos qos)
{
	return p_publisher->set_qos(p_publisher, &qos);
}


ReturnCode_t Publisher2::get_qos(inout_dds PublisherQos &qos)
{
	return p_publisher->get_qos(p_publisher, &qos);
}


ReturnCode_t Publisher2::set_listener(in_dds PublisherListener2 &listener,in_dds StatusMask mask)
{
	return p_publisher->set_listener(p_publisher, listener.getListenerPtr(), mask);
}


PublisherListener2 Publisher2::get_listener()
{
	PublisherListener2 listener(p_publisher->get_listener(p_publisher));
	return listener;
}


ReturnCode_t Publisher2::suspend_publications()
{
	return p_publisher->suspend_publications(p_publisher);
}


ReturnCode_t Publisher2::resume_publications()
{
	return p_publisher->resume_publications(p_publisher);
}


ReturnCode_t Publisher2::begin_coherent_changes()
{
	return p_publisher->begin_coherent_changes(p_publisher);
}


ReturnCode_t Publisher2::end_coherent_changes()
{
	return p_publisher->end_coherent_changes(p_publisher);
}


ReturnCode_t Publisher2::wait_for_acknowledgments(in_dds Duration_t max_wait)
{
	return p_publisher->wait_for_acknowledgments(p_publisher, max_wait);
}


DomainParticipant2 Publisher2::get_participant()
{
	DomainParticipant *p_domainParticipant = p_publisher->get_participant(p_publisher);
	DomainParticipant2 domainParticipant(p_domainParticipant);
	return domainParticipant;
}


ReturnCode_t Publisher2::set_default_datawriter_qos(in_dds DataWriterQos qos)
{
	return p_publisher->set_default_datawriter_qos(&qos);
}


ReturnCode_t Publisher2::get_default_datawriter_qos(inout_dds DataWriterQos &qos)
{
	return p_publisher->get_default_datawriter_qos(&qos);
}


ReturnCode_t Publisher2::copy_from_topic_qos(inout_dds DataWriterQos &datawriter_qos,in_dds TopicQos topic_qos)
{
	return p_publisher->copy_from_topic_qos(&datawriter_qos, &topic_qos);
}




///////////////////////////////////////

}