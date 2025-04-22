/*
	access udp 모듈...

	이력
	2010-08-9
*/
#include <core.h>


struct access_sys_t
{
    int fd;
	int bind_port;
    int i_mtu;
	bool b_auto_mtu;
};

static int  Open ( module_object_t * );
static void Close( module_object_t * );


#define CACHING_TEXT "Caching value in ms"
#define CACHING_LONGTEXT "Caching value for UDP streams."

#define AUTO_MTU_TEXT "Autodetection of MTU"
#define AUTO_MTU_LONGTEXT "Automatically detect the line's MTU."

module_define_start(access_udp);
    set_shortname( "UDP" );
    set_description("UDP input");
    set_category( CAT_INPUT );
    add_integer( "udp-caching", DEFAULT_PTS_DELAY / 1000, NULL, CACHING_TEXT, CACHING_LONGTEXT, true );
    add_bool( "udp-auto-mtu", true, NULL, AUTO_MTU_TEXT, AUTO_MTU_LONGTEXT, true );
    set_capability( "access", 0 );
    add_shortcut( "udp" );
    add_shortcut( "udpstream" );
    add_shortcut( "udp4" );
    add_shortcut( "udp6" );
    set_callbacks( Open, Close );
module_define_end();


static data_t *GetData(access_t *p_access);
static data_t *GetDataFrom(access_t *p_access, struct sockaddr *client, int *l);
static data_t *GetData_For_Service(service_t * p_service, access_t *p_access, int spdp_fd, int sedp_fd, int data_fd);
static int GetBind(access_t *p_access);

static int  Open (module_object_t *p_this)
{
	access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;

	int  i_bind_port, i_server_port = 0;
	char *psz_parser, *psz_server_addr, *psz_bind_addr = "";
	char *psz_name = NULL;

	if( !p_access->psz_path ||  (strncmp( p_access->psz_path, "udp:", 4 )))
	{
		return MODULE_ERROR_NORMAL;
	}

	psz_name = strdup( &p_access->psz_path[4] );
	
	var_create( OBJECT(p_access), "ipv4", VAR_BOOL | VAR_DOINHERIT );
    var_create( OBJECT(p_access), "ipv6", VAR_BOOL | VAR_DOINHERIT );


	if( *p_access->psz_access )
    {
        value_t val;
        if( !strncmp( p_access->psz_access,"udp4",6))
        {
            val.b_bool = true;
            var_set( OBJECT(p_access), "ipv4", val );

            val.b_bool = false;
            var_set( OBJECT(p_access), "ipv6", val );
        }
        else if( !strncmp( p_access->psz_access, "udp6", 6 ))
        {
            val.b_bool = true;
            var_set( OBJECT(p_access), "ipv6", val );

            val.b_bool = false;
            var_set( OBJECT(p_access), "ipv4", val );
        }
    }
	
	i_bind_port = var_create_get_integer( OBJECT(p_access), "server-port" );

	/* [serveraddr[:serverport]][@[bindaddr]:[bindport]] */
    psz_parser = strchr( psz_name, '@' );
    if( psz_parser != NULL )
    {
        /* Found bind address and/or bind port */
        *psz_parser++ = '\0';
        psz_bind_addr = psz_parser;

        if( *psz_parser == '[' )
            psz_parser = strchr( psz_parser, ']' );

        if( psz_parser != NULL )
        {
            psz_parser = strchr( psz_parser, ':' );
            if( psz_parser != NULL )
            {
                *psz_parser++ = '\0';
                i_bind_port = atoi( psz_parser );
            }
        }
    }

    psz_server_addr = psz_name;
	psz_parser = psz_server_addr;
    if( *psz_server_addr == '[' )
        psz_parser = strchr( psz_name, ']' );

    if( psz_parser != NULL )
    {
        psz_parser = strchr( psz_parser, ':' );
        if( psz_parser != NULL )
        {
            *psz_parser++ = '\0';
            i_server_port = atoi( psz_parser );
        }
    }

	trace_msg(OBJECT(p_access), TRACE_DEBUG, "[Access UDP Module] Opening server=%s:%d local=%s:%d", psz_server_addr, i_server_port, psz_bind_addr, i_bind_port);

	p_access->pf_data = GetData;
	p_access->pf_data_for_service = GetData_For_Service;
	p_access->pf_getBind = GetBind;
	p_access->pf_dataFrom = GetDataFrom;

	p_access->p_sys = p_sys = malloc( sizeof( access_sys_t ) );
    memset (p_sys, 0, sizeof (*p_sys));

	p_sys->fd = p_access->fd = net_open_udp( OBJECT(p_access), psz_bind_addr, &i_bind_port, psz_server_addr, i_server_port);
	if( p_sys->fd < 0 )
    {
		trace_msg(OBJECT(p_access), TRACE_ERROR, "[Access UDP Module] Cannot open socket", psz_server_addr, i_server_port, psz_bind_addr, i_bind_port );
        FREE( psz_name );
        FREE( p_sys );
		return MODULE_ERROR_NETWORK;
    }

	p_sys->bind_port = i_bind_port;
    FREE( psz_name );

	p_sys->i_mtu = var_create_get_integer( OBJECT(p_access), "mtu" );

	if( p_sys->i_mtu <= 1 ){
        p_sys->i_mtu  = 1500;
	}

    p_sys->b_auto_mtu = var_create_get_bool( OBJECT(p_access), "udp-auto-mtu" );;

    var_create( OBJECT(p_access), "udp-caching", VAR_INTEGER | VAR_DOINHERIT );

	return MODULE_SUCCESS;
}

static void Close(module_object_t *p_this)
{
	access_t     *p_access = (access_t*)p_this;
	access_sys_t *p_sys = p_access->p_sys;

	net_close( p_sys->fd );
	FREE(p_access->p_sys);
}


static data_t *GetData(access_t *p_access)
{
	access_sys_t *p_sys = p_access->p_sys;
    data_t      *p_data = NULL;
	int			i_size = 0;

    p_data = data_new(p_sys->i_mtu);
    i_size = net_read( OBJECT(p_access), p_sys->fd, NULL, p_data->p_data, p_sys->i_mtu, false);

    if(i_size <= 0 )
    {
        data_release(p_data);
        return NULL;
    }

	//printf("GetData : %d\n", i_size);

	p_data->i_size = i_size;

    if( (p_data->i_size >= p_sys->i_mtu) && p_sys->b_auto_mtu &&  p_sys->i_mtu < 32767 )
    {
        p_sys->i_mtu *= 2;
		trace_msg(OBJECT(p_access), TRACE_WARM, "[Access UDP Module] increasing MTU to %d", p_sys->i_mtu );
    }

    return p_data;
};

static data_t *GetDataFrom(access_t *p_access, struct sockaddr *client , int *l)
{
	access_sys_t *p_sys = p_access->p_sys;
	data_t      *p_data = NULL;
	int			i_size = 0;

	p_data = data_new(p_sys->i_mtu);
	i_size = net_readFrom(OBJECT(p_access), p_sys->fd, NULL, p_data->p_data, p_sys->i_mtu, false, client, l);

	if (i_size <= 0)
	{
		data_release(p_data);
		return NULL;
	}

	//printf("GetDataFrom : %d\n", i_size);

	p_data->i_size = i_size;

	if ((p_data->i_size >= p_sys->i_mtu) && p_sys->b_auto_mtu &&  p_sys->i_mtu < 32767)
	{
		p_sys->i_mtu *= 2;
		trace_msg(OBJECT(p_access), TRACE_WARM, "[Access UDP Module] increasing MTU to %d", p_sys->i_mtu);
	}

	return p_data;
};


//by jun
static data_t *GetData_For_Service(service_t * p_service, access_t *p_access, int spdp_fd, int sedp_fd, int data_fd)
{

	service_sys_t *p_sys1 = p_service->p_sys;
	access_sys_t *p_sys = p_access->p_sys;
	
    data_t      *p_data = NULL;
	int			i_size = 0;
	uint8_t		data[3000];

   // p_data = data_new(p_sys->i_mtu);

	p_service->p_networkdata[0] = NULL;
	p_service->p_networkdata[1] = NULL;
	p_service->p_networkdata[2] = NULL;

    i_size = net_read_for_service(p_service, data, p_sys->i_mtu, false, spdp_fd, sedp_fd, data_fd, &p_service->b_end);

    //if(i_size <= 0 )
    //{
    //    data_release(p_data);
    //    return NULL;
    //}

	/*p_data->i_size = i_size;

    if( (p_data->i_size >= p_sys->i_mtu) && p_sys->b_auto_mtu &&  p_sys->i_mtu < 32767 )
    {
        p_sys->i_mtu *= 2;
		trace_msg(OBJECT(p_access), TRACE_WARM, "[Access UDP Module] increasing MTU to %d", p_sys->i_mtu );
    }*/

    return p_data;
};


static int GetBind(access_t *p_access)
{
	if(p_access != NULL)
	{
		return p_access->p_sys->bind_port;
	}

	return -1;
}