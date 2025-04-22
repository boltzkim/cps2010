/*
	UDP ����...

	�ۼ��� : 
	�̷�
	2010-08-9 : ����
*/

/**
	@file udp.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DDS udp ���� API�� ������ ���� 

*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



/** 
	@brief DDS���� Multicast ������ Hop Limit�� �����ϴ� �Լ� 
	@param[in] p_this DCPS Module �� ACCESS Service Module ������   
	@param[in] fd DDS���� ����ϴ� Multicast ����  ����
	@param[in] family ������ AF_INET Ÿ�� ���� ����  
	@param[in] hlim DDS Multicast Hop Limit �� ���� 
	@return	Multicast ���� Hop Limit ���� ���� ���� �� ��ȯ
*/
static int NetSetMcastHopLimit(module_object_t *p_this, int fd, int family, int hlim )
{
#ifndef SYS_BEOS
    int proto, cmd;
	//socket type
    switch( family )
    {
        case AF_INET:
            proto = SOL_IP;
            cmd = IP_MULTICAST_TTL;
            break;

#ifdef IPV6_MULTICAST_HOPS
        case AF_INET6:
            proto = SOL_IPV6;
            cmd = IPV6_MULTICAST_HOPS;
            break;
#endif

        default:
			trace_msg(p_this,TRACE_ERROR,"%s", strerror(EAFNOSUPPORT));
            return MODULE_ERROR_NORMAL;
    }
	//hop limit ����
    if( setsockopt( fd, proto, cmd, (char*)&hlim, sizeof( hlim ) ) < 0 )
    {
        unsigned char buf;

        buf = (unsigned char)(( hlim > 255 ) ? 255 : hlim);
        if( setsockopt( fd, proto, cmd, &buf, sizeof( buf ) ) )
            return MODULE_ERROR_NORMAL;
    }
#endif
    return MODULE_SUCCESS;
}

/** 
	@brief DDS���� Multicast Socket�� �����ϴ� �Լ� 
	@param[in] p_this DCPS Module �� ACCESS Service Module ������    
	@param[in] fd DDS���� ����ϴ� Multicast ����  ����
	@param[in] family ������ AF_INET Ÿ�� ���� ���� 
	@param[in] str DDS ���Ͽ� ������ Multicast �ּ� ����
	@return	Multicast ���� ���� ���� ���� �� ��ȯ
*/
static int Net_SetMcastIface(module_object_t *p_this, int fd, int family, const char *str)
{
    switch( family )
    {
#ifndef SYS_BEOS
        case AF_INET:
        {	//AF_INET
            struct in_addr addr;
			//�ּ� ��ȯ
#if !defined(HAVE_INET_PTON)
            if( inet_pton1( AF_INET, str, &addr) <= 0 )
#else
			if( inet_pton( AF_INET, str, &addr) <= 0 )
#endif
            {
				trace_msg(p_this, TRACE_ERROR, "Invalid multicast interface %s", str);
                return MODULE_ERROR_NORMAL;
            }
			//multicast �ּ� ����
            if( setsockopt( fd, SOL_IP, IP_MULTICAST_IF, (char*)&addr, sizeof( addr ) ) < 0)
            {
				trace_msg(p_this, TRACE_ERROR, "Cannot use %s as multicast interface: %s", str, strerror(errno));
                return MODULE_ERROR_NORMAL;
            }
            break;
        }
#endif

#ifdef IPV6_MULTICAST_IF
        case AF_INET6:
        {
#ifdef _MSC_VER
			int scope = atoi( str );
#else
            int scope = if_nametoindex( str );
#endif

            if( scope == 0 )
            {
				trace_msg(p_this, TRACE_ERROR, "Invalid multicast interface %s", str );
                return MODULE_ERROR_NORMAL;
            }

            if( setsockopt(fd, SOL_IPV6, IPV6_MULTICAST_IF, (char*)&scope, sizeof( scope ) ) < 0 )
            {
				trace_msg(p_this, TRACE_ERROR, "Cannot use %s as multicast interface: %s", str, strerror(errno));
                return MODULE_ERROR_NORMAL;
            }
            break;
        }
#endif

        default:
			trace_msg(p_this, TRACE_WARM, "%s", strerror(EAFNOSUPPORT));
            return MODULE_ERROR_NORMAL;
    }

	return MODULE_SUCCESS;
}

/** 
	@brief DDS���� UDP ������ �����ϴ� �Լ� 
	@param[in] p_this DCPS Module �� ACCESS Service Module ������  
	@param[in] psz_host DCPS���� ������ host �ּ� ����
	@param[in] i_port Host Port ��ȣ ���� 
	@param[in] i_hlim DDS Multicast Socket�� Hop Limit ���� 
	@param[in] i_fd multicast ���� ����
	@return	Multicast ���� ���� ���� ���� ���� �� ��ȯ
*/
int connect_udp(module_object_t* p_this, const char* psz_host, int i_port, int i_hlim, int i_fd)
{
    struct addrinfo hints, *res=NULL, *ptr=NULL;
    int		i_val, i_handle = -1;
    bool	b_unreach = false;

    if(i_port == 0) i_port = DEFALT_UDP_PORT;

	if( i_hlim < 1 ){
		//ttl ����
        i_hlim = var_create_get_integer( p_this, "ttl" );
	}
	//���� addr type ���� 
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;

	trace_msg(p_this, TRACE_DEBUG, "net: connecting to %s port %d", psz_host, i_port);
	//res�� �ּ� ����
    i_val = get_addr_info( p_this, psz_host, i_port, &hints, &res );

    if( i_val )
    {
		trace_msg(p_this, TRACE_ERROR, "cannot resolve %s port %d : %s", psz_host, i_port, gai_str_error(i_val));
        return -1;
    }

    for( ptr = res; ptr != NULL; ptr = ptr->ai_next )
    {
        int fd;
        char *psz_mif;
		//���� ���� 
		if(i_fd == 0)
			fd = net_socket( p_this, ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		else
			fd = i_fd;

        if( fd == -1 )
            continue;
#if !defined( SYS_BEOS )
        else
        {
			if(i_fd == 0){
				int i_val;
				//���� option ����
				i_val = 0x80000;
				setsockopt( fd, SOL_SOCKET, SO_RCVBUF, (void *)&i_val,
							sizeof( i_val ) );
				i_val = 0x80000;
				setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (void *)&i_val,
							sizeof( i_val ) );

				i_val = 1;
				setsockopt( fd, SOL_SOCKET, SO_BROADCAST, (void*)&i_val,
							sizeof( i_val ) );
			}
        }
#endif
		//hop limit ����
		if( i_hlim > 0 && i_fd == 0){
            NetSetMcastHopLimit( p_this, fd, ptr->ai_family, i_hlim );
		}

        psz_mif = config_get_psz( p_this, (ptr->ai_family != AF_INET) ? "miface" : "interface-addr" );
        if( psz_mif != NULL && i_fd == 0)
        {	//UDP ���Ͽ� multicast ����
            Net_SetMcastIface( p_this, fd, ptr->ai_family, psz_mif );
            FREE( psz_mif );
        }
			//UDP ���� ����
        if( connect( fd, ptr->ai_addr, ptr->ai_addrlen ) == 0 )
        {
            i_handle = fd;
            break;
        }

#if defined( WIN32 ) || defined( UNDER_CE )
        if( WSAGetLastError () == WSAENETUNREACH )
#else
        if( errno == ENETUNREACH )
#endif
            b_unreach = true;
        else
        {
			trace_msg(p_this, TRACE_WARM, "%s port %d : %s", psz_host, i_port, strerror( errno ));
            net_close(fd);
            continue;
        }
    }

    free_addr_info( res );

    if( i_handle == -1 )
    {
        if( b_unreach )
		{
			trace_msg(p_this, TRACE_ERROR, "Host %s port %d is unreachable", psz_host, i_port);
		}
        return -1;
    }

    return i_handle;
}

/** 
	@brief DDS���� UDP ���� ������ �����ϴ� �Լ� 
	@param[in] p_this DCPS Module �� ACCESS Service Module ������  
	@param[in] psz_bind DCPS���� ������ host�� IP �ּ� ����
	@param[in] i_bind DCPS���� ������ host�� Port ����
	@param[in] psz_server DCPS���� ������ Server�� IP �ּ� ���� 
	@param[in] i_server DCPS���� ������ Server�� Port ����
	@return	Multicast ���� ���� ���� ���� ���� �� ��ȯ
*/
int net_open_udp( module_object_t* p_this, const char* psz_bind, int* i_bind, const char* psz_server, int i_server )
{
    value_t				v4, v6;
    void				*private;
    network_socket_t	sock;
    moduleload_t		*p_network = NULL;

    if( psz_server == NULL ) psz_server = "";
    if( psz_bind == NULL ) psz_bind = "";

    sock.psz_bind_addr   = psz_bind;
    sock.i_bind_port     = *i_bind;
    sock.psz_server_addr = psz_server;
    sock.i_server_port   = i_server;
    sock.i_ttl           = 0;
    sock.v6only          = 0;
    sock.i_handle        = -1;


	trace_msg(p_this, TRACE_DEBUG, "net: connecting to '[%s]:%d@[%s]:%d'", psz_server, i_server, psz_bind, *i_bind);

    var_create( p_this, "ipv4", VAR_BOOL | VAR_DOINHERIT );
    var_get( p_this, "ipv4", &v4 );
    var_create( p_this, "ipv6", VAR_BOOL | VAR_DOINHERIT );
    var_get( p_this, "ipv6", &v6 );

    if( !v4.b_bool )
    {
		if( v6.b_bool ){
            sock.v6only = 1;
		}

        private = p_this->v_private;
        p_this->v_private = (void*)&sock;
		p_network = moduleUse( p_this, "network", "ipv6", true );

		if(p_network != NULL){
            moduleUnuse( p_this, p_network );
		}

        p_this->v_private = private;

		if((sock.i_handle != -1) && ((sock.v6only == 0) || v6.b_bool)){
            return sock.i_handle;
		}
    }
	//IPV4
    if( !v6.b_bool )
    {
        int fd6 = sock.i_handle;
		//p_access���� sock ���� 
        private = p_this->v_private;
        p_this->v_private = (void*)&sock;
		//ipv4�� ���� ���� p_this->v_private�� ���� ����
        p_network = moduleUse( p_this, "network", "ipv4", true);

		if( p_network != NULL ){
            moduleUnuse( p_this, p_network );
		}

		*i_bind = sock.i_bind_port;

        p_this->v_private = private;

        if( fd6 != -1 )
        {
            if( sock.i_handle != -1 )
            {
				trace_msg(p_this, TRACE_WARM, "net: lame IPv6/IPv4 dual-stack present, using only IPv4.");
                net_close(fd6);
            }
            else
                sock.i_handle = fd6;
        }
    }

	if( sock.i_handle == -1 ){
		trace_msg(p_this, TRACE_DEBUG,  "net: connection to '[%s]:%d@[%s]:%d' failed", psz_server, i_server, psz_bind, *i_bind);
	}
	//������ ���� ����
    return sock.i_handle;
}