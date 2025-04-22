#include <core.hh>

namespace cps
{


DomainParticipantFactory*	DomainParticipantFactory2::p_dpf;
DomainParticipantFactory2	DomainParticipantFactory2::s_DomainParticipantFactory2;

	
DomainParticipantFactory2::DomainParticipantFactory2()
{
	
	if(p_dpf == NULL)
	{
		p_dpf = DomainParticipantFactory_get_instance();
	}
}

DomainParticipantFactory2 DomainParticipantFactory2::get_instance()
{
	
	return s_DomainParticipantFactory2;
}

DomainParticipant2 DomainParticipantFactory2::create_participant(in_dds DomainId_t domain_id,in_dds DomainParticipantQos qos,in_dds DomainParticipantListener2 &listener,in_dds StatusMask mask)
{

	DomainParticipant *domainparticipant = p_dpf->create_participant(domain_id, &qos, listener.getListenerPtr(), mask);
	DomainParticipant2 participant(domainparticipant);
	return participant;
}


ReturnCode_t DomainParticipantFactory2::delete_participant(DomainParticipant2 participant)
{
	return p_dpf->delete_participant(participant.getDomainParticipant());
}


DomainParticipant2 DomainParticipantFactory2::lookup_participant(in_dds DomainId_t domain_id)
{
	DomainParticipant *domainparticipant = p_dpf->lookup_participant(domain_id);

	DomainParticipant2 participant(domainparticipant);
	return participant;
}

ReturnCode_t DomainParticipantFactory2::set_default_participant_qos(in_dds DomainParticipantQos qos)
{
	return p_dpf->set_default_participant_qos(&qos);
}

ReturnCode_t DomainParticipantFactory2::get_default_participant_qos(inout_dds DomainParticipantQos &qos)
{
	return p_dpf->get_default_participant_qos(&qos);
}

ReturnCode_t DomainParticipantFactory2::set_qos(in_dds DomainParticipantFactoryQos qos)
{
	return p_dpf->set_qos(&qos);
}

ReturnCode_t DomainParticipantFactory2::get_qos(inout_dds DomainParticipantFactoryQos &qos)
{
	return p_dpf->get_qos(&qos);
}


} // namespace cps end
