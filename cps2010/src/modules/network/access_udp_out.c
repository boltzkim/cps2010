/*
	access udp out 모듈...

	이력
	2010-10-15
*/
#include <core.h>





#define SOUT_CFG_PREFIX "udp-out-"


typedef struct access_thread_t
{
    MODULE_COMMON_FIELD

    data_fifo_t *p_fifo;

    int         i_handle;
    int64_t     i_caching;
    int         i_group;
	bool		is_confirm_target;
	bool		is_use_service_fifo;
#ifdef _MSC_VER
	SOCKADDR_IN target_addr;
#else
	struct sockaddr_in target_addr;
#endif
} access_thread_t;

struct access_out_sys_t
{
	access_thread_t *p_thread;
	int	fd;
};





static int  Open ( module_object_t * );
static void Close( module_object_t * );


#define CACHING_TEXT "Caching value (ms)"
#define CACHING_LONGTEXT  "Default caching value for out UDP. This value should be set in milliseconds." 

module_define_start(access_udp_out);
    set_description("UDP output");
    set_shortname( "UDP" );
    set_category( CAT_OUT );
	add_integer( SOUT_CFG_PREFIX "caching", DEFAULT_PTS_DELAY / 1000, NULL, CACHING_TEXT, CACHING_LONGTEXT, true );
    set_capability( "access out", 100 );
    add_shortcut( "udp" );
    set_callbacks( Open, Close );
module_define_end();


static int Write( access_out_t *p_access, data_t *p_data );
static void ThreadWrite( module_object_t * );

static int  Open(module_object_t *p_this)
{
	access_out_t       *p_access = (access_out_t*)p_this;
	access_out_sys_t *p_sys = NULL;
	int i_handle;
	char	*p_parser = NULL;
    char	*p_dst_addr = NULL;
    int		i_port = 0;

	if(p_access == NULL || p_access->p_name == NULL) return MODULE_ERROR_NORMAL;

	p_sys = p_access->p_sys = malloc(sizeof(access_out_sys_t));
	memset(p_sys, 0, sizeof(access_out_sys_t));

	if(!p_sys)
	{
		return MODULE_ERROR_MEMORY;
	}

	


	p_sys->fd = p_access->fd;
	p_sys->p_thread =  object_create( OBJECT(p_access), sizeof( access_thread_t ) );

	if( !p_sys->p_thread )
    {
        trace_msg(OBJECT(p_access), TRACE_ERROR, "[Access UDP OUT Module] Cannot create thread");
		FREE(p_sys);
		return MODULE_ERROR_THREAD;
    }

	object_attach( OBJECT(p_sys->p_thread), OBJECT(p_access) );

    p_sys->p_thread->b_end  = false;

	if(p_this->p_parent->i_object_type == OBJECT_SERVICE)
	{
		if(((service_t*)p_this->p_parent)->p_network_fifo){
			p_sys->p_thread->p_fifo = ((service_t*)p_this->p_parent)->p_network_fifo;
			p_sys->p_thread->is_use_service_fifo = true;
		}

	}else{
		p_sys->p_thread->p_fifo = data_fifo_new();
		p_sys->p_thread->is_use_service_fifo = false;
	}


	if(p_access->p_name && strlen(p_access->p_name) > 0)
	{
		p_sys->p_thread->is_confirm_target = true;

		p_parser = strdup( p_access->p_name );

		p_dst_addr = p_parser;
		i_port = 0;

		if ( *p_parser == '[' )
		{
			while( *p_parser && *p_parser != ']' )
			{
				p_parser++;
			}
		}
		while( *p_parser && *p_parser != ':' )
		{
			p_parser++;
		}
		if( *p_parser == ':' )
		{
			*p_parser = '\0';
			p_parser++;
			i_port = atoi( p_parser );
		}

		if(p_sys->fd)
		{
			i_handle = p_sys->fd;
			p_sys->p_thread->target_addr.sin_family = AF_INET;
			p_sys->p_thread->target_addr.sin_port = htons(i_port);
			p_sys->p_thread->target_addr.sin_addr.s_addr = inet_addr(p_dst_addr);


		}else{
			i_handle = connect_udp( p_this, p_dst_addr, i_port, 0, p_sys->fd);
		}
		

		


		/*if(i_port != 7400) i_handle = connect_udp( p_this, p_dst_addr, i_port, 0, p_sys->fd);
		else i_handle =  p_sys->fd;*/

		if( i_handle == -1 )
		{
			 trace_msg(OBJECT(p_access), TRACE_ERROR, "[Access UDP OUT Module]failed to open a connection (udp)");
			 object_destroy( OBJECT(p_sys->p_thread) );
			 FREE(p_sys);
			 return MODULE_ERROR_NETWORK;
		}
	}else
	{
		p_sys->p_thread->is_confirm_target = false;

		i_handle = p_sys->fd;
	}

	if(!p_sys->p_thread->is_use_service_fifo){
		if( thread_create( OBJECT(p_sys->p_thread), "out write thread", (void*)ThreadWrite,
							   THREAD_PRIORITY_HIGHEST, false ) )
		{
			trace_msg(OBJECT(p_access), TRACE_ERROR, "[Access UDP OUT Module] cannot spawn access out thread" );
			object_destroy( OBJECT(p_sys->p_thread) );
			return MODULE_ERROR_THREAD;
		}
	}

	p_sys->p_thread->i_handle = i_handle;

	p_sys->p_thread->i_caching = var_create_get_integer(p_this, SOUT_CFG_PREFIX "caching");
	p_access->mtu = var_create_get_integer(p_this, "mtu");

	p_access->pf_write = Write;

	trace_msg(OBJECT(p_access), TRACE_TRACE, "[Access UDP OUT Module] udp access output opened(%s:%d)",
             p_dst_addr, i_port );

    FREE( p_dst_addr );

	return MODULE_SUCCESS;
}

static void  Close(module_object_t *p_this)
{
	access_out_t       *p_access = (access_out_t*)p_this;
	access_out_sys_t *p_sys = p_access->p_sys;
	
	p_sys->p_thread->b_end = true;

	if(!p_sys->p_thread->is_confirm_target)
	{
		if(!p_sys->p_thread->is_use_service_fifo){
			data_t *p_data = (data_t *)malloc(sizeof(data_t));
			memset(p_data,'\0',sizeof(data_t));
			data_fifo_put(p_sys->p_thread->p_fifo, p_data);

			thread_join( OBJECT(p_sys->p_thread) );

			data_fifo_release( p_sys->p_thread->p_fifo );
		}
	}

	object_detach( OBJECT(p_sys->p_thread) );
    object_destroy( OBJECT(p_sys->p_thread) );

	if(p_sys->fd == 0)
	{
		net_close(p_sys->p_thread->i_handle );
	}

	FREE(p_access->p_sys);
}


static int Write( access_out_t *p_access, data_t *p_data )
{
    access_out_sys_t   *p_sys = p_access->p_sys;
    int i_len = p_data->i_size;

#ifdef _MSC_VER
	SOCKADDR_IN target_addr;
#else
	struct sockaddr_in target_addr;
#endif

	//by jun
	p_data->socket_id = p_sys->p_thread->i_handle;
	//trace_msg(NULL, TRACE_LOG, "Write");

    //data_fifo_put( p_sys->p_thread->p_fifo, p_data );

	{
        data_t *p_pk;
        p_pk = p_data;

		target_addr.sin_family = AF_INET;
		target_addr.sin_port = htons(p_pk->i_port);
		target_addr.sin_addr.s_addr = inet_addr(p_pk->p_address);

		if (p_pk->i_size > 0)
		{
			if (sendto(p_pk->socket_id, (const char*)p_pk->p_data, p_pk->i_size,0, (struct sockaddr *) &target_addr, sizeof(target_addr)) == -1)
			{

			}
		}



		data_release(p_pk);
    }

    return( p_sys->p_thread->b_end ? -1 : i_len );
}

#ifdef _MSC_VER
#include "winsock2.h"
#endif


static void ThreadWrite( module_object_t *p_this )
{
    access_thread_t *p_thread = (access_thread_t*)p_this;
    mtime_t              i_date_last = -1;
    mtime_t              i_to_send = p_thread->i_group;
    int                  i_dropped_packets = 0;

	

    while( !p_thread->b_end )
    {
        data_t *p_pk;
        p_pk = data_fifo_get( p_thread->p_fifo );

		if(p_thread->b_end ){
			data_release(p_pk);
			break;
		}

		//trace_msg(NULL, TRACE_LOG, "ThreadWrite : %s, %d", p_pk->p_address, p_pk->i_port);


		/*if(!p_thread->is_confirm_target && p_pk->p_address != NULL && p_pk->i_port > 0)
		{*/
			p_thread->target_addr.sin_family = AF_INET;
			p_thread->target_addr.sin_port = htons(p_pk->i_port);
			p_thread->target_addr.sin_addr.s_addr = inet_addr(p_pk->p_address);

		//}


		/*if(strcmp(p_pk->p_address,"192.168.1.157") == 0)
		{
			data_release(p_pk);
			continue;
		}*/

		if(p_pk->i_size > 0){
			//printf("send data to %s:%d (%d)\r\n",p_pk->p_address, p_pk->i_port, p_pk->i_size);
			if(sendto(p_thread->i_handle, p_pk->p_data, p_pk->i_size,0, (struct sockaddr *) &p_thread->target_addr, sizeof(p_thread->target_addr)) == -1)
			{
				trace_msg(OBJECT(p_this), TRACE_WARM, "send error : %s:%d", p_pk->p_address, p_pk->i_port);
			}

			//Sleep(50);

			/*if( send( p_thread->i_handle, p_pk->p_data, p_pk->i_size, 0 )
				  == -1 )
			{
				trace_msg(OBJECT(p_this), TRACE_WARM, "send error");
			}*/

		}

        if( i_dropped_packets )
        {
			trace_msg(OBJECT(p_this), TRACE_TRACE, "dropped %i packets", i_dropped_packets );
            i_dropped_packets = 0;
        }

		data_release(p_pk);
    }
}

