#if !defined(__LISTENER_HH__)
#define __LISTENER_HH__
#include <functional>

namespace cps
{


//_inline void* ToVoidStar( void* Dummy, ... )
//{
//	va_list	list;
//	void*	RetVal;
//	
//	va_start( list, Dummy );
//	RetVal = va_arg( list, void* );
//	va_end( list );
//
//	return RetVal;
//}

class DomainParticipantListenerCallBack
{
public:
	virtual ~DomainParticipantListenerCallBack() {}
	virtual void on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)=0;
	virtual void on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)=0;
	virtual void on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)=0;
	virtual void on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)=0;
	virtual void on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)=0;
	virtual void on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)=0;
	virtual void on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)=0;
	virtual void on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)=0;
	virtual void on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)=0;
	virtual void on_data_available(in_dds DataReader *p_reader)=0;
	virtual void on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)=0;
	virtual void on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)=0;
	virtual void on_data_on_readers(in_dds Subscriber *p_subscriber)=0;

};

#ifndef SWIG_ON

void DomainParticipantListener_on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status);
void DomainParticipantListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status);
void DomainParticipantListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status);
void DomainParticipantListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status);
void DomainParticipantListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status);
void DomainParticipantListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status);
void DomainParticipantListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status);
void DomainParticipantListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status);
void DomainParticipantListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status);
void DomainParticipantListener_on_data_available(in_dds DataReader *p_reader);
void DomainParticipantListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status);
void DomainParticipantListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status);
void DomainParticipantListener_on_data_on_readers(in_dds Subscriber *p_subscriber);

#endif

class DomainParticipantListener2
{
public:

	DomainParticipantListener2(DomainParticipantListener *p_listener)
	{
		//_callback = NULL;
		memcpy(&listener, p_listener, sizeof(DomainParticipantListener));
	}

	DomainParticipantListener2()
	{
		//_callback = NULL;
		memset(&listener, 0, sizeof(DomainParticipantListener));
	}

	DomainParticipantListener getListener()
	{
		return listener;
	}

	DomainParticipantListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(DomainParticipantListenerCallBack *cb) {
		_callback = cb;
		setOn_inconsistent_topic(DomainParticipantListener_on_inconsistent_topic);
		setOn_offered_deadline_missed(DomainParticipantListener_on_offered_deadline_missed);
		setOn_offered_incompatible_qos(DomainParticipantListener_on_offered_incompatible_qos);
		setOn_liveliness_lost(DomainParticipantListener_on_liveliness_lost);
		setOn_publication_matched(DomainParticipantListener_on_publication_matched);
		setOn_requested_deadline_missed(DomainParticipantListener_on_requested_deadline_missed);
		setOn_requested_incompatible_qos(DomainParticipantListener_on_requested_incompatible_qos);
		setOn_sample_rejected(DomainParticipantListener_on_sample_rejected);
		setOn_liveliness_changed(DomainParticipantListener_on_liveliness_changed);
		setOn_data_available(DomainParticipantListener_on_data_available);
		setOn_subscription_matched(DomainParticipantListener_on_subscription_matched);
		setOn_sample_lost(DomainParticipantListener_on_sample_lost);
		setOn_data_on_readers(DomainParticipantListener_on_data_on_readers);
	}

#ifndef SWIG_ON
	void setOn_inconsistent_topic(void (*on_inconsistent_topic)(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status))
	{
		listener.on_inconsistent_topic = on_inconsistent_topic;
	}

	void setOn_offered_deadline_missed(void (*on_offered_deadline_missed)(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status))
	{
		listener.on_offered_deadline_missed = on_offered_deadline_missed;
	}

	void setOn_offered_incompatible_qos(void (*on_offered_incompatible_qos)(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status))
	{
		listener.on_offered_incompatible_qos = on_offered_incompatible_qos;
	}

	void setOn_liveliness_lost(void (*on_liveliness_lost)(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status))
	{
		listener.on_liveliness_lost = on_liveliness_lost;
	}

	void setOn_publication_matched(void (*on_publication_matched)(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status))
	{
		listener.on_publication_matched = on_publication_matched;
	}

	void setOn_requested_deadline_missed(void (*on_requested_deadline_missed)(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status))
	{
		listener.on_requested_deadline_missed = on_requested_deadline_missed;
	}

	void setOn_requested_incompatible_qos(void (*on_requested_incompatible_qos)(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status))
	{
		listener.on_requested_incompatible_qos = on_requested_incompatible_qos;
	}

	void setOn_sample_rejected(void (*on_sample_rejected)(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status))
	{
		listener.on_sample_rejected = on_sample_rejected;
	}

	void setOn_liveliness_changed(void (*on_liveliness_changed)(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status))
	{
		listener.on_liveliness_changed = on_liveliness_changed;
	}

	void setOn_data_available(void (*on_data_available)(in_dds DataReader *p_reader))
	{
		listener.on_data_available = on_data_available;
	}

	void setOn_subscription_matched(void (*on_subscription_matched)(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status))
	{
		listener.on_subscription_matched = on_subscription_matched;
	}

	void setOn_sample_lost(void (*on_sample_lost)(in_dds DataReader *the_reader,in_dds SampleLostStatus *status))
	{
		listener.on_sample_lost = on_sample_lost;
	}

	void setOn_data_on_readers(void (*on_data_on_readers)(in_dds Subscriber *p_subscriber))
	{
		listener.on_data_on_readers = on_data_on_readers;
	}
#endif

private:

	DomainParticipantListener listener;

public:
	static DomainParticipantListenerCallBack *_callback;
};


class PublisherListenerCallBack
{
public:
	virtual ~PublisherListenerCallBack() {}
	virtual void on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)=0;
	virtual void on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)=0;
	virtual void on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)=0;
	virtual void on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)=0;
};

#ifndef SWIG_ON

void PublisherListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status);
void PublisherListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status);
void PublisherListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status);
void PublisherListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status);

#endif

class PublisherListener2
{
public:
	PublisherListener2(PublisherListener *p_listener)
	{
		memcpy(&listener, p_listener, sizeof(PublisherListener));
		//_callback = NULL;
	}

	PublisherListener2()
	{
		memset(&listener, 0, sizeof(PublisherListener));
		//_callback = NULL;
	}

	PublisherListener getListener()
	{
		return listener;
	}

	PublisherListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(PublisherListenerCallBack *cb) {
		_callback = cb;
		setOn_offered_deadline_missed(PublisherListener_on_offered_deadline_missed);
		setOn_offered_incompatible_qos(PublisherListener_on_offered_incompatible_qos);
		setOn_liveliness_lost(PublisherListener_on_liveliness_lost);
		setOn_publication_matched(PublisherListener_on_publication_matched);
	}

#ifndef SWIG_ON
	void setOn_offered_deadline_missed(void (*on_offered_deadline_missed)(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status))
	{
		listener.on_offered_deadline_missed = on_offered_deadline_missed;
	}

	void setOn_offered_incompatible_qos(void (*on_offered_incompatible_qos)(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status))
	{
		listener.on_offered_incompatible_qos = on_offered_incompatible_qos;
	}

	void setOn_liveliness_lost(void (*on_liveliness_lost)(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status))
	{
		listener.on_liveliness_lost = on_liveliness_lost;
	}

	void setOn_publication_matched(void (*on_publication_matched)(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status))
	{
		listener.on_publication_matched = on_publication_matched;
	}
#endif
private:
	PublisherListener listener;
public:
	static PublisherListenerCallBack *_callback;
};




class SubscriberListenerCallBack
{
public:
	virtual ~SubscriberListenerCallBack() {}
	virtual void on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)=0;
	virtual void on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)=0;
	virtual void on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)=0;
	virtual void on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)=0;
	virtual void on_data_available(in_dds DataReader *p_reader)=0;
	virtual void on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)=0;
	virtual void on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)=0;
	virtual void on_data_on_readers(in_dds Subscriber *p_subscriber)=0;

};

#ifndef SWIG_ON

void SubscriberListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status);
void SubscriberListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status);
void SubscriberListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status);
void SubscriberListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status);
void SubscriberListener_on_data_available(in_dds DataReader *p_reader);
void SubscriberListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status);
void SubscriberListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status);
void SubscriberListener_on_data_on_readers(in_dds Subscriber *p_subscriber);

#endif


class SubscriberListener2
{
public:
	SubscriberListener2(SubscriberListener *p_listener)
	{
		memcpy(&listener, p_listener, sizeof(SubscriberListener));
	}

	SubscriberListener2()
	{
		memset(&listener, 0, sizeof(SubscriberListener));
	}

	SubscriberListener getListener()
	{
		return listener;
	}

	SubscriberListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(SubscriberListenerCallBack *cb) {
		_callback = cb;
		setOn_requested_deadline_missed(SubscriberListener_on_requested_deadline_missed);
		setOn_requested_incompatible_qos(SubscriberListener_on_requested_incompatible_qos);
		setOn_sample_rejected(SubscriberListener_on_sample_rejected);
		setOn_liveliness_changed(SubscriberListener_on_liveliness_changed);
		setOn_data_available(SubscriberListener_on_data_available);
		setOn_subscription_matched(SubscriberListener_on_subscription_matched);
		setOn_sample_lost(SubscriberListener_on_sample_lost);
		setOn_data_on_readers(SubscriberListener_on_data_on_readers);
	}

#ifndef SWIG_ON
	void setOn_requested_deadline_missed(void (*on_requested_deadline_missed)(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status))
	{
		listener.on_requested_deadline_missed = on_requested_deadline_missed;
	}

	void setOn_requested_incompatible_qos(void (*on_requested_incompatible_qos)(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status))
	{
		listener.on_requested_incompatible_qos = on_requested_incompatible_qos;
	}

	void setOn_sample_rejected(void (*on_sample_rejected)(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status))
	{
		listener.on_sample_rejected = on_sample_rejected;
	}

	void setOn_liveliness_changed(void (*on_liveliness_changed)(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status))
	{
		listener.on_liveliness_changed = on_liveliness_changed;
	}

	void setOn_data_available(void (*on_data_available)(in_dds DataReader *p_reader))
	{
		listener.on_data_available = on_data_available;
	}

	void setOn_subscription_matched(void (*on_subscription_matched)(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status))
	{
		listener.on_subscription_matched = on_subscription_matched;
	}

	void setOn_sample_lost(void (*on_sample_lost)(in_dds DataReader *the_reader,in_dds SampleLostStatus *status))
	{
		listener.on_sample_lost = on_sample_lost;
	}

	void setOn_data_on_readers(void (*on_data_on_readers)(in_dds Subscriber *p_subscriber))
	{
		listener.on_data_on_readers = on_data_on_readers;
	}

#endif
private:
	SubscriberListener listener;
public:
	static SubscriberListenerCallBack *_callback;
};


class TopicListenerCallBack
{
public:
	virtual ~TopicListenerCallBack() {}
	virtual void on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)=0;

};

#ifndef SWIG_ON

void TopicListener_on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status);

#endif

class TopicListener2
{
public:
	TopicListener2(TopicListener *p_listener)
	{
		memcpy(&listener, p_listener, sizeof(TopicListener));
	}

	TopicListener2()
	{
		memset(&listener, 0, sizeof(TopicListener));
	}

	TopicListener getListener()
	{
		return listener;
	}

	TopicListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(TopicListenerCallBack *cb) {
		_callback = cb;
		setOn_inconsistent_topic(TopicListener_on_inconsistent_topic);
	}

#ifndef SWIG_ON
	void setOn_inconsistent_topic(void (*on_inconsistent_topic)(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status))
	{
		listener.on_inconsistent_topic = on_inconsistent_topic;
	}
#endif
private:
	TopicListener listener;
public:
	static TopicListenerCallBack *_callback;
};


class DataWriterListenerCallBack
{
public:
	virtual ~DataWriterListenerCallBack() {}
	virtual void on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)=0;
	virtual void on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)=0;
	virtual void on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)=0;
	virtual void on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)=0;
};

#ifndef SWIG_ON


void DataWriterListener_on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status);
void DataWriterListener_on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status);
void DataWriterListener_on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status);
void DataWriterListener_on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status);

#endif



class DataWriterListener2
{
public:
	DataWriterListener2(DataWriterListener *p_listener)
	{
		memcpy(&listener, p_listener, sizeof(DataWriterListener));
	}

	DataWriterListener2()
	{
		memset(&listener, 0, sizeof(DataWriterListener));
	}

	DataWriterListener getListener()
	{
		return listener;
	}

	DataWriterListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(DataWriterListenerCallBack *cb) {
		_callback = cb;
		setOn_offered_deadline_missed(DataWriterListener_on_offered_deadline_missed);
		setOn_offered_incompatible_qos(DataWriterListener_on_offered_incompatible_qos);
		setOn_liveliness_lost(DataWriterListener_on_liveliness_lost);
		setOn_publication_matched(DataWriterListener_on_publication_matched);
	}

#ifndef SWIG_ON
	void setOn_offered_deadline_missed(void (*on_offered_deadline_missed)(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status))
	{
		listener.on_offered_deadline_missed = on_offered_deadline_missed;
	}

	void setOn_offered_incompatible_qos(void (*on_offered_incompatible_qos)(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status))
	{
		listener.on_offered_incompatible_qos = on_offered_incompatible_qos;
	}

	void setOn_liveliness_lost(void (*on_liveliness_lost)(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status))
	{
		listener.on_liveliness_lost = on_liveliness_lost;
	}

	void setOn_publication_matched(void (*on_publication_matched)(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status))
	{
		listener.on_publication_matched = on_publication_matched;
	}

#endif

private:
	DataWriterListener listener;

public:
	static DataWriterListenerCallBack *_callback;
};



class DataReaderListenerCallBack
{
public:
	virtual ~DataReaderListenerCallBack() {}
	virtual void on_inconsistent_topic(in_dds Topic *the_topic, in_dds InconsistentTopicStatus *status)=0;
	virtual void on_offered_deadline_missed(in_dds DataWriter *writer,in_dds OfferedDeadlineMissedStatus *status)=0;
	virtual void on_offered_incompatible_qos(in_dds DataWriter *writer,in_dds OfferedIncompatibleQosStatus *status)=0;
	virtual void on_liveliness_lost(in_dds DataWriter *writer,in_dds LivelinessLostStatus *status)=0;
	virtual void on_publication_matched(in_dds DataWriter *writer,in_dds PublicationMatchedStatus *status)=0;
	virtual void on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status)=0;
	virtual void on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status)=0;
	virtual void on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status)=0;
	virtual void on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status)=0;
	virtual void on_data_available(in_dds DataReader *p_reader)=0;
	virtual void on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status)=0;
	virtual void on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status)=0;
	virtual void on_data_on_readers(in_dds Subscriber *p_subscriber)=0;

};

#ifndef SWIG_ON


void DataReaderListener_on_requested_deadline_missed(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status);
void DataReaderListener_on_requested_incompatible_qos(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status);
void DataReaderListener_on_sample_rejected(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status);
void DataReaderListener_on_liveliness_changed(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status);
void DataReaderListener_on_data_available(in_dds DataReader *p_reader);
void DataReaderListener_on_subscription_matched(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status);
void DataReaderListener_on_sample_lost(in_dds DataReader *the_reader,in_dds SampleLostStatus *status);

#endif

class DataReaderListener2
{
public:

	DataReaderListener2(DataReaderListener *p_listener)
	{
		memcpy(&listener, p_listener, sizeof(DataReaderListener));
	}

	DataReaderListener2()
	{
		memset(&listener, 0, sizeof(DataReaderListener));
	}

	DataReaderListener getListener()
	{
		return listener;
	}

	DataReaderListener *getListenerPtr()
	{
		return &listener;
	}

	void setCallback(DataReaderListenerCallBack *cb) {
		_callback = cb;
		setOn_requested_deadline_missed(DataReaderListener_on_requested_deadline_missed);
		setOn_requested_incompatible_qos(DataReaderListener_on_requested_incompatible_qos);
		setOn_sample_rejected(DataReaderListener_on_sample_rejected);
		setOn_liveliness_changed(DataReaderListener_on_liveliness_changed);
		setOn_data_available(DataReaderListener_on_data_available);
		setOn_subscription_matched(DataReaderListener_on_subscription_matched);
		setOn_sample_lost(DataReaderListener_on_sample_lost);
	}

#ifndef SWIG_ON
	void setOn_requested_deadline_missed(void (*on_requested_deadline_missed)(in_dds DataReader *the_reader,in_dds RequestedDeadlineMissedStatus *status))
	{
		listener.on_requested_deadline_missed = on_requested_deadline_missed;
	}

	void setOn_requested_incompatible_qos(void (*on_requested_incompatible_qos)(in_dds DataReader *the_reader,in_dds RequestedIncompatibleQosStatus *status))
	{
		listener.on_requested_incompatible_qos = on_requested_incompatible_qos;
	}

	void setOn_sample_rejected(void (*on_sample_rejected)(in_dds DataReader *the_reader,in_dds SampleRejectedStatus *status))
	{
		listener.on_sample_rejected = on_sample_rejected;
	}

	void setOn_liveliness_changed(void (*on_liveliness_changed)(in_dds DataReader *the_reader,in_dds LivelinessChangedStatus *status))
	{
		listener.on_liveliness_changed = on_liveliness_changed;
	}

	void setOn_data_available(void (*on_data_available)(in_dds DataReader *p_reader))
	{
		listener.on_data_available = on_data_available;
	}

	void setOn_subscription_matched(void (*on_subscription_matched)(in_dds DataReader *the_reader,in_dds SubscriptionMatchedStatus *status))
	{
		listener.on_subscription_matched = on_subscription_matched;
	}

	void setOn_sample_lost(void (*on_sample_lost)(in_dds DataReader *the_reader,in_dds SampleLostStatus *status))
	{
		listener.on_sample_lost = on_sample_lost;
	}
#endif

private:
	DataReaderListener listener;
public:
	static DataReaderListenerCallBack *_callback;
};



} // namespace cps end




#endif