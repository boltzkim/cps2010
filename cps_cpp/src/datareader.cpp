#include <core.hh>


namespace cps
{

DataReader2::DataReader2(DataReader *dataReader) : Entity2((Entity*)dataReader)
{
	p_dataReader = dataReader;
}

DataReader *DataReader2::getDataReader()
{
	return p_dataReader;
}


////////////////////////////////////

ReadCondition2 DataReader2::create_readcondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	ReadCondition *p_readCondition = p_dataReader->create_readcondition(p_dataReader, sample_states, view_states, instance_states);
	ReadCondition2 readCondition(p_readCondition);
	return readCondition;
}


QueryCondition2 DataReader2::create_querycondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states,in_dds string query_expression,in_dds StringSeq query_parameters)
{
	QueryCondition *p_queryCondition = p_dataReader->create_querycondition(p_dataReader, sample_states, view_states, instance_states, query_expression, &query_parameters);
	QueryCondition2 queryCondition(p_queryCondition);
	return queryCondition;
}


ReturnCode_t DataReader2::delete_readcondition(in_dds ReadCondition2 condition)
{
	return p_dataReader->delete_readcondition(p_dataReader, condition.getReadCondition());
}


ReturnCode_t DataReader2::delete_contained_entities()
{
	return p_dataReader->delete_contained_entities(p_dataReader);
}


ReturnCode_t DataReader2::set_qos(in_dds DataReaderQos qos)
{
	return p_dataReader->set_qos(p_dataReader, &qos);
}


ReturnCode_t DataReader2::get_qos(inout_dds DataReaderQos &qos)
{
	return p_dataReader->get_qos(p_dataReader, &qos);
}


ReturnCode_t DataReader2::set_listener(in_dds DataReaderListener2 &listener,in_dds StatusMask mask)
{
	return p_dataReader->set_listener(p_dataReader, listener.getListenerPtr(), mask);
}


DataReaderListener2 DataReader2::get_listener()
{
	DataReaderListener2 listener(p_dataReader->get_listener(p_dataReader));
	return listener;
}


TopicDescription2 DataReader2::get_topicdescription()
{
	TopicDescription2 topicDescription(p_dataReader->get_topicdescription(p_dataReader));
	return topicDescription;
}


Subscriber2 DataReader2::get_subscriber()
{
	Subscriber2 subscriber(p_dataReader->get_subscriber(p_dataReader));
	return subscriber;
}



ReturnCode_t DataReader2::get_sample_rejected_status(inout_dds SampleRejectedStatus &status)
{
	return p_dataReader->get_sample_rejected_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::get_liveliness_changed_status(inout_dds LivelinessChangedStatus &status)
{
	return p_dataReader->get_liveliness_changed_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::get_requested_deadline_missed_status(inout_dds RequestedDeadlineMissedStatus &status)
{
	return p_dataReader->get_requested_deadline_missed_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::get_requested_incompatible_qos_status(inout_dds RequestedIncompatibleQosStatus &status)
{
	return p_dataReader->get_requested_incompatible_qos_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::get_subscription_matched_status(inout_dds SubscriptionMatchedStatus &status)
{
	return p_dataReader->get_subscription_matched_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::get_sample_lost_status(inout_dds SampleLostStatus &status)
{
	return p_dataReader->get_sample_lost_status(p_dataReader, &status);
}


ReturnCode_t DataReader2::wait_for_historical_data(in_dds Duration_t max_wait)
{
	return p_dataReader->wait_for_historical_data(p_dataReader, max_wait);
}


ReturnCode_t DataReader2::get_matched_publications(inout_dds InstanceHandleSeq &publication_handles)
{
	return p_dataReader->get_matched_publications(p_dataReader, &publication_handles);
}


ReturnCode_t DataReader2::get_matched_publication_data(inout_dds PublicationBuiltinTopicData &publication_data,in_dds InstanceHandle_t publication_handle)
{
	return p_dataReader->get_matched_publication_data(p_dataReader, &publication_data, publication_handle);
}


//////////////////////////////////////


FooDataReader2::FooDataReader2(FooDataReader *fooDataReader)
:DataReader2((DataReader*)fooDataReader )
{
	p_fooDataReader = fooDataReader;
}

FooDataReader *FooDataReader2::getFooDataReader()
{
	return p_fooDataReader;
}

///////////////////////////////


ReturnCode_t FooDataReader2::read(inout_dds FooSeq2 &data_values, inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{

	/*printf("1 FooDataReader2, %p, %p\r\n", data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr());
	printf("2 FooDataReader2, %p, %p\r\n", &data_values, &sample_infos);*/

	return p_fooDataReader->read(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::read(inout_dds FooSeq *data_values, inout_dds SampleInfoSeq *sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	return p_fooDataReader->read(p_fooDataReader,data_values,sample_infos, max_samples, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::take(inout_dds FooSeq2 &data_values, inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	return p_fooDataReader->take(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::read_w_condition(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds ReadCondition2 condition)
{
	return p_fooDataReader->read_w_condition(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, condition.getReadCondition());
}

ReturnCode_t FooDataReader2::take_w_condition(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds ReadCondition2 condition)
{
	return p_fooDataReader->take_w_condition(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, condition.getReadCondition());
}

ReturnCode_t FooDataReader2::read_next_sample(inout_dds Foo2 &data_value,inout_dds SampleInfo2 &sample_info)
{
	Foo  *tmp_foo = data_value.getFoo();
	SampleInfo  *tmp_SampleInfo = sample_info.getSampleInfo();

	return  p_fooDataReader->read_next_sample(p_fooDataReader, &tmp_foo, &tmp_SampleInfo);
}

ReturnCode_t FooDataReader2::take_next_sample(inout_dds Foo2 &data_value,inout_dds SampleInfo2 &sample_info)
{
	Foo  *tmp_foo = data_value.getFoo();
	SampleInfo  *tmp_SampleInfo = sample_info.getSampleInfo();

	return  p_fooDataReader->take_next_sample(p_fooDataReader, &tmp_foo, &tmp_SampleInfo);
}

ReturnCode_t FooDataReader2::read_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	return p_fooDataReader->read_instance(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, a_handle, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::take_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	return p_fooDataReader->take_instance(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, a_handle, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::read_next_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t &previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{

	return p_fooDataReader->read_next_instance(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, &previous_handle, sample_states, view_states, instance_states);
}

ReturnCode_t FooDataReader2::take_next_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t &previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	return p_fooDataReader->take_next_instance(p_fooDataReader, data_values.getFooSeqPtr(), sample_infos.getSampleInfoSeqPtr(), max_samples, &previous_handle, sample_states, view_states, instance_states);

}

ReturnCode_t FooDataReader2::get_key_value(inout_dds Foo2 &key_holder, in_dds InstanceHandle_t handle)
{
	return p_fooDataReader->get_key_value(p_fooDataReader, key_holder.getFoo(), handle);
}

InstanceHandle_t FooDataReader2::lookup_instance(in_dds Foo2 key_holder)
{
	return p_fooDataReader->lookup_instance(p_fooDataReader, key_holder.getFoo());
}



////////////////////////////////////


}