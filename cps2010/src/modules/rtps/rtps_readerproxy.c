/*
	RTSP ReaderProxy class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"




int *Is_exist_in_cachechange_for_writer(GUID_t a_guid, rtps_cachechange_t *p_change, bool pushmode, bool *p_b_found)
{
	
	cache_for_guid *p_cache_for_guid = NULL;
	cache_for_guid	*p_cache_atom = NULL;

	p_cache_atom = (cache_for_guid *)p_change->cache_for_guids.p_head_first;

	while(p_cache_atom)
	{
		if(memcmp(&a_guid, &p_cache_atom->guid, sizeof(GUID_t)) == 0)
		{
			*p_b_found = true;
			return &p_cache_atom->status;
		}

		p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
	}

	*p_b_found = false;

	p_cache_for_guid = (cache_for_guid *)malloc(sizeof(cache_for_guid));
	

	memset(p_cache_for_guid, 0, sizeof(cache_for_guid));
	memcpy(&p_cache_for_guid->guid, &a_guid, sizeof(GUID_t));

	if(pushmode){
		p_cache_for_guid->status = UNSENT;
	}else{
		p_cache_for_guid->status = UNACKNOWLEDGED;
	}

	p_cache_for_guid->is_relevant = true;

	insert_linked_list(&p_change->cache_for_guids, (linked_list_atom_t *)p_cache_for_guid);

	/*INSERT_ELEM( p_change->pp_cache_for_guids, p_change->i_cache_for_guids,
							p_change->i_cache_for_guids, p_cache_for_guid);*/

	return &p_cache_for_guid->status;
}


void rtps_readerproxy_acked_changes_for_reader_set(const rtps_cachechange_t* const p_cachechange, const GUID_t* const p_remote_reader_guid);
bool is_dropable_cachechange(const rtps_cachechange_t* const p_cachechange);

/*
	This operation creates a new RTPS ReaderProxy. The newly-created reader proxy ‘this’ is initialized as follows:

	this.attributes := <as specified in the constructor>;
	this.changes_for_reader := RTPS::Writer.writer_cache.changes;

	FOR_EACH change in (this.changes_for_reader) DO {
		IF ( DDS_FILTER(this, change) THEN change.is_relevant := FALSE;
		ELSE change.is_relevant := TRUE;

		IF ( RTPS::Writer.pushMode == true) THEN change.status := UNSENT;
		ELSE change.status := UNACKNOWLEDGED;
	}
	
	The above logic indicates that the newly-created ReaderProxy initializes its set of ‘changes_for_reader’ to contain all the
	CacheChanges in the Writer’s HistoryCache.
	The change is marked as ‘irrelevant’ if the application of any of the DDS-DataReader filters indicates the change is not
	relevant to that particular reader. The DDS specification indicates that a DataReader may provide a time-based filter as
	well as a content-based filter. These filters should be applied in a manner consistent with the DDS specification to select
	any changes that are irrelevant to the DataReader.
	The status is set depending on the value of the RTPS Writer attribute ‘pushMode.’
*/
rtps_readerproxy_t* rtps_readerproxy_new(rtps_writer_t* p_rtps_writer, GUID_t guid, Locator_t* p_metatraffic_unicast_locator, Locator_t* p_metatraffic_multicast_locator, bool expects_inline_qos, DurabilityQosPolicyKind reader_durability_kind)
{
	rtps_readerproxy_t	*p_rtps_readerproxy = malloc(sizeof(rtps_readerproxy_t));

	memset(p_rtps_readerproxy, 0, sizeof(rtps_readerproxy_t));
	p_rtps_readerproxy->b_is_active = true;
	p_rtps_readerproxy->expects_inline_qos = expects_inline_qos;
	p_rtps_readerproxy->pp_multicast_locator_list = NULL;
	p_rtps_readerproxy->pp_unicast_locator_list = NULL;
	p_rtps_readerproxy->i_unicast_locator_list = 0;
	p_rtps_readerproxy->i_multicast_locator_list = 0;
	p_rtps_readerproxy->flags = 0;

	// rtps writer의 모든 writer_cache에 있는 cachechange를 가져 온다.
	//p_rtps_readerproxy->pp_changes_for_reader = p_rtps_writer->writer_cache->pp_changes;
	//p_rtps_readerproxy->i_changes_for_reader = p_rtps_writer->writer_cache->i_changes;

	p_rtps_readerproxy->changes_for_reader.p_head_first = NULL;
	p_rtps_readerproxy->changes_for_reader.p_head_last = NULL;
	p_rtps_readerproxy->changes_for_reader.i_linked_size = 0;
	//p_rtps_readerproxy->i_changes_for_reader = 0;

	//for(i = 0; i < p_rtps_readerproxy->i_changes_for_reader; i++)
	//{
	//	//p_rtps_readerproxy->pp_changes_for_reader[i]->isRelevant = true;
	//	if(p_rtps_writer->pushMode)
	//	{
	//		p_rtps_readerproxy->pp_changes_for_reader[i]->status = UNSENT;
	//	}else{
	//		p_rtps_readerproxy->pp_changes_for_reader[i]->status = UNACKNOWLEDGED;
	//	}
	//}
	//
	INSERT_ELEM( p_rtps_readerproxy->pp_unicast_locator_list, p_rtps_readerproxy->i_unicast_locator_list,
							p_rtps_readerproxy->i_unicast_locator_list, p_metatraffic_unicast_locator);

	INSERT_ELEM( p_rtps_readerproxy->pp_multicast_locator_list, p_rtps_readerproxy->i_multicast_locator_list,
						p_rtps_readerproxy->i_multicast_locator_list, p_metatraffic_multicast_locator);

	memcpy(&p_rtps_readerproxy->remote_reader_guid, &guid,sizeof(GUID_t));
	memcpy(&p_rtps_readerproxy->remote_endpoint_guid, &guid,sizeof(GUID_t));
	p_rtps_readerproxy->i_for_writer_state = ANNOUNCING_STATE;
	p_rtps_readerproxy->i_for_writer_fromReader_state = WAITING_STATE;


	mutex_init(&p_rtps_readerproxy->object_lock);

	p_rtps_readerproxy->p_rtps_writer = p_rtps_writer;
	p_rtps_readerproxy->remote_endpoint_guid.entity_id = ENTITYID_UNKNOWN;
	p_rtps_readerproxy->is_zero = false;

	if (p_rtps_writer->behavior_type == STATEFUL_TYPE && reader_durability_kind == VOLATILE_DURABILITY_QOS)
	{
		int i=0;
		cache_for_guid *cache_for_guids;
		rtps_cachechange_t *p_change_atom = NULL;

		HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

		update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
		p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;

		while(p_change_atom)
		{
			cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(guid, p_change_atom);
			cache_for_guids->is_relevant = false;
			cache_for_guids->status = UNDERWAY;
			p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		}

		HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	}



	p_rtps_readerproxy->is_ignore = false;
	p_rtps_readerproxy->is_heartbeatsend_and_wait_for_ack = false;
	p_rtps_readerproxy->not_ack_count = 0;
	p_rtps_readerproxy->reliablility_kind = BEST_EFFORT_RELIABILITY_QOS;
	p_rtps_readerproxy->durability_kind = reader_durability_kind;

	p_rtps_readerproxy->is_destroy = false;

	return p_rtps_readerproxy;
}

void rtps_readerproxy_destroy(rtps_readerproxy_t *p_rtps_readerproxy)
{

	p_rtps_readerproxy->is_destroy = true;

	if(p_rtps_readerproxy == NULL) return;

	while(p_rtps_readerproxy->i_unicast_locator_list)
	{
		FREE(p_rtps_readerproxy->pp_unicast_locator_list[0]);
		REMOVE_ELEM( p_rtps_readerproxy->pp_unicast_locator_list, p_rtps_readerproxy->i_unicast_locator_list, 0);
	}

	FREE(p_rtps_readerproxy->pp_unicast_locator_list);

	while(p_rtps_readerproxy->i_multicast_locator_list)
	{
		FREE(p_rtps_readerproxy->pp_multicast_locator_list[0]);
		REMOVE_ELEM( p_rtps_readerproxy->pp_multicast_locator_list, p_rtps_readerproxy->i_multicast_locator_list, 0);
	}

	FREE(p_rtps_readerproxy->pp_multicast_locator_list);

	mutex_destroy(&p_rtps_readerproxy->object_lock);
	FREE(p_rtps_readerproxy);

	trace_msg(NULL, TRACE_LOG, "readerproxy destroyed.");
}


void rtps_readerproxy_destroy2(rtps_readerproxy_t *p_rtps_readerproxy)
{

	while (p_rtps_readerproxy->i_unicast_locator_list)
	{
		FREE(p_rtps_readerproxy->pp_unicast_locator_list[0]);
		REMOVE_ELEM(p_rtps_readerproxy->pp_unicast_locator_list, p_rtps_readerproxy->i_unicast_locator_list, 0);
	}

	FREE(p_rtps_readerproxy->pp_unicast_locator_list);

	while (p_rtps_readerproxy->i_multicast_locator_list)
	{
		FREE(p_rtps_readerproxy->pp_multicast_locator_list[0]);
		REMOVE_ELEM(p_rtps_readerproxy->pp_multicast_locator_list, p_rtps_readerproxy->i_multicast_locator_list, 0);
	}

	FREE(p_rtps_readerproxy->pp_multicast_locator_list);

	mutex_destroy(&p_rtps_readerproxy->object_lock);
	FREE(p_rtps_readerproxy);

	trace_msg(NULL, TRACE_LOG, "readerproxy destroyed.");
}



void update_readerproxy_from_rtps_writer(rtps_readerproxy_t** p_rtps_readerproxy)
{

	int	*i_p;
	bool _found;
	rtps_cachechange_t *p_change_atom = NULL;

	/*if ((*p_rtps_readerproxy)->is_destroy)
	{
		rtps_readerproxy_destroy2(*p_rtps_readerproxy);
		*p_rtps_readerproxy = NULL;
		return;
	}*/
	/*
	int i;

	FREE(p_rtps_readerproxy->pp_changes_for_reader);
	p_rtps_readerproxy->i_changes_for_reader = 0;
	for(i = 0; i <p_rtps_readerproxy->p_rtps_writer->writer_cache->i_changes; i++){
		if(memcmp(&p_rtps_readerproxy->remote_reader_guid, &p_rtps_readerproxy->p_rtps_writer->writer_cache->pp_changes[i]->writerGuid, sizeof(GUID_t)) == 0)
		{
			INSERT_ELEM( p_rtps_readerproxy->pp_changes_for_reader,p_rtps_readerproxy->i_ch1anges_for_reader,
							p_rtps_readerproxy->i_changes_for_reader, p_rtps_readerproxy->p_rtps_writer->writer_cache->pp_changes[i]);
		}
	}*/

	/*FREE(p_rtps_readerproxy->pp_changes_for_reader);
	p_rtps_readerproxy->i_changes_for_reader = 0;*/

	p_change_atom = (rtps_cachechange_t *)(*p_rtps_readerproxy)->p_rtps_writer->p_writer_cache->p_head_last;


	if((*p_rtps_readerproxy)->p_rtps_writer->behavior_type != STATEFUL_TYPE
		|| ((rtps_statefulwriter_t *)(*p_rtps_readerproxy)->p_rtps_writer)->b_new_matched_readers == true)
	{
		while(p_change_atom)
		{
			i_p = Is_exist_in_cachechange_for_writer((*p_rtps_readerproxy)->remote_reader_guid, p_change_atom, (*p_rtps_readerproxy)->p_rtps_writer->push_mode, &_found);
			//if(_found) break;
			p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
		}

		
	}

	/*for (i = 0; i <p_rtps_readerproxy->p_rtps_writer->p_writer_cache->i_changes; i++)
	{
		INSERT_ELEM( p_rtps_readerproxy->pp_changes_for_reader,p_rtps_readerproxy->i_changes_for_reader,
						p_rtps_readerproxy->i_changes_for_reader, p_rtps_readerproxy->p_rtps_writer->p_writer_cache->pp_changes[i]);
	}*/

	(*p_rtps_readerproxy)->changes_for_reader.p_head_first = (*p_rtps_readerproxy)->p_rtps_writer->p_writer_cache->p_head_first;
	(*p_rtps_readerproxy)->changes_for_reader.p_head_last = (*p_rtps_readerproxy)->p_rtps_writer->p_writer_cache->p_head_last;
	(*p_rtps_readerproxy)->changes_for_reader.i_linked_size = (*p_rtps_readerproxy)->p_rtps_writer->p_writer_cache->i_linked_size;
}

void rtps_readerproxy_acked_changes_set(rtps_readerproxy_t *p_rtps_readerproxy, SequenceNumber_t committed_seq_num)
{
	
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;

	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		return;
	}
	/*for(i = 0; i < count; i++)
	{
		 
		for(j=0; j < p_rtps_readerproxy->i_changes_for_reader; j++)
		{
			if(p_rtps_readerproxy->pp_changes_for_reader[i]->sequence_number.high < committed_seq_num.bitmap_base.high)
			{
				p_rtps_readerproxy->pp_changes_for_reader[i]->status = ACKNOWLEDGED;
			}else if(p_rtps_readerproxy->pp_changes_for_reader[i]->sequence_number.high == committed_seq_num.bitmap_base.high)
			{
				if(p_rtps_readerproxy->pp_changes_for_reader[i]->sequence_number.low <= committed_seq_num.bitmap_base.low)
				{
					p_rtps_readerproxy->pp_changes_for_reader[i]->status = ACKNOWLEDGED;
				}
			}
		}

		committed_seq_num.bitmap_base = SequnceNumberDec(committed_seq_num.bitmap_base);
	}*/
	

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_last;

	while(p_change_atom)
	{
		if (sequnce_number_left_bigger_and_equal(committed_seq_num, p_change_atom->sequence_number))
		{
			cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
			
			
			cache_for_guids->status = ACKNOWLEDGED;
			cache_for_guids->is_relevant = false;//by kki(130917)

//by kki...(history and reliability)
			rtps_readerproxy_acked_changes_for_reader_set(p_change_atom, &p_rtps_readerproxy->remote_reader_guid);
		}

		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
	}
	

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
}

rtps_cachechange_t *rtps_readerproxy_next_requested_change(rtps_readerproxy_t *p_rtps_readerproxy)
{
	rtps_cachechange_t *p_rtps_cachechange = NULL;
	
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;

	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL) {
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		return NULL;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

		if (cache_for_guids->status == REQUESTED)
		{
			HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
			return p_change_atom;
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	return p_rtps_cachechange;
}

rtps_cachechange_t *rtps_readerproxy_next_unsent_chage(rtps_readerproxy_t *p_rtps_readerproxy)
{
	rtps_cachechange_t *p_rtps_cachechange = NULL;

	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;

	//HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL) return NULL;

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

		if(cache_for_guids->status == UNSENT)
		{
			//HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
			return p_change_atom;
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	//HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	return p_rtps_cachechange;
}

linked_list_head_t rtps_readerproxy_unsent_changes_nolock(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size, bool is_just_exist)
{
	linked_list_head_t linked_list_head;
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;

	int i_count = 0;
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;
	
	
	//HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL)
	{
		*p_size = 0;
		return linked_list_head;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
		

		if (cache_for_guids->status == UNSENT)
		{



			/*INSERT_ELEM(pp_rtps_cachechanges, i_count,
						i_count, p_change_atom);*/
			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)p_change_atom);
			i_count++;

			if(is_just_exist)
			{
				break;
			}

		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	//HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	*p_size = i_count;
	return linked_list_head;
}

linked_list_head_t rtps_readerproxy_unsent_changes(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size, bool is_just_exist)
{
	linked_list_head_t linked_list_head;
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;

	int i_count = 0;
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;
	
	
	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		*p_size = 0;
		return linked_list_head;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
		

		if (cache_for_guids->status == UNSENT)
		{



			/*INSERT_ELEM(pp_rtps_cachechanges, i_count,
						i_count, p_change_atom);*/
			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)p_change_atom);
			i_count++;

			if(is_just_exist)
			{
				break;
			}

		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	*p_size = i_count;
	return linked_list_head;
}

linked_list_head_t rtps_readerproxy_requested_changes(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size)
{
	linked_list_head_t linked_list_head;
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size=0;
	int i=0;
	cache_for_guid *cache_for_guids;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;
	
	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		*p_size = 0;
		return linked_list_head;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

		i++;
		if(cache_for_guids == NULL) continue;
		
		if(cache_for_guids->status == REQUESTED)
		{

			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)p_change_atom);
			i_size++;
			
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	*p_size = i_size;
	return linked_list_head;
}


linked_list_head_t rtps_readerproxy_requested_changes_unlock(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size)
{
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size=0;
	cache_for_guid *cache_for_guids;
	linked_list_head_t linked_list_head;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;
	
	//HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);
	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
		*p_size = 0;
		return linked_list_head;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

		if(cache_for_guids->status == REQUESTED)
		{

			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)p_change_atom);
			i_size++;
			
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	//HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->writer_cache);

	*p_size = i_size;
	return linked_list_head;
}

void rtps_readerproxy_requested_chanages_set(rtps_readerproxy_t *p_rtps_readerproxy, SequenceNumberSet req_seq_num_set)
{
	int32_t count = req_seq_num_set.numbits;
	int32_t i;
	int32_t j;
	SequenceNumber_t check_requested_seq_num = req_seq_num_set.bitmap_base;
	SequenceNumber_t check_seq_num = sequnce_number_dec(req_seq_num_set.bitmap_base);
	bool is_found = false;
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;

	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);

	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		return;
	}

	for(i = 0; i < count; i++)
	{
		

		/*if( check_requested_seq_num.low % p_rtps_readerproxy->p_rtps_writer->p_dataWriter->test_for_reliablity_division != 0)
		{
			check_requested_seq_num = sequnce_number_inc(check_requested_seq_num);
			continue;
		}*/

		if((req_seq_num_set.bitmap[i/32] & 1 << (31-(i % 32))))
		{
			j = 0;
		 
			p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
			while(p_change_atom)
			{
				j++;
				if(memcmp(&p_change_atom->sequence_number, &check_requested_seq_num, sizeof(SequenceNumber_t)) == 0)
				{

					if(p_change_atom->is_blockcount != -1)
					{
						p_change_atom->is_blockcount = 1;
					}
					else
					{
						if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_start != -1
							&& p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_last != -1
							&& p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_last > p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_start){
								int32_t div = p_change_atom->sequence_number.low % 20;

							if (div >= p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_start 
								&& div <= p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_bulk_last)
							{
								p_change_atom->is_blockcount = 0;
							}

						}else{


							if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_division == 0)
							{
								p_change_atom->is_blockcount = 0;
							}
							else if((p_change_atom->sequence_number.low % p_rtps_readerproxy->p_rtps_writer->p_datawriter->test_for_reliablity_division) != 0)
							{
								p_change_atom->is_blockcount = 0;
							}
							else
							{
								p_change_atom->is_blockcount = 1;
							}
						}
					}

					if (p_change_atom->is_blockcount)
					{
						
						cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

						if (p_rtps_readerproxy->p_rtps_writer->push_mode == true)
						{
							cache_for_guids->status = UNSENT;
							cache_for_guids->is_relevant = true;
						}
						else
						{
							cache_for_guids->status = REQUESTED;
							cache_for_guids->is_relevant = true;
						}
					}

					is_found = true;
				}
				p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
			}

			if (j)
			{
				p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_last;

				if (!is_found && memcmp(&p_change_atom->sequence_number, &check_seq_num, sizeof(SequenceNumber_t)) != 0)
				{
					//이때 GAP을 보내자.
					Gap a_gap;
					memset(a_gap.gap_list.bitmap, '\0', sizeof(a_gap.gap_list.bitmap));
					a_gap.reader_id.value = ENTITYID_UNKNOWN;
					a_gap.writer_id.value = p_rtps_readerproxy->p_rtps_writer->guid.entity_id;
					a_gap.gap_start.value = check_requested_seq_num;
					a_gap.gap_list.bitmap_base = sequnce_number_inc(check_requested_seq_num);
					a_gap.gap_list.numbits = 0;
					//a_gap.gap_list.bitmap[0] |= 1 << 31;
					rtps_send_gap_with_dst_to((rtps_writer_t *)p_rtps_readerproxy->p_rtps_writer, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0],a_gap, check_seq_num);
				}
			}
		}

		check_requested_seq_num = sequnce_number_inc(check_requested_seq_num);
	}

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
}




linked_list_head_t rtps_readerproxy_unacked_changes(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size, bool is_just_exist)
{
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	linked_list_head_t linked_list_head;

	rtps_cachechange_t *p_change_atom = NULL;
	int i_size = 0;
	cache_for_guid *cache_for_guids;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;
	*p_size = 0;

	linked_list_head.p_head_first = NULL;
	linked_list_head.p_head_last = NULL;
	linked_list_head.i_linked_size = 0;

	
	
	
	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);

	if (p_rtps_readerproxy == NULL)
	{
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		*p_size = 0;
		return linked_list_head;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
		if(cache_for_guids == NULL) continue;
		

		if (cache_for_guids->status == UNACKNOWLEDGED ||
			cache_for_guids->status == UNDERWAY )
		{

			insert_linked_list_for_extra_change(&linked_list_head, (linked_list_atom_t *)p_change_atom);
			/*INSERT_ELEM( pp_rtps_cachechanges, i_size,
						i_size, p_change_atom);*/
			i_size++;

			if(is_just_exist)
			{
				break;
			}

		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	*p_size = i_size;
	return linked_list_head;
}


rtps_cachechange_t **rtps_readerproxy_underway_changes(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size)
{
	rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	int i_size = 0;
	cache_for_guid *cache_for_guids;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_writer_t*		p_rtps_writer = p_rtps_readerproxy->p_rtps_writer;


	*p_size = 0;
	
	HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
	update_readerproxy_from_rtps_writer(&p_rtps_readerproxy);

	if (p_rtps_readerproxy == NULL) {
		HISTORYCACHE_UNLOCK(p_rtps_writer->p_writer_cache);
		return NULL;
	}

	p_change_atom = (rtps_cachechange_t *)p_rtps_readerproxy->changes_for_reader.p_head_first;
	while(p_change_atom)
	{
		cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);

		if (cache_for_guids->status == UNDERWAY)
		{

			INSERT_ELEM( pp_rtps_cachechanges, i_size,
						i_size, p_change_atom);

		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

	*p_size = i_size;
	return pp_rtps_cachechanges;
}

