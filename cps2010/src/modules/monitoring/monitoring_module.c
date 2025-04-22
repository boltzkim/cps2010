#include "monitoring_module.h"



void register_type_module_monitoring(DomainParticipant* p_participant)
{
	if (p_participant == NULL) return;
	{
		FooTypeSupport* p_foo_typesupport;
		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_ParticipantDomain_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_ParticipantDomain);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:ParticipantDomain");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_Topic_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_Topic);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:Topic");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_Publisher_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_Publisher);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:Publisher");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_Subscriber_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_Subscriber);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:Subscriber");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_DataWriter_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_DataWriter);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:DataWriter");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_DataReader_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_DataReader);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:DataReader");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_DataBandwidth_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_DataBandwidth);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:DataBandwidth");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_WriterProxy_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_WriterProxy);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:WriterProxy");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_ReaderProxy_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_ReaderProxy);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:ReaderProxy");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_WarningErrorConditions_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_WarningErrorConditions);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:WarningErrorConditions");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_SamplesinforamtionReader_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_SamplesinforamtionReader);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:SamplesinforamtionReader");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_SamplesinforamtionWriter_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_SamplesinforamtionWriter);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:SamplesinforamtionWriter");

		p_foo_typesupport = get_new_foo_type_support();
		insert_monitoring_Log_Parameters(p_foo_typesupport);
		p_foo_typesupport->i_size = sizeof(struct monitoring_Log);
		p_foo_typesupport->register_type(p_foo_typesupport, p_participant, "monitoring:Log");
	}
}

