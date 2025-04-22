/*
	TRACE 관련 헤더
	작성자 : 
	이력
	2010-08-8 : 시작

*/

#if !defined(__TRACE_H)
#define __TRACE_H

struct trace_t
{
    MODULE_COMMON_FIELD

	moduleload_t	*p_moduleload;
	void ( *pf_trace ) (trace_t *p_trace, int mode, char *p_str);

	trace_sys_t		*p_sys;
};


#ifdef __cplusplus
extern "C" {
#endif

	int	launchTraceModule(module_object_t *p_this);
	void  trace_msg(module_object_t *p_this,int mode, const char *psz_format, ...);

#ifdef __cplusplus
}
#endif


#endif