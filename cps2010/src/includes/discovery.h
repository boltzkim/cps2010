/*
	디스커버리 관련 헤더
	작성자 : 
	이력
	2010-09-27 : 시작

*/

#if !defined(__DISCOVERY_H)
#define __DISCOVERY_H

typedef struct discovery_sys_t discovery_sys_t;

struct discovery_t
{
    MODULE_COMMON_FIELD

	moduleload_t*	p_moduleload;

	discovery_sys_t*	p_sys;
};


#ifdef __cplusplus
extern "C" {
#endif

	int launch_discovery_module(module_object_t* p_this);

#ifdef __cplusplus
}
#endif

#endif