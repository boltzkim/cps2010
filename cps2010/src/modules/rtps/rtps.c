/*
	RTPS 서비스 모듈
	작성자 : 
	이력
	2010-07-26 : 처음 시작
*/


#include "rtps.h"
#include "print_whs_rhs.h"

#define MAX_THREAD_NUM 256

struct service_sys_t
{
	bool			b_not;
	access_t*		p_spdp_access;
	access_t*		p_sedp_access;
	access_t*		p_access;
	//
	access_out_t*	p_spdp_accessout;
	access_out_t*	p_sedp_accessout;
	access_out_t*	p_accessout;
	//
	char*			psz_path;
	//
	char*			psz_addr;
	int				i_spdp_port;
	int				i_sedp_port;
	int				i_port;
	//
	mutex_t			spdp_lock;
    cond_t			spdp_wait;
	mutex_t			sedp_lock;
    cond_t			sedp_wait;
	mutex_t			default_lock;
    cond_t			default_wait;
	////////
	module_thread_t threadID_SPDP;
	module_thread_t threadID_SEDP;
	module_thread_t threadID_DEFAULT;
	int				i_fragmentsize;
	//by jun
	module_thread_t threadID_network[MAX_THREAD_NUM];
	int				network_thread_count;	
	module_thread_t threadID_ThreadWriter;
	bool			reliable_multicast_enable;
	//char			TMPBUFFER[1500];
};


static int  Open  ( module_object_t * );
static void Close ( module_object_t * );

///static void ServiceThread(module_object_t* p_this);
static int rtps_add_thread(module_object_t* p_this, Entity* p_entity);

#define MODULE_PREFIX "rtps-"


#define SPDP_MULTICAST_ADD_TEXT "SPDP Multicast address."
#define SPDP_MULTICAST_ADD_LONGTEXT "RTPS SPDP Multicast Address. It's default address is 239.255.0.1"

#define SPDP_MULTICAST_PORT_TEXT "SPDP Multicast port"
#define SPDP_MULTICAST_PORT_LONGTEXT "RTPS SPDP Multicast Address. It's default address is 7400"

#define SEDP_MULTICAST_PORT_TEXT "SEDP Multicast port"
#define SEDP_MULTICAST_PORT_LONGTEXT "RTPS SEDP Multicast Address. It's default address is 7410"

#define MULTICAST_PORT_TEXT "Multicast port"
#define MULTICAST_PORT_LONGTEXT "RTPS Multicast Address. It's default address is 7420"


#define FRAGMENT_SIZE_TEXT "Fragment Size"
#define FRAGMENT_SIZE_LONGTEXT "The Maximum fragment size is 64k"


#define RELIABLE_MULTICAST_TEXT "Reliable Writer Multicast support"
#define RELIABLE_MULTICAST_LONGTEXT "Reliable Writer send a data on multicast"

module_define_start(rtpsService)
	set_category( CAT_SERVICE );
	set_description( "RTPS Service" );
	set_capability( "service", 200 );
	add_string(MODULE_PREFIX"spdp-multicast-addr","239.255.0.1",NULL,SPDP_MULTICAST_ADD_TEXT,SPDP_MULTICAST_ADD_LONGTEXT, false);
	add_integer(MODULE_PREFIX"spdp-multicast-port",7400,NULL, SPDP_MULTICAST_PORT_TEXT, SPDP_MULTICAST_PORT_LONGTEXT, false);
	//add_integer(MODULE_PREFIX"sedp-multicast-port",7410,NULL, SEDP_MULTICAST_PORT_TEXT, SEDP_MULTICAST_PORT_LONGTEXT, false);
	//add_integer(MODULE_PREFIX"multicast-port",7420,NULL, MULTICAST_PORT_TEXT, MULTICAST_PORT_LONGTEXT, false);
	add_integer(MODULE_PREFIX"sedp-multicast-port",0,NULL, SEDP_MULTICAST_PORT_TEXT, SEDP_MULTICAST_PORT_LONGTEXT, false);
	add_integer(MODULE_PREFIX"multicast-port",0,NULL, MULTICAST_PORT_TEXT, MULTICAST_PORT_LONGTEXT, false);
	add_integer(MODULE_PREFIX"fragmentsize",64000,NULL, FRAGMENT_SIZE_TEXT, FRAGMENT_SIZE_LONGTEXT, false);
	add_bool(MODULE_PREFIX"reliable_writer_muliticast_enable",true,NULL,RELIABLE_MULTICAST_TEXT, RELIABLE_MULTICAST_LONGTEXT, false);
	set_callbacks( Open, Close );
	add_shortcut( "rtsp" );
module_define_end()


Locator_t defaultUnicastLocator;
Locator_t defaultMulticastLocator;

static int init_network(service_t* p_service);
static int SetStringAddressToLocatorAddress(char* p_address, octet* p_locatoraddress);
///////////////////////////////////
///static void ServiceSPDPThread(module_object_t *p_this);
///static void ServiceSEDPThread(module_object_t *p_this);
///static void ServiceThread(module_object_t *p_this);
static void NetworkThread(module_object_t *p_this);
static access_out_t	*GetAccessOut(module_object_t * p_this, int type);
static message_t		**rtps_read(module_object_t * p_this, DataReader *p_datareader, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
static message_t		**rtps_take(module_object_t * p_this, DataReader *p_datareader, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
static message_t		**rtps_read_instance(module_object_t * p_this, DataReader *p_datareader, InstanceHandle_t handle, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
static message_t		**rtps_take_instance(module_object_t * p_this, DataReader *p_datareader, InstanceHandle_t handle, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);

//jeman
DomainId_t g_domain_id;
void set_global_domain_id(const DomainId_t domain_id)
{
	g_domain_id = domain_id;
}
DomainId_t get_global_domain_id(void)
{
	return g_domain_id;
}


///////////////////////////////////////////////////////////////////////////
//by jun
static void rtps_threadwriter_network_thread_set(service_sys_t *p_sys);
static int Create_network(module_object_t *p_this);


static int thread_count = 1;


static int DefaultPort = 7400;
static int MetatrafficPort = 7200;

void set_default_port( int port )
{
	DefaultPort = port;
}

int get_default_port()
{
	return DefaultPort;
}

void set_metatraffic_port( int port )
{
	MetatrafficPort = port;
}

int get_metatraffic_port()
{
	return MetatrafficPort;
}



#ifdef _MSC_VER
typedef unsigned (WINAPI *PTHREAD_START) (void *);
#endif

static void Check_Thread();

static int Open(module_object_t* p_this)
{
	service_t* p_service = (service_t*)p_this;
	module_t* p_module = p_this->p_module;
	service_sys_t *p_sys = NULL;

	p_service->add_entity = rtps_add_entity;
	p_service->remove_entity = rtps_remove_entity;
	p_service->write = rtps_write;
	p_service->dispose = rtps_dispose;
	p_service->unregister = rtps_unregister;
	p_service->getaccessout = GetAccessOut;

	p_service->add_thread = rtps_add_thread;

	//
	p_service->take = rtps_take;
	p_service->read = rtps_read;
	p_service->take_instance = rtps_take_instance;
	p_service->read_instance = rtps_read_instance;

	//timestamp by jun
	p_service->write_timestamp = rtps_write_timestamp;
	p_service->dispose_timestamp = rtps_dispose_timestamp;
	p_service->unregister_timestamp = rtps_unregister_timestamp;


	////
	{
		char *local_ip = get_default_local_address();
		char *psz_if_addr = config_get_psz(OBJECT(p_module), "interface-addr");
		printf("**************************************************************\r\n");
		printf("*		local ip : %s					\r\n", local_ip);
		printf("*		interface-addr : %s				\r\n", psz_if_addr);
		printf("**************************************************************\r\n");

		if (strcmp(local_ip, "127.0.0.1") == 0)
		{
			pause_system();
		}
	}


	/// 디버거 용..
//#ifdef _MSC_VER
//	{
//		unsigned threadID;
//		(module_thread_t)_beginthreadex(NULL, 0, (PTHREAD_START)Check_Thread, NULL, 0, &threadID);
//	}
//#endif

	///

	
	////
	
	p_service->p_network_fifo = data_fifo_new();

	if (!p_module)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Use Module \"RTPS\" module. p_module is NULL.");
		return MODULE_ERROR_CREATE;
	}

	p_sys = p_service->p_sys = (service_sys_t*)malloc(sizeof(service_sys_t));
	memset(p_sys, 0, sizeof(service_sys_t));

	if (!p_service->p_sys)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Allocate service_sys_t in \"RTPS\" module.");
		return MODULE_ERROR_MEMORY;
	}


	p_sys->i_fragmentsize = var_create_get_integer(p_this,MODULE_PREFIX"fragmentsize");

	p_sys->threadID_SPDP = 0;
	p_sys->threadID_SEDP = 0;
	p_sys->threadID_DEFAULT = 0;
	
	p_sys->psz_addr = var_create_get_string(p_this,MODULE_PREFIX"spdp-multicast-addr");

	//jeman
	p_sys->i_spdp_port = var_create_get_integer(p_this,MODULE_PREFIX"spdp-multicast-port");
	p_sys->i_sedp_port = var_create_get_integer(p_this,MODULE_PREFIX"sedp-multicast-port");
	p_sys->i_port = var_create_get_integer(p_this,MODULE_PREFIX"multicast-port");

	if(p_sys->i_spdp_port != 0){
		p_sys->i_spdp_port = 7400+250*g_domain_id+0;
	}

	if(p_sys->i_sedp_port != 0){
		p_sys->i_sedp_port = 7400+250*g_domain_id+10;
	}



	p_sys->reliable_multicast_enable = var_create_get_bool(p_this, MODULE_PREFIX"reliable_writer_muliticast_enable");

	set_default_port(p_sys->i_sedp_port);
	set_metatraffic_port(p_sys->i_port);

	mutex_init(&p_sys->spdp_lock );
	cond_init(&p_sys->spdp_wait );
	mutex_init(&p_sys->sedp_lock );
	cond_init(&p_sys->sedp_wait );
	mutex_init(&p_sys->default_lock );
	cond_init(&p_sys->default_wait );

	p_sys->p_spdp_accessout = NULL;
	p_sys->p_sedp_accessout = NULL;
	p_sys->p_accessout  = NULL;
	

	if (MODULE_SUCCESS != init_network(p_service))
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Init Networ Module in \"RTPS\" module.");
		return MODULE_FAIL;
	}

	if (Create_network(OBJECT(p_this)))
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Init NetworWriter Module in \"RTPS\" module.");
		return MODULE_FAIL;
	}

	if (p_sys->p_spdp_access)
	{
		FREE(p_sys->psz_path);
		asprintf(&p_sys->psz_path,"%s:%d",p_sys->psz_addr, p_sys->i_spdp_port);
		p_sys->p_spdp_accessout = access_out_new(p_this, "udp", p_sys->psz_path, p_sys->p_spdp_access->fd);
	}

	if (p_sys->p_sedp_access)
	{
		char* p_path_temp;
		asprintf(&p_path_temp,"%s:%d",p_sys->psz_addr, p_sys->i_sedp_port);
		p_sys->p_sedp_accessout = access_out_new(p_this, "udp", p_path_temp, p_sys->p_sedp_access->fd);
		FREE(p_path_temp);
	}
		
	if (p_sys->p_access)
	{
		char* p_path_temp;
		asprintf(&p_path_temp,"%s:%d",p_sys->psz_addr, p_sys->i_port);
		p_sys->p_accessout = access_out_new(p_this, "udp", p_path_temp, p_sys->p_access->fd);
		FREE(p_path_temp);
	}

	//FREE(p_sys->psz_path);
	// It's not necessary... by pws
	//rtps_threadwriter_network_thread_set(p_sys);

	return MODULE_SUCCESS;
}


static void rtps_threadwriter_network_thread_set(service_sys_t *p_sys){

	/*p_sys->p_spdp_accessout->p_sys->p_thread->p_fifo = p_sys->p_network_fifo;
	p_sys->p_sedp_accessout->p_sys->p_thread->p_fifo = p_sys->p_network_fifo;
	p_sys->p_accessout->p_sys->p_thread->p_fifo = p_sys->p_network_fifo;*/
}
	

static void Close(module_object_t *p_this)
{
	service_t *p_service = (service_t *)p_this;
	service_sys_t *p_sys = p_service->p_sys;
	int i;

	if(p_sys->p_spdp_access) p_sys->p_spdp_access->b_end = true;
	if(p_sys->p_sedp_access) p_sys->p_sedp_access->b_end = true;
	if(p_sys->p_access) p_sys->p_access->b_end = true;

	p_service->b_end = true;
/*	
	if(p_sys->threadID_SPDP)
	{
		thread_join2(p_sys->threadID_SPDP);
	}

	if(p_sys->threadID_SEDP)
	{
		thread_join2(p_sys->threadID_SEDP);
	}

	if(p_sys->threadID_DEFAULT)
	{
		thread_join2(p_sys->threadID_DEFAULT);
	}
*/

	//print_not_free_object();
	return;



	for(i=0; i < thread_count ; i++){
		printf("# %d thread close\n",i);
		if(p_sys->threadID_network[i]){
			/*data_t *p_data = (data_t *)malloc(sizeof(data_t));
			memset(p_data,'\0',sizeof(data_t));
			data_fifo_put(p_sys->p_sedp_access->pf_data_for_service, p_data);*/

			thread_join2(p_sys->threadID_network[i]);
		}
	}

	if(p_sys->threadID_ThreadWriter)
	{
		data_t *p_data = (data_t *)malloc(sizeof(data_t));
		memset(p_data,'\0',sizeof(data_t));
		data_fifo_put(p_service->p_network_fifo, p_data);
		thread_join2(p_sys->threadID_ThreadWriter);
	}


	if(p_sys->p_spdp_access)
	{
		access_delete(p_sys->p_spdp_access);
	}

	if(p_sys->p_sedp_access)
	{
		access_delete(p_sys->p_sedp_access);
	}

	if(p_sys->p_access)
	{
		access_delete(p_sys->p_access);
	}

	if(p_sys->p_spdp_accessout)
	{
		access_out_delete(p_sys->p_spdp_accessout);
	}

	if(p_sys->p_sedp_accessout)
	{
		access_out_delete(p_sys->p_sedp_accessout);
	}

	if(p_sys->p_accessout)
	{
		access_out_delete(p_sys->p_accessout);
	}

	destory_liveliness();



	mutex_destroy(&p_sys->spdp_lock );
	cond_destroy(&p_sys->spdp_wait );
	mutex_destroy(&p_sys->sedp_lock );
	cond_destroy(&p_sys->sedp_wait );
	mutex_destroy(&p_sys->default_lock );
	cond_destroy(&p_sys->default_wait );

	FREE(p_sys->psz_path);
	FREE(p_sys->psz_addr);


	FREE(p_sys);
}


void rtps_get_default_multicast_locator(service_t *p_service, Locator_t *p_locator)
{
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_port;
	SetStringAddressToLocatorAddress(p_sys->psz_addr, p_locator->address);
}

void rtps_get_default_unicast_locator(service_t *p_service, Locator_t *p_locator)
{
	char	*psz_addr_uni = NULL;
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_port;
	psz_addr_uni = get_default_local_address();
	if(psz_addr_uni)
	{
		SetStringAddressToLocatorAddress(psz_addr_uni, p_locator->address);
	}

	FREE(psz_addr_uni);
}

void rtps_get_sedp_multicast_locator(service_t *p_service, Locator_t *p_locator)
{
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_sedp_port;
	SetStringAddressToLocatorAddress(p_sys->psz_addr, p_locator->address);
}

void rtps_get_sedp_unicast_locator(service_t *p_service, Locator_t *p_locator)
{
	char	*psz_addr_uni = NULL;
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_sedp_port;
	psz_addr_uni = get_default_local_address();
	if(psz_addr_uni)
	{
		SetStringAddressToLocatorAddress(psz_addr_uni, p_locator->address);
	}

	FREE(psz_addr_uni);
}

void rtps_get_spdp_multicast_locator(service_t *p_service, Locator_t *p_locator)
{
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_spdp_port;
	SetStringAddressToLocatorAddress(p_sys->psz_addr, p_locator->address);
}

void rtps_get_spdp_unicast_locator(service_t *p_service, Locator_t *p_locator)
{
	char	*psz_addr_uni = NULL;
	service_sys_t *p_sys = p_service->p_sys;

	p_locator->kind = LOCATOR_KIND_UDPv4;
	p_locator->port = p_sys->i_spdp_port;
	psz_addr_uni = get_default_local_address();
	if(psz_addr_uni)
	{
		SetStringAddressToLocatorAddress(psz_addr_uni, p_locator->address);
	}

	FREE(psz_addr_uni);
}



// 네트워크 관련 초기화.
static int init_network(service_t* p_service)
{
	char*	psz_addr_uni = NULL;
	int i;
	service_sys_t* p_sys = p_service->p_sys;
	
	defaultMulticastLocator.kind = LOCATOR_KIND_UDPv4;
	defaultMulticastLocator.port = p_sys->i_spdp_port;
	
	SetStringAddressToLocatorAddress(p_sys->psz_addr, defaultMulticastLocator.address);

	psz_addr_uni = get_default_local_address();
	if (psz_addr_uni)
	{
		SetStringAddressToLocatorAddress(psz_addr_uni, defaultUnicastLocator.address);
	}

	FREE(psz_addr_uni);

	//////////// SEDP ACCESS ///////////
	//asprintf(&p_sys->psz_path,"udp:@:%d",p_sys->i_sedp_port);
	asprintf(&p_sys->psz_path,"udp:@%s:%d", p_sys->psz_addr, p_sys->i_sedp_port);
	trace_msg(OBJECT(p_service),TRACE_TRACE,"SEDP ACCESS : %s", p_sys->psz_path);

	p_sys->p_sedp_access = access_new(OBJECT(p_service), "udp4", p_sys->psz_path);

	if (!p_sys->p_sedp_access)
	{
		trace_msg(OBJECT(p_service), TRACE_ERROR, "cannot create SEDP ACCESS : %s", p_sys->psz_path);
		Close(OBJECT(p_service));
		return MODULE_ERROR_NETWORK;
	}

	FREE(p_sys->psz_path);

	set_default_port(p_sys->p_sedp_access->pf_getBind(p_sys->p_sedp_access));

	/////////////////////


	//////////// ACCESS ///////////
	//asprintf(&p_sys->psz_path,"udp:@:%d",p_sys->i_port);
	asprintf(&p_sys->psz_path, "udp:@%s:%d", p_sys->psz_addr, p_sys->i_port);
	trace_msg(OBJECT(p_service), TRACE_TRACE,"ACCESS : %s", p_sys->psz_path);

	p_sys->p_access = access_new(OBJECT(p_service), "udp4", p_sys->psz_path);

	if (!p_sys->p_access)
	{
		Close(OBJECT(p_service));
		return MODULE_ERROR_NETWORK;
	}
	FREE(p_sys->psz_path);

	set_metatraffic_port(p_sys->p_access->pf_getBind(p_sys->p_access));

	/////////////////////


	//////////// SPDP ACCESS ///////////
	asprintf(&p_sys->psz_path, "udp:@%s:%d", p_sys->psz_addr, p_sys->i_spdp_port);
	trace_msg(OBJECT(p_service), TRACE_TRACE, "SPDP ACCESS: %s", p_sys->psz_path);

	p_sys->p_spdp_access = access_new(OBJECT(p_service), "udp4", p_sys->psz_path);

	if (!p_sys->p_spdp_access)
	{
		Close(OBJECT(p_service));
		return MODULE_ERROR_NETWORK;
	}
	/////////////////////

	trace_msg(OBJECT(p_service),TRACE_DEBUG,"RTPS Service Started.. %s",p_sys->psz_path);

	for (i = 0; i < thread_count ; i++)
	{
		printf("# %d thread created\n",i);
		if (!(p_sys->threadID_network[i] = thread_create2(OBJECT(p_service), &p_sys->spdp_wait, &p_sys->spdp_lock, "RTPS SPDP Service", (void*)NetworkThread, 0, false)))
    	{
			Close(OBJECT(p_service));
			return MODULE_ERROR_CREATE;
    	}
	}
	p_sys->network_thread_count = thread_count;
/*

	if( !(p_sys->threadID_SPDP = thread_create2( OBJECT(p_service),&p_sys->spdp_wait, &p_sys->spdp_lock, "RTPS SPDP Service", (void*)ServiceSPDPThread, 0, false )) )
    {
		Close(OBJECT(p_service));
		return MODULE_ERROR_CREATE;
    }

	if( !(p_sys->threadID_SEDP = thread_create2( OBJECT(p_service), &p_sys->sedp_wait, &p_sys->sedp_lock, "RTPS SEDP Service", (void*)ServiceSEDPThread, 0, false )) )
    {
		Close(OBJECT(p_service));
		return MODULE_ERROR_CREATE;
    }

	if( !(p_sys->threadID_DEFAULT = thread_create2( OBJECT(p_service), &p_sys->default_wait, &p_sys->default_lock, "RTPS Service", (void*)ServiceThread, 0, false )) )
    {
		Close(OBJECT(p_service));
		return MODULE_ERROR_CREATE;
    }
*/
	return MODULE_SUCCESS;
}


static int SetStringAddressToLocatorAddress(char* p_address, octet* p_locatoraddress)
{
	int32_t ad = inet_addr(p_address);
	memset(p_locatoraddress,0,16);

	*((int32_t*)&p_locatoraddress[12]) = inet_addr(p_address);

	return 1;
}


static DataFull* processReceivedData(service_t* p_service, data_t* p_data, GuidPrefix_t** pp_remoteGuidPrefix, GuidPrefix_t** pp_dstGuidPrefi, EntityId_t** pp_readerEntityId, EntityId_t** pp_writerEntityId);


static int32_t sendcount = 0;
static int64_t sendbytes = 0;

int32_t getSendCount()
{
	return sendcount;
}


int64_t getSendBytes()
{
	return sendbytes;
}


static int32_t receivedcount = 0;
static int64_t receivedbytes = 0;


int32_t getReceivedCount()
{
	return receivedcount;
}


int64_t getReceivedBytes()
{
	return receivedbytes;
}


static int32_t sendcountThroughput = 0;
static int32_t sendbytesThroughput = 0;

int32_t getSendCountThroughput()
{
	return sendcountThroughput;
}


int32_t getSendBytesThroughput()
{
	return sendbytesThroughput;
}


static int32_t received_count_throughput = 0;
static int32_t received_bytes_throughput = 0;


int32_t getreceived_count_throughput()
{
	return received_count_throughput;
}


int32_t getreceived_bytes_throughput()
{
	return received_bytes_throughput;
}


void resetThroughput()
{
	sendcountThroughput = 0;
	sendbytesThroughput = 0;
	received_count_throughput = 0;
	received_bytes_throughput = 0;
}


static void NetworkThread(module_object_t* p_this)
{
	service_t* p_service = (service_t*)p_this;
	service_sys_t* p_sys = p_service->p_sys;
	access_t* p_access = NULL;
	DataFull* p_datafull = NULL;
	rtps_reader_t* p_rtpsReader = NULL;
	EntityId_t* p_readerEntityId = NULL;
	EntityId_t* p_writerEntityId = NULL;
	GuidPrefix_t* p_dstGuidPrefix = NULL;
	GuidPrefix_t* p_remoteGuidPrefix = NULL;
//	GUID_t a_matchedGuid;

	int i;

	if (!p_sys || !p_sys->p_spdp_access)
	{
		return;
	}

	p_access = p_sys->p_sedp_access;


	thread_ready((module_object_t *)p_service);


	while (!p_service->b_end)
	{
//by jun	
		data_t* p_data = p_access->pf_data_for_service(p_service, p_access, p_service->p_sys->p_spdp_access->fd, p_service->p_sys->p_sedp_access->fd, p_service->p_sys->p_access->fd);

		//if (p_data == NULL) continue;

		
		for(i = 0; i < 3; i++)
		{ 


			if(p_service->p_networkdata[i] != NULL)
			{
				receivedcount++;
				receivedbytes += p_service->p_networkdata[i]->i_size;

				received_count_throughput++;
				received_bytes_throughput += p_service->p_networkdata[i]->i_size;
		
				//if(receivedcount % 3 == 0)
					p_datafull = processReceivedData(p_service, p_service->p_networkdata[i], &p_remoteGuidPrefix, &p_dstGuidPrefix, &p_readerEntityId, &p_writerEntityId);

				if (p_service->b_end)
				{
					FREE(p_remoteGuidPrefix);
					FREE(p_dstGuidPrefix);
					FREE(p_readerEntityId);
					FREE(p_writerEntityId);
					if(p_service->p_networkdata[i]) data_release(p_service->p_networkdata[i]);
					break;
				}

				FREE(p_remoteGuidPrefix);
				FREE(p_dstGuidPrefix);
				FREE(p_readerEntityId);
				FREE(p_writerEntityId);
				if (p_service->p_networkdata[i]) data_release(p_service->p_networkdata[i]);
			}
		}
	}
}


/*
static void NetworkThread(module_object_t *p_this)
{
	service_t *p_service = (service_t *)p_this;
	service_sys_t *p_sys = p_service->p_sys;
	access_t *p_access = NULL;
	DataFull	*p_datafull = NULL;
	rtps_reader_t *p_rtpsReader = NULL;
	EntityId_t *p_readerEntityId = NULL;
	EntityId_t *p_writerEntityId = NULL;
	GuidPrefix_t *p_dstGuidPrefix = NULL;
	GuidPrefix_t *p_remoteGuidPrefix = NULL;
	GUID_t a_matchedGuid;

	if(!p_sys || !p_sys->p_spdp_access)
	{
		return;
	}

	p_access = p_sys->p_sedp_access;


	thread_ready((module_object_t *)p_service);


	while(!p_service->b_end)
	{
//by jun	
		data_t *p_data = p_access->pf_data_for_service(p_service, p_access, p_service->p_sys->p_spdp_access->fd, p_service->p_sys->p_sedp_access->fd, p_service->p_sys->p_access->fd);

		
		p_datafull = processReceivedData(p_service, p_data, &p_remoteGuidPrefix, &p_dstGuidPrefix, &p_readerEntityId, &p_writerEntityId);

		if(p_service->b_end) {
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			break;
		}

		if(p_readerEntityId == NULL || p_writerEntityId == NULL)
		{
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			continue;
		}


		//if(p_dstGuidPrefix == NULL) // 이건 INFO_TS, DATA(p)를 의미함
		//{

			if((memcmp(p_readerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
			|| (memcmp(p_writerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);

			}else if((memcmp(p_readerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
			}else{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
				memcpy(&a_matchedGuid.guidPrefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

				{
					int size=0;
					int i;
					rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId, &size);

					for(i=0; i < size; i++)
					{
						p_rtpsReader = (rtps_reader_t *) pp_endpoint[i]; 

						p_data->p_address = malloc(sizeof(GuidPrefix_t));
						memcpy(p_data->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

						if(i == size - 1){
							data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
						}else{
							data_fifo_put(p_rtpsReader->p_data_fifo, data_duplicate(p_data));
						}
					}

					FREE(pp_endpoint);

					//p_rtpsReader = NULL;
				}
			}

			
		//}else{

		//	if((memcmp(p_readerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
		//	|| (memcmp(p_writerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
		//	{
		//		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();

		//	}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
		//		|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
		//	{
		//		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();

		//	}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
		//		|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
		//	{
		//		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();

		//	}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
		//		|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
		//	{
		//		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();

		//	}else if((memcmp(p_readerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
		//		|| (memcmp(p_writerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
		//	{
		//		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
		//	}else{
		//		memcpy(&a_matchedGuid.entity_id, p_readerEntityId, sizeof(EntityId_t));
		//		memcpy(&a_matchedGuid.guidPrefix, p_dstGuidPrefix, sizeof(GuidPrefix_t));

		//		p_rtpsReader = (rtps_reader_t *)rtps_get_endpoint(a_matchedGuid);

		//		if(p_rtpsReader == NULL){
		//			memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
		//			memcpy(&a_matchedGuid.guidPrefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));
		//			p_rtpsReader = (rtps_reader_t *)rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId);
		//			if(p_rtpsReader)
		//			{
		//				p_data->p_address = malloc(sizeof(GuidPrefix_t));
		//				memcpy(p_data->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));
		//			}
		//		}
		//	}

		//	if(p_rtpsReader == NULL)
		//	{
		//		memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
		//	}
		//}

		//if(p_rtpsReader == NULL && p_dstGuidPrefix == NULL)
		//{
		//	p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();
		//}

		FREE(p_remoteGuidPrefix);
		FREE(p_dstGuidPrefix);
		FREE(p_readerEntityId);
		FREE(p_writerEntityId);

		if(p_rtpsReader)
		{
		//	data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{

			if(p_data) data_release(p_data);

			if(p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		}

		p_rtpsReader = NULL;
	}
}
*/
/*
static void ServiceSPDPThread(module_object_t *p_this)
{
	service_t *p_service = (service_t *)p_this;
	service_sys_t *p_sys = p_service->p_sys;
	access_t *p_access = NULL;
	DataFull	*p_datafull = NULL;
	rtps_reader_t *p_rtpsReader = NULL;
	EntityId_t *p_readerEntityId = NULL;
	EntityId_t *p_writerEntityId = NULL;
	GuidPrefix_t *p_dstGuidPrefix = NULL;
	GuidPrefix_t *p_remoteGuidPrefix = NULL;
	GUID_t a_matchedGuid;

	if(!p_sys || !p_sys->p_spdp_access)
	{
		return;
	}

	p_access = p_sys->p_spdp_access;


	thread_ready((module_object_t *)p_service);


	while(!p_service->b_end)
	{
		data_t *p_data = p_access->pf_data(p_access);
		p_datafull = processReceivedData(p_service, p_data, &p_remoteGuidPrefix, &p_dstGuidPrefix, &p_readerEntityId, &p_writerEntityId);

		if(p_service->b_end) {
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			break;
		}

		if(p_readerEntityId == NULL || p_writerEntityId == NULL)
		{
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			continue;
		}


		if(p_dstGuidPrefix == NULL) // 이건 INFO_TS, DATA(p)를 의미함
		{

			if((memcmp(p_readerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
			|| (memcmp(p_writerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
			}else{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
				memcpy(&a_matchedGuid.guid_prefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

				{
					int size=0;
					int i;
					rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId, &size);

					for(i=0; i < size; i++)
					{
						p_rtpsReader = (rtps_reader_t *) pp_endpoint[i]; 

						p_data->p_address = malloc(sizeof(GuidPrefix_t));
						memcpy(p_data->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

						if(i == size - 1){
							data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
						}else{
							data_fifo_put(p_rtpsReader->p_data_fifo, data_duplicate(p_data));
						}
					}

					p_rtpsReader = NULL;
					FREE(pp_endpoint);
				}
			}

			
		}else{

			memcpy(&a_matchedGuid.entity_id, p_readerEntityId, sizeof(EntityId_t));
			memcpy(&a_matchedGuid.guid_prefix, p_dstGuidPrefix, sizeof(GuidPrefix_t));

			p_rtpsReader = (rtps_reader_t *)rtps_get_endpoint(a_matchedGuid);

			if(p_rtpsReader == NULL)
			{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
			}
		}

		if(p_rtpsReader == NULL && p_dstGuidPrefix == NULL)
		{
			p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();
		}

		FREE(p_remoteGuidPrefix);
		FREE(p_dstGuidPrefix);
		FREE(p_readerEntityId);
		FREE(p_writerEntityId);

		if(p_rtpsReader)
		{
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{

			if(p_data) data_release(p_data);

			if(p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		}

		p_rtpsReader = NULL;
	}
}
*/
/*
static void ServiceSEDPThread(module_object_t *p_this)
{
	service_t *p_service = (service_t *)p_this;
	service_sys_t *p_sys = p_service->p_sys;
	access_t *p_access = NULL;
	DataFull	*p_datafull = NULL;
	rtps_reader_t *p_rtpsReader = NULL;
	EntityId_t *p_readerEntityId = NULL;
	EntityId_t *p_writerEntityId = NULL;
	GuidPrefix_t *p_dstGuidPrefix = NULL;
	GuidPrefix_t *p_remoteGuidPrefix = NULL;
	GUID_t a_matchedGuid;

	if(!p_sys || !p_sys->p_spdp_access)
	{
		return;
	}

	p_access = p_sys->p_sedp_access;


	thread_ready((module_object_t *)p_service);


	while(!p_service->b_end)
	{
		data_t *p_data = p_access->pf_data(p_access);
		p_datafull = processReceivedData(p_service, p_data, &p_remoteGuidPrefix, &p_dstGuidPrefix, &p_readerEntityId, &p_writerEntityId);

		if(p_service->b_end) {
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			break;
		}

		if(p_readerEntityId == NULL || p_writerEntityId == NULL)
		{
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			continue;
		}


		if(p_dstGuidPrefix == NULL) // 이건 INFO_TS, DATA(p)를 의미함
		{

			if((memcmp(p_readerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
			|| (memcmp(p_writerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
			}else{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
				memcpy(&a_matchedGuid.guid_prefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

				{
					int size=0;
					int i;
					rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId, &size);

					for(i=0; i < size; i++)
					{
						p_rtpsReader = (rtps_reader_t *) pp_endpoint[i]; 

						p_data->p_address = malloc(sizeof(GuidPrefix_t));
						memcpy(p_data->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

						if(i == size - 1){
							data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
						}else{
							data_fifo_put(p_rtpsReader->p_data_fifo, data_duplicate(p_data));
						}
					}

					p_rtpsReader = NULL;
					FREE(pp_endpoint);
				}
			}

			
		}else{

			memcpy(&a_matchedGuid.entity_id, p_readerEntityId, sizeof(EntityId_t));
			memcpy(&a_matchedGuid.guid_prefix, p_dstGuidPrefix, sizeof(GuidPrefix_t));

			p_rtpsReader = (rtps_reader_t *)rtps_get_endpoint(a_matchedGuid);

			if(p_rtpsReader == NULL)
			{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
			}
		}

		if(p_rtpsReader == NULL && p_dstGuidPrefix == NULL)
		{
			p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();
		}

		FREE(p_remoteGuidPrefix);
		FREE(p_dstGuidPrefix);
		FREE(p_readerEntityId);
		FREE(p_writerEntityId);

		if(p_rtpsReader)
		{
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{

			if(p_data) data_release(p_data);

			if(p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		}

		p_rtpsReader = NULL;
	}
}
*/
/*
static void ServiceThread(module_object_t *p_this)
{
	service_t *p_service = (service_t *)p_this;
	service_sys_t *p_sys = p_service->p_sys;
	access_t *p_access = NULL;
	DataFull	*p_datafull = NULL;
	rtps_reader_t *p_rtpsReader = NULL;
	EntityId_t *p_readerEntityId = NULL;
	EntityId_t *p_writerEntityId = NULL;
	GuidPrefix_t *p_dstGuidPrefix = NULL;
	GuidPrefix_t *p_remoteGuidPrefix = NULL;
	GUID_t a_matchedGuid;

	if(!p_sys || !p_sys->p_spdp_access)
	{
		return;
	}

	p_access = p_sys->p_access;


	thread_ready((module_object_t *)p_service);


	while(!p_service->b_end)
	{
		data_t *p_data = p_access->pf_data(p_access);
		p_datafull = processReceivedData(p_service, p_data, &p_remoteGuidPrefix, &p_dstGuidPrefix, &p_readerEntityId, &p_writerEntityId);

		if(p_service->b_end) {
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			break;
		}

		if(p_readerEntityId == NULL || p_writerEntityId == NULL)
		{
			FREE(p_remoteGuidPrefix);
			FREE(p_dstGuidPrefix);
			FREE(p_readerEntityId);
			FREE(p_writerEntityId);
			if(p_data) data_release(p_data);
			continue;
		}


		if(p_dstGuidPrefix == NULL) // 이건 INFO_TS, DATA(p)를 의미함
		{

			if((memcmp(p_readerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
			|| (memcmp(p_writerEntityId, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();

			}else if((memcmp(p_readerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
				|| (memcmp(p_writerEntityId, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
			{
				p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
			}else{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
				memcpy(&a_matchedGuid.guid_prefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

				{
					int size=0;
					int i;
					rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId, &size);

					for(i=0; i < size; i++)
					{
						p_rtpsReader = (rtps_reader_t *) pp_endpoint[i]; 

						p_data->p_address = malloc(sizeof(GuidPrefix_t));
						memcpy(p_data->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));

						if(i == size - 1){
							data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
						}else{
							data_t *tmpdata = data_duplicate(p_data);
							tmpdata->p_address = malloc(sizeof(GuidPrefix_t));
							memcpy(tmpdata->p_address, p_remoteGuidPrefix, sizeof(GuidPrefix_t));
							data_fifo_put(p_rtpsReader->p_data_fifo, tmpdata);
						}
					}

					p_rtpsReader = NULL;
					FREE(pp_endpoint);
				}
			}

			
		}else{
*/
			/*if(memcmp(p_readerEntityId, &ENTITYID_UNKNOWN, sizeof(EntityId_t)) == 0){
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
				memcpy(&a_matchedGuid.guidPrefix, p_remoteGuidPrefix, sizeof(GuidPrefix_t));
				p_rtpsReader = (rtps_reader_t *)rtps_matched_writerproxy_reader(a_matchedGuid, *p_readerEntityId);
			}else{
				memcpy(&a_matchedGuid.entity_id, p_readerEntityId, sizeof(EntityId_t));
				p_rtpsReader = (rtps_reader_t *)rtps_get_endpoint(a_matchedGuid);
				memcpy(&a_matchedGuid.guidPrefix, p_dstGuidPrefix, sizeof(GuidPrefix_t));
			}*/
/*
			

			//p_rtpsReader = (rtps_reader_t *)rtps_get_endpoint(a_matchedGuid);

			

			if(p_rtpsReader == NULL)
			{
				memcpy(&a_matchedGuid.entity_id, p_writerEntityId, sizeof(EntityId_t));
			}
		}

		if(p_rtpsReader == NULL && p_dstGuidPrefix == NULL)
		{
			p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();
		}

		FREE(p_remoteGuidPrefix);
		FREE(p_dstGuidPrefix);
		FREE(p_readerEntityId);
		FREE(p_writerEntityId);

		if(p_rtpsReader)
		{
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{

			if(p_data) data_release(p_data);

			if(p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		}

		p_rtpsReader = NULL;
	}
}
*/

static DataFull* processReceivedData(service_t* p_service, data_t* p_data, GuidPrefix_t** pp_remoteGuidPrefix, GuidPrefix_t** pp_dstGuidPrefi, EntityId_t** pp_readerEntityId, EntityId_t** pp_writerEntityId)
{
	DataFull* p_ret = NULL;

	if (p_data)
	{
//		trace_msg(OBJECT(p_service),TRACE_TRACE,"received data %d", p_data->i_size);
		p_ret = rtps_message_parser(OBJECT(p_service), p_data->p_data, p_data->i_size, pp_remoteGuidPrefix, pp_dstGuidPrefi, pp_readerEntityId, pp_writerEntityId);

		//data_release(p_data);
	}

	return p_ret;
}


static access_out_t* GetAccessOut(module_object_t* p_this, int type)
{
	service_t* p_service = (service_t *)p_this;
	service_sys_t* p_sys = p_service->p_sys;

	if (!p_sys) return NULL;

	if (type == SPDP_OUT_TYPE)
	{
		return p_sys->p_spdp_accessout;
	}
	else if (type == SEDP_OUT_TYPE)
	{
		return p_sys->p_sedp_accessout;
	}
	else if (type == DEFAULT_OUT_TYPE)
	{
		return p_sys->p_accessout;
	}

	return NULL;
}


int rtps_get_fragmentsize(module_object_t* p_this)
{
	service_t* p_service = (service_t*)p_this;
	service_sys_t* p_sys = p_service->p_sys;

	if (!p_sys) return 64000;

	return p_sys->i_fragmentsize;
}


static void ThreadWrite2(module_object_t* p_this);


static int Create_network(module_object_t* p_this)
{
	service_t* p_service = (service_t *) p_this;
	service_sys_t* p_sys = p_service->p_sys;
	p_sys->threadID_ThreadWriter = p_service->thread_id;




	if (thread_create(OBJECT(p_service), "out write thread", (void*)ThreadWrite2, THREAD_PRIORITY_HIGHEST, false))
    {
		trace_msg(OBJECT(p_service), TRACE_ERROR, "[Access UDP OUT Module] cannot spawn access out thread");
       
        return MODULE_ERROR_THREAD;
    }

	p_sys->threadID_ThreadWriter = p_service->thread_id;
	return 0;
}

extern int BoundarySize(int i_size);



static int kkkk = 0;

typedef struct over_pk_t over_pk_t;


struct over_pk_t
{
	_LINKED_LIST_ATOM

	int				i_large_send;
	uint8_t*		p_address;
	int				i_port;
	char			TMPBUFFER[1500];
};


//by jun
static void ThreadWrite2(module_object_t* p_this)
{
    service_t* p_service = (service_t*)p_this;
	service_sys_t* p_sys = p_service->p_sys;

	mtime_t              i_date_last = -1;
    int                  i_dropped_packets = 0;

	int i=0;
	//bool	b_large_send = false;
	//int		i_large_send= 0;

	int i_header_size = sizeof(Header);
	data_t *p_over_pk=NULL;
	bool	is_over = false;
	data_t	*p_pk = NULL;


	linked_list_head_t _pk_header;

	



	
#ifdef _MSC_VER
	SOCKADDR_IN target_addr;
#else
	struct sockaddr_in target_addr;
#endif


	_pk_header.i_linked_size = 0;
	_pk_header.p_head_first = NULL;
	_pk_header.p_head_last = NULL;

	memset(&target_addr, 0, sizeof(target_addr));


    while (!p_service->b_end)
    {
		

		
       
		if(is_over)
		{
			p_pk = p_over_pk;
			is_over = false;

		}else{
		
			p_pk = data_fifo_get(p_service->p_network_fifo);
		}

		

		if (p_service->b_end)
		{
			data_release(p_pk);
			break;
		}

		//printf("data send : %d\r\n", p_service->p_network_fifo->i_depth);
		

		if (p_pk->i_size > 0)
		{
			int send_fd = p_pk->socket_id;
			
			bool is_found = false;


			


			target_addr.sin_family = AF_INET;
			target_addr.sin_port = htons(p_pk->i_port);
			target_addr.sin_addr.s_addr = inet_addr(p_pk->p_address);

			

			if(p_service->p_network_fifo->i_depth >= 20)
			{
				over_pk_t *p_over_pk_atom = (over_pk_t *)_pk_header.p_head_first;
				while(p_over_pk_atom)
				{

					if(p_over_pk_atom->i_port == p_pk->i_port
						&& strcmp(p_over_pk_atom->p_address, p_pk->p_address) == 0)
					{
						is_found = true;
						break;
					}
					
					p_over_pk_atom = (over_pk_t *)p_over_pk_atom->p_next;
				}

				if(is_found == false)
				{
					over_pk_t * p_over_pk = malloc(sizeof(over_pk_t));
					memset(p_over_pk, 0, sizeof(over_pk_t));

					p_over_pk->i_large_send = 0;
					p_over_pk->p_address = strdup(p_pk->p_address);
					p_over_pk->i_port = p_pk->i_port;

					insert_linked_list(&_pk_header, (linked_list_atom_t *)p_over_pk);

					p_over_pk_atom = p_over_pk;
				}

				if(p_over_pk_atom->i_large_send + p_pk->i_size > 1500)
				{
					is_over = true;
					p_over_pk = p_pk;
					
				}else{

					bool has_header = false;

					if(p_over_pk_atom->i_large_send != 0 && p_pk->i_size > i_header_size)
					{
						

						Header a_Header = *(Header*)p_pk->p_data;

						if((a_Header.protocol._rtps[0] == 'R' 
							&& a_Header.protocol._rtps[1] == 'T'
							&& a_Header.protocol._rtps[2] == 'P'
							&& a_Header.protocol._rtps[3] == 'S'))
						{
							has_header = true;
						}
					}
					
					if(has_header)
					{

						memcpy(&p_over_pk_atom->TMPBUFFER[p_over_pk_atom->i_large_send], &p_pk->p_data[i_header_size], p_pk->i_size-i_header_size);
						p_over_pk_atom->i_large_send +=  p_pk->i_size-i_header_size;
						p_over_pk_atom->i_large_send = BoundarySize(p_over_pk_atom->i_large_send);

					}else{
						memcpy(&p_over_pk_atom->TMPBUFFER[p_over_pk_atom->i_large_send], p_pk->p_data, p_pk->i_size);
						p_over_pk_atom->i_large_send +=  p_pk->i_size;
						p_over_pk_atom->i_large_send = BoundarySize(p_over_pk_atom->i_large_send);
					}
					data_release(p_pk);
					continue;
				}

				
				/*if(p_over_pk_atom->i_large_send > 1420)
				{

					while(true)
					{

						if (sendto(p_pk->socket_id, (const char*)p_over_pk_atom->TMPBUFFER, p_over_pk_atom->i_large_send,0, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1)
						{
							trace_msg(OBJECT(p_this), TRACE_WARM, "send error : %s:%d", p_pk->p_address, p_pk->i_port);
						}
					}
				}else{*/

					if (sendto(p_pk->socket_id, (const char*)p_over_pk_atom->TMPBUFFER, p_over_pk_atom->i_large_send,0, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1)
						{
							trace_msg(OBJECT(p_this), TRACE_WARM, "send error : %s:%d", p_pk->p_address, p_pk->i_port);
						}
				//}

				p_over_pk_atom->i_large_send = 0;



			}else if(p_service->p_network_fifo->i_depth < 20){
				over_pk_t *p_over_pk_atom;
				over_pk_t *p_tmp_over_pk_atom;
//				b_large_send = false;

				p_over_pk_atom = (over_pk_t *)_pk_header.p_head_first;


				while(p_over_pk_atom)
				{
					p_tmp_over_pk_atom = (over_pk_t *)p_over_pk_atom->p_next;

					target_addr.sin_port = htons(p_over_pk_atom->i_port);
					target_addr.sin_addr.s_addr = inet_addr(p_over_pk_atom->p_address);

					if(p_over_pk_atom->i_large_send)
					{

						if (sendto(p_pk->socket_id, (const char*)p_over_pk_atom->TMPBUFFER, p_over_pk_atom->i_large_send,0, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1)
						{
							trace_msg(OBJECT(p_this), TRACE_WARM, "send error : %s:%d", p_pk->p_address, p_pk->i_port);
						}

						p_over_pk_atom->i_large_send = 0;
					}


					remove_linked_list(&_pk_header, (linked_list_atom_t *)p_over_pk_atom);

					FREE(p_over_pk_atom->p_address);
					FREE(p_over_pk_atom);
					p_over_pk_atom = p_tmp_over_pk_atom;


				}

				target_addr.sin_port = htons(p_pk->i_port);
				target_addr.sin_addr.s_addr = inet_addr(p_pk->p_address);


				if (sendto(p_pk->socket_id, (const char*)p_pk->p_data, p_pk->i_size,0, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1)
				{
					trace_msg(OBJECT(p_this), TRACE_WARM, "send error : %s:%d", p_pk->p_address, p_pk->i_port);
				}
			}
			
		}


		if(is_over == false)
		{
			data_release(p_pk);
		}
	}
}


/*
static message_t **rtps_read(module_object_t * p_this, DataReader *p_datareader, int *p_size)
{
	int i;
	message_t **pp_message = NULL;
	rtps_reader_t *p_rtpsReader = p_datareader->related_rtps_reader;
	int i_count = 0;

	if(p_rtpsReader == NULL) return pp_message;

	HISTORYCACHE_LOCK(p_rtpsReader->reader_cache);

	for(i=0; i < p_rtpsReader->reader_cache->i_changes; i++)
	{
		rtps_cachechange_t *p_rtps_cachechange = p_rtpsReader->reader_cache->changes[i];
		if(p_rtps_cachechange->is_blockcount && p_rtps_cachechange->data_value != NULL){

			if(p_rtps_cachechange->p_org_message == NULL)
			{
				SerializedPayloadForReader *p_serializedForReader = (SerializedPayloadForReader *)p_rtps_cachechange->data_value;
				message_t *p_message = messageNewForBultinReader(p_datareader, &p_rtps_cachechange->data_value->value[4], p_serializedForReader->i_size - 4);

				p_rtps_cachechange->p_org_message = p_message;
				insert_instanceset_datareader(p_datareader, p_rtps_cachechange, p_message);

				INSERT_ELEM(pp_message, i_count, i_count, p_message);
			}else{
				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_rtps_cachechange->p_org_message));
			}
		}

		p_rtps_cachechange->b_read = true;
	}



	HISTORYCACHE_UNLOCK(p_rtpsReader->reader_cache);

	*p_size = i_count;

	if(i_count !=0 )
	{
		*p_size = i_count;
	}

	return pp_message;
}

static message_t **rtps_take(module_object_t * p_this, DataReader *p_datareader, int *p_size)
{
	int i;
	message_t **pp_message = NULL;
	rtps_reader_t *p_rtpsReader = p_datareader->related_rtps_reader;
	int i_count = 0;

	if(p_rtpsReader == NULL) return pp_message;

	HISTORYCACHE_LOCK(p_rtpsReader->reader_cache);

	for(i=0; i < p_rtpsReader->reader_cache->i_changes; i++)
	{
		rtps_cachechange_t *p_rtps_cachechange = p_rtpsReader->reader_cache->changes[i];
		if(p_rtps_cachechange->is_blockcount && p_rtps_cachechange->data_value != NULL && p_rtps_cachechange->b_read == false){

			if(p_rtps_cachechange->p_org_message == NULL)
			{
				SerializedPayloadForReader *p_serializedForReader = (SerializedPayloadForReader *)p_rtps_cachechange->data_value;
				message_t *p_message = messageNewForBultinReader(p_datareader, &p_rtps_cachechange->data_value->value[4], p_serializedForReader->i_size-4);
				p_rtps_cachechange->b_read = true;
				p_rtps_cachechange->p_org_message = p_message;
				insert_instanceset_datareader(p_datareader, p_rtps_cachechange, p_message);

				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_message));
				
			}else{
				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_rtps_cachechange->p_org_message));
			}
		}
	}

	HISTORYCACHE_UNLOCK(p_rtpsReader->reader_cache);

	*p_size = i_count;

	return pp_message;
}

*/

/*
	This operation accesses a collection of Data values from the DataReader. The size of the returned collection will be
	limited to the specified max_samples. The properties of the data_values collection and the setting of the
	PRESENTATION QoS policy (see Section 7.1.3.6, "PRESENTATION,” on page 110) may impose further limits on the
	size of the returned 'list.'
		1. If PRESENTATION access_scope is INSTANCE, then the returned collection is a 'list' where samples belonging
			to the same data-instance are consecutive.
		2. If PRESENTATION access_scope is TOPIC and ordered_access is set to FALSE, then the returned collection is a
			'list' where samples belonging to the same data-instance are consecutive.
		3. If PRESENTATION access_scope is TOPIC and ordered_access is set to TRUE, then the returned collection is a
			'list' were samples belonging to the same instance may or may not be consecutive. This is because to preserve
			order it may be necessary to mix samples from different instances.
		4. If PRESENTATION access_scope is GROUP and ordered_access is set to FALSE, then the returned collection is
			a 'list' where samples belonging to the same data instance are consecutive.
		5. If PRESENTATION access_scope is GROUP and ordered_access is set to TRUE, then the returned collection
			contains at most one sample. The difference in this case is due to the fact that it is required that the application is
			able to read samples belonging to different DataReader objects in a specific order.

	In any case, the relative order between the samples of one instance is consistent with the DESTINATION_ORDER
	QosPolicy:
		-  If DESTINATION_ORDER is BY_RECEPTION_TIMESTAMP, samples belonging to the same instances will appear
		in the relative order in which they were received (FIFO, earlier samples ahead of the later samples).
		-  If DESTINATION_ORDER is BY_SOURCE_TIMESTAMP, samples belonging to the same instances will appear in
		the relative order implied by the source_timestamp (FIFO, smaller values of source_timestamp ahead of the larger
		values).

	In addition to the collection of samples, the read operation also uses a collection of SampleInfo structures (sample_infos),
	see Section 7.1.2.5.5, "SampleInfo Class,” on page 91.

	The initial (input) properties of the data_values and sample_infos collections will determine the precise behavior of read
	operation. For the purposes of this description the collections are modeled as having three properties: the current-length
	(len), the maximum length (max_len), and whether the collection container owns the memory of the elements within
	(owns). PSM mappings that do not provide these facilities may need to change the signature of the read operation slightly
	to compensate for it.

	The initial (input) values of the len, max_len, and owns properties for the data_values and sample_infos collections
	govern the behavior of the read operation as specified by the following rules:
		1. The values of len, max_len, and owns for the two collections must be identical. Otherwise read will and return
			PRECONDITION_NOT_MET.
		2. On successful output, the values of len, max_len, and owns will be the same for both collections.
		3. If the input max_len==0, then the data_values and sample_infos collections will be filled with elements that are
			'loaned' by the DataReader. On output, owns will be FALSE, len will be set to the number of values returned, and
			max_len will be set to a value verifying max_len >= len. The use of this variant allows for zero-copy22 access to the
			data and the application will need to "return the loan” to the DataWriter using the return_loan operation (see Section
			7.1.2.5.3.20).
		4. If the input max_len>0 and the input owns==FALSE, then the read operation will fail and return
			PRECONDITION_NOT_MET. This avoids the potential hard-to-detect memory leaks caused by an application
			forgetting to "return the loan.”
		5. If input max_len>0 and the input owns==TRUE, then the read operation will copy the Data values and
			SampleInfo values into the elements already inside the collections. On output, owns will be TRUE, len will be set
			to the number of values copied, and max_len will remain unchanged. The use of this variant forces a copy but the
			application can control where the copy is placed and the application will not need to "return the loan.” The number
			of samples copied depends on the relative values of max_len and max_samples:
				-  If max_samples = LENGTH_UNLIMITED, then at most max_len values will be copied. The use of this variant
				lets the application limit the number of samples returned to what the sequence can accommodate.
				-  If max_samples <= max_len, then at most max_samples values will be copied. The use of this variant lets the
				application limit the number of samples returned to fewer that what the sequence can accommodate.
				-  If max_samples > max_len, then the read operation will fail and return PRECONDITION_NOT_MET. This
				avoids the potential confusion where the application expects to be able to access up to max_samples, but that
				number can never be returned, even if they are available in the DataReader, because the output sequence cannot
				accommodate them.

	As described above, upon return the data_values and sample_infos collections may contain elements "loaned” from the
	DataReader. If this is the case, the application will need to use the return_loan operation (see Section 7.1.2.5.3.20) to
	return the "loan” once it is no longer using the Data in the collection. Upon return from return_loan, the collection will
	have max_len=0 and owns=FALSE.

	The application can determine whether it is necessary to "return the loan” or not based on how the state of the collections
	when the read operation was called, or by accessing the 'owns' property. However, in many cases it may be simpler to
	always call return_loan, as this operation is harmless (i.e., leaves all elements unchanged) if the collection does not have
	a loan.

	To avoid potential memory leaks, the implementation of the Data and SampleInfo collections should disallow changing
	the length of a collection for which owns==FALSE. Furthermore, deleting a collection for which owns==FALSE should
	be considered an error.

	On output, the collection of Data values and the collection of SampleInfo structures are of the same length and are in a
	one-to-one correspondence. Each SampleInfo provides information, such as the source_timestamp, the sample_state,
	view_state, and instance_state, etc. about the corresponding sample.

	Some elements in the returned collection may not have valid data. If the instance_state in the SampleInfo is
	NOT_ALIVE_DISPOSED or NOT_ALIVE_NO_WRITERS, then the last sample for that instance in the collection, that
	is, the one whose SampleInfo has sample_rank==0 does not contain valid data. Samples that contain no data do not count
	towards the limits imposed by the RESOURCE_LIMITS QoS policy.

	The act of reading a sample sets its sample_state to READ. If the sample belongs to the most recent generation of the
	instance, it will also set the view_state of the instance to NOT_NEW. It will not affect the instance_state of the instance.

	This operation must be provided on the specialized class that is generated for the particular application data-type that is
	being read.

	If the DataReader has no samples that meet the constraints, the return value will be NO_DATA.

*/

static message_t** rtps_read(module_object_t* p_this, DataReader* p_datareader, int32_t max_samples, int* p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
{
	int i;
	message_t** pp_message = NULL;
	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;
	int i_count = 0;
	int32_t len = 0;
	int32_t copy_len = 0;
	int32_t copy_count = 0;
	int32_t which = 0;
	int test = 0;

	if (p_rtpsReader == NULL) return pp_message;
	if (p_datareader->i_instanceset == 0) return pp_message;

	if(p_datareader->p_datareader_listener && p_datareader->p_datareader_listener->on_data_available)
	{
	}else{
		HISTORYCACHE_LOCK(p_rtpsReader->p_reader_cache);
	}


	for (i = 0; i < p_datareader->i_instanceset; i++)
	{
		len += p_datareader->pp_instanceset[i]->i_messages;
	}

	if (max_samples == LENGTH_UNLIMITED || max_samples == 0)
	{
		copy_len = p_datareader->datareader_qos.history.depth;

		if (copy_len > len)
		{
			copy_len = len;
		}		
	}
	else if (max_samples <= p_datareader->max_len)
	{
		copy_len = max_samples;
	}
	else if (max_samples > p_datareader->max_len)
	{
		/**p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;*/

		copy_len = p_datareader->max_len;
	}
/*
	if(len < copy_len)
	{
		*p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->reader_cache);
		return pp_message;
	}
*/
	test =  p_datareader->i_message_order;

	//added by kyy(Destination Order)////////////////////////////////////////////////////////////////////////
	//pp_message를 ordering 하여 Application에 제공
	//read 하기 전 ordering 수행
	if (p_datareader->datareader_qos.destination_order.kind == BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS || p_datareader->p_subscriber->subscriber_qos.presentation.ordered_access == true)
	{
		if(p_datareader->datareader_qos.destination_order.kind == BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)
		{
			qosim_dcps_message_ordering_source_timestamp(p_datareader->pp_message_order, p_datareader->i_message_order);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (p_datareader->i_message_order >= copy_len)
	{
		//최신 copy_len(history_depth) 만큼 sample을 얻기 위한 code 주석 처리(14.05.14)
/*		which = p_datareader->i_message_order-copy_len;

		for (i = 0; i < p_datareader->i_message_order; i++)
		{
			if (p_datareader->pp_message_order[i]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
			{
				which = i;
				break;
			}
		}
    	//printf("imessage : %d, which : %d\n", test, which);

		if (p_datareader->i_message_order - (which+1) < copy_len)
		{
			which = p_datareader->i_message_order - copy_len;
		}
*/
		//added by kyy(Presentation QoS)////////////////////////////////////////////////////
		//read 시작점 결정
/*		if (p_datareader->p_subscriber->subscriber_qos.presentation.access_scope == TOPIC_PRESENTATION_QOS && p_datareader->p_subscriber->subscriber_qos.presentation.coherent_access == true)
		{
			if (p_datareader->last_coh_number < which + copy_len && which > 0)
			{
				which = p_datareader->last_coh_number - copy_len;

				if (which <= 0)
				{
					which = 0;
					copy_len = p_datareader->last_coh_number;
				}

				//printf("which : %d\n", which);
			}
			/////////////////////////////////////////////////////////////////////////////////
		}
*/	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//added by kyy(Presentation QoS)////////////////////////////////////////////////////////////////////////
	//coherent_set 마지막 Data는 응용에서 읽지 않음
	if(p_datareader->p_subscriber->subscriber_qos.presentation.access_scope == TOPIC_PRESENTATION_QOS && p_datareader->p_subscriber->subscriber_qos.presentation.coherent_access == true)
	{
		if(p_datareader->is_coherent_set_end != true)
		{
			if(p_datareader->i_message_order - 2 >= 0 && p_datareader->last_coh_number > 0)
			{
				if(p_datareader->pp_message_order[p_datareader->i_message_order - 2]->coherent_set_number == p_datareader->pp_message_order[p_datareader->i_message_order-1]->coherent_set_number)
				{
					//printf("imessage size is : %d\n", p_datareader->i_message_order);
				}
				else
					return pp_message;
			}
			else
				return pp_message;
		}
		else
		{
			p_datareader->is_coherent_set_end = false;

		}
	}


	for (i = which; i< p_datareader->i_message_order && copy_count < copy_len; i++)
	{
		&p_datareader->pp_message_order[0];
		&p_datareader->pp_message_order[1];
		if (p_datareader->pp_message_order[i]->sampleInfo.sample_state & sample_states
			&& p_datareader->pp_message_order[i]->sampleInfo.view_state & view_states
			&& p_datareader->pp_message_order[i]->sampleInfo.instance_state & instance_states)
		{
			////if(p_datareader->pp_message_order[i]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE){

			////added by kyy(Lifespan QoS)////////////////////////////////////////////////////////////////////////
			//Time_t time;
			//time.sec = p_datareader->pp_message_order[i]->sampleInfo.source_timestamp.sec + p_datareader->pp_message_order[i]->lifespan_duration.sec;//Lifespan 값 얻어와야 함(OK)
			//time.nanosec = p_datareader->pp_message_order[i]->sampleInfo.source_timestamp.nanosec + p_datareader->pp_message_order[i]->lifespan_duration.nanosec;//Lifespan 값 얻어와야 함(OK);

			//////////////////////////////////////////////////////////////////////////////////////////////////////
			////added by kyy(Presentation QoS)////////////////////////////////////////////////////////////////////////
			////coherent_set 마지막 Data는 응용에서 읽지 않음
			//if(p_datareader->p_subscriber->subscriber_qos.presentation.access_scope == TOPIC_PRESENTATION_QOS && p_datareader->p_subscriber->subscriber_qos.presentation.coherent_access == true)
			//{
			//	if(i>0)
			//	{
			//		if(p_datareader->pp_message_order[i-1]->coherent_set_number == p_datareader->pp_message_order[i]->coherent_set_number)
			//		{

			//		}
			//		else
			//			break;
			//	}
			//}

			//if (time.sec < currenTime().sec && p_datareader->pp_message_order[i]->lifespan_duration.sec != 2147483647)
			//{
			//	p_datareader->pp_message_order[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
			//	/*			mutex_lock(&p_datareader->entity_lock);
			//	REMOVE_ELEM(p_datareader->pp_message_order,p_datareader->i_message_order,i);
			//	p_datareader->pp_message_order[i]->related_cachechange == p_rtpsReader->reader_cache->changes[i]
			//	rtps_cachechange_ref(p_rtpsReader->reader_cache->changes[i], true, false);
			//	rtps_cachechange_destory(p_rtpsReader->reader_cache->changes[i]);
			//	REMOVE_ELEM( p_rtpsReader->reader_cache->changes, p_rtpsReader->reader_cache->i_changes, i);
			//	mutex_unlock(&p_datareader->entity_lock);
			//	*/			
			//	INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_message_order[i]));
			//	copy_count++;
			//}
			//else{
			//	p_datareader->pp_message_order[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
			//	INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_message_order[i]));
			//	copy_count++;
			//}


		

			if(p_datareader->pp_message_order[i]->sampleInfo.sample_state != READ_SAMPLE_STATE)
			{
				p_datareader->pp_message_order[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_message_order[i]));
				copy_count++;
			}


			//readcondition by jun
			read_change_sample_status(p_datareader,i);
			read_change_instance_status(p_datareader,i);

		}
	}

	/*
	for (i = 0; i < p_datareader->i_instanceset && copy_count < copy_len; i++)
	{
	for (j = 0; j < p_datareader->pp_instanceset[i]->i_messages && copy_count < copy_len; j++){
	//일단은 안읽어 간것만 가져가도록....... 그렇지 않으면 키에 있는 값만 읽어간 처음에거만....
	if (p_datareader->pp_instanceset[i]->pp_message[j]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
	{
	p_datareader->pp_instanceset[i]->pp_message[j]->sampleInfo.sample_state = READ_SAMPLE_STATE;
	INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_instanceset[i]->pp_message[j]));
	copy_count++;
	}
	}
	}
	*/

	if(p_datareader->p_datareader_listener && p_datareader->p_datareader_listener->on_data_available)
	{
	}else{
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
	}

	

	*p_size = i_count;

	if (i_count != 0)
	{
		*p_size = i_count;
	}

	return pp_message;
}


static message_t** rtps_take(module_object_t* p_this, DataReader* p_datareader, int32_t max_samples, int* p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
{
	message_t** pp_message = NULL;
	int i_count = 0;
	int i;
	int32_t len = 0;
	int32_t copy_len = 0;
	int32_t copy_count = 0;
	int32_t test = 0;

	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;
	if (p_datareader->i_instanceset == 0) return pp_message;

	HISTORYCACHE_LOCK(p_rtpsReader->p_reader_cache);

	for (i = 0; i < p_datareader->i_instanceset; i++)
	{
		len += p_datareader->pp_instanceset[i]->i_messages;
	}

	if (max_samples == LENGTH_UNLIMITED || max_samples == 0)
	{
		copy_len = p_datareader->datareader_qos.history.depth;
	}
	else if (max_samples <= p_datareader->max_len)
	{
		copy_len = max_samples;
	}
	else if (max_samples > p_datareader->max_len)
	{
		*p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	/*
	if (len < copy_len)
	{
	*p_size = 0;
	HISTORYCACHE_UNLOCK(p_rtpsReader->reader_cache);
	return pp_message;
	}
	*/


	//printf("p_datareader->i_message_order %d\r\n", p_datareader->i_message_order );

	for (i = 0; i < p_datareader->i_message_order && (copy_count < copy_len); i++)
	{
		if (p_datareader->pp_message_order[i]->sampleInfo.sample_state & sample_states
			&& p_datareader->pp_message_order[i]->sampleInfo.view_state & view_states
			&& p_datareader->pp_message_order[i]->sampleInfo.instance_state & instance_states)
		{
			//if(p_datareader->pp_message_order[i]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
			{
				p_datareader->pp_message_order[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_message_order[i]));
				copy_count++;

				//readcondition by jun
				take_change_instance_status(p_datareader, i);

				//added by kyy(Presentation QoS)
				/*				if (p_datareader->pp_message_order[i]->is_coherent_set == false)
				{
				p_datareader->pp_message_order[i]->is_coherent_set = true;
				*p_size = i_count;
				rtps_cachechange_ref((rtps_cachechange_t *)p_datareader->pp_message_order[i]->v_related_cachechange, false, false);
				remove_in_instanceset(p_datareader->pp_message_order[i]);
				rtps_cachechange_destory((rtps_cachechange_t *)p_datareader->pp_message_order[i]->v_related_cachechange);
				REMOVE_ELEM(p_datareader->pp_message_order, p_datareader->i_message_order, i);
				break;
				}
				*/				//////////////////////////////////////////////////////////////////////

				rtps_cachechange_ref((rtps_cachechange_t *)p_datareader->pp_message_order[i]->v_related_cachechange, false, false);
				remove_in_instanceset(p_datareader->pp_message_order[i]);
				rtps_cachechange_destory((rtps_cachechange_t *)p_datareader->pp_message_order[i]->v_related_cachechange);
				REMOVE_ELEM(p_datareader->pp_message_order, p_datareader->i_message_order, i);
				i--;				
			}
		}
	}

	/*
	while (p_datareader->i_instanceset && copy_count < copy_len)
	{
	int i_cachesize = p_datareader->pp_instanceset[0]->i_messages;
	while (p_datareader->pp_instanceset[0]->i_messages && copy_count < copy_len)
	{
	if (p_datareader->pp_instanceset[0]->pp_message[0]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
	{
	p_datareader->pp_instanceset[0]->pp_message[0]->sampleInfo.sample_state = READ_SAMPLE_STATE;

	INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_datareader->pp_instanceset[0]->pp_message[0]));
	rtps_cachechange_ref((rtps_cachechange_t *)p_datareader->pp_instanceset[0]->pp_message[0]->related_cachechange, false, false);
	rtps_cachechange_destory((rtps_cachechange_t *)p_datareader->pp_instanceset[0]->pp_message[0]->related_cachechange);
	REMOVE_ELEM( p_datareader->pp_instanceset[0]->pp_message, p_datareader->pp_instanceset[0]->i_messages, 0);

	copy_count++;
	}
	}

	if (p_datareader->pp_instanceset[0]->i_messages == 0)
	{
	instanceset_remove((Entity*)p_datareader, p_datareader->pp_instanceset[0]);
	REMOVE_ELEM( p_datareader->pp_instanceset, p_datareader->i_instanceset, 0);
	}
	}
	*/

	HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
	*p_size = i_count;

	return pp_message;
}


static message_t** rtps_read_instance(module_object_t* p_this, DataReader* p_datareader, InstanceHandle_t handle, int32_t max_samples, int* p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
{
	int i;
	message_t** pp_message = NULL;
	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;
	int i_count = 0;
	int32_t len=0;
	int32_t copy_len = 0;
	int32_t copy_count = 0;
	int32_t which = 0;
	instanceset_t* p_instanceset = NULL;

	if (p_rtpsReader == NULL) return pp_message;
	if (p_datareader->i_instanceset == 0) return pp_message;

	HISTORYCACHE_LOCK(p_rtpsReader->p_reader_cache);

	for (i = 0; i < p_datareader->i_instanceset; i++)
	{
		if (p_datareader->pp_instanceset[i] == (instanceset_t*)handle)
		{
			p_instanceset = p_datareader->pp_instanceset[i];
			len += p_datareader->pp_instanceset[i]->i_messages;
			break;
		}
	}

	if (len == 0)
	{
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	if (max_samples == LENGTH_UNLIMITED || max_samples == 0)
	{
		copy_len = p_datareader->datareader_qos.history.depth;
	}
	else if (max_samples <= p_datareader->max_len)
	{
		copy_len = max_samples;
	}
	else if (max_samples > p_datareader->max_len)
	{
		*p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	/*
	if (len < copy_len)
	{
	*p_size = 0;
	HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
	return pp_message;
	}
	*/
	if (p_instanceset->i_messages >= copy_len)
	{
		which = p_instanceset->i_messages-copy_len;

		for (i = 0; i < p_instanceset->i_messages; i++)
		{
			if (p_instanceset->pp_messages[i]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
			{
				which = i; 
				break;
			}
		}

		if(p_instanceset->i_messages - (which+1) < copy_len)
		{
			which = p_instanceset->i_messages - copy_len;
		}
	}
	
	for (i = 0/*which*/; i< p_instanceset->i_messages && copy_count < copy_len; i++)
	{
		if (p_datareader->pp_message_order[i]->sampleInfo.sample_state & sample_states
			&& p_datareader->pp_message_order[i]->sampleInfo.view_state & view_states
			&& p_datareader->pp_message_order[i]->sampleInfo.instance_state & instance_states)
		{
			p_instanceset->pp_messages[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
			INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_instanceset->pp_messages[i]));
			copy_count++;
		}
	}

	//readcondition by jun
	read_instance_change_sample_status(p_datareader, p_instanceset);
	read_instance_change_instance_status(p_datareader,p_instanceset);

	HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);

	*p_size = i_count;

	if (i_count != 0)
	{
		*p_size = i_count;
	}

	return pp_message;
}


static message_t** rtps_take_instance(module_object_t * p_this, DataReader *p_datareader, InstanceHandle_t handle, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
{
	message_t** pp_message = NULL;
	int i_count = 0;
	int i;
	int32_t len=0;
	int32_t copy_len = 0;
	int32_t copy_count = 0;

	instanceset_t* p_instanceset = NULL;

	rtps_reader_t* p_rtpsReader = p_datareader->p_related_rtps_reader;
	if(p_datareader->i_instanceset == 0) return pp_message;

	HISTORYCACHE_LOCK(p_rtpsReader->p_reader_cache);

	for (i = 0; i < p_datareader->i_instanceset; i++)
	{
		if (p_datareader->pp_instanceset[i] == (instanceset_t*)handle)
		{
			p_instanceset = p_datareader->pp_instanceset[i];
			len += p_datareader->pp_instanceset[i]->i_messages;
			break;
		}
	}

	if (len == 0)
	{
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	if (max_samples == LENGTH_UNLIMITED || max_samples == 0)
	{
		copy_len = p_datareader->datareader_qos.history.depth;
	}
	else if (max_samples <= p_datareader->max_len)
	{
		copy_len = max_samples;
	}
	else if (max_samples > p_datareader->max_len)
	{
		*p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	if (len < copy_len)
	{
		*p_size = 0;
		HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
		return pp_message;
	}

	for (i = 0; i < p_instanceset->i_messages && (copy_count < copy_len); i++)
	{
		if(p_instanceset->pp_messages[i]->sampleInfo.sample_state & sample_states
			&& p_instanceset->pp_messages[i]->sampleInfo.view_state & view_states
			&& p_instanceset->pp_messages[i]->sampleInfo.instance_state & instance_states)
		{

			//if (p_instanceset->pp_messages[i]->sampleInfo.sample_state == NOT_READ_SAMPLE_STATE)
			{
				p_instanceset->pp_messages[i]->sampleInfo.sample_state = READ_SAMPLE_STATE;
				INSERT_ELEM(pp_message, i_count, i_count, message_duplicate(p_instanceset->pp_messages[0]));
				copy_count++;

				//readcondition by jun
				take_change_instance_status(p_datareader, i);

				//added by kyy(Presentation QoS)
				
				if (p_instanceset->pp_messages[i]->is_coherent_set == false)
				{
					p_instanceset->pp_messages[i]->is_coherent_set = true;
					*p_size = copy_count;

					rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange, false, false);
					remove_in_order((Entity *)p_datareader, p_instanceset->pp_messages[i]);
					rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange);
					REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
					break;
				}

				rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange, false, false);
				remove_in_order((Entity *)p_datareader, p_instanceset->pp_messages[i]);
				rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange);
				REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
				i--;
			}
		}
	}

	//readcondition by jun
	take_instance_change_instance_status(p_datareader, p_instanceset);

	HISTORYCACHE_UNLOCK(p_rtpsReader->p_reader_cache);
	*p_size = i_count;

	return pp_message;
}


//by jun
//여러개의 Reader_Thread를 대신해서 하나의 스레드로 동작하기 위한 Reader_Thread
static int rtps_add_thread(module_object_t* p_this, Entity* p_entity)
{
	DomainParticipant* p_domainparticipant;
	p_domainparticipant = (DomainParticipant*)p_entity; 


	set_domainparticipant(p_domainparticipant);
	
	switch (p_domainparticipant->i_flag_reader_writer_thread)
	{
		case READER_THREAD:
			p_domainparticipant->thread_id = thread_create2(p_domainparticipant, &p_domainparticipant->entity_wait, &p_domainparticipant->entity_lock, "RTPS Reader", (void*)reader_thread2, 0, false );
			break;
		case WRITER_THREAD:
//writer by jun
//Wrtier 스레드 생성
			p_domainparticipant->thread_id2 = thread_create2(p_domainparticipant, &p_domainparticipant->entity_wait, &p_domainparticipant->entity_lock, "RTPS Writer", (void*)writer_thread2, 0, false );
#ifdef Android
			
			//msleep(2000);
#endif

			//trace_msg(p_this, TRACE_LOG,"p_domainparticipant->thread_id2 : %p", p_domainparticipant->thread_id2);

			break;
		default:
			trace_msg(p_this, TRACE_ERROR,"RTPS addEntity UNKNOWN_ENTITY");
			return MODULE_ERROR_UNKNOWN_ENTITY;
			break;
	}

	return MODULE_SUCCESS;
}


void set_enable_multicast(service_t* p_service, rtps_statefulwriter_t* p_statefulwriter)
{
	service_sys_t *p_sys=p_service->p_sys;
	p_statefulwriter->b_enable_multicast = p_sys->reliable_multicast_enable;
}


////////////////////////////////////////////////////
//////////// 디버깅을 위한 Thread
////////////////////////////////////////////////////


static rtps_writer_t *p_participant_writer = NULL;
static rtps_writer_t *p_pub_writer = NULL;
static rtps_writer_t *p_sub_writer = NULL;
static rtps_writer_t *p_topic_writer = NULL;
static rtps_writer_t *p_liveness_writer = NULL;

static rtps_reader_t *p_participant_reader = NULL;
static rtps_reader_t *p_pub_reader = NULL;
static rtps_reader_t *p_sub_reader = NULL;
static rtps_reader_t *p_topic_reader = NULL;
static rtps_reader_t *p_liveness_reader = NULL;

static DWORD tid;


static DomainParticipant* p_check_domainparticipant;



static bool standby = false;

bool all_standby()
{
	return standby;
}


void set_domainparticipant(DomainParticipant *p_domainparticipant)
{
	p_check_domainparticipant = p_domainparticipant;
}

void set_participant_writer(rtps_writer_t *p_writer)
{
	
	p_participant_writer = p_writer;

	p_writer->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Participant Writer", tid);
}

void set_pub_writer(rtps_writer_t *p_writer)
{
	p_pub_writer = p_writer;
	p_writer->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Pub Writer", tid);
}

void set_sub_writer(rtps_writer_t *p_writer)
{
	p_sub_writer = p_writer;
	p_writer->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Sub Writer", tid);
}

void set_topic_writer(rtps_writer_t *p_writer)
{
	p_topic_writer = p_writer;
	p_writer->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Topic Writer", tid);
}

void set_liveness_writer(rtps_writer_t *p_writer)
{
	p_liveness_writer = p_writer;
	p_writer->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Liveness Writer", tid);

	if (p_liveness_writer != NULL && p_liveness_reader != NULL)
	{
		standby = true;
	}
}


void set_participant_reader(rtps_reader_t *p_reader)
{
	p_participant_reader = p_reader;

	p_reader->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Participant Reader", tid);
}

void set_pub_reader(rtps_reader_t *p_reader)
{
	p_pub_reader = p_reader;
	p_reader->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Pub Reader", tid);
}

void set_sub_reader(rtps_reader_t *p_reader)
{
	p_sub_reader = p_reader;
	p_reader->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Sub Reader", tid);
}

void set_topic_reader(rtps_reader_t *p_reader)
{
	p_topic_reader = p_reader;
	p_reader->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Topic Reader", tid);
}

void set_liveness_reader(rtps_reader_t *p_reader)
{
	p_liveness_reader = p_reader;
	p_reader->checked_time = currenTime();
	trace_msg(NULL, TRACE_LOG, "Add [Process:%d] Liveness Reader", tid);

	if (p_liveness_writer != NULL && p_liveness_reader != NULL)
	{
		standby = true;
	}

}




Time_t time_subtraction(const Time_t minuend_time, const Time_t subtrahend_time);


static bool check_time_writer(Time_t _now, rtps_writer_t *p_writer, Time_t *diff_time)
{
	Time_t diff_t;

	if (p_writer == NULL) return false;

	diff_t = time_subtraction(_now, p_writer->checked_time);;

	if (diff_t.sec >= 5)
	{
		//printf("[Process:%d] check_time_writer[Depth:%d]\r\n", tid, p_check_domainparticipant->p_writer_thread_fifo->i_depth);

		*diff_time = diff_t;
		return true;
	}

	return false;
}


static bool check_time_reader(Time_t _now, rtps_reader_t *p_reader, Time_t *diff_time)
{
	Time_t diff_t;

	if (p_reader == NULL) return false;

	diff_t = time_subtraction(_now, p_reader->checked_time);;

	if (diff_t.sec >= 5)
	{

		
		//printf("[Process:%d] check_time_reader[Depth:%d]\r\n", tid, p_check_domainparticipant->p_reader_thread_fifo->i_depth);

		*diff_time = diff_t;
		return true;
	}

	return false;
}



static void Check_Thread()
{

	Time_t now_time = currenTime();
	Time_t diff_time;

	tid = GetCurrentProcessId();
	

	while (is_quit() == false)
	{
		now_time = currenTime();

		if (check_time_writer(now_time, p_participant_writer, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_participant_writer[%d]\r\n", p_check_domainparticipant->p_writer_thread_fifo->i_depth, tid, diff_time.sec);
		}

		if (check_time_writer(now_time, p_pub_writer, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_pub_writer[%d]\r\n", p_check_domainparticipant->p_writer_thread_fifo->i_depth, tid, diff_time.sec);
		}

		if (check_time_writer(now_time, p_sub_writer, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_sub_writer[%d]\r\n", p_check_domainparticipant->p_writer_thread_fifo->i_depth, tid, diff_time.sec);
		}

		if (check_time_writer(now_time, p_topic_writer, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_topic_writer[%d]\r\n", p_check_domainparticipant->p_writer_thread_fifo->i_depth, tid, diff_time.sec);
		}

		if (check_time_writer(now_time, p_liveness_writer, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_liveness_writer[%d]\r\n", p_check_domainparticipant->p_writer_thread_fifo->i_depth, tid, diff_time.sec);
		}


		/////////////////////////
		if (check_time_reader(now_time, p_participant_reader, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_participant_reader[%d](%d,%d)\r\n", p_check_domainparticipant->p_reader_thread_fifo->i_depth, tid, diff_time.sec, get_default_port(), get_metatraffic_port());
		}

		if (check_time_reader(now_time, p_pub_reader, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_pub_reader[%d](%d,%d)\r\n", p_check_domainparticipant->p_reader_thread_fifo->i_depth, tid, diff_time.sec, get_default_port(), get_metatraffic_port());
		}

		if (check_time_reader(now_time, p_sub_reader, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_sub_reader[%d](%d,%d)\r\n", p_check_domainparticipant->p_reader_thread_fifo->i_depth, tid, diff_time.sec, get_default_port(), get_metatraffic_port());
		}

		if (check_time_reader(now_time, p_topic_reader, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_topic_reader[%d](%d,%d)\r\n", p_check_domainparticipant->p_reader_thread_fifo->i_depth, tid, diff_time.sec, get_default_port(), get_metatraffic_port());
		}

		if (check_time_reader(now_time, p_liveness_reader, &diff_time))
		{
			printf("Error[Depth:%d][Process:%d] p_liveness_reader[%d](%d,%d)\r\n", p_check_domainparticipant->p_reader_thread_fifo->i_depth, tid, diff_time.sec, get_default_port(), get_metatraffic_port());
		}

		Sleep(500);

	}


}
