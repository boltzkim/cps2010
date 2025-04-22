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
Lifespan QoS Policy를 위한 모듈.

이력
2012-11-26
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"

extern bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2);
extern Duration_t time_addition_d(const Duration_t augend_time, const Duration_t addend_time);

/************************************************************************/
/* Lifespan QoS Policy												    */
/* get 하는 부분 필요                                                   */
/* lifespan에 대한 검사와 데이터 삭제 작업 필요						    */
/************************************************************************/

//Print Lifespan QoS Information
//void qosim_print_lifespan(Duration_t lifespan)
//{
//	printf("Lifespan Information is %d.%d\n",lifespan.sec, lifespan.nanosec);
//}


//SEDP Data에서 Lifespan QoS 값 얻어오기
void qosim_find_lifespan_qos_from_parameterlist(qos_policy_t* const p_this, rtps_writerproxy_t* p_writerproxy, char* p_value, int32_t i_size)
{
	char* p_lifespan_value = NULL;
	int32_t	i_found = 0;

	//Lifespan QoS 찾기
	find_parameter_list(p_value, i_size, PID_LIFESPAN, &p_lifespan_value, &i_found);
	if (i_found && p_lifespan_value)
	{
		p_writerproxy->lifespan = *(Duration_t*)p_lifespan_value;
	}
	
	//qosim_print_lifespan(p_writerproxy->lifespan);

	i_found = 0;
}


//Writer가 보낸 Source Timestamp와 Lifespan값을 CacheChange에 저장
void qosim_input_lifespan(qos_policy_t* const p_this, rtps_cachechange_t* p_rtps_cachechange, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	p_rtps_cachechange->source_timestamp.sec = p_statefulreader->source_timestamp.sec;
	p_rtps_cachechange->source_timestamp.nanosec = p_statefulreader->source_timestamp.nanosec;
	p_rtps_cachechange->lifespan_duration.sec = p_writerproxy->lifespan.sec;
	p_rtps_cachechange->lifespan_duration.nanosec = p_writerproxy->lifespan.nanosec;
//	printf("source_time = %d , %d\n", p_rtps_cachechange->source_timestamp.sec,p_rtps_cachechange->source_timestamp.nanosec );
}


//added by kyy
void qosim_process_lifespan_for_reader_historycache(rtps_reader_t* p_rtps_reader)
{
	int i;
	Duration_t receive_time = current_duration();
//	int i_cache_size;
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_cachechange_t *p_tmp_atom_change = NULL;

	HISTORYCACHE_LOCK(p_rtps_reader->p_reader_cache);

//	i_cache_size = p_rtps_reader->p_reader_cache->i_changes;

	p_change_atom = (rtps_cachechange_t *)p_rtps_reader->p_reader_cache->p_head_last;

	i=p_rtps_reader->p_reader_cache->i_linked_size-1;

	while(p_change_atom)
	{

		p_tmp_atom_change = (rtps_cachechange_t *)p_change_atom->p_prev;


		if (receive_time.sec - p_change_atom->source_timestamp.sec > p_change_atom->lifespan_duration.sec)
		{
			//p_rtpsreader1 = (rtps_reader_t*) p_rtps_reader->reader_cache->pp_changes[i]->p_org_message->related_rtps_reader;
//			REMOVE_ELEM(p_rtps_reader->p_datareader->pp_message_order,p_rtps_reader->p_datareader->i_message_order,i);
			mutex_lock(&p_rtps_reader->p_datareader->entity_lock);
			REMOVE_ELEM(p_rtps_reader->p_datareader->pp_message_order,p_rtps_reader->p_datareader->i_message_order,i);
			mutex_unlock(&p_rtps_reader->p_datareader->entity_lock);
			printf("SSSsize is %d\n",p_rtps_reader->p_datareader->i_message_order);
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->pp_changes[i]);
			remove_linked_list((linked_list_head_t *)p_rtps_reader->p_reader_cache, (linked_list_atom_t *)p_change_atom);

			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM( p_rtps_reader->p_reader_cache->pp_changes, p_rtps_reader->p_reader_cache->i_changes, i);
		}
		else if (receive_time.sec - p_change_atom->source_timestamp.sec == p_change_atom->lifespan_duration.sec)
		{
			mutex_lock(&p_rtps_reader->p_datareader->entity_lock);
			REMOVE_ELEM(p_rtps_reader->p_datareader->pp_message_order,p_rtps_reader->p_datareader->i_message_order,i);
			mutex_unlock(&p_rtps_reader->p_datareader->entity_lock);
//			REMOVE_ELEM(p_rtps_reader->p_datareader->pp_message_order,p_rtps_reader->p_datareader->i_message_order,i);
			printf("SSSsize is %d\n",p_rtps_reader->p_datareader->i_message_order);
			remove_linked_list((linked_list_head_t *)p_rtps_reader->p_reader_cache, (linked_list_atom_t *)p_change_atom);
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->pp_changes[i]);
			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM( p_rtps_reader->p_reader_cache->pp_changes, p_rtps_reader->p_reader_cache->i_changes, i);
			//REMOVE_ELEM(p_rtps_reader->p_datareader->pp_message_order,p_rtps_reader->p_datareader->i_message_order,i);
		}
		p_change_atom = p_tmp_atom_change;
		i--;
	}

	/*for (i = 0; i < p_rtps_reader->p_reader_cache->i_changes; i++)
	{
		printf("source[%d] : %d\n",i, p_rtps_reader->p_reader_cache->pp_changes[i]->source_timestamp.sec);
		printf("%d\n",receive_time.sec - p_rtps_reader->p_reader_cache->pp_changes[i]->source_timestamp.sec);
	}*/
	HISTORYCACHE_UNLOCK(p_rtps_reader->p_reader_cache);
}


//added by kyy
void qosim_process_lifespan_for_reader_historycache_stateful(rtps_statefulreader_t* p_statefulreader)
{
	int i;
	Duration_t receive_time = current_duration();
	
	rtps_statefulreader_t* p_statefulreader1;
	rtps_cachechange_t *p_change_atom = NULL;

	HISTORYCACHE_LOCK(p_statefulreader->p_reader_cache);


	p_change_atom = (rtps_cachechange_t *)p_statefulreader->p_reader_cache->p_head_last;
	i=p_statefulreader->p_reader_cache->i_linked_size-1;

	while(p_change_atom)
	{
		if (receive_time.sec - p_change_atom->source_timestamp.sec > p_change_atom->lifespan_duration.sec)
		{
			p_statefulreader1 = (rtps_statefulreader_t*) p_change_atom->p_org_message->v_related_rtps_reader;
			REMOVE_ELEM(p_statefulreader1->p_datareader->pp_message_order, p_statefulreader1->p_datareader->i_message_order, i);
			printf("SSSsize is %d\n",p_statefulreader1->p_datareader->i_message_order);
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->pp_changes[i]);
			//REMOVE_ELEM(p_statefulreader->reader_cache->pp_changes[i]->p_org_message->related_rtps_reader)
			remove_linked_list((linked_list_head_t *)p_statefulreader->p_reader_cache, (linked_list_atom_t *)p_change_atom);
			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM(p_statefulreader->p_reader_cache->pp_changes, p_statefulreader->p_reader_cache->i_changes, i);
		}
		else if (receive_time.sec - p_change_atom->source_timestamp.sec == p_change_atom->lifespan_duration.sec)
		{
			p_statefulreader1 = (rtps_statefulreader_t*) p_change_atom->p_org_message->v_related_rtps_reader;
			REMOVE_ELEM(p_statefulreader1->p_datareader->pp_message_order, p_statefulreader1->p_datareader->i_message_order, i);
			printf("SSSsize is %d\n",p_statefulreader1->p_datareader->i_message_order);
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->pp_changes[i]);
			remove_linked_list((linked_list_head_t *)p_statefulreader->p_reader_cache, (linked_list_atom_t *)p_change_atom);
			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM( p_statefulreader->p_reader_cache->pp_changes, p_statefulreader->p_reader_cache->i_changes, i);
		}

		/*printf("source[%d] : %d\n",i, p_statefulreader->p_reader_cache->pp_changes[i]->source_timestamp.sec);
		printf("%d\n",receive_time.sec - p_statefulreader->p_reader_cache->pp_changes[i]->source_timestamp.sec);*/
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
		i--;
	}

	HISTORYCACHE_UNLOCK(p_statefulreader->p_reader_cache);
}


void qosim_process_lifespan_for_writer_historycache_stateful(rtps_statefulwriter_t* p_statefulwriter)
{
//	int i;
	
	Duration_t send_time = current_duration();
	rtps_cachechange_t *p_change_atom = NULL;
	rtps_cachechange_t *p_tmp_atom_change = NULL;

	HISTORYCACHE_LOCK(p_statefulwriter->p_writer_cache);


	p_change_atom = (rtps_cachechange_t *)p_statefulwriter->p_writer_cache->p_head_last;

	while(p_change_atom)
	{
		p_tmp_atom_change = (rtps_cachechange_t *)p_change_atom->p_prev;

//		printf("source[%d] : %d\n",i, p_statefulwriter->writer_cache->pp_changes[i]->source_timestamp.sec);

		if (send_time.sec - p_change_atom->source_timestamp.sec > p_statefulwriter->p_datawriter->datawriter_qos.lifespan.duration.sec)
		{
			remove_linked_list((linked_list_head_t *)p_statefulwriter->p_writer_cache, (linked_list_atom_t *)p_change_atom);
			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM( p_statefulwriter->p_writer_cache->pp_changes, p_statefulwriter->p_writer_cache->i_changes, i);
		}
		else if (send_time.sec - p_change_atom->source_timestamp.sec == p_statefulwriter->p_datawriter->datawriter_qos.lifespan.duration.sec)
		{
			remove_linked_list((linked_list_head_t *)p_statefulwriter->p_writer_cache, (linked_list_atom_t *)p_change_atom);
			rtps_cachechange_ref(p_change_atom, true, false);
			rtps_cachechange_destory(p_change_atom);
			//REMOVE_ELEM( p_statefulwriter->p_writer_cache->pp_changes, p_statefulwriter->p_writer_cache->i_changes, i);
		}

		//		printf("%d\n",receive_time.sec - p_statefulreader->reader_cache->pp_changes[i]->source_timestamp.sec);
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_prev;
	}

	HISTORYCACHE_UNLOCK(p_statefulwriter->p_writer_cache);
	/*
	for(i=0; i<p_statefulwriter->writer_cache->i_changes; i++)
	{
		printf("source[%d] : %d\n",i, p_statefulwriter->writer_cache->pp_changes[i]->source_timestamp.sec);
	}*/
}
