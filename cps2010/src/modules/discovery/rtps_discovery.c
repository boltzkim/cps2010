/*
	RTPS 서비스 모듈
	작성자 : 
	이력
	2010-09-27 : 처음 시작
*/


#include "rtps_discovery.h"


struct discovery_sys_t
{
	bool			b_not;
	access_t		*p_access;
	char			*psz_path;
};

#define MODULE_PREFIX "rtps-discovery"


#define MULTICAST_ADD_TEXT "Multicast address."
#define MULTICAST_ADD_LONGTEXT "RTPS Discovery Multicast Address. It's default address is 239.255.0.1"

#define MULTICAST_PORT_TEXT "Multicast port"
#define MULTICAST_PORT_LONGTEXT "RTPS Discovery Multicast Address. It's default address is 7400"

static int  Open  ( module_object_t * );
static void Close ( module_object_t * );

module_define_start(rtpsDiscovery)
	set_category( CAT_DISCOVERY );
	set_description( "RTPS Discovery" );
	set_capability( "discovery", 200 );
	add_string(MODULE_PREFIX"multicast-addr","239.255.0.1",NULL,MULTICAST_ADD_TEXT,MULTICAST_ADD_LONGTEXT, false);
	add_integer(MODULE_PREFIX"multicast-port",7400,NULL, MULTICAST_PORT_TEXT, MULTICAST_PORT_LONGTEXT, false);
	set_callbacks( Open, Close );
	add_shortcut( "rtsp-discovery" );
module_define_end()

static void ServiceThread(module_object_t *p_this);


Locator_t defaultUnicastLocator;
Locator_t defaultMulticastLocator;


static int SetStringAddressToLocatorAddress(char* p_address, octet *p_locatoraddress);

static int  Open(module_object_t *p_this)
{

	/*discovery_t *p_discovery = (discovery_t *)p_this;
	module_t *p_module = p_this->p_module;
	char	*psz_addr = NULL;
	char	*psz_addr_uni = NULL;
	int		i_port;

	if(!p_module)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Use Module \"RTPS Discovery\" module. p_module is NULL.");
		return MODULE_ERROR_CREATE;
	}
	
	p_discovery->p_sys = malloc(sizeof(discovery_sys_t));
	if(!p_discovery->p_sys)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Allocate discovery_sys_t in \"RTPS Discovery\" module.");
		return MODULE_ERROR_MEMORY;
	}
	memset(p_discovery->p_sys,'\0',sizeof(discovery_sys_t));

	psz_addr = varCreateGetString(p_this,MODULE_PREFIX"multicast-addr");
	i_port = varCreateGetInteger(p_this,MODULE_PREFIX"multicast-port");
	
	defaultMulticastLocator.kind = LOCATOR_KIND_UDPv4;
	defaultMulticastLocator.port = i_port;
	

	SetStringAddressToLocatorAddress(psz_addr, defaultMulticastLocator.address);
	psz_addr_uni = getDefaultLocalAddress();
	if(psz_addr_uni)
	{
		SetStringAddressToLocatorAddress(psz_addr_uni, defaultUnicastLocator.address);
	}

	FREE(psz_addr_uni);

	asprintf(&p_discovery->p_sys->psz_path,"udp:@%s:%d",psz_addr, i_port);

	FREE(psz_addr);
	

	p_discovery->p_sys->p_access = accessNew(p_this, "udp4", p_discovery->p_sys->psz_path);

	if(!p_discovery->p_sys->p_access)
	{
		FREE(p_discovery->p_sys->p_access);
		return MODULE_ERROR_NETWORK;
	}

	trace_msg(OBJECT(p_this),TRACE_DEBUG,"RTPS Discovery Service Started.. %s",p_discovery->p_sys->psz_path);


	if( thread_create( OBJECT(p_discovery), "discovery", (void*)ServiceThread, 0, false ) )
    {
		FREE(p_discovery->p_sys);
		return MODULE_ERROR_CREATE;
    }

	if(rtps_period_thread_start(p_discovery) != MODULE_SUCCESS)
	{
		Close(p_this);
		return MODULE_ERROR_THREAD;
	}*/

	return MODULE_SUCCESS;
}


static void Close(module_object_t *p_this)
{
	//discovery_t *p_discovery = (discovery_t *)p_this;
	//discovery_sys_t *p_sys = p_discovery->p_sys;

	//p_sys->p_access->b_end = true;
	//p_discovery->b_end = true;
	//FREE(p_sys->psz_path);

	//rtps_period_thread_end(p_discovery);
	//thread_join(OBJECT(p_this));

	//if(p_sys->p_access)
	//{
	//	accessDelete(p_sys->p_access);
	//}

	//FREE(p_discovery->p_sys);
}

static DataFull *processReceivedData(discovery_t *p_discovery, data_t *p_data);

static void ServiceThread(module_object_t *p_this)
{
	discovery_t *p_discovery = (discovery_t *)p_this;
	discovery_sys_t *p_sys = p_discovery->p_sys;
	access_t *p_access = NULL;
	

	if(!p_sys || !p_sys->p_access)
	{
		return;
	}

	p_access = p_sys->p_access;


	thread_ready((module_object_t *)p_discovery);


	while(!p_discovery->b_end)
	{
		processReceivedData(p_discovery, p_access->pf_data(p_access));
	}
}


static DataFull *processReceivedData(discovery_t *p_discovery, data_t *p_data)
{
	DataFull *p_ret = NULL;
	if(p_data)
	{
		trace_msg(OBJECT(p_discovery),TRACE_TRACE,"received data %d", p_data->i_size);
//		p_ret = rtps_message_parser(OBJECT(p_discovery), p_data->p_data, p_data->i_size);

		data_release(p_data);
	}

	return p_ret;

}

static int32_t SetStringAddressToLocatorAddress(char* p_address, octet *p_locatoraddress)
{
	int32_t ad = inet_addr(p_address);
	memset(p_locatoraddress,0,16);

	*((int32_t*)&p_locatoraddress[12]) = inet_addr(p_address);

	return 1;
}



/*
	The RTPS Simple Participant Discovery Protocol (SPDP) uses a simple approach to announce and detect the presence of
	Participants in a domain.
	For each Participant, the SPDP creates two RTPS built-in Endpoints: the SPDPbuiltinParticipantWriter and the
	SPDPbuiltinParticipantReader.
	The SPDPbuiltinParticipantWriter is an RTPS Best-Effort StatelessWriter. The HistoryCache of the
	SPDPbuiltinParticipantWriter contains a single data-object of type SPDPdiscoveredParticipantData. The value of this
	data-object is set from the attributes in the Participant. If the attributes change, the data-object is replaced.
	The SPDPbuiltinParticipantWriter periodically sends this data-object to a pre-configured list of locators to announce the
	Participant’s presence on the network. This is achieved by periodically calling StatelessWriter::unsent_changes_reset,
	which causes the StatelessWriter to resend all changes present in its HistoryCache to all locators. The periodic rate at
	which the SPDPbuiltinParticipantWriter sends out the SPDPdiscoveredParticipantData defaults to a PSM specified
	value. This period should be smaller than the leaseDuration specified in the SPDPdiscoveredParticipantData (see also
	Section 8.5.3.3.2).
	The pre-configured list of locators may include both unicast and multicast locators. Port numbers are defined by each
	PSM. These locators simply represent possible remote Participants in the network, no Participant need actually be present.
	By sending the SPDPdiscoveredParticipantData periodically, Participants can join the network in any order.
	The SPDPbuiltinParticipantReader receives the SPDPdiscoveredParticipantData announcements from the remote
	Participants. The contained information includes what Endpoint Discovery Protocols the remote Participant supports. The
	proper Endpoint Discovery Protocol is then used for exchanging Endpoint information with the remote Participant.
	Implementations can minimize any start-up delays by sending an additional SPDPdiscoveredParticipantData in response
	to receiving this data-object from a previously unknown Participant, but this behavior is optional. Implementations may
	also enable the user to choose whether to automatically extend the pre-configured list of locators with new locators from
	newly discovered Participants. This enables a-symmetric locator lists. These last two features are optional and not
	required for the purpose of interoperability.
*/

