#if !defined(__TOPIC_HH__)
#define __TOPIC_HH__

namespace cps
{
class DomainParticipant2;

class TopicDescription2
{
public:
	TopicDescription2(TopicDescription *topicDescription);
	TopicDescription *getTopicDescription();
///////////////////////////////

	string get_type_name();
	string get_name();
	DomainParticipant2 get_participant();

//////////////////////////////

	
private:
	TopicDescription *p_topicDescription;
};


class Topic2 : public Entity2, public TopicDescription2
{
public:
	Topic2(Topic *topic);
	Topic *getTopic();
////////////////////////////////

	ReturnCode_t set_qos(in_dds TopicQos qos);
	ReturnCode_t get_qos(inout_dds TopicQos &qos);
	ReturnCode_t set_listener(in_dds TopicListener2 &listener,in_dds StatusMask mask);
	TopicListener2 get_listener();

	ReturnCode_t get_inconsistent_topic_status(inout_dds InconsistentTopicStatus &a_status);

////////////////////////////////

	
private:
	Topic *p_topic;

};

class ContentFilteredTopic2 : public Entity2, public TopicDescription2
{
public:
	ContentFilteredTopic2(ContentFilteredTopic *topic);
	ContentFilteredTopic *getContentFilteredTopic();

////////////////////////////////////////
	string get_filter_expression();
	ReturnCode_t get_expression_parameters(inout_dds StringSeq &expression_parameters);
	ReturnCode_t set_expression_parameters(in_dds StringSeq expression_parameters);
	Topic2 get_related_topic();

////////////////////////////////////////
private:
	ContentFilteredTopic *p_contentFilteredTopic;

};

class MultiTopic2 : public Entity2, public TopicDescription2
{
public:
	MultiTopic2(MultiTopic *topic);
	MultiTopic *getMultiTopic();

	////////////////////////////////////////

	string get_subscription_expression();
	ReturnCode_t get_expression_parameters(inout_dds StringSeq &expression_parameters);
	ReturnCode_t set_expression_parameters(in_dds StringSeq expression_parameters);

	////////////////////////////////////////
	
private:
	MultiTopic *p_multiTopic;

};



} // namespace cps end




#endif