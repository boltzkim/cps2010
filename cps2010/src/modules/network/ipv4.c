/*
	ip4v 모듈...

	이력
	2010-08-9
*/
#include <core.h>

#if defined( _MSC_VER ) || defined(UNDER_CE)
#   if defined(UNDER_CE) && defined(sockaddr_storage)
#       undef sockaddr_storage
#   endif
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <iphlpapi.h>
#   define close closesocket
#   if defined(UNDER_CE)
#       undef IP_MULTICAST_TTL
#       define IP_MULTICAST_TTL 3
#       undef IP_ADD_MEMBERSHIP
#       define IP_ADD_MEMBERSHIP 5
#   endif
#else
#   include <netdb.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   ifdef HAVE_ARPA_INET_H
#       include <arpa/inet.h>
#   endif
#endif

static int OpenUDP(module_object_t *p_this);


module_define_start(IPv4);
    set_shortname( "IPv4" );
    set_description( "UDP/IPv4 network abstraction layer");
    set_capability( "network", 50 );
    set_category(CAT_INPUT);
    set_callbacks(OpenUDP, NULL);
module_define_end();


static int BuildAddr(module_object_t *p_obj, struct sockaddr_in * p_socket, const char * psz_address, int i_port)
{
    struct addrinfo hints, *res;
    int i_val;

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

	trace_msg(p_obj, TRACE_DEBUG, "resolving %s:%d...", psz_address, i_port);
    i_val = get_addr_info( p_obj, psz_address, i_port, &hints, &res );
    if( i_val )
    {
		trace_msg(p_obj, TRACE_WARM, "%s: %s", psz_address, gai_str_error(i_val));

        return -1;
    }

    memcpy( p_socket, res->ai_addr, sizeof( *p_socket ) );

    free_addr_info( res );

    return( 0 );
}


#if defined(_MSC_VER) || defined(UNDER_CE)
#	define WINSOCK_STRERROR_SIZE 20
	static const char *winsock_strerror( char *buf )
	{
		snprintf( buf, WINSOCK_STRERROR_SIZE, "Winsock error %d", WSAGetLastError());
		buf[WINSOCK_STRERROR_SIZE - 1] = '\0';
		return buf;
	}
#endif


static int OpenUDP(module_object_t * p_this)
{
    network_socket_t * p_socket = p_this->v_private;
    const char * psz_bind_addr = p_socket->psz_bind_addr;
    int i_bind_port = p_socket->i_bind_port;
    const char * psz_server_addr = p_socket->psz_server_addr;
    int i_server_port = p_socket->i_server_port;

    int i_handle, i_opt;
    struct sockaddr_in loc, rem;
    value_t val;
    bool do_connect = true;

	int namelen;
#if defined(_MSC_VER) || defined(UNDER_CE)
    char strerror_buf[WINSOCK_STRERROR_SIZE];
#	define strerror( x ) winsock_strerror(strerror_buf)
#endif

    p_socket->i_handle = -1;

	if( BuildAddr(p_this, &loc, psz_bind_addr, i_bind_port) || BuildAddr(p_this, &rem, psz_server_addr, i_server_port)){
		trace_msg(p_this, TRACE_ERROR, "cannot BuildAddr (%s)", strerror(errno));
		return MODULE_ERROR_NORMAL;
	}

    if( (i_handle = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    {
		trace_msg(p_this, TRACE_ERROR, "cannot create socket (%s)", strerror(errno));
        return MODULE_ERROR_NORMAL;
    }


    i_opt = 1;
    setsockopt( i_handle, SOL_SOCKET, SO_REUSEADDR, (void *) &i_opt, sizeof( i_opt ));

#ifdef SO_REUSEPORT
    i_opt = 1;
    setsockopt( i_handle, SOL_SOCKET, SO_REUSEPORT, (void *) &i_opt, sizeof( i_opt ) );
#endif

#ifdef SO_RCVBUF
    i_opt = 0x80000;
	if( setsockopt( i_handle, SOL_SOCKET, SO_RCVBUF, (void *) &i_opt, sizeof(i_opt)) == -1){
		trace_msg(p_this, TRACE_DEBUG, "cannot configure socket (SO_RCVBUF: %s)", strerror(errno));
	}
    i_opt = 0x80000;
	if( setsockopt( i_handle, SOL_SOCKET, SO_SNDBUF, (void *) &i_opt, sizeof(i_opt)) == -1){
		trace_msg(p_this, TRACE_DEBUG, "cannot configure socket (SO_SNDBUF: %s)", strerror(errno));
	}

	i_opt = 1;

#if 0

#if defined(_MSC_VER) || defined(UNDER_CE)


	if( ioctlsocket( i_handle, FIONBIO, &i_opt) != 0)
	{
		trace_msg(p_this, TRACE_DEBUG, "cannot set NonBlock : %s", strerror(errno));
	}
#else

	if( fcntl( i_handle, F_SETFL, O_NONBLOCK, &i_opt) == -1)
	{
		trace_msg(p_this, TRACE_DEBUG, "cannot set NonBlock : %s", strerror(errno));
	}

#endif

#endif

#endif

	i_opt = 1;


//#if defined( _MSC_VER ) || defined( UNDER_CE )
    if( IN_MULTICAST( ntohl( loc.sin_addr.s_addr ) ) )
    {
        struct sockaddr_in stupid = loc;
        stupid.sin_addr.s_addr = INADDR_ANY;

        if( bind( i_handle, (struct sockaddr *)&stupid, sizeof( stupid ) ) < 0 )
        {
			trace_msg(p_this, TRACE_WARM , "cannot bind socket (%d)", strerror(errno));
            close( i_handle );
            return 0;
        }
    }
    else
//#endif
    /* Bind it */
    if( bind( i_handle, (struct sockaddr *)&loc, sizeof( loc ) ) < 0 )
    {
		trace_msg(p_this, TRACE_WARM , "cannot bind socket (%s)", strerror(errno));
        close( i_handle );
        return 0;
    }

#if !defined( SYS_BEOS )
    if( loc.sin_addr.s_addr == INADDR_ANY )
    {
        i_opt = 1;
		if( setsockopt(i_handle, SOL_SOCKET, SO_BROADCAST, (void*) &i_opt,  sizeof(i_opt)) == -1){
			trace_msg(p_this, TRACE_WARM , "cannot configure socket (SO_BROADCAST: %s)", strerror(errno));
		}
    }
#endif

	if(p_socket->i_bind_port == 0){
		struct sockaddr_in temp;
		namelen = sizeof(loc);

		temp.sin_addr = loc.sin_addr;

		if (getsockname(i_handle, (struct sockaddr *) &loc, &namelen) < 0)
		{

			trace_msg(p_this, TRACE_TRACE , "getsockname() : %d", p_socket->i_bind_port);
			exit(3);
		}
		trace_msg(p_this, TRACE_TRACE , "Port assigned is %d\n", ntohs(loc.sin_port));
		p_socket->i_bind_port =  ntohs(loc.sin_port);
		loc.sin_addr = temp.sin_addr;
	}


#if !defined( SYS_BEOS )
    if( IN_MULTICAST( ntohl(loc.sin_addr.s_addr) ) )
    {
        char * psz_if_addr = get_default_local_address();
		//config_get_psz( p_this, "interface-addr" );

		trace_msg(p_this, TRACE_DEBUG, "interface-addr (%s)", psz_if_addr);

        if (( rem.sin_addr.s_addr != INADDR_ANY ))
        {
#ifndef IP_ADD_SOURCE_MEMBERSHIP
            errno = ENOSYS;
#else
            struct ip_mreq_source imr;

#ifdef _MSC_VER
            imr.imr_multiaddr.s_addr = loc.sin_addr.s_addr;
            imr.imr_sourceaddr.s_addr = rem.sin_addr.s_addr;
#else 
			
#ifdef Android
			
			imr.imr_multiaddr = loc.sin_addr.s_addr;
            imr.imr_sourceaddr = rem.sin_addr.s_addr;
#else
			imr.imr_multiaddr.s_addr = loc.sin_addr.s_addr;
            imr.imr_sourceaddr.s_addr = rem.sin_addr.s_addr;
#endif

#endif

			if( psz_if_addr != NULL && *psz_if_addr  && inet_addr(psz_if_addr) != INADDR_NONE){
#ifdef _MSC_VER
                imr.imr_interface.s_addr = inet_addr(psz_if_addr);
#else 

#ifdef Android
				imr.imr_interface = inet_addr(psz_if_addr);
#else
				imr.imr_interface.s_addr = inet_addr(psz_if_addr);
#endif

#endif
			}else{
#ifdef _MSC_VER
                imr.imr_interface.s_addr = INADDR_ANY;
#else 
#ifdef Android
				imr.imr_interface = INADDR_ANY;
#else
				imr.imr_interface.s_addr = INADDR_ANY;
#endif
#endif
			}

			if( psz_if_addr != NULL ){
                FREE( psz_if_addr );
			}

			trace_msg(p_this, TRACE_DEBUG , "IP_ADD_SOURCE_MEMBERSHIP multicast request");

			if(setsockopt(i_handle, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char*)&imr, sizeof(struct ip_mreq_source)) == 0){
                do_connect = false;
			}else
#endif
            {
				trace_msg(p_this, TRACE_WARM , "Source specific multicast failed (%s) - check if your OS really supports IGMPv3", strerror(errno));
                goto igmpv2;
            }
        }
        else
igmpv2:
        {
            struct ip_mreq imr;

            imr.imr_interface.s_addr = INADDR_ANY;
            imr.imr_multiaddr.s_addr = loc.sin_addr.s_addr;
            if( psz_if_addr != NULL && *psz_if_addr
             && inet_addr(psz_if_addr) != INADDR_NONE )
            {
				uint32_t test = inet_addr(psz_if_addr);
                imr.imr_interface.s_addr = inet_addr(psz_if_addr);
            }
#if defined (WIN32) || defined (UNDER_CE)
            else
            {
                typedef DWORD (CALLBACK * GETBESTINTERFACE) ( IPAddr, PDWORD );
                typedef DWORD (CALLBACK * GETIPADDRTABLE) ( PMIB_IPADDRTABLE, PULONG, BOOL );

                GETBESTINTERFACE OurGetBestInterface;
                GETIPADDRTABLE OurGetIpAddrTable;
                HINSTANCE hiphlpapi = LoadLibrary("Iphlpapi.dll");
                DWORD i_index;

                if( hiphlpapi )
                {
                    OurGetBestInterface = (void *)GetProcAddress( hiphlpapi, "GetBestInterface");
                    OurGetIpAddrTable = (void *)GetProcAddress( hiphlpapi,  "GetIpAddrTable");
                }

                if( hiphlpapi && OurGetBestInterface && OurGetIpAddrTable && OurGetBestInterface( loc.sin_addr.s_addr, &i_index ) == NO_ERROR )
                {
                    PMIB_IPADDRTABLE p_table;
                    DWORD i = 0;
					int c = 0;

					trace_msg(p_this, TRACE_DEBUG , "Winsock best interface is %lu", (uint32_t)i_index);

                    OurGetIpAddrTable( NULL, &i, 0 );

                    p_table = (PMIB_IPADDRTABLE)malloc( i );
                    if( p_table != NULL )
                    {
                        if( OurGetIpAddrTable( p_table, &i, 0 ) == NO_ERROR )
                        {
                            for( c = p_table->dwNumEntries-1; c >= 0; c-- )
                            {
                                if( p_table->table[c].dwIndex == i_index )
                                {
                                    imr.imr_interface.s_addr =  p_table->table[c].dwAddr;
									trace_msg(p_this, TRACE_DEBUG , "using interface 0x%08x", p_table->table[c].dwAddr);
                                }
                            }
                        }
						else{
							trace_msg(p_this, TRACE_WARM , "GetIpAddrTable failed");
						}
                        FREE( p_table );
                    }
                }
				else{
					trace_msg(p_this, TRACE_WARM , "GetBestInterface failed");
				}

				if( hiphlpapi){
					FreeLibrary( hiphlpapi );
				}
            }
#endif
            if( psz_if_addr != NULL ) FREE( psz_if_addr );

			//trace_msg(p_this, TRACE_DEBUG , "IP_ADD_MEMBERSHIP multicast request");

            if( setsockopt(i_handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&imr, sizeof(struct ip_mreq) ) == -1 )
            {
				trace_msg(p_this, TRACE_ERROR , "failed to join IP multicast group (%s)", strerror(errno));

                close( i_handle );
                return 0;
            }

			trace_msg(p_this, TRACE_DEBUG, "test %d.%d.%d.%d", imr.imr_interface.s_addr & 0xFF
																   , imr.imr_interface.s_addr >> 8 & 0xFF
																   , imr.imr_interface.s_addr >> 16 & 0xFF
																   , imr.imr_interface.s_addr >> 24 & 0xFF);
         }
    }

#if !defined (__linux__) && !defined (_MSC_VER)
    else
#endif

#endif
    if( rem.sin_addr.s_addr != INADDR_ANY )
    {
        if(do_connect && connect(i_handle, (struct sockaddr *) &rem, sizeof(rem)))
        {
			trace_msg(p_this, TRACE_WARM , "cannot connect socket (%s)", strerror(errno));
            close( i_handle );
            return 0;
        }

#if !defined( SYS_BEOS )
        if( IN_MULTICAST( ntohl(rem.sin_addr.s_addr) ) )
        {
            int i_ttl = p_socket->i_ttl;

            char * psz_mif_addr = config_get_psz( p_this, "interface-addr" );
            if( psz_mif_addr )
            {
                struct in_addr intf;
                intf.s_addr = inet_addr(psz_mif_addr);
                FREE( psz_mif_addr  );

                if( setsockopt( i_handle, IPPROTO_IP, IP_MULTICAST_IF, (char*)&intf, sizeof( intf ) ) < 0)
                {
					trace_msg(p_this, TRACE_DEBUG , "failed to set multicast interface (%s).", strerror(errno));
                    close( i_handle );
                    return 0;
                }
            }

			if( i_ttl <= 0 ){
                i_ttl = config_get_int( p_this, "ttl" );
			}

            if( i_ttl > 0 )
            {
                unsigned char ttl = (unsigned char) i_ttl;

                if( setsockopt( i_handle, IPPROTO_IP, IP_MULTICAST_TTL,
                                &ttl, sizeof( ttl ) ) < 0 )
                {
					trace_msg(p_this, TRACE_DEBUG , "failed to set ttl (%s). Let's try it the integer way.", strerror(errno));

                    if( setsockopt(i_handle, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&i_ttl, sizeof( i_ttl ) ) <0 )
                    {
						trace_msg(p_this, TRACE_ERROR , "failed to set ttl (%s)", strerror(errno));
                        close( i_handle );
                        return 0;
                    }
                }
            }
        }
#endif
    }


	//u_char loop = 0;
	//if (setsockopt(i_handle, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loop, sizeof(loop)) == -1)
	//{
	//	trace_msg(p_this, TRACE_ERROR, "failed to Multicast loop disable. (%s)", strerror(errno));
	//	close(i_handle);
	//}

    p_socket->i_handle = i_handle;

    if( var_get( p_this, "mtu", &val ) != MODULE_SUCCESS )
    {
        var_create( p_this, "mtu", VAR_INTEGER | VAR_DOINHERIT );
        var_get( p_this, "mtu", &val );
    }
    p_socket->i_mtu = val.i_int;

	

    return 0;
}

