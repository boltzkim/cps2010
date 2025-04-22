#include <core.hh>


namespace cps
{

DomainParticipantListenerCallBack *DomainParticipantListener2::_callback = NULL;
PublisherListenerCallBack *PublisherListener2::_callback = NULL;
SubscriberListenerCallBack *SubscriberListener2::_callback = NULL;
TopicListenerCallBack *TopicListener2::_callback = NULL;
DataWriterListenerCallBack *DataWriterListener2::_callback = NULL;
DataReaderListenerCallBack *DataReaderListener2::_callback = NULL;

void DomainParticipantListener_on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)
{
	

	DomainParticipantListener2::_callback->on_inconsistent_topic(the_topic, status);
}

void DomainParticipantListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)
{
	DomainParticipantListener2::_callback->on_offered_deadline_missed(writer, status);
}

void DomainParticipantListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)
{
	DomainParticipantListener2::_callback->on_offered_incompatible_qos(writer, status);
}

void DomainParticipantListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)
{
	DomainParticipantListener2::_callback->on_liveliness_lost(writer, status);
}

void DomainParticipantListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)
{
	DomainParticipantListener2::_callback->on_publication_matched(writer, status);
}

void DomainParticipantListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)
{
	DomainParticipantListener2::_callback->on_requested_deadline_missed(the_reader, status);
}

void DomainParticipantListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)
{
	DomainParticipantListener2::_callback->on_requested_incompatible_qos(the_reader, status);
}

void DomainParticipantListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)
{
	DomainParticipantListener2::_callback->on_sample_rejected(the_reader, status);
}

void DomainParticipantListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)
{
	DomainParticipantListener2::_callback->on_liveliness_changed(the_reader, status);
}

void DomainParticipantListener_on_data_available(in_dds DataReader *p_reader)
{
	DomainParticipantListener2::_callback->on_data_available(p_reader);
}

void DomainParticipantListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)
{
	DomainParticipantListener2::_callback->on_subscription_matched(the_reader, status);
}

void DomainParticipantListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)
{
	DomainParticipantListener2::_callback->on_sample_lost(the_reader, status);
}

void DomainParticipantListener_on_data_on_readers(in_dds Subscriber *p_subscriber)
{
	DomainParticipantListener2::_callback->on_data_on_readers(p_subscriber);
}


void PublisherListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)
{
	PublisherListener2::_callback->on_offered_deadline_missed(writer, status);
}

void PublisherListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)
{
	PublisherListener2::_callback->on_offered_incompatible_qos(writer, status);
}

void PublisherListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)
{
	PublisherListener2::_callback->on_liveliness_lost(writer, status);
}

void PublisherListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)
{
	PublisherListener2::_callback->on_publication_matched(writer, status);
}


void SubscriberListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)
{
	SubscriberListener2::_callback->on_requested_deadline_missed(the_reader, status);
}

void SubscriberListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)
{
	SubscriberListener2::_callback->on_requested_incompatible_qos(the_reader, status);
}

void SubscriberListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)
{
	SubscriberListener2::_callback->on_sample_rejected(the_reader, status);
}

void SubscriberListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)
{
	SubscriberListener2::_callback->on_liveliness_changed(the_reader, status);
}

void SubscriberListener_on_data_available(in_dds DataReader *p_reader)
{
	SubscriberListener2::_callback->on_data_available(p_reader);
}

void SubscriberListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)
{
	SubscriberListener2::_callback->on_subscription_matched(the_reader, status);
}

void SubscriberListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)
{
	SubscriberListener2::_callback->on_sample_lost(the_reader, status);
}

void SubscriberListener_on_data_on_readers(in_dds Subscriber *p_subscriber)
{
	SubscriberListener2::_callback->on_data_on_readers(p_subscriber);
}

void TopicListener_on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)
{
	TopicListener2::_callback->on_inconsistent_topic(the_topic, status);
}

void DataWriterListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)
{
	DataWriterListener2::_callback->on_offered_deadline_missed(writer, status);
}

void DataWriterListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)
{
	DataWriterListener2::_callback->on_offered_incompatible_qos(writer, status);
}

void DataWriterListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)
{
	DataWriterListener2::_callback->on_liveliness_lost(writer, status);
}

void DataWriterListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)
{
	DataWriterListener2::_callback->on_publication_matched(writer, status);
}

void DataReaderListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)
{
	DataReaderListener2::_callback->on_requested_deadline_missed(the_reader, status);
}

void DataReaderListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)
{
	DataReaderListener2::_callback->on_requested_incompatible_qos(the_reader, status);
}

void DataReaderListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)
{
	DataReaderListener2::_callback->on_sample_rejected(the_reader, status);
}

void DataReaderListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)
{
	DataReaderListener2::_callback->on_liveliness_changed(the_reader, status);
}

void DataReaderListener_on_data_available(in_dds DataReader *p_reader)
{
	DataReaderListener2::_callback->on_data_available(p_reader);
}

void DataReaderListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)
{
	DataReaderListener2::_callback->on_subscription_matched(the_reader, status);
}

void DataReaderListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)
{
	DataReaderListener2::_callback->on_sample_lost(the_reader, status);
}



}