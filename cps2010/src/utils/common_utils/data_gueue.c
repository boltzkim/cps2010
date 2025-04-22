/*
	네트워크 데이타 관련...

	작성자 : 
	이력
	2010-08-8 : 시작
*/
/**
	@file data_gueue.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief 	Network Data를 사용하는 기능함수를 정의한 파일
	@details RTPS 아래에서 사용하는 Network 계층에서 사용하는 Data에 대한 생성/삭제 및 Data 및 Job Queue 관리와 관련된 기능을 수행함
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

/**
	@brief Network Data를 생성하는 함수
	@details RTPS에서 사용하는 통신 프로토콜인 UDP/IP에서 사용하는 Network Data 생성
	@param[in] i_size 생성할 Data의 크기
	@return	생성된 Network Data를 가리키는 포인터 
*/
data_t* data_new( int i_size )
{
	data_t *p_data = malloc(sizeof(data_t));

	memset(p_data, 0, sizeof(data_t));

	p_data->p_next = NULL;
    p_data->p_prev = NULL;

	//writer by jun
	//data 부분에 대한 NULL 추가 필요, 죽는 경우 발생
	p_data->p_data = NULL;

	p_data->i_size = i_size;
	if(i_size){
		p_data->p_data = malloc(i_size);
		memset(p_data->p_data,0,i_size);
	}
	p_data->sequence = 0;

	p_data->current_time = currenTime();
	p_data->p_address = NULL;
	p_data->i_port = 0;
	
	return p_data;
}

/**
	@brief Network Data를 삭제하는 함수
	@details RTPS에서 사용하는 통신 프로토콜인 UDP/IP에서 사용하는 Network Data 삭제
	@param[in] p_data 삭제할 Network Data를 가리키는 포인터
*/
void data_release( data_t* p_data )
{
	FREE(p_data->p_address);
	FREE(p_data->p_data);
    FREE(p_data);
}

/**
	@brief Network Data에 대한 Fifo Queue를 생성하는 함수
	@details Network Data들을 관리하기 위한 Fifo Queue 생성
	@return	생성된 Network Data Fifo Queue를 가리키는 포인터 
*/
data_fifo_t* data_fifo_new()
{
    data_fifo_t *p_fifo;

    p_fifo = malloc( sizeof( data_fifo_t));
	memset(p_fifo, 0, sizeof(data_fifo_t));

    mutex_init(&p_fifo->lock );
    cond_init(&p_fifo->wait );
    p_fifo->i_depth = p_fifo->i_size = 0;
    p_fifo->p_first = NULL;
    p_fifo->pp_last = &p_fifo->p_first;

	//writer by jun
	//새로운 writer가 생기거나 데이터의 전달과 같은 외부 이벤트를 처리
	//반복되는 job 주기적인 pdp 전송과 같은 것은 카운트 되지 않음
	p_fifo->i_new_job = 0;

    return p_fifo;
}

/**
	@brief Network Data에 대한 Fifo Queue에서 가장 처음 들어온 Data를 얻어오는 함수
	@details Network Data들을 관리하는 Fifo Queue에서 가장 처음 들어온 Data를 반환하고 Fifo Queue의 size를 하나 줄임(Take)
	@param[in] p_fifo Network Data를 얻어올 Network Data Fifo Queue를 가리키는 포인터
	@return	가장 처음 들어온 Network Data를 가리키는 포인터 
*/
data_t* data_fifo_get( data_fifo_t* p_fifo )
{
    data_t *m;

    mutex_lock( &p_fifo->lock );

    while( p_fifo->p_first == NULL )
    {
        cond_wait( &p_fifo->wait, &p_fifo->lock );
    }

    m = p_fifo->p_first;

    p_fifo->p_first = m->p_next;
    p_fifo->i_depth--;
	p_fifo->i_size -= m->i_size;

    if( p_fifo->p_first == NULL )
    {
        p_fifo->pp_last = &p_fifo->p_first;
    }

    mutex_unlock( &p_fifo->lock );

    m->p_next = NULL;
    return m;
}

/**
	@brief Network Data Fifo Queue의 Data를 삭제하는 함수
	@details Network Data들을 관리하는 Fifo Queue에서 모든 Data 삭제, Fifo Queue는 삭제하지 않음
	@param[in] p_fifo 삭제할 Network Data Fifo Queue를 가리키는 포인터
*/
void data_fifo_empty( data_fifo_t* p_fifo )
{
    data_t *m;

    mutex_lock( &p_fifo->lock );
    for( m = p_fifo->p_first; m != NULL; )
    {
        data_t *p_next;

        p_next = m->p_next;
        data_release( m );
        m = p_next;
    }

    p_fifo->i_depth = p_fifo->i_size = 0;
    p_fifo->p_first = NULL;
    p_fifo->pp_last = &p_fifo->p_first;
    mutex_unlock( &p_fifo->lock );
}

/**
	@brief Network Data Fifo Queue를 삭제하는 함수
	@details Network Data들을 관리하는 Fifo Queue에서 모든 Data 삭제
	@param[in] p_fifo 삭제할 Network Data Fifo Queue를 가리키는 포인터
*/
void data_fifo_release( data_fifo_t* p_fifo )
{
    data_fifo_empty( p_fifo );
    cond_destroy( &p_fifo->wait );
    mutex_destroy( &p_fifo->lock );
    FREE( p_fifo );
}

/**
	@brief Network Data Fifo Queue에 Data를 추가하는 함수
	@details Network Data들을 관리하는 Fifo Queue에서 Data를 추가
	@param[in] p_fifo Data를 추가할 Network Data Fifo Queue를 가리키는 포인터
	@param[in] p_data 추가할 Network Data를 가리키는 포인터
	@return	Data Fifo Queue Size
*/
int data_fifo_put( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
    mutex_lock( &p_fifo->lock );

    do
    {
        i_size += p_data->i_size;

        *p_fifo->pp_last = p_data;
        p_fifo->pp_last = &p_data->p_next;
        p_fifo->i_depth++;
        p_fifo->i_size += p_data->i_size;

        p_data = p_data->p_next;

    } while( p_data );


    cond_signal( &p_fifo->wait );
    mutex_unlock( &p_fifo->lock );

    return i_size;
}



//writer by jun
//queue의 맨앞 job을 가지고 옴
/**
	@brief Job에 대한 Fifo Queue에서 가장 처음 들어온 Job을 얻어오는 함수
	@details Job들을 관리하는 Fifo Queue에서 가장 처음 들어온 Job을 반환하고 Fifo Queue의 size를 하나 줄임(Take)
	@param[in] p_fifo Job을 얻어올 Job Fifo Queue를 가리키는 포인터
	@return	가장 처음 들어온 Job을 가리키는 포인터 
*/
data_t* timed_job_queue_data_fifo_get( data_fifo_t* p_fifo )
{
    data_t *m;

    mutex_lock( &p_fifo->lock );

    m = p_fifo->p_first;
	if( m == NULL ){
    	mutex_unlock( &p_fifo->lock );
		return NULL;
	}

    p_fifo->p_first = m->p_next;
    p_fifo->i_depth--;
	p_fifo->i_size -= m->i_size;

    if( p_fifo->p_first == NULL )
    {
        p_fifo->pp_last = &p_fifo->p_first;
    }

    mutex_unlock( &p_fifo->lock );

    m->p_next = NULL;
    return m;
}

//writer by jun
//queue의 맨앞 job을 가지고 옴 여기서 	
//p_fifo->i_new_job--은 새로이 추가된 job을 처리하고 반복적으로 동작시키지 않음을 나타냄
/**
	@brief Job에 대한 Fifo Queue에서 가장 처음 들어온 Job을 얻어오는 함수
	@details Job들을 관리하는 Fifo Queue에서 가장 처음 들어온 Job을 반환하고 Fifo Queue의 size를 하나 줄임(Take)\n
             timed_job_queue_data_fifo_get() 함수와 다른 것은 새로이 추가된 Job을 처리하고 반복적으로 동작시키지 않는 기능이 추가됨
	@param[in] p_fifo Job을 얻어올 Job Fifo Queue를 가리키는 포인터
	@return	가장 처음 들어온 Job을 가리키는 포인터 
*/
data_t* timed_job_queue_data_fifo_get_remove_job( data_fifo_t* p_fifo )
{
    data_t *m;

    mutex_lock( &p_fifo->lock );

    m = p_fifo->p_first;
	if( m == NULL ){
    	mutex_unlock( &p_fifo->lock );
		return NULL;
	}

    p_fifo->p_first = m->p_next;
    p_fifo->i_depth--;
	p_fifo->i_size -= m->i_size;

    if( p_fifo->p_first == NULL )
    {
        p_fifo->pp_last = &p_fifo->p_first;
    }
	p_fifo->i_new_job--;
//	printf("remove i_new_job %d\n",p_fifo->i_new_job);
    mutex_unlock( &p_fifo->lock );

    m->p_next = NULL;
    return m;
}

//writer by jun
//queue에 새로운 job을 추가하는데 시간을 비교해서 우선순위를 줌 	
/**
	@brief Job Queue에 시간을 비교하여 Job을 추가하는 함수
	@details Job들을 관리하는 Job Queue에서 Job을 추가, 시간을 비교해서 우선순위를 줌
	@param[in] p_fifo Job을 추가 할 Job Fifo Queue를 가리키는 포인터
	@param[in] p_data 추가할 Job을 가리키는 포인터
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_time_compare_data_fifo_put( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp = NULL;
	data_t *old_temp = NULL;

	rtps_writer_t *p_t_rtps_writer = NULL;


    mutex_lock( &p_fifo->lock );			//이것을 가지고 서로 경쟁 경쟁 안하도록 해야 

	temp = &p_fifo->p_first;

	while(1){

		if((*temp) == NULL) {
			*temp = p_data;
			break;
		}
	    else {

			if((*temp)->next_wakeup_time.sec < p_data->next_wakeup_time.sec){
		        temp = &(*temp)->p_next;
				}
			else if(((*temp)->next_wakeup_time.sec == p_data->next_wakeup_time.sec ) &&
				((*temp)->next_wakeup_time.nanosec <= p_data->next_wakeup_time.nanosec )){
		        temp = &(*temp)->p_next;
			}
			else {
				old_temp = *temp;
				*temp = p_data;
				p_data->p_next = old_temp;
				break;
			}

	    }
	}
	p_fifo->pp_last = &p_data->p_next;
    p_fifo->i_depth++;
    p_fifo->i_size += p_data->i_size;

    p_data = p_data->p_next;
	
    cond_signal( &p_fifo->wait );
    mutex_unlock( &p_fifo->lock );

    return i_size;
}


//writer by jun
//queue에 새로운 job을 추가하는데 시간을 비교해서 우선순위를 줌
//여기서는 각 job의 우선순위도 비교하는데 여기에 추가되는 job은 반복되지 않고 한번만 수행되는 job을 처리
/**
	@brief Job Queue에 시간을 비교하여 Job을 추가하는 함수
	@details Job들을 관리하는 Job Queue에서 Job을 추가, 시간을 비교해서 우선순위를 줌\n
	         각 Job의 우선순위도 비교하는데 여기에 추가되는 Job은 반복되지 않고 한번만 수행되는 Job을 처리
	@param[in] p_fifo Job을 추가 할 Job Fifo Queue를 가리키는 포인터
	@param[in] p_data 추가할 Job을 가리키는 포인터
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_time_compare_data_fifo_put_add_job( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp = NULL;
	data_t *old_temp = NULL;

	rtps_writer_t *p_t_rtps_writer = NULL;


    mutex_lock( &p_fifo->lock );			//이것을 가지고 서로 경쟁 경쟁 안하도록 해야 

	temp = &p_fifo->p_first;

	while(1){

		if((*temp) == NULL) {
			*temp = p_data;
			break;
		}
	    else {
			if( (*temp)->priority > p_data->priority) {
				old_temp = *temp;
				*temp = p_data;
				p_data->p_next = old_temp;
				break;
			}
			else if( (*temp)->priority < p_data->priority) {
				temp = &(*temp)->p_next;
			}
			else{

				if((*temp)->next_wakeup_time.sec < p_data->next_wakeup_time.sec){
			        temp = &(*temp)->p_next;
					}
				else if(((*temp)->next_wakeup_time.sec == p_data->next_wakeup_time.sec ) &&
					((*temp)->next_wakeup_time.nanosec <= p_data->next_wakeup_time.nanosec )){
			        temp = &(*temp)->p_next;
				}
				else {
					old_temp = *temp;
					*temp = p_data;
					p_data->p_next = old_temp;
					break;
				}
			}

	    }
	}
	p_fifo->pp_last = &p_data->p_next;
    p_fifo->i_depth++;
    p_fifo->i_size += p_data->i_size;

    p_data = p_data->p_next;

	p_fifo->i_new_job++;

    cond_signal( &p_fifo->wait );
    mutex_unlock( &p_fifo->lock );

	timed_job_queue_traverse(p_fifo);
    return i_size;
}

/**
	@brief Job Queue에 Job을 추가하는 함수
	@details Job들을 관리하는 Job Queue에서 Job을 추가
	@param[in] p_fifo Job을 추가 할 Job Fifo Queue를 가리키는 포인터
	@param[in] p_data 추가할 Job을 가리키는 포인터
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_data_fifo_put( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp;
    mutex_lock( &p_fifo->lock );			//이것을 가지고 서로 경쟁 경쟁 안하도록 해야 

	temp = &p_fifo->p_first;

	if(p_fifo->p_first == NULL)
		{

		*temp = p_data;
		}
    else {
		for( ;*temp != NULL; )
	    {
//	        (*temp)->p_next
	        temp = &(*temp)->p_next;
//			temp = &p_next;
	    }
		*temp = p_data;
    }


	p_fifo->pp_last = &p_data->p_next;
    p_fifo->i_depth++;
    p_fifo->i_size += p_data->i_size;

    p_data = p_data->p_next;
	
    cond_signal( &p_fifo->wait );
    mutex_unlock( &p_fifo->lock );

    return i_size;
}

//writer by jun
//queue에 존재하는 job을 보여줌
/**
	@brief Job Queue의 안의 Job 개수를 Count 하는 함수
	@details Job들을 관리하는 Job Queue에 존재하는 모든 Job의 갯수를 반환
	@param[in] p_fifo Count 할 Job Fifo Queue를 가리키는 포인터
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_traverse(data_fifo_t *p_fifo)
{
    int i_size = 0;

	data_t **temp = NULL;

    mutex_lock( &p_fifo->lock );

	temp = &p_fifo->p_first;

	while(1){
		if((*temp) == NULL) {
			break;
		}
	    else {

	        temp = &(*temp)->p_next;
	    }
	}
    mutex_unlock( &p_fifo->lock );
    return i_size;
}

//writer by jun
//새로운 job의 개수를 카운트 반복되는 job 아님
/**
	@brief Job Queue의 새로운 Job 개수를 Count 하는 함수
	@details Job들을 관리하는 Job Queue에서 반복되지 않는 Job의 갯수를 반환
	@param[in] p_fifo Count 할 Job Fifo Queue를 가리키는 포인터
	@return	Data Job Fifo Queue Size
*/
int get_number_new_job(data_fifo_t *p_fifo)
{
	int i_new_job;
    mutex_lock( &p_fifo->lock );
	i_new_job = p_fifo->i_new_job;
    mutex_unlock( &p_fifo->lock );

    return i_new_job;
}