/*
	RTSP StatefulWriter class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"

rtps_statefulwriter_t *rtps_statefulwriter_new(module_object_t * p_this)
{
	rtps_statefulwriter_t *p_statefulwriter = malloc(sizeof(rtps_statefulwriter_t));
	memset(p_statefulwriter, 0, sizeof(rtps_statefulwriter_t));

	rtps_writer_init((rtps_writer_t *)p_statefulwriter);

	p_statefulwriter->pp_matched_readers = NULL;
	p_statefulwriter->i_matched_readers = 0;
	p_statefulwriter->behavior_type = STATEFUL_TYPE;
	p_statefulwriter->last_send_heartbeat = currenTime();
	p_statefulwriter->last_nack_time = currenTime();
	p_statefulwriter->heatbeat_count = 0;
	p_statefulwriter->b_enable_multicast = false;

	//set_enable_multicast((service_t*)p_this, p_statefulwriter);

	return p_statefulwriter;
}

void rtps_statefulwriter_destory(rtps_statefulwriter_t *p_statefulwriter)
{
	if(p_statefulwriter)
	{
		mutex_lock(&p_statefulwriter->object_lock);
		while(p_statefulwriter->i_matched_readers)
		{
			rtps_readerproxy_destroy(p_statefulwriter->pp_matched_readers[0]);
			REMOVE_ELEM( p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers, 0);
		}
		mutex_unlock(&p_statefulwriter->object_lock);
	}
}

/*
bool rtps_statefulwriter_matched_reader_add(rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy)
{
	bool b_ret = false;

	if(rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, p_reader_proxy->remote_reader_guid) == 0)
	{
		mutex_lock(&p_statefulwriter->object_lock);
		INSERT_ELEM( p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers,
						 p_statefulwriter->i_matched_readers, p_reader_proxy );
		mutex_unlock(&p_statefulwriter->object_lock);

		add_readerproxy(p_statefulwriter, p_reader_proxy);
		// reader가 추가되 었으니, signal을 보내자..
		cond_signal(&p_statefulwriter->object_wait);
		b_ret = true;
	}else
	{
		//기존에 같은게 있을 경우 기존 것을 지우자...
		//이유는 proxy에 데이터가 있어서 업데이트가 안됨...
		//rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_reader_proxy);

		//mutex_lock(&p_statefulwriter->object_lock);
		//INSERT_ELEM( p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers,
		//				 p_statefulwriter->i_matched_readers, p_reader_proxy );
		//mutex_unlock(&p_statefulwriter->object_lock);
		
		rtps_readerproxy_destroy(p_reader_proxy);
	}

	trace_msg(NULL, TRACE_LOG, "rtps_statefulwriter_matched_reader_add : %s, %d", p_statefulwriter->p_dataWriter->p_topic->topic_name, p_statefulwriter->i_matched_readers);

	return b_ret;
	
}
*/



bool rtps_statefulwriter_matched_reader_add(rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy)
{
	bool b_ret = false;

	if(rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, p_reader_proxy->remote_reader_guid) == 0)
	{
		mutex_lock(&p_statefulwriter->object_lock);
		INSERT_ELEM( p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers,
						 p_statefulwriter->i_matched_readers, p_reader_proxy );
		p_statefulwriter->b_new_matched_readers = true;
		mutex_unlock(&p_statefulwriter->object_lock);

		monitoring__add_readerproxy(p_statefulwriter, p_reader_proxy);

		//status by jun
		change_PublicationMathced_status(p_statefulwriter->p_datawriter, p_statefulwriter->i_matched_readers, (InstanceHandle_t)p_reader_proxy);

		// reader가 추가되 었으니, signal을 보내자..
		
		//writer by jun
		//matched reader가 생기는 경우 수행 
		//job은 한번만 수행되며, 우선 순위는 2로 설정
		//job이 추가가 되면 Writer_Thread에 시그널을 보냄
		{
			data_t* p_jobdata=NULL;
			DataWriter				*p_dataWriter;

			p_jobdata = data_new(0);

			p_jobdata->p_rtps_writer = (rtps_writer_t *)p_statefulwriter;
			p_dataWriter = p_statefulwriter->p_datawriter;
	//		p_jobdata->b_thread_first = true;

			p_jobdata->initial_time = currenTime();

			p_jobdata->next_wakeup_time.sec = 0;
			p_jobdata->next_wakeup_time.nanosec = 0;
			p_jobdata->priority = 2;

			//timed_job_queue_time_compare_dataFifoPut_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
			trace_msg(NULL, TRACE_LOG, "cond_signal rtps_statefulwriter_matched_reader_add");
			cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
		}
	
//		cond_signal(&p_statefulwriter->object_wait);

		b_ret = true;
	}else
	{
		//기존에 같은게 있을 경우 기존 것을 지우자...
		//이유는 proxy에 데이터가 있어서 업데이트가 안됨...
		//rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_reader_proxy);

		//mutex_lock(&p_statefulwriter->object_lock);
		//INSERT_ELEM( p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers,
		//				 p_statefulwriter->i_matched_readers, p_reader_proxy );
		//mutex_unlock(&p_statefulwriter->object_lock);
		
		rtps_readerproxy_destroy(p_reader_proxy);
	}

	trace_msg(NULL, TRACE_LOG, "rtps_statefulwriter_matched_reader_add : %s, %d", p_statefulwriter->p_datawriter->p_topic->topic_name, p_statefulwriter->i_matched_readers);

	return b_ret;
	
}

void rtps_statefulwriter_matched_reader_remove(rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy)
{
	int i;
	int i_size;

	rtps_cachechange_t	*p_change_atom = NULL;
	cache_for_guid		*p_cache_for_guids_back = NULL;
	cache_for_guid		*p_cache_for_guids = NULL;

	if(p_reader_proxy == NULL) return;

	mutex_lock(&p_statefulwriter->object_lock);
	i_size = p_statefulwriter->i_matched_readers;
	for(i=i_size-1; i >=0;i--)
	{
		if(memcmp(&p_statefulwriter->pp_matched_readers[i]->remote_reader_guid, &p_reader_proxy->remote_reader_guid, sizeof(GUID_t)) == 0)
		{

			p_change_atom = (rtps_cachechange_t *)p_statefulwriter->p_writer_cache->p_head_first;


			while(p_change_atom)
			{

				
				p_cache_for_guids = (cache_for_guid *)p_change_atom->cache_for_guids.p_head_first;

				while(p_cache_for_guids)
				{
					p_cache_for_guids_back = (cache_for_guid *)p_cache_for_guids->p_next;
					if(memcmp(&p_cache_for_guids->guid, &p_reader_proxy->remote_reader_guid, sizeof(GUID_t)) == 0)
					{
						remove_linked_list(&p_change_atom->cache_for_guids, (linked_list_atom_t *)p_cache_for_guids);
						FREE(p_cache_for_guids);
					}

					p_cache_for_guids = p_cache_for_guids_back;

				}

				p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
			}


		//	int j;

			/*UpdateReaderrProxyFromrtpsWriter(p_statefulwriter->pp_matched_readers[i]);

			for (j=0; j < p_statefulwriter->pp_matched_readers[i]->i_changes_for_reader; j++)
			{
				p_statefulwriter->pp_matched_readers[i]->changes_for_reader[j]->b_isdisposed = true;
			}*/

			
			monitoring__delete_readerproxy(p_statefulwriter, p_reader_proxy);
			rtps_readerproxy_destroy(p_statefulwriter->pp_matched_readers[i]);

			REMOVE_ELEM(p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers, i);

			trace_msg(NULL, TRACE_LOG, "%17s's ReaderProxy destroyed. [H.C=%07d, i_matched_readers=%d]", p_statefulwriter->p_datawriter->p_topic->topic_name, p_statefulwriter->p_writer_cache->i_linked_size, p_statefulwriter->i_matched_readers);

			//status by jun
			change_publication_mathced_current_count_status(p_statefulwriter->p_datawriter, p_statefulwriter->i_matched_readers);

			
			break;
		}
	}

	mutex_unlock(&p_statefulwriter->object_lock);
}

rtps_readerproxy_t *rtps_statefulwriter_matched_reader_lookup(rtps_statefulwriter_t *p_statefulwriter, GUID_t a_reader_guid)
{
	int i_size = 0;
	int i;

	mutex_lock(&p_statefulwriter->object_lock);

	i_size = p_statefulwriter->i_matched_readers;

	
	for (i = i_size - 1; i >= 0; i--)
	{

		if (p_statefulwriter->pp_matched_readers[i]->is_destroy == true)
		{
			rtps_readerproxy_destroy2(p_statefulwriter->pp_matched_readers[i]);
			REMOVE_ELEM(p_statefulwriter->pp_matched_readers, p_statefulwriter->i_matched_readers, i);
			continue;
		}

		if(memcmp(&p_statefulwriter->pp_matched_readers[i]->remote_reader_guid, &a_reader_guid, sizeof(GUID_t)) == 0)
		{
			mutex_unlock(&p_statefulwriter->object_lock);
			return p_statefulwriter->pp_matched_readers[i];
		}
	}


	mutex_unlock(&p_statefulwriter->object_lock);
	return NULL;
}

bool rtps_statefulwriter_is_acked_by_all(rtps_readerproxy_t *p_rtps_readerproxy, rtps_cachechange_t *p_change)
{
	
	return false;
}

///////////////////////////////////////////////////////////



void SendHeartBeat(rtps_statefulwriter_t *p_rtps_statefulwriter, rtps_readerproxy_t *p_rtps_readerproxy, bool is_final)
{
	Heartbeat a_heartBeatData;
	DataWriter *p_dataWriter = p_rtps_statefulwriter->p_datawriter;
	//send


	if(p_rtps_readerproxy->reliablility_kind == BEST_EFFORT_RELIABILITY_QOS)
	{
		return;
	}



	//printf("send heartbeat\r\n");

	check_HeatBeat_Count(p_rtps_readerproxy);
	p_rtps_readerproxy->is_heartbeatsend_and_wait_for_ack = true;

	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		//printf("send heartbeat... pub\r\n");
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		a_heartBeatData.reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}else{
		//a_heartBeatData.reader_id.value = ENTITYID_UNKNOWN;
		//[120212,김경일] INFO_DST와 함께 보내는 HeartBeat는 ReaderId값을 채워서 보내야 하는 것 같다. (RTI 패킷분석을 참고)
		a_heartBeatData.reader_id.value = p_rtps_readerproxy->remote_reader_guid.entity_id;
	}

	memcpy(&a_heartBeatData.writer_id, &p_rtps_statefulwriter->guid.entity_id, 4);
	
	a_heartBeatData.first_sn.value = rtps_historycache_get_seq_num_min(p_rtps_statefulwriter->p_writer_cache);

//by kki...(reliability)
	// VOLATILE_DURABILITY인 경우에는 연결되기 전에 보냈던 data는 보낼 필요가 없다.
	{//by kki(130917)
		if (p_rtps_readerproxy->durability_kind == VOLATILE_DURABILITY_QOS)
		{
			int i_size, i;
			cache_for_guid* p_cache_for_guid;
			rtps_cachechange_t *p_change_atom = NULL;
			linked_list_head_t linked_list_head = rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_size, false);
			p_change_atom = (rtps_cachechange_t *)linked_list_head.p_head_first;

			i=0;
			i_size = linked_list_head.i_linked_size;

			while(p_change_atom)
			{
				p_cache_for_guid = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
				if (p_cache_for_guid->is_relevant == true) break;

				i++;
				p_change_atom = (rtps_cachechange_t *)p_change_atom->p_extra;
			}

		//	if (i_size && i < i_size) a_heartBeatData.first_sn.value = ((rtps_cachechange_t *)linked_list_head.p_head_last)->sequence_number;
			if (i_size && i < i_size) a_heartBeatData.first_sn.value = ((rtps_cachechange_t *)linked_list_head.p_head_first)->sequence_number;

			//FREE(pp_cachechanges);
		}
	}

	if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		a_heartBeatData.last_sn.value.high = 0;
		a_heartBeatData.last_sn.value.low = 0;
	}
	else
	{
		a_heartBeatData.last_sn.value = rtps_historycache_get_seq_num_max(p_rtps_statefulwriter->p_writer_cache);
	}

	a_heartBeatData.count.value = ++p_rtps_statefulwriter->heatbeat_count;


	//if(!(a_heartBeatData.first_sn.value.high == 0 && a_heartBeatData.first_sn.value.low == 0) || p_rtps_readerproxy->is_zero){

		if(p_rtps_statefulwriter->b_enable_multicast)
		{
			rtps_send_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_multicast_locator_list[0], a_heartBeatData, is_final);
		}else{

			rtps_send_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], a_heartBeatData, is_final);
		}

		p_rtps_readerproxy->is_zero = false;
	//}
}


static void Increase_bultin_heartbeatTime(rtps_statefulwriter_t *p_rtps_statefulwriter)
{
	//heartbeat 시간 설정...


	return;
	
	//이걸 왜하지. ㅜㅜ
	if (p_rtps_statefulwriter->heartbeat_period.sec == 1)
	{
		p_rtps_statefulwriter->heartbeat_period.sec = 10;
	}
	else if (p_rtps_statefulwriter->heartbeat_period.sec == 10)
	{
		p_rtps_statefulwriter->heartbeat_period.sec = 30;
	}
	else if (p_rtps_statefulwriter->heartbeat_period.sec == 30)
	{
		p_rtps_statefulwriter->heartbeat_period.sec = 60;
	}
	else if (p_rtps_statefulwriter->heartbeat_period.sec == 60)
	{
		p_rtps_statefulwriter->heartbeat_period.sec = 120;
	}
	else
	{
		p_rtps_statefulwriter->heartbeat_period.sec += 120;
	}

	return;
}


void ChangeStatefulwriterMulticast(rtps_statefulwriter_t *p_rtps_statefulwriter, rtps_cachechange_t *p_rtps_cachechange, Locator_t *p_matched_locator)
{
	int i_size = p_rtps_statefulwriter->i_matched_readers;
	int i=0;
	int j=0;
	cache_for_guid	*p_cache_atom = NULL;


	if(i_size >= 2){

		for(i=0; i < i_size; i++)
		{
			if(memcmp(p_rtps_statefulwriter->pp_matched_readers[i]->pp_multicast_locator_list[0], p_matched_locator, sizeof(Locator_t)) == 0)
			{
				
				bool updated = false;

				p_cache_atom = (cache_for_guid *)p_rtps_cachechange->cache_for_guids.p_head_first;

				while(p_cache_atom)
				{
					if(memcmp(&p_cache_atom->guid, &p_rtps_statefulwriter->pp_matched_readers[i]->remote_reader_guid, sizeof(GUID_t)) == 0)
					{
						p_cache_atom->is_relevant = true;
						p_cache_atom->status = UNDERWAY;
						updated = true;
						break;
					}

					p_cache_atom = (cache_for_guid *)p_cache_atom->p_next;
				}

				if(updated)
				{
					p_rtps_statefulwriter->pp_matched_readers[i]->i_for_writer_state = PUSHING_STATE;
					p_rtps_statefulwriter->pp_matched_readers[i]->flags = 0x00;
				}
			}
		}
	}

}

void reliable_statefulwriter_behavior( rtps_statefulwriter_t* p_rtps_statefulwriter )
{
	int i , size;
	int	i_unsentchanges = 0;
	rtps_readerproxy_t *p_rtps_readerproxy = NULL;
	//rtps_cachechange_t **p_rtps_cachechanges = NULL;
	rtps_cachechange_t *p_change = NULL;
	Time_t current;
	bool is_sentHeartbeat = false;
	int i_unacked = 0;
//	int j;
	int i_request_chages = 0;
	//rtps_cachechange_t **request_chaanges = NULL;
	DataWriter *p_dataWriter = p_rtps_statefulwriter->p_datawriter;
//time by jun
	Time_t message_time;

	//added by kyy
	bool correct_deadline = true;
	bool send_data = false;

	cache_for_guid		*cache_for_guids;



	/*if(p_rtps_statefulwriter->p_datawriter->id == 23)
	{
		return;
	}*/

	mutex_lock(&p_rtps_statefulwriter->object_lock);
	size = p_rtps_statefulwriter->i_matched_readers;
	current = currenTime();



	//if(p_rtps_statefulwriter->i_matched_readers == 0) trace_msg(NULL, TRACE_LOG, "in reliable_StatefulWriter_Behavior i_matched_readers : %d", p_rtps_statefulwriter->i_matched_readers);

	for (i = 0; i < size; i++)
	{
		p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];

		if (p_rtps_readerproxy->is_ignore == true) continue;

		send_data = false;

		

		do
		{

			/*if(p_rtps_statefulwriter->p_datawriter->id == 23)
		{
			break;
		}*/

			{
				linked_list_head_t linked_list_head;

				//rtps_cachechange_t **tmp_p_rtps_cachechanges = NULL;
				/*if (p_rtps_statefulwriter->push_mode == true)
				{
					linked_list_head = rtps_readerproxy_unsent_changes(p_rtps_readerproxy, &i_unsentchanges, true);
				}
				else
				{
					linked_list_head = rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_unsentchanges, true); 
				}*/


				if (p_rtps_statefulwriter->push_mode == true)
				{
					linked_list_head = rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_unsentchanges, true); 
				}
				else
				{
					linked_list_head = rtps_readerproxy_unsent_changes(p_rtps_readerproxy, &i_unsentchanges, true);
				}

				if (linked_list_head.i_linked_size)
				{
					send_data = true;
					//FREE(tmp_p_rtps_cachechanges);
				}
			}


			


			if (p_rtps_statefulwriter->push_mode == true)		//send all
			{
				linked_list_head_t linked_list_head;


				HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

				p_rtps_readerproxy->i_for_writer_state = ANNOUNCING_STATE;

				linked_list_head = rtps_readerproxy_unsent_changes_nolock(p_rtps_readerproxy, &i_unsentchanges, false);

				if((p_rtps_readerproxy->i_for_writer_state == ANNOUNCING_STATE) && linked_list_head.i_linked_size)
				{
					p_rtps_readerproxy->i_for_writer_state = PUSHING_STATE;
				}
				
				if(p_rtps_readerproxy->i_for_writer_state == PUSHING_STATE && (linked_list_head.i_linked_size == 0 || i_unsentchanges == 0))
				{
					p_rtps_readerproxy->i_for_writer_state = ANNOUNCING_STATE;
				}
				
				if(p_rtps_readerproxy->i_for_writer_state == PUSHING_STATE && linked_list_head.i_linked_size)
				{
					/*
						8.4.9.2.4 Transition T4

						This transition is triggered by the guard condition [RP::can_send() == true] indicating that the RTPS Writer
						‘the_rtps_writer’ has the resources needed to send a change to the RTPS Reader represented by the ReaderProxy
						‘the_reader_proxy.’

						The transition performs the following logical actions in the virtual machine:

						a_change := the_reader_proxy.next_unsent_change();
						a_change.status := UNDERWAY;
						if (a_change.is_relevant) {
							DATA = new DATA(a_change);
							IF (the_reader_proxy.expectsInlineQos) {
								DATA.inline_qos := the_rtps_writer.related_dds_writer.qos;
							}
							DATA.reader_id := ENTITYID_UNKNOWN;
							send DATA;
						}
						else {
							GAP = new GAP(a_change.sequence_number);
							GAP.reader_id := ENTITYID_UNKNOWN;
							send GAP;
						}
					*/

					//이부분 수행에 있어서 안좋은 점이 있어서 스펙대로는 안함..

					

				
					//p_change = rtps_readerproxy_next_unsent_chage(p_rtps_readerproxy);
					{
						rtps_cachechange_t *p_change_atom = NULL;
						p_change_atom = (rtps_cachechange_t *)linked_list_head.p_head_first;
						
						while(p_change_atom)
						{
							p_change = p_change_atom;

							message_time = p_change->source_timestamp;
							/*if(p_rtps_statefulwriter->pushMode){
								p_change->is_relevant = true;
							}else{
								*p_change->status = UNDERWAY;
							}*/

							

							cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change);

							

							////////cache_for_guids->is_relevant = true;//by kki(130917)
							cache_for_guids->status = UNDERWAY;

							if(cache_for_guids->is_relevant)
							{
								bool is_send_data_and_heartbeat = false;
								DataFull *p_data = rtps_data_new(p_change);

								//added by kyy(Presentation QoS Writer Side)
								//InlineQoS 입력조건 설정
								//if(1){//p_rtps_readerproxy->expectsInlineQos){
								if (p_rtps_readerproxy->p_rtps_writer->is_builtin == false)
								{
									p_rtps_readerproxy->expects_inline_qos = true;
								}
								////////////////////////////////////////////////////////////////////////

								if (p_rtps_readerproxy->expects_inline_qos)
								{
									SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
									ParameterWithValue		*p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_serializedPlayloadwriter->entity_key_guid);

									insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);


									/*INSERT_ELEM( p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters,
										p_data->inline_qos.i_parameters, p_parameter);*/

									//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
									//Make COHERENT SET PARAMETER
									if (p_rtps_readerproxy->p_rtps_writer->is_builtin != true)
									{
										qosim_process_coherent_set(p_rtps_readerproxy, p_change);										
										qosim_make_coherent_set_parameter(p_data,&p_rtps_readerproxy->p_rtps_writer->p_writer_cache->coherent_set_number);										
									}
									///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

									p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);

									insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
								}

								p_data->octets_to_inline_qos = 16;
								p_data->extra_flags = 0;
								p_data->reader_id.value = ENTITYID_UNKNOWN;


								if(p_rtps_statefulwriter->last_change_sequence_number.low % p_rtps_statefulwriter->p_writer_cache->history_max_length == 0)
								{
									is_send_data_and_heartbeat = true;
									
								}

								//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								//HeartBeat 전송
								if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->p_publisher->begin_coherent == false && p_change->p_data_value->p_value == NULL)
								{
									is_send_data_and_heartbeat = true;
									//SendHeartBeat(p_rtps_statefulwriter,p_rtps_readerproxy,false);
									//printf("Send HEART BEAT Message");
								}
								//////////////////////////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////




								if(is_send_data_and_heartbeat)
								{
									if(p_rtps_statefulwriter->b_enable_multicast)
									{
										rtps_send_data_and_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy, *p_rtps_readerproxy->pp_multicast_locator_list[0], p_data, true, false, message_time);
									}else{
									
										rtps_send_data_and_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, false, message_time);
									}

								}else{

									/*if(p_rtps_statefulwriter->p_datawriter->id == 23)
									{
										p_rtps_statefulwriter->p_datawriter->id = 23;
									}*/

									if(p_rtps_statefulwriter->b_enable_multicast)
									{
										rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_multicast_locator_list[0], p_data, true, message_time);
									}else{
									
										rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, message_time);
									}
								}

								

							
								p_rtps_readerproxy->i_for_writer_state = PUSHING_STATE;

								send_data = true;

								
								//p_rtps_readerproxy->flags = 0x00;


								if(p_rtps_statefulwriter->b_enable_multicast)
								{
									//멀티캐스트가 같은 proxy의 cache_for_guids의 상태를 변경한다.

									ChangeStatefulwriterMulticast(p_rtps_statefulwriter, p_change, p_rtps_readerproxy->pp_multicast_locator_list[0]);
								
								}

								if(is_send_data_and_heartbeat)
								{
									//block_write_for_waiting_ack(p_rtps_statefulwriter);
								}

							}

							

							p_change_atom = (rtps_cachechange_t *)p_change_atom->p_extra;
						}
						//added_by_kyy(DataWriter Deadline QoS)
						//Deadline 검사 후 base_time 수정
						if(p_rtps_readerproxy->start == 0)
						{
							current = currenTime();
							p_rtps_readerproxy->deadline_base_time.sec = current.sec;
							p_rtps_readerproxy->deadline_base_time.nanosec = current.nanosec;

//							printf("Deadline first basetime : %d\n", p_rtps_readerproxy->deadline_base_time.sec);
							//			printf("Deadline first basetime : %d\n", p_rtps_readerproxy->deadline_base_time.nanosec);
							p_rtps_readerproxy->start++;
						}
						correct_deadline = qos_compare_deadline(current_duration(), &(p_rtps_readerproxy->deadline_base_time), p_rtps_statefulwriter->p_datawriter->datawriter_qos.deadline.period);
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						//status by jun 
						if (correct_deadline == false)
						{
							change_OfferedDeadlineMissed_status(p_rtps_statefulwriter->p_datawriter, 0);
						}
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						//added by kyy(DataWriter Lifespan QoS)
						//qosim_process_lifespan_for_writer_historycache_stateful(p_rtps_statefulwriter);

						
					}

					
				}
				HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

				
			}



			
			if( p_rtps_readerproxy->i_for_writer_state == ANNOUNCING_STATE)
			{
				
				linked_list_head_t linked_list_head = rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_unacked, true);

				if(linked_list_head.i_linked_size)
				{
					p_rtps_readerproxy->i_for_writer_state = ANNOUNCING_STATE;
				}else
				{
					p_rtps_readerproxy->i_for_writer_state = IDLE_STATE;
				}

				//FREE(pp_rtps_cachechange);
				
			}

			

			//if(i == 1)
			//{
			//	i = i;
			//}

			if(p_rtps_readerproxy->i_for_writer_fromReader_state == WAITING_STATE)
			{
				int i_requested = 0;
				linked_list_head_t linked_list_head = rtps_readerproxy_requested_changes(p_rtps_readerproxy, &i_requested);
			
				//if(i == 1)
				//{
				//	i = i;
				//}

				if(linked_list_head.i_linked_size)
				{
					p_rtps_readerproxy->i_for_writer_fromReader_state = MUST_REPAIR_STATE;
				}
				//FREE(pp_request_cachechange);
			}

			if(p_rtps_readerproxy->i_for_writer_fromReader_state == MUST_REPAIR_STATE)
			{

				//if(i == 1)
				//{
				//	i = i;
				//}

				if (p_rtps_statefulwriter->nack_response_delay.sec < (current.sec - p_rtps_statefulwriter->last_nack_time.sec))
				{
					p_rtps_readerproxy->i_for_writer_fromReader_state = REPAIRING_STATE;
					if (i == size-1) p_rtps_statefulwriter->last_nack_time = currenTime();

				}
				else if (p_rtps_statefulwriter->nack_response_delay.sec == (current.sec - p_rtps_statefulwriter->last_nack_time.sec))
				{
					if (p_rtps_statefulwriter->nack_response_delay.nanosec < (current.nanosec - p_rtps_statefulwriter->last_nack_time.nanosec))
					{
						p_rtps_readerproxy->i_for_writer_fromReader_state = REPAIRING_STATE;
						if (i == size-1) p_rtps_statefulwriter->last_nack_time = currenTime();
					}
				}
				//else{
				//	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
				//	{
				//		printf("%d, %d %d\r\n", p_rtps_statefulwriter->nackResponseDelay.sec , current.sec - p_rtps_statefulwriter->last_nack_time.sec, current.nanosec - p_rtps_statefulwriter->last_nack_time.nanosec);
				//		//p_rtps_readerproxy->i_for_writer_fromReader_state = REPAIRING_STATE;
				//		//p_rtps_statefulwriter->last_nack_time = currenTime();
				//	}
				//}
			}


			if(p_rtps_readerproxy->i_for_writer_fromReader_state == REPAIRING_STATE)
			{
				
				//if(i == 1)
				//{
				//	i = i;
				//}

				linked_list_head_t linked_list_head = rtps_readerproxy_requested_changes(p_rtps_readerproxy, &i_request_chages);
				if(linked_list_head.i_linked_size == 0)
				{
					p_rtps_readerproxy->i_for_writer_fromReader_state = WAITING_STATE;
				}
				

			}

			if (!p_rtps_statefulwriter->push_mode && p_rtps_readerproxy->i_for_writer_fromReader_state == REPAIRING_STATE)
			{
				/*
					8.4.9.2.12 Transition T12
					This transition is triggered by the guard condition [RP::can_send() == true] indicating that the RTPS Writer
					‘the_rtps_writer’ has the resources needed to send a change to the RTPS Reader represented by the ReaderProxy
					‘the_reader_proxy.’
					The transition performs the following logical actions in the virtual machine:

					a_change := the_reader_proxy.next_requested_change();
					a_change.status := UNDERWAY;

					if (a_change.is_relevant) {
						DATA = new DATA(a_change, the_reader_proxy.remoteReaderGuid);
						IF (the_reader_proxy.expectsInlineQos) {
							DATA.inline_qos := the_rtps_writer.related_dds_writer.qos;
						}
						send DATA;
					}
					else {
						GAP = new GAP(a_change.sequence_number, the_reader_proxy.remoteReaderGuid);
						send GAP;
					}
				*/
				//if(i == 1)
				//{
				//	i = i;
				//}

				HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

				p_change = rtps_readerproxy_next_requested_change(p_rtps_readerproxy);

				{
					SerializedPayloadForWriter *p_serializedPlayloadwriter = NULL;
					bool is_builtin = false;
					rtps_cachechange_t *p_change_atom = NULL;

					
					// history_cache에서 삭제했을수도 있기 때문에 lock을 하고 다시 가져온다.
					linked_list_head_t linked_list_head = rtps_readerproxy_requested_changes_unlock(p_rtps_readerproxy, &i_request_chages);

					p_change_atom = (rtps_cachechange_t *)linked_list_head.p_head_first;

					while(p_change_atom)
					{
						p_change = p_change_atom;
						cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change);

						////////cache_for_guids->is_relevant = true; // 이건 단지 임시로..//by kki(130917)
						//p_change->status = UNDERWAY;
						//if(i == size-1) *p_change->status = UNACKNOWLEDGED;

						cache_for_guids->status = UNDERWAY;
						//p_change->status = UNACKNOWLEDGED;
						if(cache_for_guids->is_relevant)
						{
							DataFull *p_data = rtps_data_new(p_change);
							message_time = p_change->source_timestamp;
							p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
							if (p_rtps_readerproxy->expects_inline_qos)
							{
								ParameterWithValue		*p_parameter;
								
								p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_serializedPlayloadwriter->entity_key_guid);

								insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);

								//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								//Make COHERENT SET PARAMETER
								if (p_rtps_readerproxy->p_rtps_writer->is_builtin != true)
								{
									qosim_process_coherent_set(p_rtps_readerproxy, p_change);										
									qosim_make_coherent_set_parameter(p_data,&p_rtps_readerproxy->p_rtps_writer->p_writer_cache->coherent_set_number);										
								}
								///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

								p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);

								insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
							}

							p_data->octets_to_inline_qos = 16;

							p_data->extra_flags = 0;
							p_data->reader_id.value = ENTITYID_UNKNOWN;
							if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
							{
								is_builtin = true;
							}
							else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
							{
								is_builtin = true;
							}
							else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
							{
								is_builtin = true;
							}
							else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
							{
								is_builtin = true;
							}

							if (!is_builtin && p_rtps_statefulwriter && p_serializedPlayloadwriter && p_rtps_statefulwriter->fragment_size && p_serializedPlayloadwriter->i_size > p_rtps_statefulwriter->fragment_size)
							{
								rtps_send_data_frag_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, message_time);
								//rtps_sendDataFragTo((rtps_writer_t *)p_rtps_statefulwriter, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data);
							}
							else
							{
								rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, message_time);
							}
							p_rtps_readerproxy->i_for_writer_fromReader_state = REPAIRING_STATE;
							cache_for_guids->is_relevant = false;

						}
						else
						{
							Gap a_gap;
							memset(a_gap.gap_list.bitmap, '\0', sizeof(a_gap.gap_list.bitmap));
							a_gap.reader_id.value = ENTITYID_UNKNOWN;
							a_gap.writer_id.value = p_rtps_statefulwriter->guid.entity_id;
							a_gap.gap_start.value = p_change->sequence_number;
							a_gap.gap_list.bitmap_base = sequnce_number_inc(p_change->sequence_number);
							a_gap.gap_list.numbits = 0;
							//a_gap.gap_list.bitmap[0] |= 1 << 31;
							rtps_send_gap_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0],a_gap, p_change->sequence_number);
						}

						send_data = true;

						//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//HeartBeat 전송
						if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->p_publisher->begin_coherent == false && p_change->p_data_value->p_value == NULL)
						{
							SendHeartBeat(p_rtps_statefulwriter,p_rtps_readerproxy,false);
							//printf("Send HEART BEAT Message");
						}
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						p_change_atom = (rtps_cachechange_t *)p_change_atom->p_extra;
					}

					
				}

				HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
				
			}

//			FREE(p_rtps_cachechanges);
//			FREE(request_chaanges);
			break;
			
		}while(p_rtps_readerproxy->i_for_writer_state != IDLE_STATE);

		//////////////////////////////////////
		if( /*(state == ANNOUNCING_STATE) && *//*p_rtps_cachechanges &&*/ p_rtps_readerproxy && (p_rtps_readerproxy->i_unicast_locator_list > 0))
		{
			bool is_builtin = false;

			if (p_rtps_statefulwriter->p_datawriter->builtin_type)
			{
				is_builtin = true;

			}

			if (p_rtps_statefulwriter->p_datawriter->builtin_type == BUILTIN_LIVELINESSP2P_TYPE)
			{
				is_builtin = true;
			}
		
			/*if((memcmp(&p_rtps_statefulwriter->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				is_builtin = true;
			}else if((memcmp(&p_rtps_statefulwriter->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
			{
				is_builtin = true;
			}else if((memcmp(&p_rtps_statefulwriter->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
			{
				is_builtin = true;
			}else if((memcmp(&p_rtps_statefulwriter->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
			{
				is_builtin = true;
			}*/

			

			if (p_rtps_statefulwriter->heartbeat_period.sec < (current.sec - p_rtps_statefulwriter->last_send_heartbeat.sec) || 
				(p_rtps_statefulwriter->heartbeat_period.sec == (current.sec - p_rtps_statefulwriter->last_send_heartbeat.sec) && 
				p_rtps_statefulwriter->heartbeat_period.nanosec < (current.nanosec - p_rtps_statefulwriter->last_send_heartbeat.nanosec)))
			{
				bool is_final = true;
				rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_unacked, true);
				//request_chaanges = rtps_readerproxy_requested_changes(p_rtps_readerproxy, &i_request_chages);

				if (i_unacked || i_request_chages)
				{
					is_final = false;
				}

				//send
				if (send_data == true)
				{
					SendHeartBeat(p_rtps_statefulwriter, p_rtps_readerproxy, false);
					p_rtps_readerproxy->flags = 0;
					is_sentHeartbeat = true;

				}else if (p_rtps_readerproxy->flags == 0)
				{
					SendHeartBeat(p_rtps_statefulwriter, p_rtps_readerproxy, false);
					is_sentHeartbeat = true;
				}
				else {


					if (p_rtps_statefulwriter->p_datawriter->builtin_type == BUILTIN_LIVELINESSP2P_TYPE)
					{
					
						if (p_rtps_readerproxy->flags & 0x01)
						{
							SendHeartBeat(p_rtps_statefulwriter, p_rtps_readerproxy, false);
							is_sentHeartbeat = true;
							//	p_rtps_readerproxy->flags = 0;
						}
					}
					else {
						if (p_rtps_readerproxy->flags & 0x02 /*&& p_rtps_readerproxy->is_zero != 0*/)
						{
							// heart beat 보내지 않음
							//					SendHeartBeat(p_rtps_statefulwriter, p_rtps_readerproxy, false);
						}
						else if (p_rtps_readerproxy->flags & 0x01)
						{
							SendHeartBeat(p_rtps_statefulwriter, p_rtps_readerproxy, false);
							is_sentHeartbeat = true;
							//	p_rtps_readerproxy->flags = 0;
						}

					}


					


				}

				
				
				if (is_builtin) Increase_bultin_heartbeatTime(p_rtps_statefulwriter);

				if (send_data)
				{
					//reset
					p_rtps_readerproxy->flags = 0;
				}


			}
			
		}

	}

	if(p_rtps_statefulwriter->behavior_type == STATEFUL_TYPE)
	{
		p_rtps_statefulwriter->b_new_matched_readers = false;
	}

	if(is_sentHeartbeat)
	{
		p_rtps_statefulwriter->last_send_heartbeat = current;
	}

	mutex_unlock(&p_rtps_statefulwriter->object_lock);
}

void best_effort_statefulwriter_behavior( rtps_statefulwriter_t* p_rtps_statefulwriter )
{
	int i , size;
	int	i_unsentchanges = 0;
	rtps_readerproxy_t *p_rtps_readerproxy = NULL;
	//rtps_cachechange_t **p_rtps_cachechanges = NULL;
	rtps_cachechange_t *p_change = NULL;
	Time_t current;
	bool is_sentHeartbeat = false;
	int j;
	Locator_t			a_nulllocator;
	Locator_t			a_sendTolocator;

	//added by kyy
	bool correct_deadline = true;
	cache_for_guid *cache_for_guids;
	Time_t message_time;
	
	memset(&a_nulllocator,0,sizeof(Locator_t));
	memset(&a_sendTolocator,0,sizeof(Locator_t));
	
	mutex_lock(&p_rtps_statefulwriter->object_lock);
	size = p_rtps_statefulwriter->i_matched_readers;
	current = currenTime();

	/*if(size == 0)
	{
		printf("size(0) %s\r\n", p_rtps_statefulwriter->p_dataWriter->p_topic->topic_name);
	}*/

	if(size > 0)
	//for(i=0; i < size ; i++)
	{
		i = 0;

		p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];
		do{

			linked_list_head_t linked_list_head = rtps_readerproxy_unsent_changes(p_rtps_readerproxy, &i_unsentchanges, true);

			/*if((p_rtps_readerproxy->i_for_writer_state == IDLE_STATE) && p_rtps_cachechanges)
			{
				p_rtps_readerproxy->i_for_writer_state = PUSHING_STATE;
			}
			
			if(p_rtps_readerproxy->i_for_writer_state == PUSHING_STATE && (p_rtps_cachechanges == NULL || i_unsentchanges == 0))
			{
				p_rtps_readerproxy->i_for_writer_state = IDLE_STATE;
			}*/
			
			if(/*p_rtps_readerproxy->i_for_writer_state == PUSHING_STATE && */linked_list_head.i_linked_size)
			{
				////////////////////////////////////////
				////////////////////////////////////////
				HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
				p_change = rtps_readerproxy_next_unsent_chage(p_rtps_readerproxy);

				message_time = p_change->source_timestamp;

				cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change);

				if (p_change && cache_for_guids->is_relevant)
				{
					SerializedPayloadForWriter *p_serializedPlayloadwriter = NULL;
					Locator_t	*p_sentlocator = NULL;
					Locator_t	**pp_sentlocators = NULL;
					int			i_sentlocators = 0;
					int			k;
					bool		b_found_same_locator = false;
					
					

					for(j=0; j < size ; j++){
						DataFull *p_data;

						b_found_same_locator = false;

						if(p_rtps_statefulwriter->pp_matched_readers[j]->is_ignore == true) continue;


						if(memcmp(&a_nulllocator, p_rtps_statefulwriter->pp_matched_readers[j]->pp_multicast_locator_list[0], sizeof(Locator_t)) == 0)
						{
							if(memcmp(&a_nulllocator, p_rtps_statefulwriter->pp_matched_readers[j]->pp_unicast_locator_list[0], sizeof(Locator_t)) == 0)
							{
								continue;
							}
							
							memcpy(&a_sendTolocator, p_rtps_statefulwriter->pp_matched_readers[j]->pp_unicast_locator_list[0], sizeof(Locator_t));

						}else {
							memcpy(&a_sendTolocator, p_rtps_statefulwriter->pp_matched_readers[j]->pp_multicast_locator_list[0], sizeof(Locator_t));
						}


						for(k=0; k < i_sentlocators; k++)
						{
							if(memcmp(pp_sentlocators[k], &a_sendTolocator, sizeof(Locator_t)) == 0)
							{
								b_found_same_locator = true;
								break;
							}
						}


						
						if(b_found_same_locator)
						{
							continue;
						}


						p_sentlocator = malloc(sizeof(Locator_t));

						memset(p_sentlocator, 0, sizeof(Locator_t));

						memcpy(p_sentlocator, &a_sendTolocator, sizeof(Locator_t));

						INSERT_ELEM(pp_sentlocators, i_sentlocators, i_sentlocators, p_sentlocator);

						p_data = rtps_data_new(p_change);

						p_data->octets_to_inline_qos = 0;
						p_data->inline_qos.p_head_first = NULL;
						p_data->inline_qos.p_head_last = NULL;
						p_data->inline_qos.i_linked_size =0;

						p_data->extra_flags = 0;

						p_data->reader_id.value = ENTITYID_UNKNOWN;
						//p_data->reader_id.value = p_rtps_statefulwriter->pp_matched_readers[j]->remoteEndpointGuid.entity_id;
						//if(p_rtps_readerproxy->expectsInlineQos)
						{
							SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
							ParameterWithValue		*p_parameter = NULL;

							if(p_rtps_statefulwriter->topic_kind == WITH_KEY){
							
								if(p_change->p_org_message != NULL && p_change->p_org_message->i_key)
								{
									p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, p_change->p_org_message->key_guid);

									insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
								}
							}

							if(p_change->kind == NOT_ALIVE_DISPOSED)
							{
								char status_info[] = {0x00,0x00,0x00,0x01};
								p_parameter = rtps_make_parameter(PID_STATUS_INFO, 4, status_info);
								insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
							}else if(p_change->kind == NOT_ALIVE_UNREGISTERED)
							{
								char status_info[] = {0x00,0x00,0x00,0x02};
								p_parameter = rtps_make_parameter(PID_STATUS_INFO, 4, status_info);
								insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
							}


							p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);

							insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
						}
							
						cache_for_guids->status = UNDERWAY;
						
						p_data->octets_to_inline_qos = 16;

						p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;

						//printf("p_data->p_serialized_data : %p\r\n",p_data->p_serialized_data);
						//printf("p_rtps_statefulwriter->fragment_size : %d\r\n", p_rtps_statefulwriter->fragment_size);
						//printf("p_serializedPlayloadwriter : %d\r\n", p_serializedPlayloadwriter->i_size);
						

						//if(p_change->kind == ALIVE){

						if(p_serializedPlayloadwriter == NULL)
						{
							destory_datafull(p_data);
						}else{
							if ((p_rtps_statefulwriter->fragment_size != 0) && ((p_serializedPlayloadwriter->i_size - 4) > p_rtps_statefulwriter->fragment_size))
							{
								rtps_send_data_frag_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, a_sendTolocator, p_data, message_time);
								//rtps_send_data_frag_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, *p_rtps_statefulwriter->pp_matched_readers[j]->pp_unicast_locator_list[0], p_data);
							}
							else
							{
								rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, a_sendTolocator, p_data, false, message_time);
								//rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, *p_rtps_statefulwriter->pp_matched_readers[j]->pp_unicast_locator_list[0], p_data);
							
							}
						//}else if(p_change->kind == NOT_ALIVE_DISPOSED){


							//p_data->p_serialized_data = (SerializedPayload	*)rtps_generate_disposeunregister(p_rtps_statefulwriter->p_dataWriter, (rtps_writer_t *)p_rtps_statefulwriter, (rtps_writer_t *)p_rtps_statefulwriter, true, false);
							//rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, a_sendTolocator, p_data, false);
						
						
						//}else if(p_change->kind == NOT_ALIVE_UNREGISTERED){
							//TODO[120821,김경일] NOT_ALIVE_DISPOSED와 NOT_ALIVE_UNREGISTERED를 함께 설정하고 있음.
							//p_data->p_serialized_data = (SerializedPayload	*)rtps_generate_disposeunregister(p_rtps_statefulwriter->p_dataWriter, (rtps_writer_t *)p_rtps_statefulwriter, (rtps_writer_t *)p_rtps_statefulwriter, true, true);
							//rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_statefulwriter->pp_matched_readers[j]->remote_reader_guid, a_sendTolocator, p_data, false);
						//}
						}
					}

					while(i_sentlocators)
					{
						FREE(pp_sentlocators[0]);
						REMOVE_ELEM( pp_sentlocators, i_sentlocators, 0);
					}
				}
				for(j=0; j < size ; j++){
					p_rtps_statefulwriter->pp_matched_readers[j]->i_for_writer_state = PUSHING_STATE;
				}

				HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
			}

//			FREE(p_rtps_cachechanges);
//			break;	
		//}while(p_rtps_readerproxy->i_for_writer_state != IDLE_STATE);
		}while(i_unsentchanges != 0);

		//added_by_kyy(DataWriter Deadline QoS)
		//Deadline 검사 후 base_time 수정
		if(p_rtps_readerproxy->start == 0)
		{
			current = currenTime();
			p_rtps_readerproxy->deadline_base_time.sec = current.sec;
			p_rtps_readerproxy->deadline_base_time.nanosec = current.nanosec;

//			printf("Deadline first basetime : %d\n", p_rtps_readerproxy->deadline_base_time.sec);
//			printf("Deadline first basetime : %d\n", p_rtps_readerproxy->deadline_base_time.nanosec);
			p_rtps_readerproxy->start++;
		}
		correct_deadline = qos_compare_deadline(current_duration(), &(p_rtps_readerproxy->deadline_base_time), p_rtps_statefulwriter->p_datawriter->datawriter_qos.deadline.period);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//status by jun 
		if (correct_deadline == false)
		{
			change_OfferedDeadlineMissed_status(p_rtps_statefulwriter->p_datawriter, 0);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	}
	//added by kyy
//	qosim_process_lifespan_for_writer_historycache_stateful(p_rtps_statefulwriter);

	mutex_unlock(&p_rtps_statefulwriter->object_lock);

}


void process_lifespan_for_writer_historycache(rtps_statefulwriter_t *p_statefulwriter)
{

	rtps_cachechange_t *p_change_atom = NULL;
	Duration_t send_time = current_duration();
	HISTORYCACHE_LOCK(p_statefulwriter->p_writer_cache);

	p_change_atom = (rtps_cachechange_t *)p_statefulwriter->p_writer_cache->p_head_last;

	while(p_change_atom)
	{
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
