#include <core.hh>


namespace cps
{

DomainParticipant2::DomainParticipant2(DomainParticipant *domainparticipant) : Entity2((Entity*)domainparticipant)
{
	p_domainparticipant = domainparticipant;
}

DomainParticipant *DomainParticipant2::getDomainParticipant()
{
	return p_domainparticipant;
}

//////////////////////////////////////////////////////


Publisher2 DomainParticipant2::create_publisher(in_dds PublisherQos qos,in_dds PublisherListener2 &listener,in_dds StatusMask mask)
{
	Publisher *p_publisher = p_domainparticipant->create_publisher(p_domainparticipant, &qos, listener.getListenerPtr(), mask);
	Publisher2 publisher(p_publisher);
	return publisher;
}

ReturnCode_t DomainParticipant2::delete_publisher(in_dds Publisher2 publisher)
{
	return p_domainparticipant->delete_publisher(p_domainparticipant, publisher.getPublisher());
}

Subscriber2 DomainParticipant2::create_subscriber(in_dds SubscriberQos qos,in_dds SubscriberListener2 &listener,in_dds StatusMask mask)
{
	Subscriber *p_subscriber = p_domainparticipant->create_subscriber(p_domainparticipant, &qos, listener.getListenerPtr(), mask);
	Subscriber2 subscriber(p_subscriber);
	return subscriber;
}

ReturnCode_t DomainParticipant2::delete_subscriber(in_dds Subscriber2 subscriber)
{
	return p_domainparticipant->delete_subscriber(p_domainparticipant, subscriber.getSubscriber());
}

Subscriber2 DomainParticipant2::get_builtin_subscriber()
{
	Subscriber *p_subscriber = p_domainparticipant->get_builtin_subscriber(p_domainparticipant);
	Subscriber2 subscriber(p_subscriber);
	return subscriber;
}

Topic2 DomainParticipant2::create_topic(in_dds string topic_name,in_dds string type_name,in_dds TopicQos qos,in_dds TopicListener2 &listener,in_dds StatusMask mask)
{
	Topic *p_topic = p_domainparticipant->create_topic(p_domainparticipant, topic_name, type_name, &qos, listener.getListenerPtr(), mask);
	Topic2 topic(p_topic);
	return topic;
}

ReturnCode_t DomainParticipant2::delete_topic(in_dds Topic2 topic)
{
	return p_domainparticipant->delete_topic(p_domainparticipant, topic.getTopic());
}


Topic2 DomainParticipant2::find_topic(in_dds string topic_name,in_dds Duration_t timeout)
{
	Topic *p_topic = p_domainparticipant->find_topic(p_domainparticipant, topic_name, timeout);
	Topic2 topic(p_topic);
	return topic;
}


TopicDescription2 DomainParticipant2::lookup_topicdescription(in_dds string name)
{
	TopicDescription *p_topicDescription = p_domainparticipant->lookup_topicdescription(p_domainparticipant, name);
	TopicDescription2 topicDescription(p_topicDescription);
	return topicDescription;
}


ContentFilteredTopic2 DomainParticipant2::create_contentfilteredtopic(in_dds string name,in_dds Topic2 related_topic,in_dds string filter_expression,in_dds StringSeq expression_parameters)
{
	ContentFilteredTopic *p_contentFilteredTopic = p_domainparticipant->create_contentfilteredtopic(p_domainparticipant, name, related_topic.getTopic(), filter_expression, expression_parameters);
	ContentFilteredTopic2 contentFilteredTopic(p_contentFilteredTopic);
	return contentFilteredTopic;
}


ReturnCode_t DomainParticipant2::delete_contentfilteredtopic(in_dds ContentFilteredTopic2 contentfilteredtopic)
{
	return p_domainparticipant->delete_contentfilteredtopic(p_domainparticipant, contentfilteredtopic.getContentFilteredTopic());
}


MultiTopic2 DomainParticipant2::create_multitopic(in_dds string name,in_dds string type_name,in_dds string subscription_expression,in_dds StringSeq expression_parameters)
{
	MultiTopic *p_multiTopic = p_domainparticipant->create_multitopic(p_domainparticipant, name, type_name, subscription_expression, expression_parameters);
	MultiTopic2 multiTopic(p_multiTopic);
	return multiTopic;
}


ReturnCode_t DomainParticipant2::delete_multitopic(in_dds MultiTopic2 multitopic)
{
	return p_domainparticipant->delete_multitopic(p_domainparticipant, multitopic.getMultiTopic());
}


ReturnCode_t DomainParticipant2::delete_contained_entities()
{
	return p_domainparticipant->delete_contained_entities(p_domainparticipant);
}


ReturnCode_t DomainParticipant2::set_qos(in_dds DomainParticipantQos qos)
{
	return p_domainparticipant->set_qos(p_domainparticipant, &qos);
}


ReturnCode_t DomainParticipant2::get_qos(inout_dds DomainParticipantQos &qos)
{
	return p_domainparticipant->get_qos(p_domainparticipant, &qos);
}


ReturnCode_t DomainParticipant2::set_listener(in_dds DomainParticipantListener2 &listener,in_dds StatusMask mask)
{
	return p_domainparticipant->set_listener(p_domainparticipant, listener.getListenerPtr(), mask);
}


DomainParticipantListener2 DomainParticipant2::get_listener()
{
	DomainParticipantListener *p_listener = p_domainparticipant->get_listener(p_domainparticipant);
	DomainParticipantListener2 listener(p_listener);

	return listener;
}


ReturnCode_t DomainParticipant2::ignore_participant(in_dds InstanceHandle_t handle)
{
	return p_domainparticipant->ignore_participant(p_domainparticipant, handle);
}


ReturnCode_t DomainParticipant2::ignore_topic(in_dds InstanceHandle_t handle)
{
	return p_domainparticipant->ignore_topic(p_domainparticipant, handle);
}


ReturnCode_t DomainParticipant2::ignore_publication(in_dds InstanceHandle_t handle)
{
	return p_domainparticipant->ignore_publication(p_domainparticipant, handle);
}


ReturnCode_t DomainParticipant2::ignore_subscription(in_dds InstanceHandle_t handle)
{
	return p_domainparticipant->ignore_subscription(p_domainparticipant, handle);
}


DomainId_t DomainParticipant2::get_domain_id(DomainParticipant domainparticipant)
{
	return p_domainparticipant->get_domain_id(p_domainparticipant);
}


ReturnCode_t DomainParticipant2::assert_liveliness()
{
	return p_domainparticipant->assert_liveliness(p_domainparticipant);
}


ReturnCode_t DomainParticipant2::set_default_publisher_qos(in_dds PublisherQos qos)
{
	return p_domainparticipant->set_default_publisher_qos(&qos);
}


ReturnCode_t DomainParticipant2::get_default_publisher_qos(inout_dds PublisherQos &qos)
{
	return p_domainparticipant->get_default_publisher_qos(&qos);
}


ReturnCode_t DomainParticipant2::set_default_subscriber_qos(in_dds SubscriberQos qos)
{
	return p_domainparticipant->set_default_subscriber_qos(&qos);
}


ReturnCode_t DomainParticipant2::get_default_subscriber_qos(inout_dds SubscriberQos &qos)
{
	return p_domainparticipant->get_default_subscriber_qos(&qos);
}


ReturnCode_t DomainParticipant2::set_default_topic_qos(in_dds TopicQos qos)
{
	return p_domainparticipant->set_default_topic_qos(&qos);
}


ReturnCode_t DomainParticipant2::get_default_topic_qos(inout_dds TopicQos &qos)
{
	return p_domainparticipant->get_default_topic_qos(&qos);
}



ReturnCode_t DomainParticipant2::get_discovered_participants(inout_dds InstanceHandleSeq &participant_handles)
{
	return p_domainparticipant->get_discovered_participants(p_domainparticipant, &participant_handles);
}


ReturnCode_t DomainParticipant2::get_discovered_participant_data(inout_dds ParticipantBuiltinTopicData &participant_data,in_dds InstanceHandle_t participant_handle)
{
	return p_domainparticipant->get_discovered_participant_data(p_domainparticipant, &participant_data, participant_handle);
}


ReturnCode_t DomainParticipant2::get_discovered_topics(inout_dds InstanceHandleSeq &topic_handles)
{
	return p_domainparticipant->get_discovered_topics(p_domainparticipant, &topic_handles);
}


ReturnCode_t DomainParticipant2::get_discovered_topic_data(inout_dds TopicBuiltinTopicData &topic_data,in_dds InstanceHandle_t topic_handle)
{
	return p_domainparticipant->get_discovered_topic_data(p_domainparticipant, &topic_data, topic_handle);
}


bool DomainParticipant2::contains_entity(in_dds InstanceHandle_t a_handle)
{
	return p_domainparticipant->contains_entity(p_domainparticipant, a_handle);
}


ReturnCode_t DomainParticipant2::get_current_time(inout_dds Time_t &current_time)
{
	return p_domainparticipant->get_current_time(p_domainparticipant, &current_time);
}


//////////////////////////////////////////////////////

}