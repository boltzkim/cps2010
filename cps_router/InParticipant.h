#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <../modules/rtps/rtps.h>

class InParticipant
{
public:
	InParticipant(GuidPrefix_t guid_prefix);
	~InParticipant();


	void change_prefix(GuidPrefix_t _guid_prefix);
	void setUsing(bool use);

	bool b_using;

public:

	void print();

	GuidPrefix_t guid_prefix;


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

	Locator_t METATRAFFIC_MULTICAST_LOCATOR;
	Locator_t DEFAULT_UNICAST_LOCATOR;


	void Update()
	{
		update_time = currenTime();
	}

	Time_t UpdateTime()
	{
		return update_time;
	}

	bool b_end;

private:
	bool isChanged;
	Locator_t UNICAST_LOCATOR;
	Locator_t MULTICAST_LOCATOR;
	
	Locator_t DEFAULT_MULTICAST_LOCATOR;
	Locator_t METATRAFFIC_UNICAST_LOCATOR;


	Time_t update_time;
	
};

