/*
	일반적인 관련 UTIL 함수.

	작성자 : 
	이력
	2010-08-5 : 시작
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


#ifndef _MSC_VER

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>



#endif


size_t strlcpy (char *tgt, const char *src, size_t bufsize)
{
    size_t length;

    for (length = 1; (length < bufsize) && *src; length++)
        *tgt++ = *src++;

    if (bufsize)
        *tgt = '\0';

    while (*src++)
        length++;

    return length - 1;
}


int vasprintf(char **strp, const char *fmt, va_list ap)
{
    int     i_size = 100;
    char    *p;
    int     n;

	p = malloc( i_size );
	memset(p, 0, i_size);

    if( p == NULL )
    {
        *strp = NULL;
        return -1;
    }

    for( ;; )
    {
        n = vsnprintf( p, i_size, fmt, ap );

        if (n > -1 && n < i_size)
        {
            *strp = p;
            return strlen( p );
        }
        if (n > -1)
        {
           i_size = n+1;
        }
        else
        {
           i_size *= 2;
        }
        if( (p = realloc( p, i_size ) ) == NULL)
        {
            *strp = NULL;
            return -1;
        }
    }
}

int asprintf( char **strp, const char *fmt, ... )
{
    va_list args;
    int i_ret;

    va_start( args, fmt );
    i_ret = vasprintf( strp, fmt, args );
    va_end( args );

    return i_ret;
}


static char *local_ip = NULL;

char* get_default_local_address()
{

	char *psz_if_addr = NULL;
	module_t *p_module = current_object(0);

	if(local_ip)
	{
		return strdup(local_ip);
	}

	if(p_module){
		psz_if_addr = config_get_psz( OBJECT(p_module), "interface-addr" );
	}else
	{
		return NULL;
	}

	if(psz_if_addr)
	{
		return psz_if_addr;
	}else
	{
		struct addrinfo hints, *res;
		int i_val;
		struct sockaddr_in loc;
#ifdef _MSC_VER
		DWORD i_index;
#endif
		memset( &hints, 0, sizeof( hints ) );
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE;

		i_val = get_addr_info( OBJECT(p_module), "239.255.0.1", 0, &hints, &res );
		if( i_val )
		{
			trace_msg(OBJECT(p_module), TRACE_WARM, "%s: %s", "239.255.0.1", gai_str_error(i_val));
			free_addr_info( res );
			return NULL;
		}

		memcpy( &loc, res->ai_addr, sizeof( struct sockaddr_in ) );
		free_addr_info( res );
#ifdef _MSC_VER
		if( IN_MULTICAST( ntohl( loc.sin_addr.s_addr ) ) )
		{
			if( GetBestInterface(loc.sin_addr.s_addr, &i_index) == NO_ERROR)
			{
				PMIB_IPADDRTABLE p_table;
                DWORD i = 0;
				int c = 0;

				trace_msg(OBJECT(p_module), TRACE_DEBUG , "Winsock best interface is %lu", (unsigned long)i_index);

                GetIpAddrTable( NULL, &i, 0 );

                p_table = (PMIB_IPADDRTABLE)malloc( i );
                if( p_table != NULL )
                {
                    if( GetIpAddrTable( p_table, &i, 0 ) == NO_ERROR )
                    {
                        for( c = p_table->dwNumEntries-1; c >= 0; c-- )
                        {
                            if( p_table->table[c].dwIndex == i_index )
                            {
								FREE(psz_if_addr);
								asprintf(&psz_if_addr,"%d.%d.%d.%d", p_table->table[c].dwAddr & 0xFF
																   , p_table->table[c].dwAddr >> 8 & 0xFF
																   , p_table->table[c].dwAddr >> 16 & 0xFF
																   , p_table->table[c].dwAddr >> 24 & 0xFF);
//                                imr.imr_interface.s_addr =  p_table->table[c].dwAddr;
								trace_msg(OBJECT(p_module), TRACE_DEBUG , "using interface 0x%08x", p_table->table[c].dwAddr);
								break;
							}else{
								if((p_table->table[c].dwAddr & 0xFF) != 127)
								{ 
									asprintf(&psz_if_addr,"%d.%d.%d.%d", p_table->table[c].dwAddr & 0xFF
																	   , p_table->table[c].dwAddr >> 8 & 0xFF
																	   , p_table->table[c].dwAddr >> 16 & 0xFF
																	   , p_table->table[c].dwAddr >> 24 & 0xFF);
									trace_msg(OBJECT(p_module), TRACE_DEBUG , "using interface 0x%08x", p_table->table[c].dwAddr);
								}
//                                imr.imr_interface.s_addr =  p_table->table[c].dwAddr;
							}
                        }
                    }
					else{
						trace_msg(OBJECT(p_module), TRACE_WARM , "GetIpAddrTable failed");
					}
                    FREE( p_table );
				}else{
					trace_msg(OBJECT(p_module), TRACE_WARM , "GetIpAddrTable failed");
				}
			}else{
				trace_msg(OBJECT(p_module), TRACE_WARM , "GetIpAddrTable failed");
			
			}
		}else
		{
			trace_msg(OBJECT(p_module), TRACE_WARM , "GetIpAddrTable failed");
		}
#else
		{

#ifdef Android
			{
				#define BUFFERSIZE	4000
				#define MAXADDRS	32
				static int   nextAddr = 0;




				int                 i, len, flags;
				char                buffer[BUFFERSIZE], *ptr, lastname[IFNAMSIZ], *cptr;
				struct ifconf       ifc;
				struct ifreq        *ifr, ifrcopy;
				struct sockaddr_in	*sin;

				char temp[80];

				int sockfd;

				sockfd = socket(AF_INET, SOCK_DGRAM, 0);
				if (sockfd < 0)
				{
					perror("socket failed");
					return NULL;
				}
				
				ifc.ifc_len = BUFFERSIZE;
				ifc.ifc_buf = buffer;
				
				if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
				{
					perror("ioctl error");
					return NULL;
				}
				
				lastname[0] = 0;
				
				for (ptr = buffer; ptr < buffer + ifc.ifc_len; )
				{
					ifr = (struct ifreq *)ptr;
					len = sizeof(struct sockaddr);
					ptr += sizeof(ifr->ifr_name) + len;	// for next one in buffer
				
					if (ifr->ifr_addr.sa_family != AF_INET)
					{
						continue;	// ignore if not desired address family
					}
				
					if ((cptr = (char *)strchr(ifr->ifr_name, ':')) != NULL)
					{
						*cptr = 0;		// replace colon will null
					}
				
					if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0)
					{
						continue;
					}
				
					memcpy(lastname, ifr->ifr_name, IFNAMSIZ);
				
					ifrcopy = *ifr;
					ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
					flags = ifrcopy.ifr_flags;
					if ((flags & IFF_UP) == 0)
					{
						continue;	// ignore if interface not up
					}
				
				
					sin = (struct sockaddr_in *)&ifr->ifr_addr;
					//strcpy(temp, inet_ntoa(sin->sin_addr));

					if ( ntohl(sin->sin_addr.s_addr) == INADDR_LOOPBACK)
					{
						//printf("Loop Backn");
					}else{
						asprintf(&psz_if_addr,"%s", inet_ntoa(sin->sin_addr));
						break;
						//printf("Ip Addresss : %s\r\n", temp);
					}


					++nextAddr;
				}
				
				close(sockfd);
			}
#else
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
			{
				struct ifaddrs * ifAddrStruct=NULL;
				struct ifaddrs * ifa=NULL;
				void * tmpAddrPtr=NULL;

				getifaddrs(&ifAddrStruct);

				for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
					if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
						// is a valid IP4 Address
						tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
						char addressBuffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
						trace_msg(NULL,TRACE_LOG,"%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
						psz_if_addr = strdup(addressBuffer);
					} else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
						// is a valid IP6 Address
						tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
						char addressBuffer[INET6_ADDRSTRLEN];
						inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
						trace_msg(NULL,TRACE_LOG,"%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
					} 
				}
				if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

			}

#endif
		}


#endif
		
	}


	if(psz_if_addr)
	{
		trace_msg(NULL,TRACE_LOG,"IP Address %s\n",psz_if_addr); 
		
		local_ip = strdup(psz_if_addr);
	}
	return psz_if_addr;
}


void set_string(dds_string *p_string, char* value)
{
	p_string->size = strlen(value)+1;
	memcpy(p_string->value,value,strlen(value));
	p_string->value[strlen(value)] = '\0';
}

char* get__string( dds_string* p_string )
{
	if(p_string->size)
	{
		return p_string->value;
	}

	return "";
}