#include <core.hh>


namespace cps
{


DataWriter2::DataWriter2(DataWriter *dataWriter) : Entity2((Entity*)dataWriter)
{
	p_dataWriter = dataWriter;
}

DataWriter *DataWriter2::getDataWriter()
{
	return p_dataWriter;
}

////////////////////////////////////


ReturnCode_t DataWriter2::set_qos(in_dds DataWriterQos qos)
{
	return p_dataWriter->set_qos(p_dataWriter, &qos);
}


ReturnCode_t DataWriter2::get_qos(inout_dds DataWriterQos &qos)
{
	return p_dataWriter->get_qos(p_dataWriter, &qos);
}

ReturnCode_t DataWriter2::set_listener(in_dds DataWriterListener2 &listener,in_dds StatusMask mask)
{
	return p_dataWriter->set_listener(p_dataWriter, listener.getListenerPtr(), mask);
}

DataWriterListener2 DataWriter2::get_listener()
{
	DataWriterListener2 dataWriterListener(p_dataWriter->get_listener(p_dataWriter));
	return dataWriterListener;
}

Topic2 DataWriter2::get_topic()
{
	Topic *p_topic = p_dataWriter->get_topic(p_dataWriter);
	Topic2 topic(p_topic);
	return topic;
}

Publisher2 DataWriter2::get_publisher()
{
	Publisher *p_publisher = p_dataWriter->get_publisher(p_dataWriter);
	Publisher2 publisher(p_publisher);
	return publisher;
}

ReturnCode_t DataWriter2::wait_for_acknowledgments(in_dds Duration_t max_wait)
{
	return p_dataWriter->wait_for_acknowledgments(p_dataWriter, max_wait);
}

/* Access the status*/ 
ReturnCode_t DataWriter2::get_liveliness_lost_status(inout_dds LivelinessLostStatus &status)
{
	return p_dataWriter->get_liveliness_lost_status(p_dataWriter, &status);
}

ReturnCode_t DataWriter2::get_offered_deadline_missed_status(inout_dds OfferedDeadlineMissedStatus &status)
{
	return p_dataWriter->get_offered_deadline_missed_status(p_dataWriter, &status);
}

ReturnCode_t DataWriter2::get_offered_incompatible_qos_status(inout_dds OfferedIncompatibleQosStatus &status)
{
	return p_dataWriter->get_offered_incompatible_qos_status(p_dataWriter, &status);
}

ReturnCode_t DataWriter2::get_publication_matched_status(inout_dds PublicationMatchedStatus &status)
{
	return p_dataWriter->get_publication_matched_status(p_dataWriter, &status);
}

ReturnCode_t DataWriter2::assert_liveliness()
{
	return p_dataWriter->assert_liveliness(p_dataWriter);
}

ReturnCode_t DataWriter2::get_matched_subscriptions(inout_dds InstanceHandleSeq &subscription_handles)
{
	return p_dataWriter->get_matched_subscriptions(p_dataWriter, &subscription_handles);
}

ReturnCode_t DataWriter2::get_matched_subscription_data(inout_dds SubscriptionBuiltinTopicData &subscription_data,in_dds InstanceHandle_t subscription_handle)
{
	return p_dataWriter->get_matched_subscription_data(p_dataWriter, &subscription_data, subscription_handle);
}



////////////////////////////////////////


FooDataWriter2::FooDataWriter2(FooDataWriter *fooDataWriter)
: DataWriter2((DataWriter*)fooDataWriter)
{
	p_fooDataWriter = fooDataWriter;
}

FooDataWriter *FooDataWriter2::getFooDataWriter()
{
	return p_fooDataWriter;
}

///////////////////////////////

InstanceHandle_t FooDataWriter2::register_instance(in_dds Foo2 instance_data)
{
	return p_fooDataWriter->register_instance(p_fooDataWriter, instance_data.getFoo());
}

InstanceHandle_t FooDataWriter2::register_instance_w_timestamp(FooDataWriter fooDataWriter,in_dds Foo2 instance_data,in_dds Time_t source_timestamp)
{
	return p_fooDataWriter->register_instance_w_timestamp(p_fooDataWriter, instance_data.getFoo(), source_timestamp);
}

ReturnCode_t FooDataWriter2::unregister_instance(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle)
{
	return p_fooDataWriter->unregister_instance(p_fooDataWriter, instance_data.getFoo(), handle);
}

ReturnCode_t FooDataWriter2::unregister_instance_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp)
{
	return p_fooDataWriter->unregister_instance_w_timestamp(p_fooDataWriter, instance_data.getFoo(), handle, source_timestamp);
}

ReturnCode_t FooDataWriter2::write(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle)
{
	return p_fooDataWriter->write(p_fooDataWriter, instance_data.getFoo(), handle);
}

ReturnCode_t FooDataWriter2::write_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp)
{
	return p_fooDataWriter->write_w_timestamp(p_fooDataWriter, instance_data.getFoo(), handle, source_timestamp);
}

ReturnCode_t FooDataWriter2::dispose(in_dds Foo2 instance_data, in_dds InstanceHandle_t instance_handle)
{
	return p_fooDataWriter->dispose(p_fooDataWriter, instance_data.getFoo(), instance_handle);
}

ReturnCode_t FooDataWriter2::dispose_w_timestamp(in_dds Foo2 instance_data, in_dds InstanceHandle_t instance_handle, in_dds Time_t source_timestamp)
{
	return p_fooDataWriter->dispose_w_timestamp(p_fooDataWriter, instance_data.getFoo(), instance_handle, source_timestamp);
}

ReturnCode_t FooDataWriter2::get_key_value(inout_dds Foo2 &key_holder, in_dds InstanceHandle_t handle)
{
	return p_fooDataWriter->get_key_value(p_fooDataWriter, key_holder.getFoo(), handle);
}

InstanceHandle_t FooDataWriter2::lookup_instance(in_dds Foo2 key_holder)
{
	return p_fooDataWriter->lookup_instance(p_fooDataWriter, key_holder.getFoo());
}




////////////////////////////////////////


}