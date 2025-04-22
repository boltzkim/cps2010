/*
	쓰레드 관련 헤더
	작성자 : 
	이력
	2010-07-24 : 시작 일단 윈도우만 우선 선언 어느 정도 되면 다른 플랫폼 추가

*/

#if !defined(__THREAD_H)
#define __THREAD_H

#include <stdio.h>

#if defined(_MSC_VER)
#	include <process.h>
#	include <windows.h>
#else
#   include <pthread.h>
#endif

#ifdef __APPLE__
#	include <sys/time.h>
#endif

#if defined(_MSC_VER)
typedef HANDLE module_thread_t;

typedef struct
{
    HANDLE              mutex;
    CRITICAL_SECTION    csection;
} mutex_t;

typedef struct
{
    volatile int        i_waiting_threads;
    HANDLE              event;
    HANDLE              semaphore;
    CRITICAL_SECTION    csection;
} cond_t;

#else
typedef pthread_t       module_thread_t;

typedef struct
{
    pthread_mutex_t mutex;
    module_object_t * p_this;
	int				lockcount;
} mutex_t;

typedef struct
{
    pthread_cond_t cond;
    module_object_t * p_this;
} cond_t;

#endif


static inline int mutex_lock(mutex_t * p_mutex )
{
	int i_result = 0;
#if defined(_MSC_VER)
	if( p_mutex->mutex )
    {
        WaitForSingleObject( p_mutex->mutex, INFINITE );
    }
    else
    {
		/*if(p_mutex->csection.RecursionCount >= 1)
		{
			printf("thread : %x\n",p_mutex->csection.OwningThread);
			EnterCriticalSection( &p_mutex->csection );
		}else{*/
			EnterCriticalSection( &p_mutex->csection );
		//}
    }
#else
	p_mutex->lockcount++;
	i_result = pthread_mutex_lock( &p_mutex->mutex );
#endif
	return i_result;
}


static inline int mutex_unlock(mutex_t *p_mutex )
{
	int i_result = 0;

#if defined(_MSC_VER)
	if( p_mutex->mutex )
    {
        ReleaseMutex( p_mutex->mutex );
    }
    else
    {
		LeaveCriticalSection( &p_mutex->csection );
    }
#else
	p_mutex->lockcount--;
	i_result = pthread_mutex_unlock( &p_mutex->mutex );
#endif

	return i_result;
}

static inline int cond_signal(cond_t *p_condvar )
{
	int i_result = 0;

#if defined(_MSC_VER)
	if( !p_condvar->semaphore )
    {
        PulseEvent( p_condvar->event );
    }
    else
    {
        WaitForSingleObject( p_condvar->event, INFINITE );

        if( p_condvar->i_waiting_threads )
        {
            ResetEvent( p_condvar->event );
            ReleaseSemaphore( p_condvar->semaphore, 1, 0 );
        }
    }
#else

	i_result = pthread_cond_signal( &p_condvar->cond );
#endif
	return i_result;
}


static inline int cond_wait( cond_t *p_condvar, mutex_t *p_mutex )
{

	int i_result = 0;

    int i_waiting_threads;
#if defined(_MSC_VER)
    WaitForSingleObject( p_condvar->event, INFINITE );
    p_condvar->i_waiting_threads++;

    LeaveCriticalSection( &p_mutex->csection );
    WaitForSingleObject( p_condvar->semaphore, INFINITE );

    EnterCriticalSection( &p_condvar->csection );

    i_waiting_threads = --p_condvar->i_waiting_threads;

    LeaveCriticalSection( &p_condvar->csection );

    if( !i_waiting_threads )
		SetEvent( p_condvar->event );

    mutex_lock( p_mutex );
#else
	i_result = pthread_cond_wait( &p_condvar->cond, &p_mutex->mutex );
#endif
	return i_result;
}


static inline int cond_waittimed( cond_t *p_condvar, mutex_t *p_mutex, unsigned long dwMilliseconds)
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


#ifdef __cplusplus
extern "C" {
#endif

	int threads_init( module_object_t *p_this );

	int mutex_init(mutex_t *p_mutex);
	int mutex_destroy(mutex_t *p_mutex);
	int cond_init(cond_t *p_condvar);
	int cond_destroy(cond_t *p_condvar);


	int thread_create(module_object_t *p_this, char *psz_name, void * ( *func ) ( void * ),int i_priority, bool b_wait);
	module_thread_t thread_create2(void *p_data, cond_t *p_condvar, mutex_t *p_mutex, char *psz_name, void * ( *func ) ( void * ),int i_priority, bool b_wait);
	int thread_set_priority(module_object_t *p_this, int i_priority);
	void thread_ready(module_object_t *p_this);
	void thread_join(module_object_t *p_this);
	void thread_join2(module_thread_t thread_id);
	int threads_end( module_object_t *p_this );
	int cond_waittimed2( cond_t *p_condvar, mutex_t *p_mutex, unsigned long dwMilliseconds);

#ifdef __cplusplus
};
#endif


#endif

