#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <../modules/rtps/rtps.h>

#include <string>
#include <thread>

class RemoteBroker;

class RemoteParticipant
{
public:
	RemoteParticipant(RemoteBroker *p_remoteBroker, GuidPrefix_t guid_prefix);
	~RemoteParticipant();

	static void Network(RemoteParticipant *p_RemoteParticipant);

	bool rtps_data_parse(uint8_t *p_org_data, int i_org_size, uint8_t *p_data, ushort i_size, octet flag);
	void sendPDP(uint8_t *p_org_data, int i_org_size);
	void getData();

	void change_prefix(GuidPrefix_t _guid_prefix);
	void setUsing(bool use);

	GuidPrefix_t guid_prefix;
	RemoteBroker *p_remoteBroker;
	bool b_end;
	bool b_using;

private:

	bool changeUnicast(Locator_t *p_locator, int port);
	bool changeMulticast(Locator_t *p_locator, int port);
	


	void setUNICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&UNICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			UNICAST_LOCATOR = a_locator;
			isChanged = true;
		}

	}

	void setMULTICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&MULTICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			MULTICAST_LOCATOR = a_locator;
			isChanged = true;
		}
	}

	void setDEFAULT_UNICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&DEFAULT_UNICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			DEFAULT_UNICAST_LOCATOR = a_locator;
			isChanged = true;
		}
	}

	void setDEFAULT_MULTICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&DEFAULT_MULTICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			DEFAULT_MULTICAST_LOCATOR = a_locator;
			isChanged = true;
		}
	}

	void setMETATRAFFIC_UNICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&METATRAFFIC_UNICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			METATRAFFIC_UNICAST_LOCATOR = a_locator;
			isChanged = true;
		}
	}

	void setMETATRAFFIC_MULTICAST_LOCATOR(Locator_t a_locator)
	{
		if (memcmp(&METATRAFFIC_MULTICAST_LOCATOR, &a_locator, sizeof(Locator_t)) != 0)
		{
			METATRAFFIC_MULTICAST_LOCATOR = a_locator;
			isChanged = true;
		}
	}

public:
	void Update()
	{
		update_time = currenTime();
	}

	Time_t UpdateTime()
	{
		return update_time;
	}

private:
	bool isChanged;
	Locator_t UNICAST_LOCATOR;
	Locator_t MULTICAST_LOCATOR;
	Locator_t DEFAULT_UNICAST_LOCATOR;
	Locator_t DEFAULT_MULTICAST_LOCATOR;
	Locator_t METATRAFFIC_UNICAST_LOCATOR;
	Locator_t METATRAFFIC_MULTICAST_LOCATOR;


private:
	struct sockaddr_in	bind_addr;
	std::string			bind_ip;
	int					bind_port;

	access_t			*p_access;
	access_out_t		*p_accessout;

	std::thread			*thread_id;
	Time_t update_time;
	
};

