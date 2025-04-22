/*
	thread 관련 구현 부분
	
	작성자 : 
	이력
	2010-07-26 : 시작 (윈도우게열)
*/

/**

	@file thread.c
	@date 2013.03.25
	@author ETRI
	@version 1.0
	@brief DCPS에서 사용되는 thread 및 mutex, signal 관련 실행 파일

*/

#include <core.h>

#define THREADS_UNINITIALIZED  0
#define THREADS_PENDING        1
#define THREADS_ERROR          2
#define THREADS_READY          3



#ifndef _MSC_VER   
static pthread_mutex_t once_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static volatile unsigned i_initializations = 0;
static volatile int i_status = THREADS_UNINITIALIZED;
static module_object_t *p_root = NULL;

/*
	mutex 초기화
*/
/** 
	@brief DDS에서 사용되는 Mutex 초기화 함수 
	@param[in] p_mutex 초기화 할 Mutex 포인터
	@return	Mutex 초기화 성공 실패 값 반환
*/
int mutex_init(mutex_t *p_mutex )
{
#ifdef _MSC_VER
	p_mutex->mutex = NULL;
    //InitializeCriticalSection( &p_mutex->csection );
	InitializeCriticalSectionAndSpinCount(&p_mutex->csection,  0x80000400);
    return 0;
#else
	p_mutex->lockcount = 0;
	return pthread_mutex_init( &p_mutex->mutex, NULL );
#endif
}

/*
	mutex 삭제
*/
/** 
	@brief DDS에서 사용되는 Mutex 삭제 함수 
	@param[in] p_mutex 초기화 할 Mutex 포인터
	@return	Mutex 초기화 성공 실패 값 반환
*/
int mutex_destroy(mutex_t *p_mutex )
{
#ifdef _MSC_VER
	if( p_mutex->mutex )
    {
        CloseHandle( p_mutex->mutex );
    }
    else
    {
        DeleteCriticalSection( &p_mutex->csection );
    }
    return 0;
#else
	return pthread_mutex_destroy( &p_mutex->mutex );
#endif
}

/** 
	@brief DDS에서 사용되는 Signal 초기화 함수 
	@param[in] p_condvar 초기화 할 Signal 포인터
	@return	Signal 초기화 성공 실패 값 반환
*/
int cond_init( cond_t *p_condvar )
{
#ifdef _MSC_VER
	p_condvar->i_waiting_threads = 0;

    p_condvar->semaphore = CreateSemaphore( NULL,
                                            0,
                                            0x7fffffff,
                                            NULL );

	p_condvar->event = CreateEvent( NULL, TRUE, TRUE, NULL );
	//InitializeCriticalSectionAndSpinCount(&p_condvar->csection,  0x80000400);

	InitializeCriticalSection( &p_condvar->csection );
	return !p_condvar->semaphore || !p_condvar->event;
#else
	return pthread_cond_init( &p_condvar->cond, NULL );
#endif
};


/** 
	@brief DDS에서 사용되는 Signal 삭제 함수 
	@param[in] p_condvar 초기화 할 Signal 포인터
	@return	Signal 삭제 성공 실패 값 반환
*/
int cond_destroy(cond_t *p_condvar )
{
	int i_result;

#ifdef _MSC_VER
	if( !p_condvar->semaphore )
        i_result = !CloseHandle( p_condvar->event );
    else
        i_result = !CloseHandle( p_condvar->event ) || !CloseHandle( p_condvar->semaphore );

    if( p_condvar->semaphore != NULL ) DeleteCriticalSection( &p_condvar->csection );
#else
	i_result = pthread_cond_destroy( &p_condvar->cond );
#endif

	return i_result;

}

/** 
	@brief DCPS 최상위 Object인 Libmodule에 대한 설정을 수행 하는 함수 
	@param[in] p_this 최상위 Module Object로 Service Module을 생성 관리하는 최상위 Module 포인터
	@return	Libmodule 설정 성공 실패 값 반환
*/
int threads_init( module_object_t *p_this )
{
	libmodule_t *p_libmodule = (libmodule_t *)p_this;
	int i_ret = MODULE_SUCCESS;

#ifndef _MSC_VER
	pthread_mutex_lock( &once_mutex );
#endif


	if( i_status == THREADS_UNINITIALIZED )
    {
		i_status = THREADS_PENDING;
		p_libmodule->b_ready = false;
		//libmodule 초기화
		p_root = object_create( OBJECT(p_libmodule), OBJECT_ROOT );
		
		if( p_root == NULL )
			i_ret = MODULE_ERROR_CREATE;

		if( i_ret )
        {
            i_status = THREADS_ERROR;
        }
        else
        {
            i_initializations++;
            i_status = THREADS_READY;
        }

	}else{
		i_initializations++;
	}

#ifndef _MSC_VER
	 pthread_mutex_unlock( &once_mutex );
#endif

	if( i_status != THREADS_READY )
    {
        return MODULE_ERROR_THREAD;
    }

	return i_ret;
}
/** 
	@brief DCPS 최상위 Object인 Libmodule을 종료하는 함수 
	@param[in] p_this 최상위 Module Object로 Service Module을 생성 관리하는 최상위 Module 포인터
	@return	Libmodule 삭제 성공 실패 값 반환
*/
int threads_end( module_object_t *p_this )
{

    if( i_initializations == 0 )
        return MODULE_ERROR_THREAD;

#ifndef _MSC_VER
	pthread_mutex_lock( &once_mutex );
#endif

    i_initializations--;
    if( i_initializations == 0 )
    {
        i_status = THREADS_UNINITIALIZED;
        object_destroy( p_root );
    }

#ifndef _MSC_VER
	 pthread_mutex_unlock( &once_mutex );
#endif

    return MODULE_SUCCESS;
}

#ifdef _MSC_VER
typedef unsigned (WINAPI *PTHREAD_START) (void *);
#endif

/** 
	@brief DCPS에서 사용되는 Thread 생성 함수 
	@param[in] p_this Thread 생성 Object 포인터
	@param[in] psz_name Thread의 Thread 생성 이름 변수 
	@param[in] func Thread가 실행될 thread 실행 함수 포인터 
	@param[in] i_priority 실행될 Thread의 Thread 우선순위 변수 
	@param[in] b_wait Thread 생성 확인을 위한 Flag 변수
	@return	Thread 생성 성공 실패 값 반환
*/
int thread_create(module_object_t *p_this, char *psz_name, void * ( *func ) ( void * ),int i_priority, bool b_wait)
{

	int i_ret;
    void *p_data = (void *)p_this;
	unsigned threadID;

    mutex_lock( &p_this->object_lock );

#ifdef _MSC_VER
	p_this->thread_id = (HANDLE)_beginthreadex( NULL, 0, (PTHREAD_START) func, p_data, 0, &threadID );
	if( p_this->thread_id && i_priority )
    {
        if( !SetThreadPriority(p_this->thread_id, i_priority) )
        {
            i_priority = 0;
        }
    }

	i_ret = ( p_this->thread_id ? 0 : 1 );

#else
	i_ret = pthread_create( &p_this->thread_id, NULL, func, p_data );
#endif
    

	if( i_ret == 0 )
    {
        if( b_wait )
        {
            cond_wait( &p_this->object_wait, &p_this->object_lock );
        }

        p_this->b_thread = 1;
        mutex_unlock( &p_this->object_lock );
    }
    else
    {
        mutex_unlock( &p_this->object_lock );
    }

    return i_ret;
}

/** 
	@brief DCPS에서 사용되는 Thread 생성 함수 
	@param[in] p_data Thread 생성 Object 포인터
	@param[in] p_condvar Thread에서 사용할 Signal 포인터
	@param[in] p_mutex Thread에서 사용할 Mutex 포인터 
	@param[in] psz_name Thread의 Thread 생성 이름 변수 
	@param[in] func 실행될 Thread 실행 함수 포인터 
	@param[in] i_priority 실행될 Thread의 Thread 우선순위 변수
	@param[in] b_wait Thread 생성 확인을 위한 Flag 변수
	@return	Thread 생성 성공 실패 값 반환
*/
module_thread_t thread_create2(void *p_data, cond_t *p_condvar, mutex_t *p_mutex, char *psz_name, void * ( *func ) ( void * ),int i_priority, bool b_wait)
{

	int i_ret;
	unsigned threadID;
	module_thread_t thread_id;

    mutex_lock( p_mutex );
#ifdef _MSC_VER
	thread_id = (module_thread_t)_beginthreadex( NULL, 0, (PTHREAD_START) func, p_data, 0, &threadID );
	if( threadID && i_priority )
    {
        if( !SetThreadPriority((HANDLE)threadID, i_priority) )
        {
            i_priority = 0;
        }
    }

    i_ret = ( threadID ? 0 : 1 );
#else
	i_ret = pthread_create( &thread_id, NULL, func, p_data );
#endif

	if( i_ret == 0 )
    {
        if( b_wait )
        {
            cond_wait( p_condvar, p_mutex );
        }

        mutex_unlock( p_mutex );
    }
    else
    {
        mutex_unlock( p_mutex );
    }

    return thread_id;
}
/** 
	@brief DCPS에서 사용되는 Thread 우선 순위 설정 함수 
	@param[in] p_this Thread 생성 Object 포인터
	@param[in] i_priority 실행될 Thread의 Thread 우선순위 변수
	@return	Thread 우선순위 설정 성공 실패 값 반환
*/
int thread_set_priority(module_object_t *p_this, int i_priority)
{

#ifdef _MSC_VER
	if( !SetThreadPriority(GetCurrentThread(), i_priority) )
    {
        return 1;
    }
#endif

	return 0;
}
/** 
	@brief DCPS에서 사용되는 Thread의 Mutex, Lock 초기화 함수 
	@param[in] p_this Thread 생성 Object 포인터
*/
void thread_ready(module_object_t *p_this)
{
    mutex_lock( &p_this->object_lock );
    cond_signal( &p_this->object_wait );
    mutex_unlock( &p_this->object_lock );
}
/** 
	@brief DCPS에서 사용되는 Thread의 종료를 확인하는 함수 
	@param[in] p_this Thread 생성 Object 포인터
*/
void thread_join(module_object_t *p_this)
{
#ifdef _MSC_VER
	HMODULE hmodule;
    BOOL (WINAPI *OurGetThreadTimes)( HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME* );
    FILETIME create_ft, exit_ft, kernel_ft, user_ft;
    int64_t real_time, kernel_time, user_time;

    WaitForSingleObject( p_this->thread_id, INFINITE );

	hmodule = GetModuleHandle("KERNEL32");

	OurGetThreadTimes = (BOOL (WINAPI*)( HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME* )) GetProcAddress( hmodule, "GetThreadTimes");

    if( OurGetThreadTimes &&
        OurGetThreadTimes( p_this->thread_id,
                           &create_ft, &exit_ft, &kernel_ft, &user_ft ) )
    {
        real_time = ((((int64_t)exit_ft.dwHighDateTime)<<32)| exit_ft.dwLowDateTime) - ((((int64_t)create_ft.dwHighDateTime)<<32)| create_ft.dwLowDateTime);
        real_time /= 10;

        kernel_time = ((((int64_t)kernel_ft.dwHighDateTime)<<32) | kernel_ft.dwLowDateTime) / 10;

        user_time = ((((int64_t)user_ft.dwHighDateTime)<<32) | user_ft.dwLowDateTime) / 10;
    }
    CloseHandle( p_this->thread_id );
	p_this->b_thread = 0;
#else
	pthread_join( p_this->thread_id, NULL );
#endif
}

/** 
	@brief DCPS에서 사용되는 Thread의 종료를 확인하는 함수 
	@param[in] thread_id 종료하려는 Thread ID 변수
*/
void thread_join2(module_thread_t thread_id)
{
#ifdef _MSC_VER
	HMODULE hmodule;
    BOOL (WINAPI *OurGetThreadTimes)( HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME* );
    FILETIME create_ft, exit_ft, kernel_ft, user_ft;
    int64_t real_time, kernel_time, user_time;

    WaitForSingleObject( thread_id, INFINITE );

	hmodule = GetModuleHandle("KERNEL32");

	OurGetThreadTimes = (BOOL (WINAPI*)( HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME* )) GetProcAddress( hmodule, "GetThreadTimes");

    if( OurGetThreadTimes &&
        OurGetThreadTimes( thread_id,
                           &create_ft, &exit_ft, &kernel_ft, &user_ft ) )
    {
        real_time = ((((int64_t)exit_ft.dwHighDateTime)<<32)| exit_ft.dwLowDateTime) - ((((int64_t)create_ft.dwHighDateTime)<<32)| create_ft.dwLowDateTime);
        real_time /= 10;

        kernel_time = ((((int64_t)kernel_ft.dwHighDateTime)<<32) | kernel_ft.dwLowDateTime) / 10;

        user_time = ((((int64_t)user_ft.dwHighDateTime)<<32) | user_ft.dwLowDateTime) / 10;
    }
    CloseHandle( thread_id );
#else
	pthread_join( thread_id, NULL );
#endif
}


/** 
	@brief DCPS에서 사용되는 Thread의 Signal을 기다리는 함수 
	@param[in] p_condvar Thread에서 이벤트 발생을 위해 사용되는 Signal 포인터
	@param[in] p_mutex Thread에서 Mutex 사용을 위한 Mutex 포인터
	@param[in] dwMilliseconds Signal을 기다리는 최대 시간 변수
*/
int cond_waittimed2( cond_t *p_condvar, mutex_t *p_mutex, unsigned long dwMilliseconds)
{

	int i_result = 0;

    int i_waiting_threads;
#if defined(_MSC_VER)
    WaitForSingleObject( p_condvar->event, dwMilliseconds );
    p_condvar->i_waiting_threads++;

    LeaveCriticalSection( &p_mutex->csection );
    WaitForSingleObject( p_condvar->semaphore, dwMilliseconds );

    EnterCriticalSection( &p_condvar->csection );

    i_waiting_threads = --p_condvar->i_waiting_threads;

    LeaveCriticalSection( &p_condvar->csection );

    if( !i_waiting_threads )
		SetEvent( p_condvar->event );

    mutex_lock( p_mutex );
#else
	struct timeval now;
	struct timespec ts = {0,0};
	long usec;
#ifdef __cplusplus
#else
	gettimeofday(&now, NULL);
#endif
	

#ifdef Android
	usec = (dwMilliseconds*1000);
	ts.tv_sec =  (long)(usec/1000000);
    ts.tv_nsec =  (usec%1000000)*1000;
	i_result = pthread_cond_timeout_np( &p_condvar->cond, &p_mutex->mutex, dwMilliseconds);
#else

	usec = (now.tv_usec+dwMilliseconds*1000);
	ts.tv_sec =  now.tv_sec + (long)(usec/1000000);
    ts.tv_nsec =  (usec%1000000)*1000;


	i_result = pthread_cond_timedwait( &p_condvar->cond, &p_mutex->mutex, &ts);
#endif
#endif
	return i_result;
}