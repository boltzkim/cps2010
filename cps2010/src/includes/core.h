/*
	코어의 메인 헤더파일

	작성자 : 
	이력
	2010-07-12 : 시작
*/

#if !defined(__CORE_H)
#define __CORE_H


//#define _CRTDBG_MAP_ALLOC 

#ifdef _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#define QOS_EF
#define QOS_RL
#define QOS_H_R
#define QOS_O
#define QOS_L


#ifdef __APPLE__
# include <stdlib.h>
#else
#	include <malloc.h>
#endif
#include <string.h>
#include <stdarg.h>


#ifdef _MSC_VER
#define msleep(delay)		Sleep( (int) (delay / 1000) )
#else

#ifdef Android
#define msleep(delay)		usleep(delay);
#else

#define msleep(delay)		{\
								struct timespec ts_delay;\
								ts_delay.tv_sec = delay / 1000000;\
								ts_delay.tv_nsec = (delay % 1000000) * 1000;\
								nanosleep( &ts_delay, NULL );\
							}

#endif

#endif
#define CLOCK_FREQ			1000000
#define DEFAULT_PTS_DELAY   (mtime_t)(.3*CLOCK_FREQ)
#define THREAD_SLEEP		((mtime_t)(0.010*CLOCK_FREQ))

#ifdef _MSC_VER
#	define inline __inline
#	pragma warning(disable : 4996)
#	define strcasecmp stricmp

#	if _MSC_VER < 1910
#		define snprintf _snprintf
#	endif
#endif


#if defined( _MSC_VER )
#	if defined(UNDER_CE) && defined(sockaddr_storage)
#       undef sockaddr_storage
#	endif
#   if defined(UNDER_CE)
#       define HAVE_STRUCT_ADDRINFO
#	else
//#       include <io.h>
#   endif
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <iphlpapi.h>
#	ifndef ENETUNREACH
#		if _MSC_VER < 1600
#		define ENETUNREACH WSAENETUNREACH
#		endif
#	endif
#   define net_errno (WSAGetLastError())

#else
#   if HAVE_SYS_SOCKET_H
#      include <sys/socket.h>
#   endif
#   if HAVE_NETINET_IN_H
#      include <netinet/in.h>
#   endif
#   if HAVE_ARPA_INET_H
#      include <arpa/inet.h>
#   elif defined( SYS_BEOS )
#      include <net/netdb.h>
#   endif
#   include <netdb.h>
#   define net_errno errno
#   define net_strerror strerror
#endif

#include <fcntl.h>

#ifdef _MSC_VER
#	include <io.h>
#endif

//jeman malloc 시 메모리 할당을 제대로 못받았을 경우 반복 시도 횟수 및 주기
#define MALLOC_REPEAT 5
#define MALLOC_WAITTIME 10000


///////////////////////////////////////////////////
// 메모리 할당 및 해제시 제되로 되었는지 알아보기 위해서, 출력


#ifdef _MSC_VER
//#	define __MEMEMORY_TRACING 1
#endif

#ifdef __MEMEMORY_TRACING
#	define malloc(x)		tracemalloc(__FILE__, __LINE__, __FUNCDNAME__,#x,x)
#	define free(x)			tracefree(__FILE__, __LINE__, __FUNCDNAME__,#x,x)
#	define strdup(x)		tracestrdup(__FILE__, __LINE__, __FUNCDNAME__,#x,x)
#	define realloc(x, y)	tracerealloc(__FILE__, __LINE__, __FUNCDNAME__,#x,x, y)
#endif



#ifdef __cplusplus
extern "C" {
#endif
	void *tracemalloc(const char *p_filename, const int line, const char *p_funcname, const char *p_name, size_t size);
	void tracefree(const char *p_filename, const int line, const char *p_funcname, const char *p_name, void *ptr);
	char *tracestrdup(const char *p_filename, const int line, const char *p_funcname, const char *p_name, const char *psz);
	void *tracerealloc(const char *p_filename, const int line, const char *p_funcname, const char *p_name, void *ptr, size_t size);
	void print_not_free_object();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MEMEMORY_TRACING
static inline void* malloc_zero(size_t x)
{

#ifdef _CRTDBG_MAP_ALLOC
	void * v_xx = _malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__);
#else
	void * v_xx = malloc(x);
#endif
	
	
	memset(v_xx, 0, x);

	return v_xx;
}

#endif



#ifdef __cplusplus
}
#endif

#ifndef __cplusplus

#ifdef __MEMEMORY_TRACING
//#	define malloc(x) malloc_zero(x)
#else
//#	undef malloc
//#	define malloc(x) malloc_zero(x)
#endif

#endif

//#define BEFOR_PRINT_MALLOC_TRACE \
//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),\
//                    FOREGROUND_INTENSITY | FOREGROUND_GREEN |\
//                    FOREGROUND_BLUE);



///////////////////////////////////////////////////
#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#ifndef __cplusplus
	#ifndef bool
		#define bool char
	#endif
#endif



#ifndef __cplusplus
	#define false 0
	#define true  1
#endif

#define FREE(x) if(x) {free(x); x=NULL;}



enum TRACE_TYPE
{
	TRACE_NORMAL,
	TRACE_ERROR,
	TRACE_WARM,
	TRACE_DEBUG,
	TRACE_TRACE,
	TRACE_TRACE2,
	TRACE_TRACE3,
	TRACE_LOG,
	TRACE_LOG2
};

//////////  struct define /////////////////////////

typedef struct module_object_t module_object_t;
typedef struct module_bank_t module_bank_t;
typedef struct libmodule_t libmodule_t;
typedef struct module_t module_t;
typedef struct moduleload_t moduleload_t;
typedef struct module_config_t module_config_t;
typedef struct module_list_t module_list_t;

typedef struct trace_t trace_t;
typedef struct service_t service_t;
typedef struct typesupport_t typesupport_t;
typedef struct discovery_t discovery_t;
typedef struct qos_policy_t qos_policy_t;//by kki (qos module)

//////////////////////////////////////////////////
typedef struct trace_sys_t trace_sys_t;

///////////////////////////////////////////////////
typedef struct variable_t variable_t;
typedef struct callback_entry_t callback_entry_t;

///////////////////////////////////////////////////
#ifndef DOMAINPARTICIPANT_DEFINE
	#define DOMAINPARTICIPANT_DEFINE 1
	typedef struct DomainParticipant DomainParticipant;
#endif
#ifndef WAITSET_DEFINE
        #define WAITSET_DEFINE 1
        typedef struct WaitSet WaitSet;
#endif

typedef struct message_t message_t;
typedef struct message_fifo_t message_fifo_t;
typedef struct management_t management_t;
typedef struct monitoring_t monitoring_t;
typedef struct access_t access_t;
typedef struct access_out_t access_out_t;
typedef struct data_t data_t;
typedef struct data_fifo_t data_fifo_t;
typedef struct dds_string dds_string;

#include <thread.h>
#include <module_common.h>
#include <module_config.h>
#include <trace.h>
#include <service.h>
#include <configuration.h>
#include <util.h>
#include <typesupport.h>

#include <management.h>
#include <monitoring.h>
#include <network.h>
#include <access.h>
#include <data.h>
#include <discovery.h>
#include <qos_policy.h>//by kki (qos module)

#include <stdlib.h>
#include <assert.h>

#ifndef THREAD_PRIORITY_HIGHEST
#	define THREAD_PRIORITY_HIGHEST 0
#endif



#ifdef __cplusplus
extern "C" {
#endif

	void pause_system();


#ifdef __cplusplus
}
#endif


#endif
