/*
	MODULE Library 모듈
	작성자 : 
	이력
	2010-08-5 : 처음 시작
*/
/**
	@file access_config.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief 	Object Access와 AccessOut 모듈을 사용하기 위하여 Network 속성들을 정의한 파일
	@details IPv4,6 활성화 여부, Timeout, MCU, Port 등에 대한 속성 설정
*/

#include <core.h>


#include <core.h>

#define TEST_TEXT "TEST"

#define IPV6_TEXT "Force IPv6"
#define IPV6_LONGTEXT  "IPv6 will be used by default for all connections." 

#define IPV4_TEXT "Force IPv4"
#define IPV4_LONGTEXT  "IPv4 will be used by default for all connections."

#define TIMEOUT_TEXT "TCP connection timeout"
#define TIMEOUT_LONGTEXT "Default TCP connection timeout (in milliseconds)."

#define MTU_TEXT "MTU of the network interface"
#define MTU_LONGTEXT  "MTU of the network interface, it is usually 1500 bytes."

#define SERVER_PORT_TEXT "UDP port"
#define SERVER_PORT_LONGTEXT  "This is the default port.. Default is 8301."

#define MIFACE_ADDR_TEXT "IPv4 multicast output interface address"
#define MIFACE_ADDR_LONGTEXT  "IPv4 adress for the default multicast interface."


#define ROUTERS_ADDR_TEXT "Routers destination address."
#define ROUTERS_ADDR_LONGTEXT "Routers destination address.[ex:192.168.1.2:7400,192.168.1.3:7400]"

module_define_start(main)
	set_category( CAT_SERVICE );
	set_description( "main module library" );
	set_capability( "main", 100 );
	add_bool( "test", false, NULL, TEST_TEXT, TEST_TEXT, false );
	add_bool( "ipv4", true, NULL, IPV4_TEXT, IPV4_LONGTEXT, false );
	add_bool( "ipv6", false, NULL, IPV6_TEXT, IPV6_LONGTEXT, false );
	add_integer( "ipv4-timeout", 5 * 1000, NULL, TIMEOUT_TEXT, TIMEOUT_LONGTEXT, true );
	add_integer( "mtu", 1500, NULL, MTU_TEXT, MTU_LONGTEXT, false );
	add_integer( "server-port", 8301, NULL, SERVER_PORT_TEXT, SERVER_PORT_LONGTEXT, false );
	add_string( "interface-addr", NULL, NULL, MIFACE_ADDR_TEXT, MIFACE_ADDR_LONGTEXT, true );
	add_string("routers", "192.168.0.10:8301", NULL, ROUTERS_ADDR_TEXT, ROUTERS_ADDR_LONGTEXT, false);
module_define_end()
