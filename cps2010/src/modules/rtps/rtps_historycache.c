/*
	RTSP HistoryCache class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
	Container class used to temporarily store and manage sets of changes to data-objects.
	On the Writer side it contains the history of the changes to data-objects made by the
	Writer. It is not necessary that the full history of all changes ever made is maintained.
	Rather what is needed is the partial history required to service existing and future
	matched RTPS Reader endpoints. The partial history needed depends on the DDS QoS
	and the state of the communications with the matched Reader endpoints.
	On the Reader side it contains the history of the changes to data-objects made by the
	matched RTPS Writer endpoints. It is not necessary that the full history of all changes
	ever received is maintained. Rather what is needed is a partial history containing the
	superposition of the changes received from the matched writers as needed to satisfy the
	needs of the corresponding DDS DataReader. The rules for this superposition and the
	amount of partial history required depend on the DDS QoS and the state of the
	communication with the matched RTPS Writer endpoints.
*/

#include "rtps.h"
#include "print_whs_rhs.h"

rtps_historycache_t	*rtps_historycache_new()
{
	rtps_historycache_t *p_historycache = NULL ;
	int i=0;

	//jeman hitorycache 메모리 할당 실패 시 재시도
	while(p_historycache == NULL){
		p_historycache = (rtps_historycache_t *)malloc(sizeof(rtps_historycache_t));
		memset(p_historycache, 0, sizeof(rtps_historycache_t));

		i++;
		if(i == MALLOC_REPEAT){
			free(p_historycache);
			return NULL;
		}
		//msleep(1);
	}
	//

	//p_historycache->pp_changes = NULL;
	//p_historycache->i_changes = 0;
	mutex_init(&p_historycache->object_lock);
	p_historycache->p_rtps_reader = NULL;
	p_historycache->p_rtps_writer = NULL;
	p_historycache->history_kind = KEEP_LAST_HISTORY_QOS;
	p_historycache->history_depth = INT_MAX; // 256

	p_historycache->p_head_first = NULL;
	p_historycache->p_head_last = NULL;
	p_historycache->i_linked_size = 0;

	return p_historycache;
}




void rtps_historycache_destory(rtps_historycache_t *p_historycache)
{
	linked_list_atom_t *p_atom=NULL;

	HISTORYCACHE_LOCK(p_historycache);
	while(p_historycache->i_linked_size)
	{
		p_atom = p_historycache->p_head_first;

		remove_linked_list((linked_list_head_t *)p_historycache, (linked_list_atom_t *)p_atom);

		rtps_cachechange_ref((rtps_cachechange_t *)p_atom, true, false);
		rtps_cachechange_destory((rtps_cachechange_t *)p_atom);
		//REMOVE_ELEM( p_historycache->pp_changes, p_historycache->i_changes, 0);

		
	}
	HISTORYCACHE_UNLOCK(p_historycache);

//	FREE(p_historycache->pp_changes);

	mutex_destroy(&p_historycache->object_lock);
	FREE(p_historycache);
}






void rtps_historycache_add_re_ordering(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change)
{
//	int i;
	int i_cachechange_size = p_historycache->i_linked_size;

	rtps_cachechange_ref(p_change, true, true);

	//INSERT_ELEM( p_historycache->pp_changes, p_historycache->i_changes, p_historycache->i_changes, p_change);

	insert_linked_list((linked_list_head_t *)p_historycache, (linked_list_atom_t *)p_change);

#if EDDS_TESTING
	print_rhs(p_historycache->p_rtps_reader, (SerializedPayloadForReader *)p_change->p_data_value);

	if (p_historycache->p_rtps_reader && p_historycache->p_rtps_reader->is_builtin == false)
	{
		trace_msg(OBJECT(current_object(0)), TRACE_TRACE3, "\t\t\t\t +++ ADD[%5u]", p_change->sequence_number.low);
		trace_historycache(OBJECT(current_object(0)), TRACE_TRACE3, p_historycache);
	}
#endif
	
	/*if(i_cachechange_size == 0)
		INSERT_ELEM( p_historycache->pp_changes, p_historycache->i_changes, p_historycache->i_changes, p_change);

	for(i = i_cachechange_size-1; i >= 0; i--)
	{
		if(sequnce_number_left_bigger(p_change->sequence_number, p_historycache->pp_changes[i]->sequence_number))
		{
			INSERT_ELEM( p_historycache->pp_changes, p_historycache->i_changes, i+1, p_change);
			break;
		}
	}*/


	if (p_historycache->p_rtps_writer  /*&& !p_historycache->p_rtps_writer->is_bultin*/)
	{
		p_historycache->p_rtps_writer = p_historycache->p_rtps_writer;
		p_change->p_org_message->v_related_cachechange = p_change;
		// 속도 때문에 일단 스킵
		//insert_instanceset_datawriter(p_historycache->p_rtps_writer->p_datawriter, p_change, p_change->p_org_message);

		if (p_change->instance_handle == 0
			&& p_change->p_org_message->i_key != 0)
		{
			p_change->instance_handle = p_change->p_org_message->handle;
		}
	
	}
	else if (p_historycache->p_rtps_reader /*&& !p_historycache->p_rtps_reader->is_bultin*/)
	{


		if (p_change->kind == ALIVE)
		{
			SerializedPayloadForReader *p_serializedForReader = (SerializedPayloadForReader *)p_change->p_data_value;
			message_t *p_message;


			if(p_change->p_data_value == NULL)
			{
				// gap 처리
				//return;
			}


			//added by kyy(Presentation QoS)
			//cachechange의 data가 없는 것에 대한 처리////////////////////////////////////////////////////////////////////////////
			if (!p_historycache->p_rtps_reader->is_builtin && p_change->p_data_value == NULL)
			{
				//datareader->p_message
				p_historycache->p_rtps_reader->p_datareader->is_coherent_set_end = true;

				return;
				//p_message = message_new_for_builtin_reader(p_historycache->p_rtps_reader->p_datareader, NULL, 0);
				//printf("Size2 %d\n",p_historycache->p_rtps_reader->p_datareader->i_message_order);

			}

			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			/*if(p_historycache->p_rtps_reader->is_bultin)
			{
				p_message = malloc(sizeof(message_t));
				memset(p_message,0, sizeof(message_t));
			}else */
			else if(p_change->p_org_message == NULL){
				p_message = message_new_for_builtin_reader(p_historycache->p_rtps_reader->p_datareader, &p_change->p_data_value->p_value[4], p_serializedForReader->i_size-4);
///				p_message = message_new_for_builtin_reader(p_historycache->p_rtps_reader->p_datareader, &p_change->p_data_value->p_value[4], strnlen(&p_change->p_data_value->p_value[4], p_serializedForReader->i_size-4));
			}else{
				p_message = p_change->p_org_message;
			}
			
	

			if(p_message != NULL)
			{
				memcpy(p_message->key_guid, p_change->key_guid_for_reader, 16);
			}
			else
			{
				return;
			}
			
			p_change->p_org_message = p_message;
			
			p_change->b_read = true;
			p_message->v_related_cachechange = p_change;


			//added by kyy(Presentation QoS && Destination Order)
			p_message->is_coherent_set = p_change->is_coherent_set;
			p_message->sampleInfo.source_timestamp.sec = p_change->source_timestamp.sec;
			p_message->sampleInfo.source_timestamp.nanosec = p_change->source_timestamp.nanosec;
			p_message->lifespan_duration.sec = p_change->lifespan_duration.sec;
			p_message->lifespan_duration.nanosec = p_change->lifespan_duration.nanosec;
			p_message->received_time.sec = p_change->received_time.sec;
			p_message->received_time.nanosec = p_change->received_time.nanosec;
			p_message->coherent_set_number = p_change->coherent_set_number.low;
			//printf("%d",p_message->received_time.sec);
			////////////////////////////////////////////////////////////


			//waitset by jun available
			insert_instanceset_datareader(p_historycache->p_rtps_reader->p_datareader, p_change, p_message);

			change_DataOnReaders_DataAvailable_status(p_historycache->p_rtps_reader->p_datareader);	

		}else if(p_change->kind == NOT_ALIVE_DISPOSED)
		{

		}else if(p_change->kind == NOT_ALIVE_UNREGISTERED)
		{

		}
		
	}
}

void rtps_historycache_change_dispose(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change, GUID_t key_guid)
{
//	int i;
	rtps_cachechange_t *p_change_atom = NULL;

	if(p_historycache == NULL) return;
	HISTORYCACHE_LOCK(p_historycache);
	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

	while(p_change_atom)
	{
		
		if(memcmp(&p_change_atom->writer_guid, &p_change->writer_guid, sizeof(GUID_t)) == 0)
		{
			if(p_change_atom->p_org_message != NULL
				&& memcmp(&p_change_atom->p_org_message->key_guid, &key_guid, sizeof(GUID_t)) == 0 )
			{
				p_change_atom->b_isdisposed = true;
			}

		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}


	HISTORYCACHE_UNLOCK(p_historycache);

}


void rtps_historycache_change_unregister(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change, GUID_t key_guid)
{
	//int i;

	//if(p_historycache == NULL) return;
	//HISTORYCACHE_LOCK(p_historycache);

	//for(i=0; i < p_historycache->i_changes; i++)
	//{
	//	if(memcmp(&p_historycache->pp_changes[i]->writer_guid, &p_change->writer_guid, sizeof(GUID_t)) == 0)
	//	{
	//		if(p_historycache->pp_changes[i]->p_org_message != NULL
	//			&& memcmp(&p_historycache->pp_changes[i]->p_org_message->key_guid, &key_guid, sizeof(GUID_t)) == 0 )
	//		{
	//			

	//		}

	//	}
	//}


	//HISTORYCACHE_UNLOCK(p_historycache);

}

void insert_missing_num(rtps_writerproxy_t	*p_rtps_writerproxy, SequenceNumber_t missing_num)
{
	int i;
	SequenceNumber_t *p_missig;

	for (i = 0; i < p_rtps_writerproxy->i_missing_seq_num; i++)
	{
		if (sequnce_number_equal(missing_num, *p_rtps_writerproxy->pp_missing_seq_num[i]))
		{
			return;
		}
	}

	p_missig = malloc(sizeof(SequenceNumber_t));
	memset(p_missig, 0, sizeof(SequenceNumber_t));

	*p_missig = missing_num;
	INSERT_ELEM(p_rtps_writerproxy->pp_missing_seq_num, p_rtps_writerproxy->i_missing_seq_num
		, p_rtps_writerproxy->i_missing_seq_num, p_missig);


	//trace_msg(NULL, TRACE_LOG, "insert_missing_num(%d:%d)..." , p_rtps_writerproxy->i_missing_seq_num, missing_num.low);
}


bool remove_missing_num(rtps_writerproxy_t	*p_rtps_writerproxy, SequenceNumber_t missing_num)
{
	int j;

	for (j = 0; j < p_rtps_writerproxy->i_missing_seq_num; j++)
	{
		if (sequnce_number_equal(missing_num, *p_rtps_writerproxy->pp_missing_seq_num[j]))
		{
			FREE(p_rtps_writerproxy->pp_missing_seq_num[j]);
			REMOVE_ELEM(p_rtps_writerproxy->pp_missing_seq_num, p_rtps_writerproxy->i_missing_seq_num, j);

//			trace_msg(NULL, TRACE_LOG, "remove_missing_num(%d:%d)..." , p_rtps_writerproxy->i_missing_seq_num, missing_num.low);
			return true;
		}
	}

	return false;
}


static int test = 4001;

rtps_cachechange_t *rtps_historycache_add_change(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change)
{
	int j;
	bool is_same = false;	
	rtps_cachechange_t *p_tmpchange = NULL;
	rtps_cachechange_t *p_change_atom = NULL;

	if(p_historycache == NULL) return NULL;

	
	//if(p_historycache->p_rtps_writer != NULL
	//	&& p_historycache->p_rtps_writer->p_datawriter->id == 23)
	//{

	//	if(p_historycache->i_linked_size < 1000)
	//	insert_linked_list(p_historycache, p_change);
	//	//remove_linked_list(p_historycache, p_change);
	//	return;
	//}

	//////if(p_historycache->object_lock.csection.OwningThread != 0)
	//////{
	//////	int a;

	//////	a = 0;
	//////}

	HISTORYCACHE_LOCK(p_historycache);
	//기존에 같은거 있는지 검색해서... 값이 계속 쌓이지 않도록 함...
	//printf("p_historycache->i_changes : %d\r\n", p_historycache->i_changes);

	

	

	if(p_historycache->p_rtps_reader)
	{

		//p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

		//while(p_change_atom)
		//{
		//	if((memcmp(&p_change_atom->writer_guid, &p_change->writer_guid, sizeof(GUID_t)) == 0)
		//		&& (memcmp(&p_change_atom->sequence_number, &p_change->sequence_number, sizeof(SequenceNumber_t)) == 0))
		//	{
		//		//기존에 있는거임...
		//		p_tmpchange = p_change_atom;
		//		is_same = true;
		//		break;
		//	}
		//	p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		//}
	}else
	{
		

	
	}


	/*if (p_historycache->p_rtps_reader && (memcmp(&p_historycache->p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0))
	{
		int a = 0;
		a++;
	}
*/
	/*if (p_historycache->p_rtps_reader != NULL && p_historycache->p_rtps_reader->is_builtin && p_change->sequence_number.low == 1)
	{
		return;

	}

	if (p_historycache->p_rtps_reader != NULL && p_historycache->p_rtps_reader->is_builtin && p_change->sequence_number.low == 3)
	{
		return;

	}*/


	//
	//if(p_change->p_rtps_writerproxy && p_change->p_rtps_writerproxy->i_missing_seq_num)
	//{
	//	test = test;
	//	printf("ok writerproxy[%d:%d] : %p\r\n", p_change->sequence_number.low, p_change->p_rtps_writerproxy->i_missing_seq_num, p_change->p_rtps_writerproxy);
	//
	//}

	if(!is_same){

#if 1
		//내가 모르는 코드 일단 스킵
		//if(p_change->sequence_number.low >= p_change->coherent_set_number.low)

		if(p_historycache->reliability_kind == RELIABLE_RELIABILITY_QOS){
			if(/*p_historycache->i_linked_size &&*/ p_change->p_rtps_writerproxy)
			{
				if( sequnce_number_equal(p_change->p_rtps_writerproxy->max_seq_num, sequnce_number_dec(p_change->sequence_number)) == false)
				{
					if(sequnce_number_left_bigger(p_change->p_rtps_writerproxy->max_seq_num, sequnce_number_dec(p_change->sequence_number)))
					{

						if (remove_missing_num(p_change->p_rtps_writerproxy, p_change->sequence_number) == false)
						{

							if (sequnce_number_left_bigger(p_change->p_rtps_writerproxy->min_seq_num, p_change->sequence_number))
							{
								//insert missing
								int32_t i_diff = sequnce_number_difference2(p_change->p_rtps_writerproxy->min_seq_num, p_change->sequence_number);
								SequenceNumber_t base_seq = p_change->sequence_number;


								for (j = 0; j < i_diff - 1; j++) {
									/*SequenceNumber_t *p_missig = malloc(sizeof(SequenceNumber_t));
									*p_missig = base_seq = sequnce_number_inc(base_seq);
									INSERT_ELEM(p_change->p_rtps_writerproxy->pp_missing_seq_num, p_change->p_rtps_writerproxy->i_missing_seq_num
										, p_change->p_rtps_writerproxy->i_missing_seq_num, p_missig);*/

									base_seq = sequnce_number_inc(base_seq);
									insert_missing_num(p_change->p_rtps_writerproxy, base_seq);
								}


							}
						}

					}else{
					

						if ((p_change->p_rtps_writerproxy->max_seq_num.high == 0 && p_change->p_rtps_writerproxy->max_seq_num.low == 0) == false)
						{
							//insert missing
							int32_t i_diff = sequnce_number_difference2(p_change->sequence_number, p_change->p_rtps_writerproxy->max_seq_num);
							SequenceNumber_t base_seq = p_change->p_rtps_writerproxy->max_seq_num;


							for (j = 0; j < i_diff - 1; j++) {
								/*SequenceNumber_t *p_missig = malloc(sizeof(SequenceNumber_t));
								*p_missig = base_seq = sequnce_number_inc(base_seq);


								INSERT_ELEM(p_change->p_rtps_writerproxy->pp_missing_seq_num, p_change->p_rtps_writerproxy->i_missing_seq_num
									, p_change->p_rtps_writerproxy->i_missing_seq_num, p_missig);*/


								base_seq = sequnce_number_inc(base_seq);
								insert_missing_num(p_change->p_rtps_writerproxy, base_seq);
							}
						}
					
					}
				}
			}
		}
#endif

		if(p_change->p_rtps_writerproxy){


			if (p_change->p_rtps_writerproxy->received_first_packet == false && p_change->p_rtps_writerproxy->received_first_seq_num.high == 0 && p_change->p_rtps_writerproxy->received_first_seq_num.low == 0)
			{
				p_change->p_rtps_writerproxy->received_first_seq_num = p_change->sequence_number;
			}

			p_change->p_rtps_writerproxy->received_first_packet = true;

			if(sequnce_number_left_bigger(p_change->sequence_number, p_change->p_rtps_writerproxy->max_seq_num))
			{
				if(sequnce_number_left_bigger(p_change->sequence_number, p_change->p_rtps_writerproxy->max_seq_num))
				{
					
					p_change->p_rtps_writerproxy->max_seq_num = p_change->sequence_number;
				}
				
			}

			if (sequnce_number_left_bigger(p_change->p_rtps_writerproxy->min_seq_num, p_change->sequence_number))
			{
				if (sequnce_number_left_bigger(p_change->p_rtps_writerproxy->min_seq_num, p_change->sequence_number))
				{

					p_change->p_rtps_writerproxy->min_seq_num = p_change->sequence_number;
				}

			}
			
		}

	
		/*if(p_historycache->history_max_length == 100){

			
			if(p_historycache->i_changes <1)
				rtps_historycache_add_re_ordering( p_historycache, p_change);
		}else{*/
			rtps_historycache_add_re_ordering( p_historycache, p_change);
		//}

		p_tmpchange = p_change;
	}else{
		rtps_cachechange_ref(p_change, true, false);
		rtps_cachechange_destory(p_change);
	}

	/////////
	//
	/////////
	
	if (p_historycache->p_rtps_writer && p_tmpchange)
	{
//		p_tmpchange->is_relevant = true;
		if (p_historycache->p_rtps_writer->push_mode)
		{
//			p_tmpchange->status = UNSENT;
		}else
		{
//			p_tmpchange->status = UNACKNOWLEDGED;
		}
	}

	HISTORYCACHE_UNLOCK(p_historycache);
	p_tmpchange->is_same = is_same;
	return p_tmpchange;

}

void rtps_historycache_remove_change(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change)
{

//	int i;
//	rtps_cachechange_t *p_change_atom = NULL;
//
//	if(p_historycache == NULL) return;
//	HISTORYCACHE_LOCK(p_historycache);
//
//	for(i=0; i < p_historycache->i_changes; i++)
//	{
//		if((memcmp(&p_historycache->pp_changes[i]->writer_guid, &p_change->writer_guid, sizeof(GUID_t)) == 0)
//			&& (memcmp(&p_historycache->pp_changes[i]->sequence_number, &p_change->sequence_number, sizeof(SequenceNumber_t)) == 0))
//		{
////			p_change->is_relevant = false;
//		}
//	}
//
//	
//
//	HISTORYCACHE_UNLOCK(p_historycache);
}


/************************************************************************/
/* 사용되지 않는 함수. 사용될 것 같지도 않는 함수.                      */
/************************************************************************/
SequenceNumber_t rtps_historycache_set_seq_num_min(rtps_historycache_t *p_historycache)
{
	SequenceNumber_t s = {0,0};

	return s;
}


#ifndef MAXUINT32
	#define MAXUINT32   ((uint32_t)~((uint32_t)0))
	#define MAXINT32    ((int32_t)(MAXUINT32 >> 1))
	#define MININT32    ((int32_t)~MAXINT32)
#endif

SequenceNumber_t rtps_historycache_get_seq_num_min(rtps_historycache_t *p_historycache)
{
	SequenceNumber_t s = {MAXINT32, MAXUINT32};
	
	rtps_cachechange_t *p_change_atom = NULL;

	if(p_historycache->i_linked_size == 0)
	{
		s.high = 0;
		s.low = 1;

		return s;
	}

	HISTORYCACHE_LOCK(p_historycache);

	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

	while(p_change_atom)
	{
		if(sequnce_number_left_bigger(s, p_change_atom->sequence_number))
		{
			s = p_change_atom->sequence_number;
		}

		
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;	
	}
	HISTORYCACHE_UNLOCK(p_historycache);
	return s;
}


/************************************************************************/
/* 사용되지 않는 함수. 사용될 것 같지도 않는 함수.                      */
/************************************************************************/
SequenceNumber_t rtps_historycache_set_seq_num_max(rtps_historycache_t *p_historycache)
{
	SequenceNumber_t s = {0,0};

	return s;
}


SequenceNumber_t rtps_historycache_get_seq_num_max(rtps_historycache_t *p_historycache)
{
	SequenceNumber_t s = {0,0};
	rtps_cachechange_t *p_change_atom = NULL;


	//HISTORYCACHE_LOCK(p_historycache);

	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_last;


	if(p_change_atom)
	{
		if(sequnce_number_left_bigger(p_change_atom->sequence_number, s))
		{
			s = p_change_atom->sequence_number;
		}

		
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
	}
	//HISTORYCACHE_UNLOCK(p_historycache);
	return s;
}


void HISTORYCACHE_LOCK(rtps_historycache_t *p_historycache)
{
#ifndef _MSC_VER
	//pthread_t threadA = pthread_self();
	/*if( p_historycache->object_lock.mutex.__data.__owner > 0)
	{
		printf("2 (%p)HISTORYCACHE_LOCK :(%p) count(%d), owner(%x)\r\n", threadA, &p_historycache->object_lock.mutex, p_historycache->object_lock.mutex, p_historycache->object_lock.mutex.__data.__owner);

		//assert(false);
	}*/
	//printf("1 (%p)HISTORYCACHE_LOCK :(%p) count(%d), owner(%x)\r\n", threadA, &p_historycache->object_lock.mutex, p_historycache->object_lock.mutex, p_historycache->object_lock.mutex.__data.__owner);
#endif
	mutex_lock(&p_historycache->object_lock);
#ifndef _MSC_VER
	//printf("2 (%p)HISTORYCACHE_LOCK :(%p) count(%d), owner(%d)\r\n", threadA, &p_historycache->object_lock.mutex, p_historycache->object_lock.mutex, p_historycache->object_lock.mutex.__data.__owner);
#endif

}


void HISTORYCACHE_UNLOCK(rtps_historycache_t *p_historycache)
{
#ifndef _MSC_VER
	//pthread_t threadA = pthread_self();
	//printf("3 (%p)HISTORYCACHE_UNLOCK :(%p) count(%d), owner(%x)\r\n", threadA, &p_historycache->object_lock.mutex, p_historycache->object_lock.mutex, p_historycache->object_lock.mutex.__data.__owner);
#endif
	mutex_unlock(&p_historycache->object_lock);
#ifndef _MSC_VER
	//printf("4 (%p)HISTORYCACHE_UNLOCK :(%p) count(%d), owner(%x)\r\n", threadA, &p_historycache->object_lock.mutex, p_historycache->object_lock.mutex, p_historycache->object_lock.mutex.__data.__owner);
#endif
}


void READER_LOCK(rtps_reader_t *p_reader)
{
#ifdef _MSC_VER
	mutex_lock(&p_reader->object_lock);
#endif
}


void READER_UNLOCK(rtps_reader_t *p_reader)
{
#ifdef _MSC_VER
	mutex_unlock(&p_reader->object_lock);
#endif
}


void  trace_historycache(module_object_t* p_this, int mode, rtps_historycache_t* p_historycache)
{
//	int i;
	rtps_cachechange_t *p_change_atom = NULL;

	trace_msg(p_this, mode, "\t\t\t\t\t\tHISTORY(%5u)", p_historycache->i_linked_size);

	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

	while(p_change_atom)
	{
		if (p_change_atom->p_data_value)
		{
			if (p_change_atom->p_org_message && p_change_atom->p_org_message->sampleInfo.sample_state == READ_SAMPLE_STATE)
			{
				trace_msg(p_this, mode, "\t\t\t\t\t\t[%5u]-READ", p_change_atom->sequence_number.low);
			}
			else
			{
				trace_msg(p_this, mode, "\t\t\t\t\t\t[%5u]", p_change_atom->sequence_number.low);
			}
		}
		else
		{
			trace_msg(p_this, mode, "\t\t\t\t\t\t[%5u]-GAP", p_change_atom->sequence_number.low);
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
}
