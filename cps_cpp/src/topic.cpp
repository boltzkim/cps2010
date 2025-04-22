#include <core.hh>


namespace cps
{

//////////////////////////////////////////

TopicDescription2::TopicDescription2(TopicDescription *topicDescription)
{
	p_topicDescription = topicDescription;
}

TopicDescription *TopicDescription2::getTopicDescription()
{
	return p_topicDescription;
}

string TopicDescription2::get_type_name()
{
	return p_topicDescription->get_type_name((Topic*)p_topicDescription);
}

string TopicDescription2::get_name()
{
	return p_topicDescription->get_name((Topic*)p_topicDescription);
}

DomainParticipant2 TopicDescription2::get_participant()
{
	return DomainParticipant2(p_topicDescription->get_participant((Topic*)p_topicDescription));
}

////////////////////////////////////////

Topic2::Topic2(Topic *topic) : Entity2((Entity*)topic), TopicDescription2((TopicDescription*)topic)
{
	p_topic = topic;
}

Topic *Topic2::getTopic()
{
	return p_topic;
}


ReturnCode_t Topic2::set_qos(in_dds TopicQos qos)
{
	return p_topic->set_qos(p_topic, &qos);
}

ReturnCode_t Topic2::get_qos(inout_dds TopicQos &qos)
{
	return p_topic->get_qos(p_topic, &qos);
}

ReturnCode_t Topic2::set_listener(in_dds TopicListener2 &listener,in_dds StatusMask mask)
{
	return p_topic->set_listener(p_topic, listener.getListenerPtr(), mask);
}

TopicListener2 Topic2::get_listener()
{
	return TopicListener2(p_topic->get_listener(p_topic));
}


ReturnCode_t Topic2::get_inconsistent_topic_status(inout_dds InconsistentTopicStatus &a_status)
{
	return p_topic->get_inconsistent_topic_status(p_topic, &a_status);
}



//////////////////////////////////////////////////

ContentFilteredTopic2::ContentFilteredTopic2(ContentFilteredTopic *topic) : Entity2((Entity*)topic), TopicDescription2((TopicDescription*)topic)
{
	p_contentFilteredTopic = topic;
}

ContentFilteredTopic *ContentFilteredTopic2::getContentFilteredTopic()
{
	return p_contentFilteredTopic;
}

string ContentFilteredTopic2::get_filter_expression()
{
	return p_contentFilteredTopic->get_filter_expression(p_contentFilteredTopic);
}

ReturnCode_t ContentFilteredTopic2::get_expression_parameters(inout_dds StringSeq &expression_parameters)
{
	return p_contentFilteredTopic->get_expression_parameters(p_contentFilteredTopic, &expression_parameters);
}

ReturnCode_t ContentFilteredTopic2::set_expression_parameters(in_dds StringSeq expression_parameters)
{
	return p_contentFilteredTopic->set_expression_parameters(p_contentFilteredTopic, &expression_parameters);
}

Topic2 ContentFilteredTopic2::get_related_topic()
{
	return Topic2(p_contentFilteredTopic->get_related_topic(p_contentFilteredTopic));
}


//////////////////////////////////////////////////

MultiTopic2::MultiTopic2(MultiTopic *topic) : Entity2((Entity*)topic), TopicDescription2((TopicDescription*)topic)
{
	p_multiTopic = topic;
}

MultiTopic *MultiTopic2::getMultiTopic()
{
	return p_multiTopic;
}

string MultiTopic2::get_subscription_expression()
{
	return p_multiTopic->get_subscription_expression(p_multiTopic);
}

ReturnCode_t MultiTopic2::get_expression_parameters(inout_dds StringSeq &expression_parameters)
{
	return p_multiTopic->get_expression_parameters(p_multiTopic, &expression_parameters);
}

ReturnCode_t MultiTopic2::set_expression_parameters(in_dds StringSeq expression_parameters)
{
	return p_multiTopic->set_expression_parameters(p_multiTopic, &expression_parameters);
}


//////////////////////////////////////////////////


}