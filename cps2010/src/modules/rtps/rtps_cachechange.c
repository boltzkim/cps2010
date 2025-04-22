/*
	RTSP CacheChange class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Represents an individual change made to a data-object. Includes the creation,
modification, and deletion of data-objects.
*/

#include "rtps.h"

rtps_cachechange_t* rtps_cachechange_new( ChangeKind_t kind, GUID_t guid, SequenceNumber_t seq, SerializedPayload* p_serialized_data, InstanceHandle_t handle )
{
	rtps_cachechange_t	*p_rtps_cachechange = malloc(sizeof(rtps_cachechange_t));

	memset(p_rtps_cachechange, 0, sizeof(rtps_cachechange_t));

	p_rtps_cachechange->kind = kind;
	p_rtps_cachechange->p_data_value = p_serialized_data;
	p_rtps_cachechange->instance_handle = handle;
	p_rtps_cachechange->sequence_number.high = seq.high;
	p_rtps_cachechange->sequence_number.low = seq.low;

	p_rtps_cachechange->writer_guid = guid;

//	p_rtps_cachechange->is_relevant = false;
	p_rtps_cachechange->tmp_status = UNKNOWN;
	//p_rtps_cachechange->status = p_rtps_cachechange->tmp_status;

	p_rtps_cachechange->b_read = false;
	p_rtps_cachechange->is_blockcount = -1;
	p_rtps_cachechange->is_same = false;
	p_rtps_cachechange->cache_for_guids.p_head_first = NULL;
	p_rtps_cachechange->cache_for_guids.p_head_last = NULL;
	p_rtps_cachechange->cache_for_guids.i_linked_size = 0;

	p_rtps_cachechange->b_isdisposed = false;
	p_rtps_cachechange->p_org_message = NULL;
	p_rtps_cachechange->b_ref_rtps = false;
	p_rtps_cachechange->b_ref_dcps = false;
	p_rtps_cachechange->p_rtps_writerproxy = NULL;
	
	return p_rtps_cachechange;
}


void rtps_cachechange_destory(rtps_cachechange_t *p_rtps_cachechange)
{
	cache_for_guid		*cache_for_guids;
	cache_for_guid		*p_cache_atom = NULL;


	if(p_rtps_cachechange->b_ref_rtps  || p_rtps_cachechange->b_ref_dcps){

		return;
	}


	p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

	while(p_cache_atom)
	{
		cache_for_guids = (cache_for_guid *)p_cache_atom->p_next;
		remove_linked_list(&p_rtps_cachechange->cache_for_guids, (linked_list_atom_t *)p_cache_atom);

		FREE(p_cache_atom);

		p_cache_atom = cache_for_guids;

	}

	if(p_rtps_cachechange->p_org_message && p_rtps_cachechange->p_org_message->sampleInfo.sample_state != READ_SAMPLE_STATE)
	{
		p_rtps_cachechange->p_org_message->sampleInfo.sample_state = p_rtps_cachechange->p_org_message->sampleInfo.sample_state;
	}

	if(p_rtps_cachechange->p_org_message)message_release(p_rtps_cachechange->p_org_message);

	if(p_rtps_cachechange->p_data_value) FREE(p_rtps_cachechange->p_data_value->p_value);
	FREE(p_rtps_cachechange->p_data_value);
	FREE(p_rtps_cachechange);
}


cache_for_guid *rtps_cachechange_get_associated_cache_for_guid(GUID_t guid, rtps_cachechange_t *p_rtps_cachechange)
{

	cache_for_guid	*p_cache_atom = NULL;
	cache_for_guid	*p_cache_for_guid;

	p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

	while(p_cache_atom)
	{
		if(memcmp(&p_cache_atom->guid, &guid,sizeof(guid)) == 0)
		{
			return p_cache_atom;
		}

		p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
	}


	p_cache_for_guid = (cache_for_guid *)malloc(sizeof(cache_for_guid));
	memset(p_cache_for_guid, 0, sizeof(cache_for_guid));
	memcpy(&p_cache_for_guid->guid, &guid, sizeof(GUID_t));

	p_cache_for_guid->status = UNSENT;

	p_cache_for_guid->is_relevant = true;

	insert_linked_list(&p_rtps_cachechange->cache_for_guids, (linked_list_atom_t *)p_cache_for_guid);

	//trace_msg(NULL, TRACE_LOG, "rtps_cachechange_get_associated_cache_for_guid is NULL");

	return p_cache_for_guid;
}


void rtps_cachechange_ref(rtps_cachechange_t *p_rtps_cachechange, bool isrtps, bool isref)
{

	if(isrtps)
	{
		p_rtps_cachechange->b_ref_rtps = isref;
	}else{
		p_rtps_cachechange->b_ref_dcps = isref;
	}
	
}