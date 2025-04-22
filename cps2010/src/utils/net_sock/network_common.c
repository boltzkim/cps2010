/*
	��Ʈ��ũ �Ϲ����� �Լ�.

	�ۼ��� : 
	�̷�
	2010-08-9 : ����
*/

/**

	@file network_common.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DDS network ���� API�� ������ ���� 
	@details DDS network ���� API�� ������ ���Ϸ�, ���� �ּ�, ���� ����, ���� read/write�� ���� ��� ����

*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



#ifndef _NI_MASk
#	define _NI_MASK (NI_NUMERICHOST|N I _NUMERICSERV|N I _NOFQDN|NI_NAMEREQD | NI_DGRAM)
#endif

#ifndef _AI_MASK
#	define _AI_MASK (AI_PASSIVE|AI_CANONNAME|AI_NUMERICHOST)
#endif

#if !defined(HAVE_INET_PTON)

/**
	@brief inet_pton�� ������ �Լ�   
	@param[in] af AF_INET Ÿ�� ����
	@param[in] psz_src Source Ip Address ������
	@param[in] v_dst sin_addr Ÿ���� ����� �ּ� ������ 
	@return	inet_pton ���� ���� ��ȯ 
*/
int inet_pton1( int af, const char* psz_src, void* v_dst )
{
# ifdef _MSC_VER
    struct sockaddr_storage addr;
    int len = sizeof( addr );

#	ifdef UNICODE
    wchar_t *workaround_for_ill_designed_api = malloc( MAX_PATH * sizeof(wchar_t) );
    mbstowcs( workaround_for_ill_designed_api, psz_src, MAX_PATH );
    workaround_for_ill_designed_api[MAX_PATH-1] = 0;
#	else
    char *workaround_for_ill_designed_api = strdup( psz_src );
#	endif

    if( !WSAStringToAddress(workaround_for_ill_designed_api, af, NULL, (LPSOCKADDR)&addr, &len))
    {
        FREE( workaround_for_ill_designed_api );
        return -1;
    }
    FREE( workaround_for_ill_designed_api );

    switch( af )
    {
        case AF_INET6:
            memcpy( v_dst, &((struct sockaddr_in6 *)&addr)->sin6_addr, 16 );
            break;

        case AF_INET:
            memcpy( v_dst, &((struct sockaddr_in *)&addr)->sin_addr, 4 );
            break;

        default:
            WSASetLastError( WSAEAFNOSUPPORT );
            return -1;
    }
#	else
    uint32_t ipv4;

    if( af != AF_INET )
    {
        errno = EAFNOSUPPORT;
        return -1;
    }

    ipv4 = inet_addr( psz_src );
    if( ipv4 == INADDR_NONE )
        return -1;

    memcpy( v_dst, &ipv4, 4 );
# endif
    return 0;
}

#endif

#ifndef HAVE_GAI_STRERROR
static struct
{
    int code;
    const char *msg;
} const __gai_errlist[] =
{
    { 0,              "Error 0" },
    { EAI_BADFLAGS,   "Invalid flag used" },
    { EAI_NONAME,     "Host or service not found" },
    { EAI_AGAIN,      "Temporary name service failure" },
    { EAI_FAIL,       "Non-recoverable name service failure" },
    { EAI_NODATA,     "No data for host name" },
    { EAI_FAMILY,     "Unsupported address family" },
    { EAI_SOCKTYPE,   "Unsupported socket type" },
    { EAI_SERVICE,    "Incompatible service for socket type" },
    { EAI_ADDRFAMILY, "Unavailable address family for host name" },
    { EAI_MEMORY,     "Memory allocation failure" },
    { EAI_SYSTEM,     "System error" },
    { 0,              NULL }
};

static const char *__gai_unknownerr = "Unrecognized error number";

/**
	@brief Network Error�� ����ϴ� �Լ�   
	@param[in] err_num Network Error ��ȣ 
	@return	Network Error ��ȣ�� �ش��ϴ� Error�� String ��ȯ 
*/
const char* gai_str_error( int err_num )
{
    int i;

    for (i = 0; __gai_errlist[i].msg != NULL; i++)
        if (err_num == __gai_errlist[i].code)
            return __gai_errlist[i].msg;

    return __gai_unknownerr;
}
# undef _EAI_POSITIVE_MAX

#else
const char *gai_str_error(int errnum)
{
    return gai_strerror(errnum);
}
#endif


#ifndef HAVE_GETADDRINFO
/**
	@brief addrinfo�� Free�ϴ� �Լ� 
	@param[in] res Free�� addrinfo ������
*/
static void _FreeAddrInfo(struct addrinfo *res)
{
    if (res != NULL)
    {
        if (res->ai_canonname != NULL)
            free (res->ai_canonname);
        if (res->ai_addr != NULL)
            free (res->ai_addr);
        if (res->ai_next != NULL)
            free (res->ai_next);
        free (res);
    }
}

/**
	@brief Network addrinfo Ÿ���� �ּ� ������ �����ϴ� �Լ�    
	@param[in] af AF_INET Ÿ�� ����
	@param[in] type Socket Type ���� 
	@param[in] proto IPPROTO�� Type ����
	@param[in] addr sockaddr �ּ� ������
	@param[in] addrlen sockaddr �ּ� ���� ���� 
	@param[in] canonname canonical name ������
	@return	������ �ּҿ� ���ؼ� ������ addrinfo ��ȯ
*/
static struct addrinfo *MakeAddrInfo (int af, int type, int proto, const struct sockaddr *addr, size_t addrlen, const char *canonname)
{
    struct addrinfo *res;

    res = (struct addrinfo *)malloc (sizeof (struct addrinfo));
    if (res != NULL)
    {
        res->ai_flags = 0;
        res->ai_family = af;
        res->ai_socktype = type;
        res->ai_protocol = proto;
        res->ai_addrlen = addrlen;
        res->ai_addr = malloc (addrlen);
        res->ai_canonname = NULL;
        res->ai_next = NULL;

        if (res->ai_addr != NULL)
        {
            memcpy (res->ai_addr, addr, addrlen);

            if (canonname != NULL)
            {
                res->ai_canonname = strdup (canonname);
                if (res->ai_canonname != NULL)
                    return res; /* success ! */
            }
            else
                return res;
        }
    }

    free_addr_info (res);
    return NULL;
}

/**
	@brief Network addrinfo Ÿ���� �ּ� ������ �����ϴ� �Լ�     
	@param[in] type Socket Type ����
	@param[in] proto IPPROTO�� Type ����
	@param[in] ip addrinfo ������ ȹ���� IP �ּ� 
	@param[in] port addrinfo ������ ȹ���� Port ��ȣ 
	@param[in] name canonical name ������
	@return	���� �ּҿ� ���ؼ� ������ addrinfo ��ȯ
*/
static struct addrinfo *MakeIpv4Info(int type, int proto, u_long ip, u_short port, const char *name)
{
    struct sockaddr_in addr;

    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
# ifdef HAVE_SA_LEN
    addr.sin_len = sizeof (addr);
# endif
    addr.sin_port = port;
    addr.sin_addr.s_addr = ip;

    return MakeAddrInfo(AF_INET, type, proto, (struct sockaddr*)&addr, sizeof (addr), name);
}


static int Getaddrinfo (const char *node, const char *service,
               const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo *info;
    u_long ip;
    u_short port;
    int protocol = 0, flags = 0;
    const char *name = NULL;

#ifdef WIN32
    if ((node != NULL) && (strchr (node, ':') != NULL))
       return EAI_NONAME;
#endif

    if (hints != NULL)
    {
        flags = hints->ai_flags;

		if (flags & ~_AI_MASK){
            return EAI_BADFLAGS;
		}

		if (hints->ai_family && (hints->ai_family != AF_INET)){
            return EAI_FAMILY;
		}

        switch (hints->ai_socktype)
        {
            case SOCK_STREAM:
                protocol = IPPROTO_TCP;
                break;
            case SOCK_DGRAM:
                protocol = IPPROTO_UDP;
                break;

#ifndef SYS_BEOS
            case SOCK_RAW:
#endif
            case 0:
                break;
            default:
                return EAI_SOCKTYPE;
        }
		if (hints->ai_protocol && protocol && (protocol != hints->ai_protocol)){
            return EAI_SERVICE;
		}
    }

    *res = NULL;

    if (node == NULL)
    {
		if (flags & AI_PASSIVE){
            ip = htonl (INADDR_ANY);
		}else{
            ip = htonl (INADDR_LOOPBACK);
		}
    }
    else
    if ((ip = inet_addr (node)) == INADDR_NONE)
    {
        struct hostent *entry = NULL;

		if (!(flags & AI_NUMERICHOST)){
            entry = gethostbyname (node);
		}

		if (entry == NULL){
            return EAI_NONAME;
		}

		if ((entry->h_length != 4) || (entry->h_addrtype != AF_INET)){
            return EAI_FAMILY;
		}

        ip = *((u_long *) entry->h_addr);

		if (flags & AI_CANONNAME){
            name = entry->h_name;
		}
    }

	if ((flags & AI_CANONNAME) && (name == NULL)){
        name = node;
	}

    if (service == NULL)
        port = 0;
    else
    {
        int32_t d;
        char *end;

        d = strtoul (service, &end, 0);
        if (end[0] || (d > 65535))
        {
            struct servent *entry;
            const char *protoname;

            switch (protocol)
            {
                case IPPROTO_TCP:
                    protoname = "tcp";
                    break;

                case IPPROTO_UDP:
                    protoname = "udp";
                    break;

                default:
                    protoname = NULL;
            }

            entry = getservbyname (service, protoname);
            if (entry == NULL)
                return EAI_SERVICE;

            port = entry->s_port;
        }
		else{
            port = htons ((u_short)d);
		}
    }

    if ((!protocol) || (protocol == IPPROTO_UDP))
    {
        info = MakeIpv4Info(SOCK_DGRAM, IPPROTO_UDP, ip, port, name);
        if (info == NULL)
        {
            errno = ENOMEM;
            return EAI_SYSTEM;
        }
        if (flags & AI_PASSIVE)
            info->ai_flags |= AI_PASSIVE;
        *res = info;
    }
    if ((!protocol) || (protocol == IPPROTO_TCP))
    {
        info = MakeIpv4Info(SOCK_STREAM, IPPROTO_TCP, ip, port, name);
        if (info == NULL)
        {
            errno = ENOMEM;
            return EAI_SYSTEM;
        }
        info->ai_next = *res;
		if (flags & AI_PASSIVE){
            info->ai_flags |= AI_PASSIVE;
		}
        *res = info;
    }

    return 0;
}
#endif

/**
	@brief ������ �̸��� ���� IP �ּҸ� �����ϴ� �Լ� 
	@param[in] p_this DCPS ACCESS Service Module ������
	@param[in] p_node ������ �̸����� ����� IP �ּ� ������ 
	@param[in] i_port ������ �̸����� ����� Port ��ȣ ����   
	@param[in] p_hints addrinfo ������ ���� ��Ʈ �ּ� ���� ������ 
	@param[in] pp_res ������ �̸��� ���ؼ� ������ addrinfo �ּ� ������
	@return	������ �̸��� ���ؼ� IP �ּ� ȹ�� ���� ���� �� ��ȯ
*/
int get_addr_info( module_object_t* p_this, const char* p_node, int i_port, const struct addrinfo* p_hints, struct addrinfo** pp_res )
{
    struct addrinfo hints;
    char psz_buf[NI_MAXHOST], *psz_node, psz_service[6];

    if( ( i_port > 65535 ) || ( i_port < 0 ) )
    {
		trace_msg(p_this, TRACE_DEBUG, "invalid port number %d specified", i_port );
        return EAI_SERVICE;
    }

    snprintf( psz_service, 6, "%d", i_port );

	if( p_hints == NULL ){
        memset( &hints, 0, sizeof(hints));
	}else{
        memcpy( &hints, p_hints, sizeof(hints));
	}

    if( hints.ai_family == AF_UNSPEC )
    {
        value_t val;

        var_create( p_this, "ipv4", VAR_BOOL | VAR_DOINHERIT );
        var_get( p_this, "ipv4", &val );
        if( val.b_bool )
            hints.ai_family = AF_INET;

#ifdef AF_INET6
        var_create( p_this, "ipv6", VAR_BOOL | VAR_DOINHERIT );
        var_get( p_this, "ipv6", &val );
		if( val.b_bool){
            hints.ai_family = AF_INET6;
		}
#endif
    }


    if((p_node == NULL) || (p_node[0] == '\0'))
    {
        psz_node = NULL;
    }
    else
    {
        strlcpy( psz_buf, p_node, NI_MAXHOST);

        psz_node = psz_buf;

        if( psz_buf[0] == '[' )
        {
            char *ptr;

            ptr = strrchr( psz_buf, ']' );
            if((ptr != NULL ) && (ptr[1] == '\0'))
            {
                *ptr = '\0';
                psz_node++;
            }
        }
    }

# ifdef AI_IDN
    {
        static bool i_idn = false;

        if( i_idn )
        {
            int i_ret;

            hints.ai_flags |= AI_IDN;
            i_ret = getaddrinfo( psz_node, psz_service, &hints, pp_res );

			if( i_ret != EAI_BADFLAGS ){
                return i_ret;
			}

            hints.ai_flags &= ~AI_IDN;
            i_idn = false;
			trace_msg(p_this, TRACE_DEBUG, "localized Domain Names not supported - disabled" );
        }
    }
# endif
    return getaddrinfo( psz_node, psz_service, &hints, pp_res );
}
/**
	@brief addrinfo Ÿ���� ������ ���� �ϴ� �Լ� 
	@param[in] p_infos ������ addr_info ����ü ������
*/
void free_addr_info( struct addrinfo* p_infos )
{
	freeaddrinfo(p_infos);
}

/**
	@brief ������ �����ϰ� ���Ͽ� option�� �����ϴ� �Լ�   
	@param[in] p_this ACCESS Service Module ������
	@param[in] i_family AF_INET Ÿ�� ����
	@param[in] i_sock_type soket Type ���� 
	@param[in] i_protocol PROTO Ÿ�� ����
	@return ���� option ���� ���� ���� �� ��ȯ
*/
int net_socket( module_object_t* p_this, int i_family, int i_sock_type, int i_protocol )
{
    int fd, i_val;
	//���� ����
    fd = socket( i_family, i_sock_type, i_protocol );
    if( fd == -1 )
    {
#if defined(WIN32) || defined(UNDER_CE)
		if( WSAGetLastError ( ) != WSAEAFNOSUPPORT){
			trace_msg(p_this, TRACE_WARM, "cannot create socket (%i)", WSAGetLastError());
		}
#else
		if( errno != EAFNOSUPPORT){
			trace_msg(p_this, TRACE_WARM, "cannot create socket (%s)", strerror(errno));
		}
#endif
        return -1;
    }

#if defined( _MSC_VER ) || defined( UNDER_CE )
    {
        uint32_t i_dummy = 1;
	    //non blocking
		if( ioctlsocket( fd, FIONBIO, &i_dummy ) != 0){
			trace_msg(p_this, TRACE_WARM, "cannot set socket to non-blocking mode");
		}
    }
#else
    if( fd >= FD_SETSIZE )
    {
		trace_msg(p_this, TRACE_ERROR, "cannot create socket (too many already in use)");
        net_close( fd );
        return -1;
    }

    fcntl( fd, F_SETFD, FD_CLOEXEC );
    i_val = fcntl( fd, F_GETFL, 0 );
    fcntl( fd, F_SETFL, ((i_val != -1) ? i_val : 0) | O_NONBLOCK );
#endif

    i_val = 1;
	//socket address reuse
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&i_val, sizeof(i_val));

#ifdef IPV6_V6ONLY
	if( i_family == AF_INET6 ){
		//ipv6 only
        setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&i_val, sizeof( i_val ) );
	}
#endif

#if defined( _MSC_VER ) || defined( UNDER_CE )

#	ifndef IPV6_PROTECTION_LEVEL
#		define IPV6_PROTECTION_LEVEL 23
#	endif

    if(i_family == AF_INET6)
    {
        i_val = 10;
        setsockopt( fd, IPPROTO_IPV6, IPV6_PROTECTION_LEVEL, (const char*)&i_val, sizeof( i_val ) );
    }
#endif
    return fd;
}

/**
	@brief ������ Close�ϴ� �Լ�
	@param[in] fd Close�� ���� ���� 

*/
void net_close( int fd )
{
#ifdef UNDER_CE
    CloseHandle( (HANDLE)fd );
#elif defined( _MSC_VER )
    closesocket( fd );
#else
    close( fd );
#endif
}

//by jun
//int netRead2(service_t * p_this, uint8_t *p_data, int i_data, bool b_retry)
/**
	@brief ��Ʈ��ũ �����尡 �������� ���� �����͸� �����ϴ� �Լ� 
	@param[in] p_this ACCESS Service Module ����
	@param[in] p_data ���� ���� ���� ������
	@param[in] i_data ���� ���� ���� ������ ���� 
	@param[in] b_retry ������ ���� ���� flag ����
	@param[in] spdp_fd PDP ���� ���� 
	@param[in] sedp_fd EDP ���� ����
	@param[in] data_fd DATA ���� ����
	@param[in] p_end ������ ���� ���� ����
	@return	�������� ���� ������ ������ �� ��ȯ  
*/
int net_read_for_service( service_t* p_this, uint8_t* p_data, int i_data, bool b_retry, int spdp_fd, int sedp_fd, int data_fd, bool* p_end )
{

	service_t *p_service;
	service_sys_t *p_sys;

    struct timeval  timeout;
    fd_set          fds_r, fds_e;
    int32_t             i_recv;
    int32_t             i_total = 0;
    int32_t             i_ret;

	int max_fd;

	p_service = (service_t *)p_this;
//	p_this = p_service;
	p_sys = p_service->p_sys;

	
	
    while( i_data > 0 )
    {
 
//    temp_fd = p_sys1->fd;
        do
        {
/*            if( (b_end1 == true) || (b_end2 == true) || (b_end3 == true))
            {
                return 0;
            }


*/
//PDP, EDP, DATA ���� �̺�Ʈ�� ���� select ���� 
			if(*p_end == true)
			{
				return 0;
			}
            /* Initialize file descriptor set */
            FD_ZERO( &fds_r );
            FD_SET( spdp_fd, &fds_r );
            FD_SET( sedp_fd, &fds_r );
	        FD_SET( data_fd, &fds_r );		
            FD_ZERO( &fds_e );
            FD_SET( spdp_fd, &fds_e );
            FD_SET( sedp_fd, &fds_e );
            FD_SET( data_fd, &fds_e );

			max_fd = spdp_fd;
			if(sedp_fd > spdp_fd)
				max_fd = sedp_fd;
			if(data_fd > max_fd)
				max_fd = data_fd;
				

            /* We'll wait 0.5 second if nothing happens */
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;

//        } while( (i_ret = select(temp_fd + 1, &fds_r, NULL, &fds_e, &timeout)) == 0
        } while( (i_ret = select(max_fd+1, &fds_r, NULL, &fds_e, &timeout)) == 0
                 || ( i_ret < 0 && errno == EINTR ) );

        if( i_ret < 0 )
        {
        
#if defined(_MSC_VER) || defined(UNDER_CE)
			trace_msg((module_object_t*)p_this, TRACE_ERROR, "network select error (%d)", WSAGetLastError());
#else
			trace_msg((module_object_t*)p_this, TRACE_ERROR,  "network select error (%s)", strerror(errno));
#endif
            return i_total > 0 ? i_total : -1;
       }

		

//PDP ���� 
	if(FD_ISSET(spdp_fd, &fds_r )) {
	
//������ ����
//        if((i_recv = (p_vs != NULL) ? p_vs->pf_recv( p_vs->p_sys, p_data, i_data )  : recv( spdp_fd, p_data, i_data, 0 ) ) < 0)
        if((i_recv = recv( spdp_fd, p_data, i_data, 0 ) ) < 0)			
        {
#if defined(_MSC_VER) || defined(UNDER_CE)
            if( WSAGetLastError() == WSAEWOULDBLOCK )
            {
                 
            }
            else if( WSAGetLastError() == WSAEMSGSIZE )
            {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv() failed. Increase the mtu size.");
                i_total += i_data;
            }
            else if( WSAGetLastError() == WSAEINTR ) continue;
			else {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%i)", WSAGetLastError());
			}
#else
            
			if( errno != EAGAIN ){
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%s)", strerror(errno));
			}
#endif
            return i_total > 0 ? i_total : -1;
        }
        else if( i_recv == 0 )
        {
            b_retry = false;
        }


		if (i_recv > 0) {
			p_service->p_networkdata[0] = data_new(i_recv);
			memcpy(p_service->p_networkdata[0]->p_data, p_data, i_recv);
		}
	}
//EDP ���� 
	
	
	if(FD_ISSET(sedp_fd, &fds_r )) {

//������ ����
//        if((i_recv = (p_vs != NULL) ? p_vs->pf_recv( p_vs->p_sys, p_data, i_data )  : recv( sedp_fd, p_data, i_data, 0 ) ) < 0)
        if((i_recv = recv( sedp_fd, p_data, i_data, 0 ) ) < 0)	
        {

#if defined(_MSC_VER) || defined(UNDER_CE)
            if( WSAGetLastError() == WSAEWOULDBLOCK )
            {
                 
            }
            else if( WSAGetLastError() == WSAEMSGSIZE )
            {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv() failed. Increase the mtu size.");
                i_total += i_data;
            }
            else if( WSAGetLastError() == WSAEINTR ) continue;
			else {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%i)", WSAGetLastError());
			}
#else
            
			if( errno != EAGAIN ){
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%s)", strerror(errno));
			}
#endif
            return i_total > 0 ? i_total : -1;
        }
        else if( i_recv == 0 )
        {
            b_retry = false;
        }

		if (i_recv > 0) {
			p_service->p_networkdata[1] = data_new(i_recv);
			memcpy(p_service->p_networkdata[1]->p_data, p_data, i_recv);
		}
	}
//������ ����
	
	
	if(FD_ISSET(data_fd, &fds_r )) {
		
//������ ����
//		        if((i_recv = (p_vs != NULL) ? p_vs->pf_recv( p_vs->p_sys, p_data, i_data )  : recv( data_fd, p_data, i_data, 0 ) ) < 0)
        if((i_recv = recv( data_fd, p_data, i_data, 0 ) ) < 0)	
        {
#if defined(_MSC_VER) || defined(UNDER_CE)
            if( WSAGetLastError() == WSAEWOULDBLOCK )
            {
                 
            }
            else if( WSAGetLastError() == WSAEMSGSIZE )
            {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv() failed. Increase the mtu size.");
                i_total += i_data;
            }
            else if( WSAGetLastError() == WSAEINTR ) continue;
			else {
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%i)", WSAGetLastError());
			}
#else
            
			if( errno != EAGAIN ){
				trace_msg((module_object_t*)p_this, TRACE_ERROR, "recv failed (%s)", strerror(errno));
			}
#endif
            return i_total > 0 ? i_total : -1;
        }
        else if( i_recv == 0 )
        {
            b_retry = false;
        }

		if (i_recv > 0) {
			p_service->p_networkdata[2] = data_new(i_recv);
			memcpy(p_service->p_networkdata[2]->p_data, p_data, i_recv);
		}

	}

        p_data += i_recv;
        i_data -= i_recv;
        i_total+= i_recv;
        if( !b_retry )
        {
            break;
        }
    }

   
    return i_total;
}

/**
	@brief ��Ʈ��ũ �����尡 �������� ���� �����͸� �����ϴ� �Լ� 
	@param[in] p_this ACCESS Service Module ����
	@param[in] fd PDP, EDP, DATA ���� ���� �� �ϳ�
	@param[in] p_vs fd�ܿ� ����ڰ� ������ ���� ���Ͽ� ���� ������
	@param[in] p_data ���� ���� ���� ������
	@param[in] i_data ���� ���� ���� ������ ���� 
	@param[in] b_retry ������ ���� ���� flag ����
	@return	�������� ���� ������ ������ �� ��ȯ  
*/
int net_read( module_object_t* p_this, int fd, virtual_socket_t* p_vs, uint8_t* p_data, int i_data, bool b_retry )
{
    struct timeval		timeout;
    fd_set				fds_r, fds_e;
    int32_t             i_recv;
    int32_t             i_total = 0;
    int32_t             i_ret;
    bool				b_end = p_this->b_end;

    while( i_data > 0 )
    {
        do
        {
            if( p_this->b_end == true )
            {
                return 0;
            }

            /* Initialize file descriptor set */
            FD_ZERO( &fds_r );
            FD_SET( fd, &fds_r );
            FD_ZERO( &fds_e );
            FD_SET( fd, &fds_e );

            /* We'll wait 0.5 second if nothing happens */
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000; 

        } while( (i_ret = select(fd + 1, &fds_r, NULL, &fds_e, &timeout)) == 0
                 || ( i_ret < 0 && errno == EINTR ) );

        if( i_ret < 0 )
        {
#if defined(_MSC_VER) || defined(UNDER_CE)
			trace_msg(p_this, TRACE_ERROR, "network select error (%d)", WSAGetLastError());
#else
			trace_msg(p_this, TRACE_ERROR,  "network select error (%s)", strerror(errno));
#endif
            return i_total > 0 ? i_total : -1;
        }

        if((i_recv = (p_vs != NULL) ? p_vs->pf_recv( p_vs->p_sys, p_data, i_data )  : recv( fd, p_data, i_data, 0 ) ) < 0)
        {
#if defined(_MSC_VER) || defined(UNDER_CE)
            if( WSAGetLastError() == WSAEWOULDBLOCK )
            {
                 
            }
            else if( WSAGetLastError() == WSAEMSGSIZE )
            {
				trace_msg(p_this, TRACE_ERROR, "recv() failed. Increase the mtu size.");
                i_total += i_data;
            }
            else if( WSAGetLastError() == WSAEINTR ) continue;
			else {
				trace_msg(p_this, TRACE_ERROR, "recv failed (%i)", WSAGetLastError());
			}
#else
            
			if( errno != EAGAIN ){
				trace_msg(p_this, TRACE_ERROR, "recv failed (%s)", strerror(errno));
			}
#endif
            return i_total > 0 ? i_total : -1;
        }
        else if( i_recv == 0 )
        {
            b_retry = false;
        }

        p_data += i_recv;
        i_data -= i_recv;
        i_total+= i_recv;
        if( !b_retry )
        {
            break;
        }
    }
    return i_total;
}


int net_readFrom(module_object_t* p_this, int fd, virtual_socket_t* p_vs, uint8_t* p_data, int i_data, bool b_retry, struct sockaddr *client, int *l)
{
	struct timeval		timeout;
	fd_set				fds_r, fds_e;
	int32_t             i_recv;
	int32_t             i_total = 0;
	int32_t             i_ret;
	bool				b_end = p_this->b_end;

	while (i_data > 0)
	{
		do
		{
			if (p_this->b_end == true)
			{
				return 0;
			}

			/* Initialize file descriptor set */
			FD_ZERO(&fds_r);
			FD_SET(fd, &fds_r);
			FD_ZERO(&fds_e);
			FD_SET(fd, &fds_e);

			/* We'll wait 0.5 second if nothing happens */
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000;

		} while ((i_ret = select(fd + 1, &fds_r, NULL, &fds_e, &timeout)) == 0
			|| (i_ret < 0 && errno == EINTR));

		if (i_ret < 0)
		{
#if defined(_MSC_VER) || defined(UNDER_CE)
			trace_msg(p_this, TRACE_ERROR, "network select error (%d)", WSAGetLastError());
#else
			trace_msg(p_this, TRACE_ERROR, "network select error (%s)", strerror(errno));
#endif
			return i_total > 0 ? i_total : -1;
		}

		if ((i_recv = recvfrom(fd, p_data, i_data, 0, (struct sockaddr *)client, l)) < 0)
		{
#if defined(_MSC_VER) || defined(UNDER_CE)
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{

			}
			else if (WSAGetLastError() == WSAEMSGSIZE)
			{
				trace_msg(p_this, TRACE_ERROR, "recv() failed. Increase the mtu size.");
				i_total += i_data;
			}
			else if (WSAGetLastError() == WSAEINTR) continue;
			else {
				trace_msg(p_this, TRACE_ERROR, "recv failed (%i)", WSAGetLastError());
			}
#else

			if (errno != EAGAIN) {
				trace_msg(p_this, TRACE_ERROR, "recv failed (%s)", strerror(errno));
			}
#endif
			return i_total > 0 ? i_total : -1;
		}
		else if (i_recv == 0)
		{
			b_retry = false;
		}

		p_data += i_recv;
		i_data -= i_recv;
		i_total += i_recv;
		if (!b_retry)
		{
			break;
		}
	}
	return i_total;
}

/**
	@brief �������� �����͸� �۽��ϴ� �Լ� 
	@param[in] p_this ACCESS Service Module ������
	@param[in] fd �����͸� �۽��� ���� ���� ����
	@param[in] p_vs fd�ܿ� ����ڰ� ������ ���� ���� ������
	@param[in] p_data ������ ������ ���� ������
	@param[in] i_data ������ ������ ������ ����
	@return	�������� ������ �۽��� �� ��ȯ  
*/
int net_write( module_object_t* p_this, int fd, virtual_socket_t* p_vs, const uint8_t* p_data, int i_data )
{
    struct timeval  timeout;
    fd_set          fds_w, fds_e;
    int             i_send;
    int             i_total = 0;
    int             i_ret;

    bool			b_end = p_this->b_end;

    while( i_data > 0 )
    {	//�����Ͱ� ������ ���� �˻�
        do
        {
            if( p_this->b_end != b_end )
            {
                return 0;
            }

            FD_ZERO( &fds_w );
            FD_SET( fd, &fds_w );
            FD_ZERO( &fds_e );
            FD_SET( fd, &fds_e );

            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;

        } while( (i_ret = select(fd + 1, NULL, &fds_w, &fds_e, &timeout)) == 0
                 || ( i_ret < 0 && errno == EINTR ) );

        if( i_ret < 0 )
        {
#if defined(_MSC_VER) || defined(UNDER_CE)
			trace_msg(p_this, TRACE_ERROR, "network selection error (%d)", WSAGetLastError());
#else
			trace_msg(p_this, TRACE_ERROR,  "network selection error (%s)", strerror(errno));
#endif
            return i_total > 0 ? i_total : -1;
        }
//������ ����
        if( ( i_send = (p_vs != NULL) ? p_vs->pf_send( p_vs->p_sys, p_data, i_data ) : send( fd, p_data, i_data, 0 ) ) < 0)
        {
            return i_total > 0 ? i_total : -1;
        }

        p_data += i_send;
        i_data -= i_send;
        i_total+= i_send;
    }
    return i_total;
}
