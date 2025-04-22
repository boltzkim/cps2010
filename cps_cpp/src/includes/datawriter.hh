#if !defined(__DATAWRITER_HH__)
#define __DATAWRITER_HH__

namespace cps
{
class Publisher2;

class DataWriter2 : public Entity2
{
public:
	DataWriter2(DataWriter *dataWriter);
	DataWriter *getDataWriter();

	////////////////

	ReturnCode_t set_qos(in_dds DataWriterQos qos);
	ReturnCode_t get_qos(inout_dds DataWriterQos &qos);
	ReturnCode_t set_listener(in_dds DataWriterListener2 &listener,in_dds StatusMask mask);
	DataWriterListener2 get_listener();
	Topic2 get_topic();
	Publisher2 get_publisher();
	ReturnCode_t wait_for_acknowledgments(in_dds Duration_t max_wait);
	/* Access the status*/ 
	ReturnCode_t get_liveliness_lost_status(inout_dds LivelinessLostStatus &status);
	ReturnCode_t get_offered_deadline_missed_status(inout_dds OfferedDeadlineMissedStatus &status);
	ReturnCode_t get_offered_incompatible_qos_status(inout_dds OfferedIncompatibleQosStatus &status);
	ReturnCode_t get_publication_matched_status(inout_dds PublicationMatchedStatus &status);
	ReturnCode_t assert_liveliness();
	ReturnCode_t get_matched_subscriptions(inout_dds InstanceHandleSeq &subscription_handles);
	ReturnCode_t get_matched_subscription_data(inout_dds SubscriptionBuiltinTopicData &subscription_data,in_dds InstanceHandle_t subscription_handle);

	////////////////////////////

	
private:
	DataWriter *p_dataWriter;

};



class FooDataWriter2 : public DataWriter2
{
public:
	FooDataWriter2(FooDataWriter *fooDataWriter);
	FooDataWriter *getFooDataWriter();
	///////////////////////////////

	InstanceHandle_t register_instance(in_dds Foo2 instance_data);
	InstanceHandle_t register_instance_w_timestamp(FooDataWriter fooDataWriter,in_dds Foo2 instance_data,in_dds Time_t source_timestamp);
	ReturnCode_t unregister_instance(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle);
	ReturnCode_t unregister_instance_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp);
	ReturnCode_t write(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle);
	ReturnCode_t write_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp);
	ReturnCode_t dispose(in_dds Foo2 instance_data, in_dds InstanceHandle_t instance_handle);
	ReturnCode_t dispose_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t instance_handle, in_dds Time_t source_timestamp);
	ReturnCode_t get_key_value(inout_dds Foo2 &key_holder, in_dds InstanceHandle_t handle);
	InstanceHandle_t lookup_instance(in_dds Foo2 key_holder);


	///////////////////////////////
private:
	FooDataWriter *p_fooDataWriter;
};



} // namespace cps end




#endif