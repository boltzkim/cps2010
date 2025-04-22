/*
	��Ʈ��ũ ����Ÿ ����...

	�ۼ��� : 
	�̷�
	2010-08-8 : ����
*/
/**
	@file data_gueue.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief 	Network Data�� ����ϴ� ����Լ��� ������ ����
	@details RTPS �Ʒ����� ����ϴ� Network �������� ����ϴ� Data�� ���� ����/���� �� Data �� Job Queue ������ ���õ� ����� ������
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

/**
	@brief Network Data�� �����ϴ� �Լ�
	@details RTPS���� ����ϴ� ��� ���������� UDP/IP���� ����ϴ� Network Data ����
	@param[in] i_size ������ Data�� ũ��
	@return	������ Network Data�� ����Ű�� ������ 
*/
data_t* data_new( int i_size )
{
	data_t *p_data = malloc(sizeof(data_t));

	memset(p_data, 0, sizeof(data_t));

	p_data->p_next = NULL;
    p_data->p_prev = NULL;

	//writer by jun
	//data �κп� ���� NULL �߰� �ʿ�, �״� ��� �߻�
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
	@brief Network Data�� �����ϴ� �Լ�
	@details RTPS���� ����ϴ� ��� ���������� UDP/IP���� ����ϴ� Network Data ����
	@param[in] p_data ������ Network Data�� ����Ű�� ������
*/
void data_release( data_t* p_data )
{
	FREE(p_data->p_address);
	FREE(p_data->p_data);
    FREE(p_data);
}

/**
	@brief Network Data�� ���� Fifo Queue�� �����ϴ� �Լ�
	@details Network Data���� �����ϱ� ���� Fifo Queue ����
	@return	������ Network Data Fifo Queue�� ����Ű�� ������ 
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
	//���ο� writer�� ����ų� �������� ���ް� ���� �ܺ� �̺�Ʈ�� ó��
	//�ݺ��Ǵ� job �ֱ����� pdp ���۰� ���� ���� ī��Ʈ ���� ����
	p_fifo->i_new_job = 0;

    return p_fifo;
}

/**
	@brief Network Data�� ���� Fifo Queue���� ���� ó�� ���� Data�� ������ �Լ�
	@details Network Data���� �����ϴ� Fifo Queue���� ���� ó�� ���� Data�� ��ȯ�ϰ� Fifo Queue�� size�� �ϳ� ����(Take)
	@param[in] p_fifo Network Data�� ���� Network Data Fifo Queue�� ����Ű�� ������
	@return	���� ó�� ���� Network Data�� ����Ű�� ������ 
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
	@brief Network Data Fifo Queue�� Data�� �����ϴ� �Լ�
	@details Network Data���� �����ϴ� Fifo Queue���� ��� Data ����, Fifo Queue�� �������� ����
	@param[in] p_fifo ������ Network Data Fifo Queue�� ����Ű�� ������
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
	@brief Network Data Fifo Queue�� �����ϴ� �Լ�
	@details Network Data���� �����ϴ� Fifo Queue���� ��� Data ����
	@param[in] p_fifo ������ Network Data Fifo Queue�� ����Ű�� ������
*/
void data_fifo_release( data_fifo_t* p_fifo )
{
    data_fifo_empty( p_fifo );
    cond_destroy( &p_fifo->wait );
    mutex_destroy( &p_fifo->lock );
    FREE( p_fifo );
}

/**
	@brief Network Data Fifo Queue�� Data�� �߰��ϴ� �Լ�
	@details Network Data���� �����ϴ� Fifo Queue���� Data�� �߰�
	@param[in] p_fifo Data�� �߰��� Network Data Fifo Queue�� ����Ű�� ������
	@param[in] p_data �߰��� Network Data�� ����Ű�� ������
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
//queue�� �Ǿ� job�� ������ ��
/**
	@brief Job�� ���� Fifo Queue���� ���� ó�� ���� Job�� ������ �Լ�
	@details Job���� �����ϴ� Fifo Queue���� ���� ó�� ���� Job�� ��ȯ�ϰ� Fifo Queue�� size�� �ϳ� ����(Take)
	@param[in] p_fifo Job�� ���� Job Fifo Queue�� ����Ű�� ������
	@return	���� ó�� ���� Job�� ����Ű�� ������ 
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
//queue�� �Ǿ� job�� ������ �� ���⼭ 	
//p_fifo->i_new_job--�� ������ �߰��� job�� ó���ϰ� �ݺ������� ���۽�Ű�� ������ ��Ÿ��
/**
	@brief Job�� ���� Fifo Queue���� ���� ó�� ���� Job�� ������ �Լ�
	@details Job���� �����ϴ� Fifo Queue���� ���� ó�� ���� Job�� ��ȯ�ϰ� Fifo Queue�� size�� �ϳ� ����(Take)\n
             timed_job_queue_data_fifo_get() �Լ��� �ٸ� ���� ������ �߰��� Job�� ó���ϰ� �ݺ������� ���۽�Ű�� �ʴ� ����� �߰���
	@param[in] p_fifo Job�� ���� Job Fifo Queue�� ����Ű�� ������
	@return	���� ó�� ���� Job�� ����Ű�� ������ 
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
//queue�� ���ο� job�� �߰��ϴµ� �ð��� ���ؼ� �켱������ �� 	
/**
	@brief Job Queue�� �ð��� ���Ͽ� Job�� �߰��ϴ� �Լ�
	@details Job���� �����ϴ� Job Queue���� Job�� �߰�, �ð��� ���ؼ� �켱������ ��
	@param[in] p_fifo Job�� �߰� �� Job Fifo Queue�� ����Ű�� ������
	@param[in] p_data �߰��� Job�� ����Ű�� ������
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_time_compare_data_fifo_put( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp = NULL;
	data_t *old_temp = NULL;

	rtps_writer_t *p_t_rtps_writer = NULL;


    mutex_lock( &p_fifo->lock );			//�̰��� ������ ���� ���� ���� ���ϵ��� �ؾ� 

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
//queue�� ���ο� job�� �߰��ϴµ� �ð��� ���ؼ� �켱������ ��
//���⼭�� �� job�� �켱������ ���ϴµ� ���⿡ �߰��Ǵ� job�� �ݺ����� �ʰ� �ѹ��� ����Ǵ� job�� ó��
/**
	@brief Job Queue�� �ð��� ���Ͽ� Job�� �߰��ϴ� �Լ�
	@details Job���� �����ϴ� Job Queue���� Job�� �߰�, �ð��� ���ؼ� �켱������ ��\n
	         �� Job�� �켱������ ���ϴµ� ���⿡ �߰��Ǵ� Job�� �ݺ����� �ʰ� �ѹ��� ����Ǵ� Job�� ó��
	@param[in] p_fifo Job�� �߰� �� Job Fifo Queue�� ����Ű�� ������
	@param[in] p_data �߰��� Job�� ����Ű�� ������
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_time_compare_data_fifo_put_add_job( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp = NULL;
	data_t *old_temp = NULL;

	rtps_writer_t *p_t_rtps_writer = NULL;


    mutex_lock( &p_fifo->lock );			//�̰��� ������ ���� ���� ���� ���ϵ��� �ؾ� 

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
	@brief Job Queue�� Job�� �߰��ϴ� �Լ�
	@details Job���� �����ϴ� Job Queue���� Job�� �߰�
	@param[in] p_fifo Job�� �߰� �� Job Fifo Queue�� ����Ű�� ������
	@param[in] p_data �߰��� Job�� ����Ű�� ������
	@return	Data Job Fifo Queue Size
*/
int timed_job_queue_data_fifo_put( data_fifo_t* p_fifo, data_t* p_data )
{
    int i_size = 0;
	int i=0;

	data_t **temp;
    mutex_lock( &p_fifo->lock );			//�̰��� ������ ���� ���� ���� ���ϵ��� �ؾ� 

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
//queue�� �����ϴ� job�� ������
/**
	@brief Job Queue�� ���� Job ������ Count �ϴ� �Լ�
	@details Job���� �����ϴ� Job Queue�� �����ϴ� ��� Job�� ������ ��ȯ
	@param[in] p_fifo Count �� Job Fifo Queue�� ����Ű�� ������
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
//���ο� job�� ������ ī��Ʈ �ݺ��Ǵ� job �ƴ�
/**
	@brief Job Queue�� ���ο� Job ������ Count �ϴ� �Լ�
	@details Job���� �����ϴ� Job Queue���� �ݺ����� �ʴ� Job�� ������ ��ȯ
	@param[in] p_fifo Count �� Job Fifo Queue�� ����Ű�� ������
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