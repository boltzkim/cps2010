/*
	data 관련 헤더
	작성자 : 
	이력
	2010-08-10 : 시작

*/

#if !defined(__DATA_H)
#define __DATA_H

#ifndef HANDLE_TYPE_NATIVE
	#ifdef _M_IX86
	#	define HANDLE_TYPE_NATIVE int32_t
	#else
	#	define HANDLE_TYPE_NATIVE int64_t
	#endif
	typedef HANDLE_TYPE_NATIVE InstanceHandle_t;
	typedef struct DataWriter DataWriter;
#endif

#ifndef TIME_STRUCT_T
	#define TIME_STRUCT_T
	typedef struct Time_t Time_t;
	struct Time_t {
		long sec;
		unsigned long nanosec;
	};

	typedef struct Duration_t {
		long sec;
		unsigned long nanosec;
	}Duration_t;

#endif



struct data_t
{
	data_t*			p_next;
    data_t*			p_prev;

	int				i_size;	
	uint8_t*		p_data;
	unsigned long	sequence;
	Time_t			current_time;
	///
	uint8_t*		p_address;
	int				i_port;
	int				socket_id;

	//by jun
	//공통의 queue에서 각 데이터가 누구 것인지 알기 위한 구조	
	void*			v_rtps_reader;
	rtps_writer_t*	p_rtps_writer;
	int				priority;
	//by jun job queue를 위한 데이터 구조 
	unsigned long 	wake_time;
	Time_t			initial_time;
	Time_t			next_wakeup_time;
	bool 			b_thread_first;
};


struct data_fifo_t
{
    mutex_t			lock;
    cond_t			wait;

    int				i_depth;
    data_t*			p_first;
    data_t**		pp_last;
    int				i_size;
	//by jun
	int				i_new_job;
};


static inline data_t* data_duplicate(data_t* p_data)
{
    data_t *p_dup = (data_t *)malloc(sizeof(data_t));
    if( p_dup == NULL )
        return NULL;

	memset(p_dup,'\0',sizeof(data_t));
	p_dup->i_size = p_data->i_size;

	if(p_dup->i_size){
		p_dup->p_data = (uint8_t *)malloc(p_dup->i_size);
		memcpy( p_dup->p_data, p_data->p_data, p_data->i_size );
	}

	p_data->current_time = p_dup->current_time;

	p_data->sequence = p_dup->sequence;

	//if(p_data->p_address)p_dup->p_address = strdup((const char *)p_data->p_address);
	p_data->i_port = p_dup->i_port;

    return p_dup;
}

#ifdef __cplusplus
extern "C" {
#endif

	data_t* data_new(int i_size);
	void data_release(data_t* p_data);

	data_fifo_t* data_fifo_new();
	data_t* data_fifo_get(data_fifo_t* p_fifo);
	void data_fifo_empty(data_fifo_t* p_fifo);
	void data_fifo_release(data_fifo_t* p_fifo);
	int data_fifo_put(data_fifo_t* p_fifo, data_t* p_data);

	data_t* timed_job_queue_data_fifo_get(data_fifo_t* p_fifo);
	data_t* timed_job_queue_data_fifo_get_remove_job(data_fifo_t* p_fifo);
	int timed_job_queue_time_compare_data_fifo_put(data_fifo_t* p_fifo, data_t* p_data);
	int timed_job_queue_time_compare_data_fifo_put_add_job(data_fifo_t* p_fifo, data_t* p_data);
	int timed_job_queue_data_fifo_put(data_fifo_t* p_fifo, data_t* p_data);
	int timed_job_queue_traverse(data_fifo_t* p_fifo);
	int get_number_new_job(data_fifo_t* p_fifo);

#ifdef __cplusplus
}
#endif

#endif