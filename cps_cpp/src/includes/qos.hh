#if !defined(__QOS_HH__)
#define __QOS_HH__

namespace cps
{

class DomainParticipantQos2
{
public:
	DomainParticipantQos2()
	{
		memset(&qos, 0, sizeof(DomainParticipantQos));
	}

	DomainParticipantQos getQos()
	{
		return qos;
	}

private:
	DomainParticipantQos qos;
};

} // namespace cps end




#endif