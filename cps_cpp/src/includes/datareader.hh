#if !defined(__DATAREADER_HH__)
#define __DATAREADER_HH__

namespace cps
{

class Subscriber2;

class DataReader2 : public Entity2
{
public:
	DataReader2(DataReader *dataReader);
	DataReader *getDataReader();

	////////////////


	ReadCondition2 create_readcondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	QueryCondition2 create_querycondition(in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states,in_dds string query_expression,in_dds StringSeq query_parameters);
	ReturnCode_t delete_readcondition(in_dds ReadCondition2 condition);
	ReturnCode_t delete_contained_entities();
	ReturnCode_t set_qos(in_dds DataReaderQos qos);
	ReturnCode_t get_qos(inout_dds DataReaderQos &qos);
	ReturnCode_t set_listener(in_dds DataReaderListener2 &listener,in_dds StatusMask mask);
	DataReaderListener2 get_listener();
	TopicDescription2 get_topicdescription();
	Subscriber2 get_subscriber();
	
	ReturnCode_t get_sample_rejected_status(inout_dds SampleRejectedStatus &status);
	ReturnCode_t get_liveliness_changed_status(inout_dds LivelinessChangedStatus &status);
	ReturnCode_t get_requested_deadline_missed_status(inout_dds RequestedDeadlineMissedStatus &status);
	ReturnCode_t get_requested_incompatible_qos_status(inout_dds RequestedIncompatibleQosStatus &status);
	ReturnCode_t get_subscription_matched_status(inout_dds SubscriptionMatchedStatus &status);
	ReturnCode_t get_sample_lost_status(inout_dds SampleLostStatus &status);
	ReturnCode_t wait_for_historical_data(in_dds Duration_t max_wait);
	ReturnCode_t get_matched_publications(inout_dds InstanceHandleSeq &publication_handles);
	ReturnCode_t get_matched_publication_data(inout_dds PublicationBuiltinTopicData &publication_data,in_dds InstanceHandle_t publication_handle);

	////////////////////////////

	
private:
	DataReader *p_dataReader;

};

//////////////////////////////////////////

class FooDataReader2 : public DataReader2
{
public:
	FooDataReader2(FooDataReader *fooDataReader);
	FooDataReader *getFooDataReader();
	///////////////////////////////


	ReturnCode_t read(inout_dds FooSeq2 &data_values, inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t take(inout_dds FooSeq2 &data_values, inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t read_w_condition(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos, in_dds long max_samples, in_dds ReadCondition2 condition);
	ReturnCode_t take_w_condition(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds ReadCondition2 condition);
	ReturnCode_t read_next_sample(inout_dds Foo2 &data_value,inout_dds SampleInfo2 &sample_info);
	ReturnCode_t take_next_sample(inout_dds Foo2 &data_value,inout_dds SampleInfo2 &sample_info);
	ReturnCode_t read_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t take_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t read_next_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t &previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t take_next_instance(inout_dds FooSeq2 &data_values,inout_dds SampleInfoSeq2 &sample_infos,in_dds long max_samples,in_dds InstanceHandle_t &previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t get_key_value(inout_dds Foo2 &key_holder, in_dds InstanceHandle_t handle);
	InstanceHandle_t lookup_instance(in_dds Foo2 key_holder);


	ReturnCode_t read(inout_dds FooSeq *data_values, inout_dds SampleInfoSeq *sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);



	///////////////////////////////
private:
	FooDataReader *p_fooDataReader;
};


} // namespace cps end




#endif