/*
	TCP 관련...

	작성자 : 
	이력
	2010-08-9 : 시작
*/

/**
	@file tcp.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DDS tcp 관련 API를 정의한 파일 
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


/** 
	@brief DDS에서 TCP 연결을 수행하는 함수 
	@param[in] p_this DCPS Module 중 ACCESS Service Module 포인터    
	@param[in] psz_host DDS에서 연결할 Host의 IP 주소 변수
	@param[in] i_port DDS에서 연결할 Host의 Port 변수
	@return	TCP 소켓 연결 설정 성공 실패 값 반환 
*/
int net_connect_tcp( module_object_t* p_this, const char* psz_host, int i_port )
{
    struct addrinfo hints, *res, *ptr;
    const char      *psz_realhost;
    char            *psz_socks;
    int             i_realport, i_val, i_handle = -1, i_saved_errno = 0;
    unsigned        u_errstep = 0;

	if( i_port == 0 ){
        i_port = 80;
	}

    memset( &hints, 0, sizeof( hints ) );
	//tcp 소켓
    hints.ai_socktype = SOCK_STREAM;
	//socks 변수의 값 가져오기 
    psz_socks = var_create_get_string( p_this, "socks");

    if( *psz_socks && *psz_socks != ':' )
    {
        char *psz = strchr( psz_socks, ':' );

		if( psz ){
            *psz++ = '\0';
		}

        psz_realhost = psz_socks;
		//포트 번호
        i_realport = ( psz != NULL ) ? atoi( psz ) : 1080;

		trace_msg(p_this, TRACE_DEBUG, "net: connecting to %s port %d for %s port %d", psz_realhost, i_realport, psz_host, i_port);
    }
    else
    {
        psz_realhost = psz_host;
        i_realport = i_port;

		trace_msg(p_this, TRACE_DEBUG, "net: connecting to %s port %d", psz_realhost, i_realport);
    }
	//주소 가져오기 
    i_val = get_addr_info( p_this, psz_realhost, i_realport, &hints, &res );
    if( i_val )
    {
		trace_msg(p_this, TRACE_ERROR, "cannot resolve %s port %d : %s", psz_realhost, i_realport, gai_str_error(i_val));
        FREE( psz_socks );
        return -1;
    }

    for( ptr = res; ptr != NULL; ptr = ptr->ai_next )
    {	//소켓 생성
        int fd = net_socket( p_this, ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
        if( fd == -1 )
        {
            if( u_errstep <= 0 )
            {
                u_errstep = 1;
                i_saved_errno = net_errno;
            }
			trace_msg(p_this, TRACE_DEBUG, "socket error: %s", strerror(net_errno));
            continue;
        }
		//소켓 연결
        if( connect( fd, ptr->ai_addr, ptr->ai_addrlen ) )
        {
            socklen_t i_val_size = sizeof( i_val );
            div_t d;
            struct timeval tv;
            value_t timeout;

            if( net_errno != EINPROGRESS )
            {
                if( u_errstep <= 1 )
                {
                    u_errstep = 2;
                    i_saved_errno = net_errno;
                }
				trace_msg(p_this, TRACE_DEBUG, "connect error: %s", strerror(net_errno));
                goto next_ai;
            }

            var_create( p_this, "ipv4-timeout", VAR_INTEGER | VAR_DOINHERIT);
            var_get( p_this, "ipv4-timeout", &timeout );
            if( timeout.i_int < 0 )
            {
				trace_msg(p_this, TRACE_ERROR, "invalid negative value for ipv4-timeout" );
                timeout.i_int = 0;
            }
            d = div( timeout.i_int, 100 );

			trace_msg(p_this, TRACE_DEBUG, "connection in progress");
            for (;;)
            {
                fd_set fds;
                int i_ret;

                if( p_this->b_end )
                {
					trace_msg(p_this, TRACE_DEBUG, "connection aborted");
                    net_close( fd );
                    free_addr_info( res );
                    FREE( psz_socks );
                    return -1;
                }

                FD_ZERO( &fds );
                FD_SET( fd, &fds );

                tv.tv_sec = 0;
                tv.tv_usec = (d.quot > 0) ? 100000 : (1000 * d.rem);
				//소켓 이벤트 확인
                i_ret = select( fd + 1, NULL, &fds, NULL, &tv );
				if( i_ret == 1 ){
                    break;
				}

                if( ( i_ret == -1 ) && ( net_errno != EINTR ) )
                {
					trace_msg(p_this, TRACE_WARM, "select error: %s", strerror(net_errno));
                    goto next_ai;
                }

                if( d.quot <= 0 )
                {
					trace_msg(p_this, TRACE_DEBUG, "select timed out");
                    if( u_errstep <= 2 )
                    {
                        u_errstep = 3;
                        i_saved_errno = ETIMEDOUT;
                    }
                    goto next_ai;
                }

                d.quot--;
            }

#if !defined( SYS_BEOS ) && !defined( UNDER_CE )
            if( getsockopt( fd, SOL_SOCKET, SO_ERROR, (void*)&i_val,
                            &i_val_size ) == -1 || i_val != 0 )
            {
                u_errstep = 4;
                i_saved_errno = i_val;
				trace_msg(p_this, TRACE_DEBUG, "connect error (via getsockopt): %s", net_strerror(i_val));
                goto next_ai;
            }
#endif
        }

        i_handle = fd;
        break;

next_ai:
        net_close( fd );
        continue;
    }

    free_addr_info( res );

    if( i_handle == -1 )
    {
		trace_msg(p_this, TRACE_ERROR, "Connection to %s port %d failed: %s", psz_host, i_port, net_strerror(i_saved_errno));
        FREE( psz_socks );
        return -1;
    }

    return i_handle;
}

/** 
	@brief DDS에서 TCP 연결에 대한 Listen을 수행하는 함수 
	@param[in] p_this DCPS Module 중 ACCESS Service Module 포인터    
	@param[in] psz_host DCPS에서 Listen 소켓의 IP 주소 변수
	@param[in] i_port DCPS에서 Listen 소켓의 Port 변수
	@return	Listen 소켓 연결 설정 성공 실패 값 반환
*/
int* net_listen_tcp( module_object_t* p_this, const char* psz_host, int i_port )
{
    struct addrinfo hints, *res, *ptr;
    int             i_val, *pi_handles, i_size;

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

	trace_msg(p_this, TRACE_DEBUG, "net: listening to %s port %d", psz_host, i_port );
	//ip 주소 변경
    i_val = get_addr_info( p_this, psz_host, i_port, &hints, &res );
    if( i_val )
    {
		trace_msg(p_this, TRACE_ERROR, "Cannot resolve %s port %d : %s", psz_host, i_port, gai_str_error(i_val));
        return NULL;
    }

    pi_handles = NULL;
    i_size = 1;

    for( ptr = res; ptr != NULL; ptr = ptr->ai_next )
    {
        int fd, *newpi;
		//소켓 생성
        fd = net_socket(p_this, ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
        if( fd == -1 )
        {
			trace_msg(p_this, TRACE_DEBUG, "socket error: %s", net_strerror(net_errno));
            continue;
        }

        /* Bind the socket */
        if( bind( fd, ptr->ai_addr, ptr->ai_addrlen ) )
        {
            int saved_errno;

            saved_errno = net_errno;
            net_close( fd );
#if !defined(_MSC_VER) && !defined(UNDER_CE) && !defined(Android)
            /*
            fd = rootwrap_bind( ptr->ai_family, ptr->ai_socktype,
                                ptr->ai_protocol, ptr->ai_addr,
                                ptr->ai_addrlen );
            if( fd != -1 )
            {
				trace_msg(p_this, TRACE_DEBUG, "got socket %d from rootwrap", fd);
            }
            else*/
#endif
            {
				trace_msg(p_this, TRACE_ERROR, "cannot bind socket (%s)", net_strerror(saved_errno));
                continue;
            }
        }

        /* Listen */
        if( listen( fd, 100 ) == -1 )
        {
			trace_msg(p_this, TRACE_ERROR, "cannot bring the socket in listening mode (%s)", net_strerror(net_errno));
            net_close( fd );
            continue;
        }

        newpi = (int *)realloc( pi_handles, (++i_size) * sizeof( int ) );
        if( newpi == NULL )
        {
            net_close( fd );
            break;
        }
        else
        {
            newpi[i_size - 2] = fd;
            pi_handles = newpi;
        }
    }

    free_addr_info( res );

	if( pi_handles != NULL ){
        pi_handles[i_size - 1] = -1;
	}

    return pi_handles;
}

/** 
	@brief DDS에서 TCP 연결에 대한 Accept를 수행하는 함수 
	@param[in] p_this DCPS Module 중 ACCESS Service Module 포인터     
	@param[in] pi_fd DDS에서 사용하는 Accept 소켓 변수 
	@param[in] i_wait 최대 Accept를 위하여 기다리는 시간 변수 
	@return	Accept 소켓 연결 설정 성공 실패 값 반환
*/
int net_accept( module_object_t* p_this, int* pi_fd, mtime_t i_wait )
{
    bool b_die = p_this->b_end, b_block = (i_wait < 0);

    while( p_this->b_end == b_die )
    {
        int i_val = -1, *pi, *pi_end;
        struct timeval timeout;
        fd_set fds_r, fds_e;

        pi = pi_fd;

        /* Initialize file descriptor set */
        FD_ZERO( &fds_r );
        FD_ZERO( &fds_e );

        for( pi = pi_fd; *pi != -1; pi++ )
        {
            int i_fd = *pi;

            if( i_fd > i_val )
                i_val = i_fd;

            FD_SET( i_fd, &fds_r );
            FD_SET( i_fd, &fds_e );
        }
        pi_end = pi;

        timeout.tv_sec = 0;
        timeout.tv_usec = b_block ? 500000 : (long)i_wait;
		//accetp 소켓에 대한 event 처리
        i_val = select( i_val + 1, &fds_r, NULL, &fds_e, &timeout );
        if( ( ( i_val < 0 ) && ( net_errno == EINTR ) ) || i_val == 0 )
        {
            if( b_block )
                continue;
            else
                return -1;
        }
        else if( i_val < 0 )
        {
			trace_msg(p_this, TRACE_ERROR, "network select error (%s)", net_strerror( net_errno ));
            return -1;
        }

        for( pi = pi_fd; *pi != -1; pi++ )
        {
            int i_fd = *pi;
			//event check
            if( !FD_ISSET( i_fd, &fds_r ) && !FD_ISSET( i_fd, &fds_e ) )
                continue;

            i_val = accept( i_fd, NULL, 0 );
			if( i_val < 0 ){
				trace_msg(p_this, TRACE_ERROR, "accept failed (%s)", net_strerror(net_errno));
			}
#ifndef _MSC_VER
            else if( i_val >= FD_SETSIZE )
            {
                net_close( i_val );
				trace_msg(p_this, TRACE_ERROR, "accept failed (too many sockets opened)");
            }
#endif
            else
            {
                const int yes = 1;
                setsockopt( i_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof( yes ));
#ifdef FD_CLOEXEC
                fcntl( i_fd, F_SETFD, FD_CLOEXEC );
#endif
                --pi_end;
                memmove( pi, pi + 1, pi_end - pi );
                *pi_end = i_fd;
                return i_val;
            }
        }
    }

    return -1;
}
/** 
	@brief DDS에서 TCP 연결에 대한 Listen 소켓 Close 함수 
	@param[in] pi_fd DDS에서 Close할 Listen 소켓 포인터
*/
void net_listen_close( int* pi_fd )
{
    if( pi_fd != NULL )
    {
        int *pi;

        for( pi = pi_fd; *pi != -1; pi++ )
            net_close( *pi );
        FREE( pi_fd );
    }
}

#ifdef _MSC_VER
/** 
	@brief DDS에서 TCP 연결에 대한 Error를 출력하는 함수  
	@param[in] value Socket Error 값 변수
	@return	소켓 Error에 대한 Message 반환 
*/
const char *net_strerror(int value)
{
    switch( value )
    {
        case WSAENETUNREACH:
            return "Destination unreachable";

        case WSAETIMEDOUT:
            return "Connection timed out";

        case WSAECONNREFUSED:
            return "Connection refused";

        default:
        {
            static char errmsg[14 + 5 + 1];

            if( ((unsigned)value) > 99999 )
                return "Invalid error code";

            sprintf( errmsg, "Winsock error %u", (unsigned)value );
            return errmsg;
        }
    }

    return strerror( value );
}
#endif
