/*
	RTSP Writer class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

#include "rtps.h"

rtps_writerproxy_t* rtps_writerproxy_new( rtps_reader_t* p_rtps_reader, GUID_t guid, Locator_t* p_metatraffic_unicast_locator, Locator_t* p_metatraffic_multicast_locator )
{
	rtps_writerproxy_t *p_rtps_writerproxy = malloc(sizeof(rtps_writerproxy_t));
	//p_rtps_writerproxy->changes_from_writers.p_head_first = NULL;
	//p_rtps_writerproxy->changes_from_writers.p_head_last = NULL;
	//p_rtps_writerproxy->changes_from_writers.i_linked_size = 0;


	memset(p_rtps_writerproxy, 0, sizeof(rtps_writerproxy_t));

	p_rtps_writerproxy->pp_multicast_locator_list = NULL;
	p_rtps_writerproxy->pp_unicast_locator_list = NULL;
	p_rtps_writerproxy->i_unicast_locator_list = 0;
	p_rtps_writerproxy->i_multicast_locator_list = 0;

	// rtps writer의 모든 writer_cache에 있는 cachechange를 가져 온다.
	p_rtps_writerproxy->pp_changes_from_writers = NULL;
	p_rtps_writerproxy->i_changes_from_writer = 0;

	
	INSERT_ELEM( p_rtps_writerproxy->pp_unicast_locator_list, p_rtps_writerproxy->i_unicast_locator_list,
							p_rtps_writerproxy->i_unicast_locator_list, p_metatraffic_unicast_locator);

	INSERT_ELEM( p_rtps_writerproxy->pp_multicast_locator_list, p_rtps_writerproxy->i_multicast_locator_list,
						p_rtps_writerproxy->i_multicast_locator_list, p_metatraffic_multicast_locator);

	memcpy(&p_rtps_writerproxy->remote_writer_guid, &guid,sizeof(GUID_t));


	mutex_init(&p_rtps_writerproxy->object_lock);
	p_rtps_writerproxy->i_for_reader_state = INITIAL_STATE;
	p_rtps_writerproxy->ackcheced_time = currenTime();
	p_rtps_writerproxy->i_ack_count = 0;
	p_rtps_writerproxy->i_nack_count = 0;
	p_rtps_writerproxy->p_rtps_reader = p_rtps_reader;

	//added by kyy
	p_rtps_writerproxy->start = 0;
	p_rtps_writerproxy->is_first_ack = true;
	//
	p_rtps_writerproxy->ownership_strength = 0;
	p_rtps_writerproxy->i_ref_count = 0;
	p_rtps_writerproxy->is_ignore = false;

	p_rtps_writerproxy->pp_missing_seq_num = NULL;
	p_rtps_writerproxy->i_missing_seq_num = 0;
	p_rtps_writerproxy->received_first_packet = false;

	p_rtps_writerproxy->received_first_seq_num.high = 0;
	p_rtps_writerproxy->received_first_seq_num.low = 0;

	p_rtps_writerproxy->max_seq_num.high = 0;
	p_rtps_writerproxy->max_seq_num.low = 0;
	p_rtps_writerproxy->min_seq_num.high = INT_MAX;
	p_rtps_writerproxy->min_seq_num.low = UINT_MAX;

	p_rtps_writerproxy->is_destroy = false;

	return p_rtps_writerproxy;
}


void rtps_writerproxy_destroy(rtps_writerproxy_t *p_rtps_writerproxy)
{
	Locator_t* remoteLocator;

	p_rtps_writerproxy->is_destroy = true;

	return;

	if(p_rtps_writerproxy == NULL) return;
/*added by edy100[120313]
추후 업데이트 예정 : 현재 writer proxy 삭제 안됨
*/
	
	remoteLocator = get_default_remote_participant_locator(p_rtps_writerproxy->remote_writer_guid);

	if(p_rtps_writerproxy->i_ref_count)
	{
		trace_msg(NULL, TRACE_LOG,"p_rtps_writerproxy->i_ref_count : %d", p_rtps_writerproxy->i_ref_count);
		return;
	}
	
	while(p_rtps_writerproxy->i_unicast_locator_list)
	{
		FREE(p_rtps_writerproxy->pp_unicast_locator_list[0]);
		REMOVE_ELEM( p_rtps_writerproxy->pp_unicast_locator_list, p_rtps_writerproxy->i_unicast_locator_list, 0);
	}

	FREE(p_rtps_writerproxy->pp_unicast_locator_list);

	while(p_rtps_writerproxy->i_multicast_locator_list)
	{
		FREE(p_rtps_writerproxy->pp_multicast_locator_list[0]);
		REMOVE_ELEM( p_rtps_writerproxy->pp_multicast_locator_list, p_rtps_writerproxy->i_multicast_locator_list, 0);
	}

	FREE(p_rtps_writerproxy->pp_multicast_locator_list);

	mutex_destroy(&p_rtps_writerproxy->object_lock);

	FREE(p_rtps_writerproxy);
	

	//trace_msg(NULL, TRACE_LOG, "writerproxy destroyed. ([%d.%d.%d.%d])", remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
}


void rtps_writerproxy_destroy2(rtps_writerproxy_t *p_rtps_writerproxy)
{
	Locator_t* remoteLocator;

	
	remoteLocator = get_default_remote_participant_locator(p_rtps_writerproxy->remote_writer_guid);

	if (p_rtps_writerproxy->i_ref_count)
	{
		trace_msg(NULL, TRACE_LOG, "p_rtps_writerproxy->i_ref_count : %d", p_rtps_writerproxy->i_ref_count);
		return;
	}

	while (p_rtps_writerproxy->i_unicast_locator_list)
	{
		FREE(p_rtps_writerproxy->pp_unicast_locator_list[0]);
		REMOVE_ELEM(p_rtps_writerproxy->pp_unicast_locator_list, p_rtps_writerproxy->i_unicast_locator_list, 0);
	}

	FREE(p_rtps_writerproxy->pp_unicast_locator_list);

	while (p_rtps_writerproxy->i_multicast_locator_list)
	{
		FREE(p_rtps_writerproxy->pp_multicast_locator_list[0]);
		REMOVE_ELEM(p_rtps_writerproxy->pp_multicast_locator_list, p_rtps_writerproxy->i_multicast_locator_list, 0);
	}

	FREE(p_rtps_writerproxy->pp_multicast_locator_list);

	mutex_destroy(&p_rtps_writerproxy->object_lock);

	FREE(p_rtps_writerproxy);
}


//[KKI] <p_rtps_writerproxy> 의 i_ref_count 를 하나 증가시킨다.
void rtps_writerproxy_ref(rtps_writerproxy_t *p_rtps_writerproxy)
{
	p_rtps_writerproxy->i_ref_count++;

	
	if(p_rtps_writerproxy->i_ref_count >= 2)
	{
		p_rtps_writerproxy->i_ref_count = p_rtps_writerproxy->i_ref_count;
	}
}


//[KKI] <p_rtps_writerproxy> 의 i_ref_count 를 하나 감소시킨다.
void rtps_writerproxy_unref(rtps_writerproxy_t *p_rtps_writerproxy)
{
	p_rtps_writerproxy->i_ref_count--;

	if(p_rtps_writerproxy->i_ref_count && p_rtps_writerproxy->is_destroy)
	{
		rtps_writerproxy_destroy(p_rtps_writerproxy);
	}
}


//[KKI] <p_change> 안에 <a_guid> 인 cache_for_guid 가 있으면 그것의 status 를 리턴하고,
//[KKI] 없으면 cache_for_guid 를 새로 만들고 그것의 status 를 리턴한다.
int *Is_exist_in_cachechange(GUID_t a_guid, rtps_cachechange_t *p_change, ReliabilityKind_t level)
{

	cache_for_guid *p_cache_for_guid = NULL;
	cache_for_guid	*p_cache_atom = NULL;

	p_cache_atom = (cache_for_guid *)p_change->cache_for_guids.p_head_first;

	while(p_cache_atom)
	{
		if(memcmp(&a_guid, &p_cache_atom->guid, sizeof(GUID_t)) == 0)
		{
			return &p_cache_atom->status;
		}
	}

	p_cache_for_guid = (cache_for_guid *)malloc(sizeof(cache_for_guid));
	memset(p_cache_for_guid, 0, sizeof(cache_for_guid));
	memcpy(&p_cache_for_guid->guid, &a_guid, sizeof(GUID_t));
	//RECEIVED
	//p_cache_for_guid->status = UNKNOWN;
	//이미 받았기 때문에...
	p_cache_for_guid->status = RECEIVED;
	p_cache_for_guid->is_relevant = false;

	insert_linked_list(&p_change->cache_for_guids, (linked_list_atom_t *)p_cache_for_guid);



	return &p_cache_for_guid->status;
}

/************************************************************************/
/* rtps_reader의 history_cache안의 cache_change중에서                   */
/* 자신(writer_proxy)와 관련있는 cache_change만                         */
/* changes_from_writer에 모으는 함수 (김경일)                           */
/************************************************************************/
void update_writerproxy_from_rtps_reader( rtps_writerproxy_t** p_rtps_writerproxy )
{

	//int	*i_p;
	rtps_cachechange_t *p_change_atom = NULL;

	/*if ((*p_rtps_writerproxy)->is_destroy)
	{
		rtps_writerproxy_destroy2(*p_rtps_writerproxy);
		*p_rtps_writerproxy = NULL;
		return;
	}*/

	rtps_writerproxy_ref(*p_rtps_writerproxy);

	FREE((*p_rtps_writerproxy)->pp_changes_from_writers);
	(*p_rtps_writerproxy)->i_changes_from_writer = 0;
	p_change_atom = (rtps_cachechange_t *)(*p_rtps_writerproxy)->p_rtps_reader->p_reader_cache->p_head_first;

	while(p_change_atom)
	{
		if (memcmp(&(*p_rtps_writerproxy)->remote_writer_guid, &p_change_atom->writer_guid, sizeof(GUID_t)) == 0)
		{
			//[KKI] pp_changes[i] 가 remoteWriterGuid 인 cache_for_guid 를 가졌으면 그대로, 없으면 새 cache_for_guid 를 생성하여 추가한다.
			//i_p = Is_exist_in_cachechange(p_rtps_writerproxy->remote_writer_guid, p_change_atom, -1);
//			p_rtps_writerproxy->p_rtps_reader->reader_cache->pp_changes[i]->status = i_p;

			INSERT_ELEM((*p_rtps_writerproxy)->pp_changes_from_writers, (*p_rtps_writerproxy)->i_changes_from_writer,
				(*p_rtps_writerproxy)->i_changes_from_writer, p_change_atom);
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	//p_rtps_writerproxy->pp_changes_from_writers = p_rtps_writerproxy->p_rtps_reader->reader_cache->pp_changes;
	//p_rtps_writerproxy->i_changes_from_writer = p_rtps_writerproxy->p_rtps_reader->reader_cache->i_changes;

	/*p_rtps_writerproxy->changes_from_writers.p_head_first = p_rtps_writerproxy->p_rtps_reader->p_reader_cache->p_head_first;
	p_rtps_writerproxy->changes_from_writers.p_head_last = p_rtps_writerproxy->p_rtps_reader->p_reader_cache->p_head_last;
	p_rtps_writerproxy->changes_from_writers.i_linked_size = p_rtps_writerproxy->p_rtps_reader->p_reader_cache->i_linked_size;*/


	/*p_rtps_writerproxy->changes_from_writers.p_head_first = NULL;
	p_rtps_writerproxy->changes_from_writers.p_head_last = NULL;
	p_rtps_writerproxy->changes_from_writers.i_linked_size = 0;

	while(p_change_atom)
	{
		if (memcmp(&p_rtps_writerproxy->remote_writer_guid, &p_change_atom->writer_guid, sizeof(GUID_t)) == 0)
		{

			insert_linked_list(&p_rtps_writerproxy->changes_from_writers, (linked_list_atom_t *)p_change_atom);
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
*/


	rtps_writerproxy_unref((*p_rtps_writerproxy));
}


bool find_missing_seq_num(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t num)
{
	int i;

	for(i=0; i < p_rtps_writerproxy->i_missing_seq_num; i++)
	{
		if(sequnce_number_equal(num, *p_rtps_writerproxy->pp_missing_seq_num[i]))
		{
			// find missing
			return false;
		}
	}


	return true;
}


static bool is_Exist_SequnceNum(rtps_cachechange_t** pp_changes_from_writer, rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t num)
{

	/*rtps_cachechange_t* p_rtps_cachechange;
	rtps_cachechange_t *p_change_atom = NULL;
	cache_for_guid	*p_cache_atom = NULL;

	update_writerproxy_from_rtps_reader(p_rtps_writerproxy);

	p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	while(p_change_atom)
	{
		p_rtps_cachechange = p_change_atom;

		p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

		while(p_cache_atom)
		{
			if (is_same_guid(&p_cache_atom->guid, &p_rtps_writerproxy->remote_writer_guid))
			{
				
				if(sequnce_number_equal(p_rtps_cachechange->sequence_number, num))
				{
					return true;
				}

				break;
			}
			p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
		}

		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}*/

	int i;
	rtps_cachechange_t* p_rtps_cachechange;
	cache_for_guid	*p_cache_atom = NULL;
	
	
	

	for (i = 0; i < p_rtps_writerproxy->i_changes_from_writer; i++)
	{
		p_rtps_cachechange = pp_changes_from_writer[i];

		p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

		while(p_cache_atom)
		{
			if (is_same_guid(&p_cache_atom->guid, &p_rtps_writerproxy->remote_writer_guid))
			{
				
				if(sequnce_number_equal(p_rtps_cachechange->sequence_number, num))
				{
					return true;
				}

				break;
			}
			p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
		}
	}

	return find_missing_seq_num(p_rtps_writerproxy, num);
}

/************************************************************************/
/* change_from_writer 안에 있는, status가 RECEIVED 이거나 LOST 인       */
/* cachechange 중에 SequenceNumber가 가장 큰 것을 찾는 함수이다.        */
/* 기존 함수가 오류를 발생시키는 것 같아서, 새로 작성하였다. (김경일)   */
/************************************************************************/
SequenceNumber_t rtps_writerproxy_available_changes_max(rtps_writerproxy_t *p_rtps_writerproxy)
{

	int i;
	rtps_cachechange_t* p_rtps_cachechange;
	SequenceNumber_t temp_sequence_number = {0, 0};
	cache_for_guid	*p_cache_atom = NULL;
	rtps_cachechange_t** pp_changes_from_writer;

	rtps_cachechange_t *p_change_atom = NULL;

	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL) return sequnce_number_dec(p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);

	if (p_rtps_writerproxy->i_changes_from_writer == 0)
	{
		return sequnce_number_dec(p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);
	}

	pp_changes_from_writer = p_rtps_writerproxy->pp_changes_from_writers;

	
	for (i = 0; i < p_rtps_writerproxy->i_changes_from_writer; i++)
	{
		p_rtps_cachechange = pp_changes_from_writer[i];

		p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

		while(p_cache_atom)
		{
			if (memcmp(&p_cache_atom->guid, &p_rtps_writerproxy->remote_writer_guid, sizeof(GUID_t)) == 0)
			{
				if (p_cache_atom->status == RECEIVED || p_cache_atom->status == LOST)
				{
					if (sequnce_number_left_bigger(p_rtps_cachechange->sequence_number, temp_sequence_number))
					{
						temp_sequence_number = p_rtps_cachechange->sequence_number;
					}
				}
			}

			p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
		}

	}

	return temp_sequence_number;
}

SequenceNumber_t rtps_writerproxy_available_changes_max_for_acknack(rtps_writerproxy_t *p_rtps_writerproxy)
{
	//int32_t i;
	//uint16_t j;
	
	//rtps_cachechange_t* p_rtps_cachechange;
	SequenceNumber_t temp_sequence_number = {0, 0};
	SequenceNumber_t min_sn;
	SequenceNumber_t sn;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_cachechange_t** pp_changes_from_writer;

	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_last;


	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		return sequnce_number_dec(p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);
	}

	if (p_rtps_writerproxy->i_changes_from_writer == 0)
	{
		return sequnce_number_dec(p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);
	}

	pp_changes_from_writer = p_rtps_writerproxy->pp_changes_from_writers;

	if(p_rtps_writerproxy->i_changes_from_writer) p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[p_rtps_writerproxy->i_changes_from_writer-1];
	
	for(sn = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value; sequnce_number_left_bigger_and_equal(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, sn) ; sn = sequnce_number_inc(sn))
	{
		if(!is_Exist_SequnceNum(pp_changes_from_writer, p_rtps_writerproxy, sn))
		{
			trace_msg(NULL, TRACE_WARM, "is_NOT_Exist_SequnceNum : %d", sn.low);
			return sequnce_number_dec(sn);
		}
	}

	/*if(p_change_atom)
	{
		p_rtps_writerproxy->max_seq_num = p_change_atom->sequence_number;
	}*/

	min_sn = p_rtps_writerproxy->a_remote_heartbeat.last_sn.value;




	if(p_rtps_writerproxy->i_missing_seq_num)
	{
		return *p_rtps_writerproxy->pp_missing_seq_num[0];
	}else
	{
		return sequnce_number_dec(p_rtps_writerproxy->max_seq_num);
	}

	/*for (i = 0; i < p_rtps_writerproxy->i_changes_from_writer; i++)
	{
		p_rtps_cachechange = pp_changes_from_writer[i];

		for (j = 0; j < p_rtps_cachechange->i_cache_for_guids; j++)
		{
			if (memcmp(&p_rtps_cachechange->pp_cache_for_guids[j]->guid, &p_rtps_writerproxy->remote_writer_guid, sizeof(GUID_t)) == 0)
			{
				if (p_rtps_cachechange->pp_cache_for_guids[j]->status == RECEIVED || p_rtps_cachechange->pp_cache_for_guids[j]->status == LOST)
				{
					if (sequnce_number_left_bigger(p_rtps_cachechange->sequence_number, temp_sequence_number))
					{
						temp_sequence_number = p_rtps_cachechange->sequence_number;
					}
				}

				min_sn = sequnce_number_left_bigger(min_sn, p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number) 
								? p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number : min_sn;
				break;
			}
		}
	}*/


	//return temp_sequence_number;
}

/*
{
	SequenceNumber_t sn, min_sn;
	int i_size = p_rtps_writerproxy->i_changes_from_writer;
	int i, j;
	int i_checksize;
	bool b_findsequence = false;

	sn.high = 0;
	sn.low = 0;

	
	
	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->reader_cache);

	update_writerproxy_from_rtps_reader(p_rtps_writerproxy);

	i_size = p_rtps_writerproxy->i_changes_from_writer;
	i_checksize = sequnce_number_difference(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value)+1;

	if(i_size){
		min_sn = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;

		for( i=1; i < i_size; i++)
		{
			min_sn = sequnce_number_left_bigger(min_sn, p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number) 
				? p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number : min_sn;
		}

		for( j=0; j < i_checksize; j++){
			b_findsequence = false;
			for( i=0; i < i_size; i++)
			{
				if(memcmp(&min_sn, &p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number, sizeof(SequenceNumber_t)) == 0)
				{
					if(*p_rtps_writerproxy->pp_changes_from_writers[i]->status == RECEIVED || *p_rtps_writerproxy->pp_changes_from_writers[i]->status == LOST)
					{
						if(p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number.high > sn.high){
							sn = p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number;
						}else if(p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number.high == sn.high){
							if(p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number.low > sn.low){
								sn = p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number;
							}
						}
					}else{
						HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->reader_cache);
						return sn;
					}
					b_findsequence = true;
					break;
				}
			}

			if(!b_findsequence)
			{
				if(sequnce_number_left_bigger(p_rtps_writerproxy->a_remote_heartbeat.first_sn.value, sn))
				{
					sn = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
				}
				HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->reader_cache);
				return sn;
			}

			min_sn = sequnce_number_inc(min_sn);
		}
	}
	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->reader_cache);


	return sn;
}
*/

void rtps_writerproxy_irrelevant_change_set(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num)
{
	
	bool is_found = false;
	cache_for_guid		*cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL; 
	int i_size;
	int i;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;
	

	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);

	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return;
	}


	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;

	for (i = 0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_change_atom);

		if ((p_change_atom->sequence_number.high == a_seq_num.high)
			&& (p_change_atom->sequence_number.low == a_seq_num.low))
		{
				cache_for_guids->status = RECEIVED;
				cache_for_guids->is_relevant = false;
				is_found = true;
		}
	}

	if (!is_found)
	{
		//알고리즘상 존재하지 않을 것이다.
		//뭐 존재 할 수도 있구..존재하지 않기 때문에, 새롭게 만들어서 history_cache에 만들어 넣자.
		rtps_cachechange_t	*p_rtps_cachechange = rtps_cachechange_new(ALIVE, p_rtps_writerproxy->remote_writer_guid, a_seq_num, NULL, 0);

		p_rtps_cachechange->p_rtps_writerproxy = p_rtps_writerproxy;

		p_rtps_cachechange = rtps_historycache_add_change(p_rtps_writerproxy->p_rtps_reader->p_reader_cache, p_rtps_cachechange);
		update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);


		if (p_rtps_writerproxy == NULL)
		{
			HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
			return;
		}

		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_rtps_cachechange);

		//if (p_rtps_cachechange->status == NULL)
		//{
		//	//p_rtps_cachechange->status = (ChangeForReaderStatusKind *)malloc(sizeof(ChangeForReaderStatusKind));
		//}
		

		cache_for_guids->status = RECEIVED;
		cache_for_guids->is_relevant = false;
	}

	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
}


//[KKI] <p_rtps_writerproxy> 의 changes_from_writer 중에 UNKNOWN 또는 MISSING 상태이면서 <first_available_seq_num> 보다 작거나 같은 것을 찾아서 LOST 상태로 바꾼다.
// (RTPS p.108)
// FOREACH change in this.changes_from_writer
//        SUCH-THAT ( change.status == UNKNOWN OR change.status == MISSING
//            AND seq_num < first_available_seq_num ) DO {
//     change.status := LOST;
// }
void rtps_writerproxy_lost_changes_update(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t first_available_seq_num)
{
	cache_for_guid		*cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size = p_rtps_writerproxy->i_changes_from_writer;
	int i;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;
	
	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return;
	}

	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;
	for (i = 0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];
		//[KKI] <p_rtps_writerproxy->pp_changes_from_writers[i]> 의 cache_for_guids 중에 <p_rtps_writerproxy->remoteWriterGuid> 와 같은 것을 갖는 cache_for_guid 를 찾는다.
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_change_atom);

		if (cache_for_guids->status == UNKNOWN || cache_for_guids->status == MISSING)
		{
			//[KKI] 만일 p_rtps_writerproxy->changes_from_writer[i]->sequenceNumber 가 first_available_seq_num 보다 작거나 같다면
			if (p_change_atom->sequence_number.high < first_available_seq_num.high)
			{
				cache_for_guids->status = LOST;
				//status by jun
				change_sample_lost_status(p_rtps_writerproxy->p_rtps_reader->p_datareader);
			}else if (p_change_atom->sequence_number.high == first_available_seq_num.high)
			{
				if (p_change_atom->sequence_number.low <= first_available_seq_num.low)
				{
					cache_for_guids->status = LOST;
					//status by jun
					change_sample_lost_status(p_rtps_writerproxy->p_rtps_reader->p_datareader);
				}
			}
			//TODO[KKI] 위 if 문을 아래와 같이 바꾸는 것을 추천합니다.
			//if (sequnce_number_left_bigger_and_equal(first_available_seq_num, p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number))
			//{
			//	cache_for_guids->status = LOST;
			//}
		}
		//p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
}

linked_list_head_t rtps_writerproxy_missing_changes(rtps_writerproxy_t *p_rtps_writerproxy, int *p_size)
{
	int i_missing = 0;
	int i_count = 0;
	//rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	cache_for_guid		*cache_for_guids;
	linked_list_head_t linked_list_head;
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size;
	int i;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;

	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	
	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return linked_list_head;
	}

	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;

	for( i=0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_change_atom);


		if(cache_for_guids->status == MISSING)
		{
			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)cache_for_guids);
			i_count++;
		}

		//p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	
	*p_size = i_count;

	return linked_list_head;
}

void rtps_writerproxy_missing_changes_update(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t last_available_seq_num)
{
	rtps_cachechange_t *p_change_atom = NULL;
	cache_for_guid		*cache_for_guids;
	int i_size = p_rtps_writerproxy->i_changes_from_writer;
	int i;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;
	//rtps_writerproxy_ref(p_rtps_writerproxy);
	
	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);

	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return;
	}
	
	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;
	for( i=0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_change_atom);

		if(cache_for_guids->status == UNKNOWN)
		{
			if(p_change_atom->sequence_number.high < last_available_seq_num.high){
				cache_for_guids->status = MISSING;
			}else if(p_change_atom->sequence_number.high == last_available_seq_num.high){
				if(p_change_atom->sequence_number.low <= last_available_seq_num.low){
					cache_for_guids->status = MISSING;
				}
			}
		}

		//p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	p_rtps_writerproxy->last_available_seq_num = last_available_seq_num;
	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);

	//rtps_writerproxy_unref(p_rtps_writerproxy);
}


//[KKI] <p_rtps_writerproxy> 의 changes_from_writer 중에 <a_seq_num> 인 것을 찾아서 RECEIVED 상태로 바꾼다.
void rtps_writerproxy_received_change_set(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num)
{
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size = p_rtps_writerproxy->i_changes_from_writer;
	int i;
	cache_for_guid* cache_for_guids;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;
	
	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return;
	}
	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;
	for (i = 0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];
		//[KKI] <p_rtps_writerproxy->pp_changes_from_writers[i]> 의 cache_for_guids 중에 <p_rtps_writerproxy->remoteWriterGuid> 와 같은 것을 갖는 cache_for_guid 를 찾는다.
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_writerproxy->remote_writer_guid, p_change_atom);

		if ((p_change_atom->sequence_number.high == a_seq_num.high)
			&& (p_change_atom->sequence_number.low == a_seq_num.low))
		{
				cache_for_guids->status = RECEIVED;
				//TODO[KKI] seq_num 이 같은 change 는 1개밖에 없을 것 같다.더 이상 루프를 안 돌아도 될 것이다. 
		}
		//TODO[KKI] 위 if 문을 아래와 같이 바꾸는 것을 추천합니다.
		//if (sequnce_number_equal(p_rtps_writerproxy->pp_changes_from_writers[i]->sequence_number, a_seq_num))
		//{
		//	cache_for_guids->status = RECEIVED;
		//}

		//p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
}

// heartbeat가 와서 acknack을 보내야 되는데,
// 문서에서는 현재 Data가 들어온거에 대해서 있지만,
// 존재하지 않을 경우 보내야 된다. acknack으로 보내야 된다. 
// 그래서, 데이터를 보내달라구 요청해야 함.
bool rtps_writerproxy_is_exist_cache(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num)
{
	int i_size = p_rtps_writerproxy->i_changes_from_writer;
	int i;
	rtps_cachechange_t *p_change_atom = NULL;
	bool is_exist = false;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;
	
	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return false;
	}
	//p_change_atom = (rtps_cachechange_t *)p_rtps_writerproxy->changes_from_writers.p_head_first;

	i_size = p_rtps_writerproxy->i_changes_from_writer;
	for (i = 0; i < i_size; i++)
	{
		p_change_atom = p_rtps_writerproxy->pp_changes_from_writers[i];

		if ((p_change_atom->sequence_number.high == a_seq_num.high)
			&& (p_change_atom->sequence_number.low == a_seq_num.low))
		{
				is_exist = true;
		}

		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);

	return is_exist;
}

static int test333 = 4001;

SequenceNumberSet rtps_writerproxy_missing_changes_and_missing_sequencenumberset(rtps_writerproxy_t *p_rtps_writerproxy, int32_t *p_size, SequenceNumberSet missing_seq_num_set)
{
	int32_t i_missing = 0;
	int32_t i_size;
	//int32_t k;
	int32_t i_count = 0;
	SequenceNumber_t compare_sn;
	//cache_for_guid		*cache_for_guids;
	int i,j=0;

	rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	bool is_missing = false;
	bool has_missing = false;
	rtps_reader_t* p_rtps_reader = p_rtps_writerproxy->p_rtps_reader;


	



	if(sequnce_number_left_bigger(missing_seq_num_set.bitmap_base, p_rtps_writerproxy->a_remote_heartbeat.last_sn.value))
	{
		p_rtps_writerproxy->a_remote_heartbeat.last_sn.value = sequnce_number_dec(missing_seq_num_set.bitmap_base);
	}

	missing_seq_num_set.numbits = sequnce_number_difference(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, missing_seq_num_set.bitmap_base);

	//if(sequnce_number_left_bigger_and_equal(missing_seq_num_set.bitmap_base, p_rtps_writerproxy->a_remote_heartbeat.last_sn.value))
	////if(sequnce_number_left_bigger(missing_seq_num_set.bitmap_base, p_rtps_writerproxy->a_remote_heartbeat.last_sn.value))
	//{
	//	missing_seq_num_set.numbits++;
	//}


	if (p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.high == 0
		&& p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low == 0)
	{
		missing_seq_num_set.numbits++;
	}
	else {
		missing_seq_num_set.numbits++;
	}


	

	HISTORYCACHE_LOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	
	update_writerproxy_from_rtps_reader(&p_rtps_writerproxy);

	if (p_rtps_writerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
		return missing_seq_num_set;
	}
//	i_size = p_rtps_writerproxy->changes_from_writers.i_linked_size;
	

	i_size = p_rtps_writerproxy->i_changes_from_writer;

	//if (!p_rtps_writerproxy->p_rtps_reader->is_builtin)
	//{
	//	if (p_rtps_writerproxy->is_first_ack)
	//	{
	//		//p_rtps_writerproxy->is_first_ack = false;
	//		if (i_size)
	//		{
	//			missing_seq_num_set.bitmap_base = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
	//		}
	//	}
	//}

	//if(i_size == 0)
	//{
	//	missing_seq_num_set.bitmap[i_count/32] |= 1 << (31 - (i_count % 32));

	//	i_count++;

	//	//missing_seq_num_set.numbits++;

	//	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);

	//	return missing_seq_num_set;

	//}

	

	for(compare_sn = missing_seq_num_set.bitmap_base; sequnce_number_left_bigger_and_equal(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, compare_sn); compare_sn = sequnce_number_inc(compare_sn))
	{
		is_missing = false;

		if(p_rtps_writerproxy->max_seq_num.high == 0 && p_rtps_writerproxy->max_seq_num.low == 0)
		{
			is_missing = true;
			has_missing = true;
		}else if(sequnce_number_left_bigger(compare_sn, p_rtps_writerproxy->max_seq_num))
		{
			is_missing = true;
			has_missing = true;
		}else{

			for(i=0; i < p_rtps_writerproxy->i_missing_seq_num; i++)
			{
				if(sequnce_number_equal(compare_sn, *p_rtps_writerproxy->pp_missing_seq_num[i]))
				{
					// find missing
					is_missing = true;
					has_missing = true;
					j=i;
					i_missing++;
			
					break;
				}
			}
		}



		if (is_missing)
		{
			missing_seq_num_set.bitmap[i_count/32] |= 1 << (31 - (i_count % 32));
		}
		else
		{
			missing_seq_num_set.bitmap[i_count/32] |= 0 << (31 - (i_count % 32));
		}


		i_count++;

		if (i_count >= 256)
		{
			missing_seq_num_set.numbits = 256;
			break;
		}
	}
	

	HISTORYCACHE_UNLOCK(p_rtps_writerproxy->p_rtps_reader->p_reader_cache);
	
	*p_size = i_count;

	/*if(i_missing )
	{
		printf("writerproxy[%d:%d] : %p\r\n",i_missing, missing_seq_num_set.bitmap_base.low, p_rtps_writerproxy);
	}*/


	//////////////////

	{
		int index;
		index = qos_find_exclusive_writer(p_rtps_writerproxy->p_rtps_reader, p_rtps_writerproxy); //by kki (ownership)
		if (index > -1)
		{
			if (p_rtps_writerproxy->p_rtps_reader->pp_exclusive_writers[index]->b_is_owner != true)
			{
				if (p_rtps_writerproxy->p_rtps_reader->is_builtin)
					trace_msg(NULL, TRACE_LOG, "find_exclusive_writer : %d", index);

				// not owner exclusive writer
				missing_seq_num_set.bitmap_base = sequnce_number_inc(missing_seq_num_set.bitmap_base);
				missing_seq_num_set.numbits = 0;
				missing_seq_num_set.bitmap[0/32] |= 0 << (31 - (0 % 32));
			}
		}
	}
	//////////////////

	

	

	if (missing_seq_num_set.numbits == 0)
	{
		missing_seq_num_set.numbits = 0;
		missing_seq_num_set.bitmap[0/32] |= 0 << (31 - (0 % 32));
		//missing_seq_num_set.bitmap[0/32] |= 1 << (31 - (0 % 32));

		missing_seq_num_set.bitmap_base = sequnce_number_inc(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value);
	}

	if(has_missing == false && missing_seq_num_set.numbits)
	{
		/*int64_t *num = (int64_t *)&missing_seq_num_set.bitmap_base;
		*num += missing_seq_num_set.numbits;*/

		missing_seq_num_set.bitmap_base.low += missing_seq_num_set.numbits;
		missing_seq_num_set.numbits = 0;
	}

	return missing_seq_num_set;
}