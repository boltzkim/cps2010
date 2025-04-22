#include "InParticipant.h"



InParticipant::InParticipant(GuidPrefix_t guid_prefix)
{
	b_end = false;
	isChanged = false;
	this->guid_prefix = guid_prefix;

	printf("New InParticipant :");

	for (int i = 0; i < sizeof(GuidPrefix_t); i++)
	{
		printf("%03d ", guid_prefix._guidprefix[i]);
	}

	//printf("\n");

	
	memset(&UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&MULTICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&DEFAULT_UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&DEFAULT_MULTICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&METATRAFFIC_UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&METATRAFFIC_MULTICAST_LOCATOR, 0, sizeof(Locator_t));

	update_time = currenTime();
	b_using = true;
}




InParticipant::~InParticipant()
{

	printf("Delete InParticipant\n");
	b_end = true;

}


void InParticipant::change_prefix(GuidPrefix_t _guid_prefix)
{
	this->guid_prefix = _guid_prefix;


	
}

void InParticipant::setUsing(bool use)
{

	b_using = use;

	if (use)
	{
		printf("setUsing(true) InParticipant :");
	}
	else {

		printf("setUsing(false) InParticipant :");
	}

	for (int i = 0; i < sizeof(GuidPrefix_t); i++)
	{
		printf("%03d ", guid_prefix._guidprefix[i]);
	}

	if (use == false) {
		printf("\n");
	}
	
}



static void PrintLocator(char *name, Locator_t a_locator)
{
	if (a_locator.kind == 0) return;

	printf("%s : ", name);
	if (a_locator.kind != 0)
	{
		if (a_locator.kind == LOCATOR_KIND_UDPv4)
		{
			printf("v4 ");
			printf(", IP : %d.%d.%d.%d, Port : %d\n", a_locator.address[12], a_locator.address[13], a_locator.address[14], a_locator.address[15], a_locator.port);
		}
		else { 
			printf("v6 ");
			printf(", IP : %s, Port : %d\n", a_locator.address, a_locator.port);
		}
	}
}

void InParticipant::print()
{
	/*if (isChanged)
	{ 
		printf("Print InParticipant :");

		for (int i = 0; i < sizeof(GuidPrefix_t); i++)
		{
			printf("%02x ", guid_prefix._guidprefix[i]);
		}

		printf("\n");

		PrintLocator("UNICAST_LOCATOR", UNICAST_LOCATOR);
		PrintLocator("MULTICAST_LOCATOR", MULTICAST_LOCATOR);
		PrintLocator("DEFAULT_UNICAST_LOCATOR", DEFAULT_UNICAST_LOCATOR);
		PrintLocator("DEFAULT_MULTICAST_LOCATOR", DEFAULT_MULTICAST_LOCATOR);
		PrintLocator("METATRAFFIC_UNICAST_LOCATOR", METATRAFFIC_UNICAST_LOCATOR);
		PrintLocator("METATRAFFIC_MULTICAST_LOCATOR", METATRAFFIC_MULTICAST_LOCATOR);

		isChanged = false;
	}*/
}
