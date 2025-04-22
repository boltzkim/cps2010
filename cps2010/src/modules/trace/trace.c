/*
	trace 관련 구현 부분
	
	작성자 : 
	이력
	2010-08-5
*/

#include <core.h>


static trace_t *p_static_trace = NULL;

int	launchTraceModule(module_object_t *p_this)
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	trace_t *p_trace = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "trace", p_parser->psz_capability )) )
        {
            continue;
        }

		p_trace = object_create( p_this ,OBJECT_TRACE);

		if(p_trace && (p_parser->pf_activate(OBJECT(p_trace)) == MODULE_SUCCESS))
		{
			p_trace->p_moduleload = p_parser;
			object_attach( OBJECT(p_trace), p_this );

			p_static_trace = p_trace;
		}else{
			object_destroy(OBJECT(p_trace));
			p_static_trace = NULL;
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}

void (*pfsetLog)(char *log) = 0;


#ifdef Android
#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libcps", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "libcps", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "libcps", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "libcps", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libcps", __VA_ARGS__)
#endif


#ifdef __MEMEMORY_TRACING
#	undef malloc
#	undef free
#	undef strdup
#	undef realloc
#endif



static DWORD pid = 0;

void trace_msg(module_object_t* p_this, int mode, const char* psz_format, ...)
{
	module_list_t* p_list = NULL;
	trace_t* p_trace = NULL;
	int i_index = -1;
	int     len;
	char*    p_str = NULL;

	va_list args;


	if (pid == 0)
	{
		pid = GetCurrentProcessId();
	}

	//return;
	va_start( args, psz_format );
#ifdef _MSC_VER
	len = _vscprintf( psz_format, args ) + 1;
#else
	len = 1024;
#endif


	p_str = (char*)malloc( len * sizeof(char) );
	vsprintf(p_str, psz_format,args);
	va_end( args );

	/*if (mode != TRACE_LOG)
	{
		if (p_static_trace)
		{
			p_static_trace->pf_trace(p_static_trace,mode,p_str);
		}
		else
		{
			p_list = moduleListFind(OBJECT(p_this), OBJECT_TRACE, FIND_ANYWHERE);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_trace = (trace_t *)p_list->p_values[i_index].p_object;
				p_trace->pf_trace(p_trace,mode,p_str);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}	*/

	if (mode == TRACE_ERROR)
	{
		//assert(mode != TRACE_ERROR);
	}

	if (mode == TRACE_LOG)
	{
#ifdef Android
		LOGE("%s", p_str);
#else
		//printf("[Process:%d]%s\r\n", pid, p_str);
#endif
		if (pfsetLog) pfsetLog(p_str);
	}else if (mode == TRACE_LOG2)
	{
#ifdef Android
		LOGE("%s", p_str);
#else
		printf("[Process:%d]%s\r\n", pid, p_str);
#endif
		if (pfsetLog) pfsetLog(p_str);
	}
	else
	{
#ifdef Android
		//LOGE("%s", p_str);
#else
		//printf("%s\r\n", p_str);
#endif
	}

	FREE(p_str);
}
