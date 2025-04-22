/*
	모듈의 한종류로 그냥 화면에 로그정보를 출력하기 위한 샘플로서 작성
	작성자 : 
	이력
	2010-07-26 : 처음 시작
*/


#include <core.h>

#undef inline

#ifdef _MSC_VER
#include <iostream>
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include <time.h>

#undef malloc

using namespace std;


static int  Open  ( module_object_t * );
static void Close ( module_object_t * );

#define MODULE_PREFIX "simple-trace-"

#define ENABLE_TEXT "Enable"
#define ENABLE_LONG_TEXT "Enable Simple Trace"

#define TRACE_TEXT "TRACE"
#define TRACE_LONG_TEXT "Trace print enable check"

struct trace_sys_t
{
	bool	b_trace;
	bool	b_trace2;
	bool	b_trace3;
	bool	b_debug;
	bool	b_warm;
	bool	b_error;
	bool	b_normal;
	uint32_t count;
};

module_define_start(simpletrace)
	set_category( CAT_TRACE );
	set_description( "Simple Trace" );
	set_capability( "trace", 200 );
	set_callbacks( Open, Close );
	add_shortcut( "simpletrace" );
	add_bool( (char*)MODULE_PREFIX"enable", false, NULL, (char*)ENABLE_TEXT, (char*)ENABLE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"debug", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"warm", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"error", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"normal", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"trace", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"trace2", false, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
	add_bool( (char*)MODULE_PREFIX"trace3", true, NULL, (char*)TRACE_TEXT, (char*)TRACE_LONG_TEXT, false );
module_define_end()


static FILE* p_trace_file;


static void trace(trace_t *p_trace, int mode, char *p_str);

static int  Open  ( module_object_t *p_this)
{
	trace_t *p_trace = (trace_t *)p_this;
	value_t  val;

	var_create( p_this, MODULE_PREFIX"enable", VAR_BOOL | VAR_DOINHERIT );
	var_get( p_this, MODULE_PREFIX"enable", &val );

	if(val.b_bool == false)
	{
		return MODULE_FAIL;
	}


	p_trace->p_sys = (trace_sys_t *)malloc(sizeof(trace_sys_t));
	memset(p_trace->p_sys, 0, sizeof(trace_sys_t));

	if(!p_trace->p_sys)
	{
		return MODULE_ERROR_MEMORY;
	} 

	var_create( p_this, MODULE_PREFIX"trace", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"trace2", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"trace3", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"debug", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"warm", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"error", VAR_BOOL | VAR_DOINHERIT );
	var_create( p_this, MODULE_PREFIX"normal", VAR_BOOL | VAR_DOINHERIT );

	var_get( p_this, MODULE_PREFIX"trace", &val );
	p_trace->p_sys->b_trace  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"trace2", &val );
	p_trace->p_sys->b_trace2  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"trace3", &val );
	p_trace->p_sys->b_trace3  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"debug", &val );
	p_trace->p_sys->b_debug  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"warm", &val );
	p_trace->p_sys->b_warm  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"error", &val );
	p_trace->p_sys->b_error  = val.b_bool;

	var_get( p_this, MODULE_PREFIX"normal", &val );
	p_trace->p_sys->b_normal  = val.b_bool;

	p_trace->p_sys->count = 0;

	p_trace->pf_trace = trace;

	//////////////////////////////////////////////////////////////////////////
	{
		char file_name[40];
		struct tm nowtime;
#ifdef _MSC_VER
		__time32_t aclock;
		_time32( &aclock );
		_localtime32_s(&nowtime, &aclock);
#else
		time_t aclock;
		time(&aclock);
		nowtime = *localtime(&aclock);
#endif

#ifdef _MSC_VER
		if (mkdir(".\\LOG") != ENOENT)
#else
		if (mkdir(".\\LOG", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != ENOENT)
#endif
		{
			sprintf(file_name, "LOG\\EDDS-LOG %04d-%02d-%02d %02d.%02d.%02d.log", 
				nowtime.tm_year+1900, nowtime.tm_mon+1, nowtime.tm_mday, 
				nowtime.tm_hour, nowtime.tm_min, nowtime.tm_sec);
		}
		else
		{
			sprintf(file_name, "EDDS-LOG %04d-%02d-%02d %02d.%02d.%02d.log", 
				nowtime.tm_year+1900, nowtime.tm_mon+1, nowtime.tm_mday, 
				nowtime.tm_hour, nowtime.tm_min, nowtime.tm_sec);
		}
		p_trace_file = fopen(file_name, "w");
	}
	//////////////////////////////////////////////////////////////////////////

	return MODULE_SUCCESS;
}

static void Close ( module_object_t *p_this)
{
	trace_t *p_trace = (trace_t *)p_this;
	FREE(p_trace->p_sys);

	//////////////////////////////////////////////////////////////////////////
	if (p_trace_file) fclose(p_trace_file);
	//////////////////////////////////////////////////////////////////////////
}

static void trace(trace_t *p_trace, int mode, char *p_str)
{

	const char *psz_type = NULL;

	if(p_trace->p_sys == NULL) return;

	switch(mode)
	{
		case TRACE_NORMAL:
			if(p_trace->p_sys->b_normal == false) return;

			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                     FOREGROUND_INTENSITY | FOREGROUND_RED |
   //                     FOREGROUND_GREEN | FOREGROUND_BLUE);
			psz_type = "[Normal]";
			break;
		case TRACE_ERROR:
			if(p_trace->p_sys->b_error == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                      FOREGROUND_INTENSITY | FOREGROUND_RED);
			psz_type = "[Error]";
			break;
		case TRACE_WARM:
			if(p_trace->p_sys->b_warm == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                   FOREGROUND_INTENSITY | FOREGROUND_RED |
   //                   FOREGROUND_GREEN);
			psz_type = "[Warm]";
			break;
		case TRACE_DEBUG:
			if(p_trace->p_sys->b_debug == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                  FOREGROUND_INTENSITY | FOREGROUND_RED |
   //                  FOREGROUND_BLUE);
			psz_type = "[Debug]";
			break;
		case TRACE_TRACE:
			if(p_trace->p_sys->b_trace == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                     FOREGROUND_INTENSITY | FOREGROUND_GREEN );
			psz_type = "[Trace]";
			break;
		case TRACE_TRACE2:
			if(p_trace->p_sys->b_trace2 == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                 FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			psz_type = "[Trace]";
			break;
		case TRACE_TRACE3:
			if(p_trace->p_sys->b_trace3 == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                 FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			psz_type = "[Trace]";
			break;
		default:
			if(p_trace->p_sys->b_normal == false) return;
			//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
   //                     FOREGROUND_INTENSITY | FOREGROUND_RED |
   //                     FOREGROUND_GREEN | FOREGROUND_BLUE);
			psz_type = "[Normal]";
			break;
	}

	 struct tm nowtime;
#ifdef _MSC_VER
	__time32_t aclock;
	_time32( &aclock );   // Get time in seconds.
    _localtime32_s( &nowtime, &aclock ); 
#else
	time_t aclock;
	time(&aclock);
	nowtime = *localtime( &aclock );
#endif
	
	//cout <<"["<<  nowtime.tm_hour << ':' << nowtime.tm_min << ':' << nowtime.tm_sec << "]" << psz_type << p_str << endl;

	char buff[30];
	memset(buff,0,30);
#ifdef _MSC_VER
	asctime_s( buff, sizeof(buff), &nowtime );
#else
	asctime_r( &nowtime,buff );
#endif
	buff[strlen(buff)-1] = 0x00;

	if(mode == TRACE_TRACE3)
		fprintf(p_trace_file, "[%5d:%s]%s %s\r\n", ++p_trace->p_sys->count, buff, psz_type, p_str);


	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
 //                       FOREGROUND_INTENSITY | FOREGROUND_RED |
 //                       FOREGROUND_GREEN | FOREGROUND_BLUE);

};
