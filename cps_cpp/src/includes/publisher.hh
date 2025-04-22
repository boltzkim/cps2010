#if !defined(__PUBLISHER_HH__)
#define __PUBLISHER_HH__



namespace cps
{

class DomainParticipant2;

class Publisher2 : public Entity2
{
public:
	Publisher2(Publisher *publisher);
	Publisher *getPublisher();

	////////////////

	FooDataWriter2 create_datawriter(in_dds Topic2 topic,in_dds DataWriterQos qos,in_dds DataWriterListener2 &listener,in_dds StatusMask mask);
	ReturnCode_t delete_datawriter(in_dds DataWriter2 datawriter);
	FooDataWriter2 lookup_datawriter(in_dds string topic_name);
	ReturnCode_t delete_contained_entities();
	ReturnCode_t set_qos(in_dds PublisherQos qos);
	ReturnCode_t get_qos(inout_dds PublisherQos &qos);
	ReturnCode_t set_listener(in_dds PublisherListener2 &listener,in_dds StatusMask mask);
	PublisherListener2 get_listener();
	ReturnCode_t suspend_publications();
	ReturnCode_t resume_publications();
	ReturnCode_t begin_coherent_changes();
	ReturnCode_t end_coherent_changes();
	ReturnCode_t wait_for_acknowledgments(in_dds Duration_t max_wait);
	DomainParticipant2 get_participant();
	ReturnCode_t set_default_datawriter_qos(in_dds DataWriterQos qos);
	ReturnCode_t get_default_datawriter_qos(inout_dds DataWriterQos &qos);
	ReturnCode_t copy_from_topic_qos(inout_dds DataWriterQos &datawriter_qos,in_dds TopicQos topic_qos);

	////////////////////////////

	
private:
	Publisher *p_publisher;

};


} // namespace cps end




#endif