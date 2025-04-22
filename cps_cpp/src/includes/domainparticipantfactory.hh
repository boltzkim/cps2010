#if !defined(__DOMAINPARTICIPANTFACTORY_HH__)
#define __DOMAINPARTICIPANTFACTORY_HH__

namespace cps
{


class DomainParticipantFactory2
{
private:
	int32_t		module_id;
	
	static		DomainParticipantFactory*	p_dpf;
	static		DomainParticipantFactory2	s_DomainParticipantFactory2;
public:
	DomainParticipantFactory2();
	static DomainParticipantFactory2 get_instance();
	DomainParticipant2 create_participant(in_dds DomainId_t domain_id,in_dds DomainParticipantQos qos,in_dds DomainParticipantListener2 &listener,in_dds StatusMask mask);
	
	ReturnCode_t delete_participant(in_dds DomainParticipant2 participant);
	DomainParticipant2 lookup_participant(in_dds DomainId_t domain_id);
	ReturnCode_t set_default_participant_qos(in_dds DomainParticipantQos qos);
	ReturnCode_t get_default_participant_qos(inout_dds DomainParticipantQos &qos);
	ReturnCode_t set_qos(in_dds DomainParticipantFactoryQos qos);
	ReturnCode_t get_qos(inout_dds DomainParticipantFactoryQos &qos);

};


} // namespace cps end




#endif