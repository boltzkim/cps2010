/*******************************************************************************
 * Copyright (c) 2011 Electronics and Telecommunications Research Institute
 * (ETRI) All Rights Reserved.
 * 
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	History QoS Policy�� Reliability QoS Policy�� ���� ���.

	�̷�
	2012-11-09
*/
#include <core.h>
#include "../rtps/rtps.h"


uint32_t get_millisecond(const Duration_t duration);

/************************************************************************/
/* HistoryCache �� �ִ밪�� ���Ѵ�.                                     */
/************************************************************************/
//#define HISTORY_MAX_LENGTH 2000 //2147483647

#define HISTORY_MAX_LENGTH 2000 //2147483647

int32_t qosim_get_history_max_length(qos_policy_t* const p_this, const HistoryQosPolicy* const p_history_qos, const ResourceLimitsQosPolicy* const p_resource_limits_qos)
{
	if (p_history_qos == NULL)
	{
		return 1;
	}

	if (p_history_qos->kind == KEEP_LAST_HISTORY_QOS)
	{
		return (p_resource_limits_qos->max_instances != LENGTH_UNLIMITED) ? (p_history_qos->depth * p_resource_limits_qos->max_instances) : HISTORY_MAX_LENGTH;
	}
	else
	{
		return (p_resource_limits_qos->max_samples != LENGTH_UNLIMITED) ? (p_resource_limits_qos->max_samples) : HISTORY_MAX_LENGTH;
	}

	//if(p_resource_limits_qos->max_samples != LENGTH_UNLIMITED){
	//	return (p_resource_limits_qos->max_samples <= HISTORY_MAX_LENGTH) ? (p_resource_limits_qos->max_samples) : HISTORY_MAX_LENGTH;
	//}
	//else{
	//	return HISTORY_MAX_LENGTH;

	//}

}


/************************************************************************/
/*  HistoryCache �� ���� á���� Ȯ���ϴ� �Լ�                           */
/************************************************************************/
bool historycache_is_full(const rtps_historycache_t* const p_historycache)
{
	return (p_historycache->i_linked_size >= p_historycache->history_max_length);
}

/************************************************************************/
/*  HistoryCache �� ���� á���� Ȯ���ϴ� �Լ�                           */
/************************************************************************/
bool historycache_is_full2(const rtps_historycache_t* const p_historycache, int32_t i_writerproxy_cache_size)
{
	return (i_writerproxy_cache_size >= p_historycache->history_max_length);
}


/************************************************************************/
/* key_hash �� ���� �ν��Ͻ��� ã�� �Լ�                                */
/************************************************************************/
static instanceset_t* instanceset_find(instanceset_t** const pp_instanceset, const int32_t i_instanceset, const uint8_t key_hash[])
{
	int32_t i, j;

	for (i = 0; i < i_instanceset; i++)
	{
		for (j = 0; j < pp_instanceset[i]->i_key_guid; j++)
		{
			if (memcmp(pp_instanceset[i]->pp_key_guid[j], key_hash, 16) == 0)
			{
				return pp_instanceset[i];
			}
		}
	}
	return NULL;
}


/************************************************************************/
/* Instanceset �� ���� á���� Ȯ���ϴ� �Լ�                             */
/************************************************************************/
static bool instanceset_is_full(const rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange)
{
	instanceset_t** pp_instanceset;
	int32_t i_instanceset;
	HistoryQosPolicyKind history_kind;
	instanceset_t* p_instanceset;

	// ���� �ʱ�ȭ
	if (p_historycache->p_rtps_writer)
	{
		pp_instanceset = p_historycache->p_rtps_writer->p_datawriter->pp_instanceset;
		i_instanceset = p_historycache->p_rtps_writer->p_datawriter->i_instanceset;
		history_kind = p_historycache->p_rtps_writer->p_datawriter->datawriter_qos.history.kind;
	}
	else if (p_historycache->p_rtps_reader)
	{
		pp_instanceset = p_historycache->p_rtps_reader->p_datareader->pp_instanceset;
		i_instanceset = p_historycache->p_rtps_reader->p_datareader->i_instanceset;
		history_kind = p_historycache->p_rtps_reader->p_datareader->datareader_qos.history.kind;
	}
	else
	{
		return false;
	}

	if ((p_instanceset = instanceset_find(pp_instanceset, i_instanceset, p_cachechange->key_guid_for_reader)) == NULL)
	{
		return false;
	}

	if (history_kind == KEEP_LAST_HISTORY_QOS)
	{
		return (p_instanceset->i_messages >= p_instanceset->i_history_depth);
	}
	else
	{
		if (p_instanceset->max_samples_per_instance != LENGTH_UNLIMITED)
		{
			return (p_instanceset->i_messages >= p_instanceset->max_samples_per_instance);
		}
		else
		{
			return (p_instanceset->i_messages >= HISTORY_MAX_LENGTH);
		}
	}
}


/************************************************************************/
/* HistoryQoSPolicy.kind == KEEP_ALL �̰�                               */
/* ReliabilityQoSPolicy.kind == RELIABLE�ΰ�?                           */
/************************************************************************/
static bool is_strict_reliability(const rtps_historycache_t* const p_historycache)
{
	if (p_historycache == NULL)
	{
		return false;
	}

	if (p_historycache->history_kind == KEEP_ALL_HISTORY_QOS && p_historycache->reliability_kind == RELIABLE_RELIABILITY_QOS)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* ��� reliable DataReader�κ��� ACKNACK�� �޾Ҵ°�?                   */
/************************************************************************/
static bool is_dropable_cachechange(rtps_statefulwriter_t *p_rtps_statefulwriter, const rtps_cachechange_t* const p_cachechange)
{
	int j;
	bool is_skip = false;
	cache_for_guid	*p_cache_atom = NULL;
	
	if(p_cachechange->cache_for_guids.i_linked_size == 0) return false;

	p_cache_atom = (cache_for_guid *)p_cachechange->cache_for_guids.p_head_first;

	while (p_cache_atom)
	{
		if (p_cache_atom->status != ACKNOWLEDGED)
		{
			is_skip = false;

			for (j = 0; j < p_rtps_statefulwriter->i_matched_readers; j++)
			{
				// remote reader�� best_effort�϶��� �׳� ������ �������.
				if (p_rtps_statefulwriter->pp_matched_readers[j]->reliablility_kind == BEST_EFFORT_RELIABILITY_QOS) continue;

				if (memcmp(&p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, &p_cache_atom->guid,sizeof(GUID_t)) ==0)
				{
					if (p_rtps_statefulwriter->pp_matched_readers[j]->not_ack_count > NOT_ACK_COUNT)
					{
						is_skip = true;
					}
					break;
				}
			}

			if (is_skip)
			{
				p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
				continue;
			}

			return false;
		}

		p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
	}

	return true;
}


/************************************************************************/
/* cachechange �� �����Ѵ�.                                             */
/************************************************************************/
static void remove_cachechange(rtps_historycache_t* const p_historycache, const int32_t index)
{
	int i;
	rtps_cachechange_t *p_change_atom = NULL;

	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

	for (i = 0; i < index; i++)
	{
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}

	remove_linked_list((linked_list_head_t *)p_historycache, (linked_list_atom_t *)p_change_atom);

	rtps_cachechange_ref(p_change_atom, true, false);
	if (p_historycache->p_rtps_reader && p_historycache->p_rtps_reader->is_builtin == false)
	{
	//	trace_msg(OBJECT(current_object(0)), TRACE_TRACE3, "\t\t\t\t --- DEL[%5u]", p_change_atom->sequence_number.low);

	//	printf( "\t\t\t\t --- DEL[%5u]\n", p_change_atom->sequence_number.low);
	}
	rtps_cachechange_destory(p_change_atom);
	
	/*if (p_historycache->p_rtps_reader && p_historycache->p_rtps_reader->is_builtin == false)
	{
		trace_historycache(OBJECT(current_object(0)), TRACE_TRACE3, p_historycache);
	}*/
}

/************************************************************************/
/* cachechange �� �����Ѵ�.                                             */
/************************************************************************/
static rtps_cachechange_t *remove_cachechange_linked(rtps_historycache_t* const p_historycache, rtps_cachechange_t *p_change_atom, bool is_next)
{
	linked_list_atom_t *p_want = NULL;

	if(is_next)
	{
		p_want = p_change_atom->p_next;
	}else{
		p_want = p_change_atom->p_prev;
	}

	remove_linked_list((linked_list_head_t *)p_historycache, (linked_list_atom_t *)p_change_atom);

	rtps_cachechange_ref(p_change_atom, true, false);
	rtps_cachechange_destory(p_change_atom);
	

	return (rtps_cachechange_t *)p_want;
}


/************************************************************************/
/* �� cachechange�� �ν��Ͻ��� �����鼭 ���� ������ cachechange�� ����  */
/************************************************************************/
static void remove_cachechange_by_instanceset(rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_new_cachechange)
{
	rtps_cachechange_t* p_temp_cachechange = NULL;

	for (p_temp_cachechange = (rtps_cachechange_t*)p_historycache->p_head_first; p_temp_cachechange != NULL; p_temp_cachechange = (rtps_cachechange_t*)p_temp_cachechange->p_next)
	{
		if (memcmp(p_temp_cachechange->key_guid_for_reader, p_new_cachechange->key_guid_for_reader, 16) == 0)
		{
			remove_linked_list((linked_list_head_t *)p_historycache, (linked_list_atom_t *)p_temp_cachechange);
			rtps_cachechange_ref(p_temp_cachechange, true, false);
			rtps_cachechange_destory(p_temp_cachechange);
			break;
		}
	}
}


/************************************************************************/
/* historycache�� ����á����, ���������� cachechange�� �����Ѵ�.        */
/************************************************************************/
bool qosim_check_historycache_for_writer(qos_policy_t* const p_this, rtps_writer_t* p_writer, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange)
{
	int32_t i_cache_size;

	bool b_ret = true;
	rtps_cachechange_t *p_change_atom = NULL;

	//mutex_lock(&p_historycache->object_lock);

	HISTORYCACHE_LOCK(p_historycache);

	if (instanceset_is_full(p_historycache, p_cachechange) || historycache_is_full(p_historycache))
	{
		// ���� �� �ִ� �� �켱 ����
		i_cache_size = p_historycache->i_linked_size;

		p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;
	
		while (p_change_atom)
		{
			
			if(p_historycache->reliability_kind == RELIABLE_RELIABILITY_QOS)
			{
				if (is_dropable_cachechange((rtps_statefulwriter_t *)p_writer, p_change_atom))
				{
					if (p_historycache->i_linked_size > 0
						&& ((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.i_linked_size > 0
						&& (  ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == UNDERWAY
						     || ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == ACKNOWLEDGED ))
					{
						
						p_change_atom = remove_cachechange_linked(p_historycache, p_change_atom, true);
						continue;
					}
				}
				else
				{
					break;
				}

			}
			p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		}

		// cachechange ������ ���� ���
		if (p_historycache->i_linked_size == i_cache_size)
		{
			if (is_strict_reliability(p_historycache))
			{
				//write �Լ��� block���Ѿ� �Ѵ�.
				b_ret = false;
			}
			else
			{
				// (���� ������) ù��° cachechange ����
				if (p_historycache->i_linked_size > 0)
				{
					remove_cachechange(p_historycache, 0);
				}
			}
		}
	}

	HISTORYCACHE_UNLOCK(p_historycache);

	


	if (p_writer->reliability_level == RELIABLE)
	{
		//if(b_ret == false)
		//{
		//	int j;
		//	bool is_condwait = false;
		//	rtps_cachechange_t	*p_change_atom = NULL;
		//	cache_for_guid	*p_cache_atom = NULL;
		//	rtps_statefulwriter_t *p_rtps_statefulwriter = (rtps_statefulwriter_t *)p_writer;
		//	mutex_lock(&p_writer->p_datawriter->entity_lock);

		//	p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;
		//	p_cache_atom = (cache_for_guid *)p_change_atom->cache_for_guids.p_head_first;

		//	while(p_cache_atom)
		//	{
		//		for(j=0 ; j < p_rtps_statefulwriter->i_matched_readers; j++)
		//		{
		//			// remote reader�� best_effort�϶��� don't use cond_wait
		//			if(p_rtps_statefulwriter->pp_matched_readers[j]->reliablility_kind == BEST_EFFORT_RELIABILITY_QOS) continue;

		//			if(memcmp(&p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, &p_cache_atom->guid,sizeof(GUID_t)) ==0)
		//			{
		//				if(p_rtps_statefulwriter->pp_matched_readers[j]->not_ack_count < NOT_ACK_COUNT)
		//				{
		//					is_condwait = true;
		//					break;
		//				}
		//				
		//			}
		//		}

		//		p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
		//	}

		//	//printf("lock \r\n");
		//	if(is_condwait){
		//		uint32_t waittime = get_millisecond(p_writer->p_datawriter->datawriter_qos.reliability.max_blocking_time);
		//		cond_waittimed(&p_writer->p_datawriter->entity_wait, &p_writer->p_datawriter->entity_lock, waittime);
		//		//p_writer->p_datawriter->b_write_blocked = false;
		//	}
		//	//printf("unlock \r\n");
		//	mutex_unlock(&p_writer->p_datawriter->entity_lock);
		//	


		//	b_ret = true;
		//	if(p_historycache->i_linked_size > 0
		//				&& ((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.i_linked_size > 0
		//				&& (  ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == UNDERWAY
		//				     || ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == ACKNOWLEDGED ))
		//	{
		//		mutex_lock(&p_historycache->object_lock);
		//		remove_cachechange(p_historycache, 0);
		//		mutex_unlock(&p_historycache->object_lock);
		//	}
		//}

		b_ret = true;
	}
	else
	{
		if (b_ret == false)
		{
			if (p_historycache->i_linked_size > 0
						&& ((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.i_linked_size > 0
						&& (  ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == UNDERWAY
						     || ((cache_for_guid *)((rtps_cachechange_t *)p_historycache->p_head_first)->cache_for_guids.p_head_first)->status == ACKNOWLEDGED ))
			{

				HISTORYCACHE_LOCK(p_historycache);
				remove_cachechange(p_historycache, 0);
				HISTORYCACHE_UNLOCK(p_historycache);
				
			}
		}
	}
	

	return b_ret;
}


// heartbeat�� ������ �ֱⰡ ������� �������� ��쿡�� acknack�� ���� ���� ���� ������
// �����Ͱ� ����.. rti �ϰ� �׽�Ʈ �ϸ� �������� ���� heartbeat �����°�....
//
/************************************************************************/
/* ���ڸ��� ��������� true, ������������ false �� ����               */
/************************************************************************/
bool qosim_check_historycache_for_reader(qos_policy_t* const p_this, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange)
{
	uint16_t i_cache_size;

	bool b_ret = true;
	int32_t i_changes_from_writer = 0;
	rtps_writerproxy_t* p_writerproxy = NULL;

	mutex_lock(&p_historycache->object_lock);

	
	p_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t*)p_historycache->p_rtps_reader, p_cachechange->writer_guid);
	
	if (p_writerproxy == NULL)
	{
		mutex_unlock(&p_historycache->object_lock);
		return false;
	}
	update_writerproxy_from_rtps_reader(&p_writerproxy); 

	

	i_changes_from_writer = p_writerproxy->i_changes_from_writer;

	//////////////////////////////////////////////////////////////////////////
	// ���� sequence_number �� �����͸� �̹� ���� �ִ��� �˻�
	if (i_changes_from_writer > 0)
	{
		
		if (p_writerproxy)
		{
			rtps_cachechange_t* p_change_atom = NULL;
			int i;

			//update_writerproxy_from_rtps_reader(p_writerproxy);

			

			for (i=0; i < p_writerproxy->i_changes_from_writer; i++)
			{
				
				p_change_atom = (rtps_cachechange_t *)p_writerproxy->pp_changes_from_writers[i];

				while (p_change_atom)
				{
					if (sequnce_number_equal(p_change_atom->sequence_number, p_cachechange->sequence_number))
					{
						mutex_unlock(&p_historycache->object_lock);
						return false;
					}
					else
					{
						/// �ѹ��� ����......
						break;
					}

					//p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	// Reliable ����� ���� ������� �����ؾ� �Ѵ�. ������ ��߳� ���� ������, �ٽ� �޵��� �Ѵ�.
	//printf("1 sequece num : %d\r\n", p_cachechange->sequence_number.low);
	if (p_historycache->p_rtps_reader->is_builtin == false &&
		p_historycache->reliability_kind == RELIABLE_RELIABILITY_QOS && p_historycache->i_linked_size > 0)
	{
		// writer �� ���� ���� ��츦 ó���ϱ� ���� writerproxy ���� ó���ϰ� �Ϸ��� �Ѵ�.
		//rtps_writerproxy_t* p_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t*)p_historycache->p_rtps_reader, p_cachechange->writer_guid);
		//if (p_writerproxy)
		//{
		//	SequenceNumber_t max_seq_num;

		//	//update_writerproxy_from_rtps_reader(p_writerproxy);

		//	//i_changes_from_writer = p_writerproxy->i_changes_from_writer;

		//	if (p_writerproxy->i_changes_from_writer > 0)
		//	{
		//		max_seq_num = p_writerproxy->pp_changes_from_writers[p_writerproxy->i_changes_from_writer-1]->sequence_number;

		//		for (i = p_writerproxy->i_changes_from_writer-1; i > 0 && !p_writerproxy->pp_changes_from_writers[i]->p_data_value; i--)
		//		{
		//			if (sequnce_number_left_bigger(p_writerproxy->pp_changes_from_writers[i-1]->sequence_number, max_seq_num))
		//			{
		//				max_seq_num = p_writerproxy->pp_changes_from_writers[i-1]->sequence_number;
		//			}
		//		}

		//		if (!sequnce_number_equal(p_cachechange->sequence_number, sequnce_number_inc(max_seq_num)))
		//		{
		//			// �ſ� �幮 ��쿡 seq_num�� �ǳ� �ٴ� ��찡 �߻��Ѵ�.

		//			// Reader �� ���� ack�� seq_num �� ��쿣 ��������ش�.					
		//			if (!sequnce_number_equal(p_writerproxy->last_acknack.reader_sn_state.bitmap_base, p_cachechange->sequence_number))
		//			{
		//				trace_msg(OBJECT(p_this), TRACE_TRACE3, "NOT ADD[%5u]", p_cachechange->sequence_number.low);
		//				mutex_unlock(&p_historycache->object_lock);
		//				return false;
		//			}

		//		}
		//	}
		//}
		//else
		//{
		//	mutex_unlock(&p_historycache->object_lock);
		//	//writerproxy �� �����Ƿ� �߰����� �ʴ´�.
		//	return false;
		//}
	}
	/*else if ((p_historycache->p_rtps_reader->is_builtin == false &&	p_historycache->reliability_kind == RELIABLE_RELIABILITY_QOS) == false)
	{
		i_changes_from_writer = p_historycache->i_linked_size;
	}*/

	//printf("2 sequece num : %d\r\n", p_cachechange->sequence_number.low);
	if (instanceset_is_full(p_historycache, p_cachechange) || historycache_is_full2(p_historycache, i_changes_from_writer))
	{
		rtps_cachechange_t *p_change_atom = NULL;
		// ���� �� �ִ� �� �켱 ����
		i_cache_size = p_historycache->i_linked_size;
		p_change_atom = (rtps_cachechange_t *)p_historycache->p_head_first;

		while (p_change_atom && p_historycache->i_linked_size != 0)
		{
			if (!(p_change_atom->p_org_message) ||
				(p_change_atom->p_org_message && p_change_atom->p_org_message->sampleInfo.sample_state == READ_SAMPLE_STATE))
			{
				rtps_cachechange_t* p_remove_cachechange = p_change_atom;
				rtps_writerproxy_t* p_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t*)p_historycache->p_rtps_reader, p_cachechange->writer_guid);

				// ACK ���� seq_num ���� �۰ų� ���� �͸� �����Ѵ�. (Multi-Writer �� ����� ��� �ٲ��� �Ѵ�)
				if (memcmp(p_change_atom->key_guid_for_reader, p_cachechange->key_guid_for_reader, 16) == 0)
				{
					if (p_writerproxy && p_writerproxy->last_acknack.reader_sn_state.numbits == 0 
						&& sequnce_number_left_bigger(p_writerproxy->last_acknack.reader_sn_state.bitmap_base, p_cachechange->sequence_number))
					{
						p_change_atom = remove_cachechange_linked(p_historycache, p_change_atom, true);
					}
					else if (p_writerproxy && p_writerproxy->last_acknack.reader_sn_state.numbits != 0 
						&& sequnce_number_left_bigger_and_equal(p_writerproxy->last_acknack.reader_sn_state.bitmap_base, p_cachechange->sequence_number))
					{
						if (p_historycache->i_linked_size) 
						{
							p_change_atom = remove_cachechange_linked(p_historycache, p_change_atom, true);
						}
					}
					else
					{
						break;
					}
				}
			}
			if(p_change_atom)p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		}
		
		// cachechange ������ ���� ���
		if (instanceset_is_full(p_historycache, p_cachechange) || p_historycache->history_max_length == i_changes_from_writer)
//		if (i_cache_size == p_historycache->i_linked_size)
		{
			if (is_strict_reliability(p_historycache))
			{
				SampleRejectedStatusKind sample_rejected_status;
				InstanceHandle_t last_instance_handle; // ������ �źε� ������ ������Ʈ�ϴ� �ν��Ͻ��� ���� �ڵ�.
				GUID_t key_hash;
				if (instanceset_is_full(p_historycache, p_cachechange))
				{
					sample_rejected_status = REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT;
					memcpy(&key_hash, p_cachechange->key_guid_for_reader, 16);
					last_instance_handle = (InstanceHandle_t)instanceset_find(p_historycache->p_rtps_reader->p_datareader->pp_instanceset, p_historycache->p_rtps_reader->p_datareader->i_instanceset, p_cachechange->key_guid_for_reader);
					change_SampleRejected_status(p_historycache->p_rtps_reader->p_datareader, sample_rejected_status, last_instance_handle);

					b_ret = false;
					trace_msg(OBJECT(p_this), TRACE_TRACE3, "NOT ADD[%5u]...", p_cachechange->sequence_number.low);
					trace_msg(OBJECT(p_this), TRACE_TRACE3, "\t\t\t\t$$$$$ HISTORY_SIZE:[%5d/%5d], INSTANCE_SIZE:[%5d/%5d] $$$$$", p_historycache->i_linked_size, p_historycache->history_max_length, ((instanceset_t*)last_instance_handle)->i_messages, ((instanceset_t*)last_instance_handle)->max_samples_per_instance);

					if (p_historycache->i_linked_size)
					{
						if(p_historycache->p_rtps_reader->p_datareader->i_instanceset > 1){
							remove_cachechange_by_instanceset(p_historycache, p_cachechange);
						}
						else{
							remove_cachechange(p_historycache, 0);
						}
					}
			
				}
				else if (historycache_is_full2(p_historycache, i_changes_from_writer))
				{
					//sample_rejected_status = REJECTED_BY_SAMPLES_LIMIT;
					if (p_historycache->i_linked_size)
					{
						if(p_historycache->p_rtps_reader->p_datareader->i_instanceset > 1){
							remove_cachechange_by_instanceset(p_historycache, p_cachechange);
						}
						else{
							remove_cachechange(p_historycache, 0);
						}
					}
				}
				/*memcpy(&key_hash, p_cachechange->key_guid_for_reader, 16);
				last_instance_handle = (InstanceHandle_t)instanceset_find(p_historycache->p_rtps_reader->p_datareader->pp_instanceset, p_historycache->p_rtps_reader->p_datareader->i_instanceset, p_cachechange->key_guid_for_reader);
				change_SampleRejected_status(p_historycache->p_rtps_reader->p_datareader, sample_rejected_status, last_instance_handle);

				b_ret = false;
				trace_msg(OBJECT(p_this), TRACE_TRACE3, "NOT ADD[%5u]...", p_cachechange->sequence_number.low);
				trace_msg(OBJECT(p_this), TRACE_TRACE3, "\t\t\t\t$$$$$ HISTORY_SIZE:[%5d/%5d], INSTANCE_SIZE:[%5d/%5d] $$$$$", p_historycache->i_changes, p_historycache->history_max_length, ((instanceset_t*)last_instance_handle)->i_messages, ((instanceset_t*)last_instance_handle)->max_samples_per_instance);
				*/
			}
			else
			{
				 //(���� ������) ù��° cachechange ����
				if (p_historycache->i_linked_size)
				{
					if(p_historycache->p_rtps_reader->p_datareader->i_instanceset > 1){
						remove_cachechange_by_instanceset(p_historycache, p_cachechange);
					}
					else{
						remove_cachechange(p_historycache, 0);
					}
				}
			}
		}
	}

	mutex_unlock(&p_historycache->object_lock);

	return b_ret;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void rtps_readerproxy_acked_changes_for_reader_set(const rtps_cachechange_t* const p_cachechange, const GUID_t* const p_remote_reader_guid)
{
	cache_for_guid	*p_cache_atom = NULL;

	p_cache_atom = (cache_for_guid *)p_cachechange->cache_for_guids.p_head_first;

	while(p_cache_atom)
	{
		if (memcmp(&p_cache_atom->guid, p_remote_reader_guid, sizeof(GUID_t)) == 0)
		{
			p_cache_atom->status = ACKNOWLEDGED;
		}
		p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
	}
}
