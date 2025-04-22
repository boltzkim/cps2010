#if !defined(__DOMAINPARTICIPANT_HH__)
#define __DOMAINPARTICIPANT_HH__

namespace cps
{

class DomainParticipant2 : public Entity2
{
public:
	DomainParticipant2(DomainParticipant *domainparticipant);
	DomainParticipant *getDomainParticipant();


	Publisher2 create_publisher(in_dds PublisherQos qos,in_dds PublisherListener2 &listener,in_dds StatusMask mask);
	ReturnCode_t delete_publisher(in_dds Publisher2 publisher);
	Subscriber2 create_subscriber(in_dds SubscriberQos qos,in_dds SubscriberListener2 &listener,in_dds StatusMask mask);
	ReturnCode_t delete_subscriber(in_dds Subscriber2 subscriber);
	Subscriber2 get_builtin_subscriber();
	Topic2 create_topic(in_dds string topic_name,in_dds string type_name,in_dds TopicQos qos,in_dds TopicListener2 &listener,in_dds StatusMask mask);
	ReturnCode_t delete_topic(in_dds Topic2 topic);

	Topic2 find_topic(in_dds string topic_name,in_dds Duration_t timeout);
	TopicDescription2 lookup_topicdescription(in_dds string name);
	ContentFilteredTopic2 create_contentfilteredtopic(in_dds string name,in_dds Topic2 related_topic,in_dds string filter_expression,in_dds StringSeq expression_parameters);
	ReturnCode_t delete_contentfilteredtopic(in_dds ContentFilteredTopic2 contentfilteredtopic);
	MultiTopic2 create_multitopic(in_dds string name,in_dds string type_name,in_dds string subscription_expression,in_dds StringSeq expression_parameters);
	ReturnCode_t delete_multitopic(in_dds MultiTopic2 multitopic);
	ReturnCode_t delete_contained_entities();
	ReturnCode_t set_qos(in_dds DomainParticipantQos qos);
	ReturnCode_t get_qos(inout_dds DomainParticipantQos &qos);
	ReturnCode_t set_listener(in_dds DomainParticipantListener2 &listener,in_dds StatusMask mask);
	DomainParticipantListener2 get_listener();
	ReturnCode_t ignore_participant(in_dds InstanceHandle_t handle);
	ReturnCode_t ignore_topic(in_dds InstanceHandle_t handle);
	ReturnCode_t ignore_publication(in_dds InstanceHandle_t handle);
	ReturnCode_t ignore_subscription(in_dds InstanceHandle_t handle);
	DomainId_t get_domain_id(DomainParticipant domainparticipant);
	ReturnCode_t assert_liveliness();
	ReturnCode_t set_default_publisher_qos(in_dds PublisherQos qos);
	ReturnCode_t get_default_publisher_qos(inout_dds PublisherQos &qos);
	ReturnCode_t set_default_subscriber_qos(in_dds SubscriberQos qos);
	ReturnCode_t get_default_subscriber_qos(inout_dds SubscriberQos &qos);
	ReturnCode_t set_default_topic_qos(in_dds TopicQos qos);
	ReturnCode_t get_default_topic_qos(inout_dds TopicQos &qos);

	ReturnCode_t get_discovered_participants(inout_dds InstanceHandleSeq &participant_handles);
	ReturnCode_t get_discovered_participant_data(inout_dds ParticipantBuiltinTopicData &participant_data,in_dds InstanceHandle_t participant_handle);
	ReturnCode_t get_discovered_topics(inout_dds InstanceHandleSeq &topic_handles);
	ReturnCode_t get_discovered_topic_data(inout_dds TopicBuiltinTopicData &topic_data,in_dds InstanceHandle_t topic_handle);
	bool contains_entity(in_dds InstanceHandle_t a_handle);
	ReturnCode_t get_current_time(inout_dds Time_t &current_time);

private:
	DomainParticipant *p_domainparticipant;

};


} // namespace cps end




#endif