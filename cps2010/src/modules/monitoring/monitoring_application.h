#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include "monitoring_module.h"


typedef struct monitoring_qos_t
{
	DomainParticipantFactoryQos m_domainparticipantfactoryqos;
	DomainParticipantQos m_domainparticipantqos;
	TopicQos m_topicqos;
	SubscriberQos m_subscriberqos;
	PublisherQos m_publisherqos;
	DataWriterQos m_datawriterqos;
	DataReaderQos m_datareaderqos;
} monitoring_qos_t;


typedef struct monitoring2_qos_t
{
	DomainParticipantFactoryQos m_domainparticipantfactoryqos;
	DomainParticipantQos m_domainparticipantqos;
	TopicQos m_topicqos;
	SubscriberQos m_subscriberqos;
	PublisherQos m_publisherqos;
	DataWriterQos m_datawriterqos;
	DataReaderQos m_datareaderqos;
} monitoring2_qos_t;


#ifdef __cplusplus
extern "C" {
#endif

void read_ParticipantDomain(monitoring_ParticipantDomain a_participantdomain_data);
void write_ParticipantDomain(monitoring_ParticipantDomain a_participantdomain_data);

void read_Topic(monitoring_Topic a_Topic_data);
void write_Topic(monitoring_Topic a_Topic_data);

void read_Publisher(monitoring_Publisher a_Publisher_data);
void write_Publisher(monitoring_Publisher a_Publisher_data);

void read_Subscriber(monitoring_Subscriber a_Subscriber_data);
void write_Subscriber(monitoring_Subscriber a_Subscriber_data);

void read_DataWriter(monitoring_DataWriter a_DataWriter_data);
void write_DataWriter(monitoring_DataWriter a_DataWriter_data);

void read_DataReader(monitoring_DataReader a_DataReader_data);
void write_DataReader(monitoring_DataReader a_DataReader_data);

void read_DataBandwidth(monitoring_DataBandwidth a_DataBandwidth_data);
void write_DataBandwidth(monitoring_DataBandwidth a_DataBandwidth_data);

void read_WriterProxy(monitoring_WriterProxy a_WriterProxy_data);
void write_WriterProxy(monitoring_WriterProxy a_WriterProxy_data);

void read_ReaderProxy(monitoring_ReaderProxy a_ReaderProxy_data);
void write_ReaderProxy(monitoring_ReaderProxy a_ReaderProxy_data);

void read_WarningErrorConditions(monitoring_WarningErrorConditions a_WarningErrorConditions_data);
void write_WarningErrorConditions(monitoring_WarningErrorConditions a_WarningErrorConditions_data);

void read_SamplesinforamtionReader(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data);
void write_SamplesinforamtionReader(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data);

void read_SamplesinforamtionWriter(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data);
void write_SamplesinforamtionWriter(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data);
void monitoring_wrapper_application();

#ifdef __cplusplus
}
#endif