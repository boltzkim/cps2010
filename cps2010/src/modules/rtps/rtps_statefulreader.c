/*
	RTSP StatelessReader class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"

extern Duration_t time_addition_d(const Duration_t augend_time, const Duration_t addend_time);
extern Duration_t time_subtraction_d(const Duration_t minuend_time, const Duration_t subtrahend_time);
extern bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rtps_statefulreader_t *rtps_statefulreader_new()
{
	rtps_statefulreader_t *p_rtps_statefulreader = malloc(sizeof(rtps_statefulreader_t));

	memset(p_rtps_statefulreader, 0, sizeof(rtps_statefulreader_t));

	rtps_reader_init((rtps_reader_t *)p_rtps_statefulreader);

	p_rtps_statefulreader->behavior_type = STATEFUL_TYPE;

	p_rtps_statefulreader->pp_matched_writers = NULL;
	p_rtps_statefulreader->i_matched_writers = 0;
	return p_rtps_statefulreader;
}

void rtps_statefulreader_destory(rtps_statefulreader_t *p_rtps_statefulreader)
{
	if(p_rtps_statefulreader)
	{
		READER_LOCK((rtps_reader_t*)p_rtps_statefulreader);
		while(p_rtps_statefulreader->i_matched_writers)
		{
			rtps_writerproxy_destroy(p_rtps_statefulreader->pp_matched_writers[0]);
			REMOVE_ELEM( p_rtps_statefulreader->pp_matched_writers, p_rtps_statefulreader->i_matched_writers, 0);

		}
		READER_UNLOCK((rtps_reader_t*)p_rtps_statefulreader);
	}
}


bool rtps_statefulreader_matched_writer_add(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_writer_proxy)
{
	bool b_ret = false;

	if(rtps_statefulreader_matched_writer_lookup(p_statefulreader, p_writer_proxy->remote_writer_guid) == 0)
	{
//		READER_LOCK((rtps_reader_t*)p_statefulreader);
		INSERT_ELEM( p_statefulreader->pp_matched_writers, p_statefulreader->i_matched_writers,
						 p_statefulreader->i_matched_writers, p_writer_proxy );
		p_statefulreader->p_datareader->status.liveliness_changed.alive_count++;
//		READER_UNLOCK((rtps_reader_t*)p_statefulreader);

		monitoring__add_writerproxy(p_statefulreader, p_writer_proxy);
		
		//status by jun				
		change_SubscriberMathed_status(p_statefulreader->p_datareader, p_statefulreader->i_matched_writers, (InstanceHandle_t)p_writer_proxy);

		// reader가 추가되 었으니, signal을 보냄..
		cond_signal(&p_statefulreader->object_wait);
		b_ret = true;
	}else
	{
		//기존 것은 삭제, writer_proxy에 데이터가 남아 있음.
		//
		/*rtps_statefulreader_matched_writer_remove(p_statefulreader, p_writer_proxy);
		READER_LOCK((rtps_reader_t*)p_statefulreader);
		INSERT_ELEM( p_statefulreader->pp_matched_writers, p_statefulreader->i_matched_writers,
						 p_statefulreader->i_matched_writers, p_writer_proxy );
		READER_UNLOCK((rtps_reader_t*)p_statefulreader);*/

		//trace_msg(NULL, TRACE_LOG, "rtps_statefulreader_matched_writer_add:rtps_writerproxy_destroy[%s:%d]",  p_statefulreader->p_datareader->p_topic->topic_name, 
		//	p_statefulreader->i_matched_writers);
		rtps_writerproxy_destroy(p_writer_proxy);
		// reader가 추가되 었으니, signal을 보냄..
		cond_signal(&p_statefulreader->object_wait);


		//printf("destroy\r\n");
	}

	// Reader 와 Writer가 연결 맺은 직후에, owner_writer 를 설정한다.
	qos_insert_exclusive_writer((rtps_reader_t*)p_statefulreader, p_writer_proxy);//by kki (ownership)

	return b_ret;
}

void rtps_statefulreader_matched_writer_remove(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_writer_proxy)
{
	int i;
	int i_size;
	rtps_cachechange_t	*p_change_atom = NULL;
	rtps_cachechange_t	*p_change_atom_back = NULL;

	if(p_writer_proxy == NULL) return;

	qos_remove_exclusive_writer((rtps_reader_t*)p_statefulreader, p_writer_proxy);//by kki (ownership)

	READER_LOCK((rtps_reader_t*)p_statefulreader);
	i_size = p_statefulreader->i_matched_writers;
	for(i=i_size-1; i >= 0;i--)
	{
		if(memcmp(&p_statefulreader->pp_matched_writers[i]->remote_writer_guid, &p_writer_proxy->remote_writer_guid, sizeof(GUID_t)) == 0)
		{


			p_change_atom = (rtps_cachechange_t *)p_statefulreader->p_reader_cache->p_head_first;


			while(p_change_atom)
			{
				p_change_atom_back = (rtps_cachechange_t *)p_change_atom->p_next;
				if(p_change_atom->p_rtps_writerproxy == p_writer_proxy)
				{
					remove_linked_list((linked_list_head_t *)p_statefulreader->p_reader_cache, (linked_list_atom_t *)p_change_atom);
					rtps_cachechange_ref(p_change_atom, true, false);
					rtps_cachechange_destory(p_change_atom);
				}

				p_change_atom = p_change_atom_back;
			}
			/*int j;

			UpdateWriterProxyFromrtpsReader(p_statefulreader->pp_matched_writers[i]);

			for (j=0; j < p_statefulreader->pp_matched_writers[i]->i_changes_from_writer; j++)
			{
				p_statefulreader->pp_matched_writers[i]->changes_from_writer[j]->b_isdisposed = true;
			}*/

			if (p_writer_proxy->b_liveliness_alive)
			{
				p_statefulreader->p_datareader->status.liveliness_changed.alive_count--;
			}
			else
			{
				p_statefulreader->p_datareader->status.liveliness_changed.not_alive_count--;
			}
			REMOVE_ELEM( p_statefulreader->pp_matched_writers, p_statefulreader->i_matched_writers, i);

			monitoring__delete_writerproxy(p_statefulreader, p_writer_proxy);

			rtps_writerproxy_destroy(p_writer_proxy);
			trace_msg(NULL, TRACE_LOG2, "%17s's WriterProxy destroyed. [H.C=%07d, i_matched_writers=%d]", p_statefulreader->p_datareader->p_topic->topic_name, p_statefulreader->p_reader_cache->i_linked_size, p_statefulreader->i_matched_writers);
			
			//status by jun
			change_SubscriberMathed_CurrentCount_status(p_statefulreader->p_datareader, p_statefulreader->i_matched_writers);

			
			break;
		}
	}

	READER_UNLOCK((rtps_reader_t*)p_statefulreader);

}

rtps_writerproxy_t* rtps_statefulreader_matched_writer_lookup(rtps_statefulreader_t* p_statefulreader, GUID_t a_writer_guid)
{
	int i_size = 0;
	int i;

	if (p_statefulreader == NULL)
	{
		return NULL;
	}
	/*
	if(p_statefulreader->object_lock.csection.LockCount >= 1){
		
		printf("lock count %d, %d =================\r\n",p_statefulreader->object_lock.csection.LockCount ,p_statefulreader->object_lock.csection.OwningThread);
		i_size = 0;
	}*/

	READER_LOCK((rtps_reader_t*)p_statefulreader);

	i_size = p_statefulreader->i_matched_writers;

	for (i = i_size - 1; i >= 0; i--)
	{

		if (p_statefulreader->pp_matched_writers[i]->is_destroy == true)
		{
			rtps_writerproxy_destroy2(p_statefulreader->pp_matched_writers[i]);
			REMOVE_ELEM(p_statefulreader->pp_matched_writers, p_statefulreader->i_matched_writers, i);
			trace_msg(NULL, TRACE_LOG, "remove................... %d", p_statefulreader->i_matched_writers);
			continue;
		}


		if (memcmp(&p_statefulreader->pp_matched_writers[i]->remote_writer_guid, &a_writer_guid, sizeof(GUID_t)) == 0)
		{
			READER_UNLOCK((rtps_reader_t*)p_statefulreader);
			return p_statefulreader->pp_matched_writers[i];
		}
	}

	READER_UNLOCK((rtps_reader_t*)p_statefulreader);
	return NULL;
}



// 이건 물리적으로 보낼 Writer FD를 찾아, 데이터를 보내기 위해 사용 된다.
rtps_writer_t *get_related_Writer(rtps_statefulreader_t *p_statefulreader, GUID_t a_guid)
{
	GUID_t a_myGuid;
	a_myGuid = p_statefulreader->guid;
	memcpy(&a_myGuid.entity_id, &a_guid.entity_id, sizeof(EntityId_t));

	return (rtps_writer_t *)rtps_get_endpoint(a_myGuid);
}

void send_acknack(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumberSet a_seq_num_set)
{
	AckNack a_ackNack;
	rtps_writer_t *p_statefulwriter = get_related_Writer(p_statefulreader, p_rtps_writerproxy->remote_writer_guid);



	if (p_statefulwriter == NULL)
	{
		// TODO[120127,김경일] built-in Endpoint는 reader와 writer가 짝으로 존재함.
		// Application-defined reader는 짝이 되는 writer가 존재하지 않아서, 더 이상 진행하지 못함.
		// 아무 Writer나 가져다 사용하면 된다고 함.
		// 응급으로 LivelinessWriter를 사용해보자.
		if (p_statefulreader->is_builtin == false)
		{
			p_statefulwriter = (rtps_writer_t *)rtps_get_spdp_builtin_participant_writer();
		}
	}


	if(p_statefulwriter == NULL) 
		return;

	memcpy(&a_ackNack.writer_id, &p_rtps_writerproxy->a_remote_heartbeat.writer_id, sizeof(EntityId_t));
	memcpy(&a_ackNack.reader_id, &p_statefulreader->guid.entity_id, sizeof(EntityId_t));
	//memcpy(&a_ackNack.reader_id, &p_rtps_writerproxy->a_remote_heartbeat.reader_id, sizeof(EntityId_t));
	a_ackNack.reader_sn_state = a_seq_num_set;
	//////  
	//a_ackNack.reader_sn_state.bitmap_base.high = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value.high;  
	//a_ackNack.reader_sn_state.bitmap_base.low = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value.low;  
	//a_ackNack.reader_sn_state.numbits = p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low == 0 ? 1 :p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low;  
	//a_ackNack.reader_sn_state.bitmap = 0xffffffff;  
	///////  

	a_ackNack.count.value = ++p_rtps_writerproxy->i_ack_count;

	
	/*if(p_rtps_writerproxy->pp_unicast_locator_list[0]->address[12] == 127)
	{
		
		rtps_send_acknack_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_multicast_locator_list[0],a_ackNack);
	}else{
		rtps_send_acknack_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_unicast_locator_list[0],a_ackNack);
	}*/


	if(((rtps_statefulwriter_t *)p_statefulwriter)->b_enable_multicast)
	{
		rtps_send_acknack_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_multicast_locator_list[0],a_ackNack);
	}else{
		if(p_rtps_writerproxy->pp_unicast_locator_list[0]->address[12] == 127)
		{
			trace_msg(NULL, TRACE_LOG, "Please check the WriterProxy[Maybe... WriterProxy IP set local address[127.0.0.1].. see data[p]..]\r\n");
			pause_system();
			rtps_send_acknack_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_multicast_locator_list[0],a_ackNack);
		}else{
			rtps_send_acknack_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_unicast_locator_list[0],a_ackNack);
		}
	}

	if (p_statefulreader->is_builtin == false)
	{
		Locator_t* remoteLocator = get_default_remote_participant_locator(p_rtps_writerproxy->remote_writer_guid);
		//trace_msg(OBJECT(current_object(0)), TRACE_TRACE3, "(%d.%d.%d.%d) <<< ACK[%5d]:([%5u]~)", remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15], a_ackNack.count.value, a_ackNack.reader_sn_state.bitmap_base.low);
	}
	p_rtps_writerproxy->ackcheced_time = currenTime();
	p_rtps_writerproxy->last_acknack = a_ackNack;
	p_rtps_writerproxy->i_for_reader_state = WAITING_STATE;

	p_rtps_writerproxy->is_first_ack = false;
}


void send_nackFrag(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumberSet a_seq_num_set, SequenceNumber_t a_seq)
{
	NackFrag a_nack;
	rtps_writer_t *p_statefulwriter = get_related_Writer(p_statefulreader, p_rtps_writerproxy->remote_writer_guid);

	if(p_statefulwriter == NULL) return;

	memcpy(&a_nack.writer_id, &p_rtps_writerproxy->a_remote_heartbeat.writer_id, sizeof(EntityId_t));
	memcpy(&a_nack.reader_id, &p_statefulreader->guid.entity_id, sizeof(EntityId_t));

	a_nack.writer_sn.value = a_seq;
	a_nack.fragment_number_state = a_seq_num_set;
	a_nack.count.value = ++p_rtps_writerproxy->i_nack_count;
	rtps_send_nack_frag_with_dst_to((rtps_writer_t *)p_statefulwriter, p_rtps_writerproxy->remote_writer_guid, *p_rtps_writerproxy->pp_unicast_locator_list[0],a_nack);
}

void reliable_statefulreader_behavior( rtps_statefulreader_t* p_statefulreader, DataFull* p_datafull )
{
	//rtps_cachechange_t **pp_rtps_cachechanges = NULL;
	int i_size;
	int i_matched_writers;
	int i;
	rtps_writerproxy_t *p_rtps_writerproxy = NULL;
//	Time_t	current = currenTime();


	READER_LOCK((rtps_reader_t*)p_statefulreader);

	if(p_statefulreader->pp_matched_writers == NULL || p_statefulreader->i_matched_writers == 0) {
		READER_UNLOCK((rtps_reader_t*)p_statefulreader);
		return;
	}

	i_matched_writers = p_statefulreader->i_matched_writers;

	for(i=0; i < i_matched_writers; i++){
		p_rtps_writerproxy = p_statefulreader->pp_matched_writers[i];

		if(p_rtps_writerproxy->is_ignore == true) continue;


		if(p_rtps_writerproxy->i_for_reader_state == MAY_SEND_ACK_STATE)
		{

			if(p_rtps_writerproxy->i_missing_seq_num == 0)
			{
				p_rtps_writerproxy->i_for_reader_state = WAITING_STATE;
				continue;
			}else{
				p_rtps_writerproxy->i_for_reader_state = MUST_SEND_ACK_STATE;
			}

			
		}

		if(p_rtps_writerproxy->i_for_reader_state == MUST_SEND_ACK_STATE || p_rtps_writerproxy->i_for_reader_state == MAY_SEND_ACK_STATE
			|| p_rtps_writerproxy->i_for_reader_state_for_ack == MUST_SEND_ACK_STATE || p_rtps_writerproxy->i_for_reader_state_for_ack == MAY_SEND_ACK_STATE)
		{
			SequenceNumberSet missing_seq_num_set;

			p_rtps_writerproxy->i_for_reader_state_for_ack = WAITING_STATE;



			//[120202,김경일] 비교문안에 같은 코드가 중복되는데, 유지보수가 어려워지기 때문에 비교문을 수정함.
			/*if(p_statefulreader->heartbeatResponseDelay.sec < (current.sec - p_rtps_writerproxy->ackcheced_time.sec) ||
				(p_statefulreader->heartbeatResponseDelay.sec == (current.sec - p_rtps_writerproxy->ackcheced_time.sec) &&
				p_statefulreader->heartbeatResponseDelay.nanosec < (current.nanosec - p_rtps_writerproxy->ackcheced_time.nanosec)))*/
			{
				/*FOREACH change IN the_writer_proxy.missing_changes() DO
				ADD change.sequenceNumber TO missing_seq_num_set.set;
				send ACKNACK(missing_seq_num_set);*/



				missing_seq_num_set.bitmap_base = sequnce_number_inc(rtps_writerproxy_available_changes_max_for_acknack(p_rtps_writerproxy));
				missing_seq_num_set.numbits = 0;
				memset(missing_seq_num_set.bitmap, '\0', sizeof(uint32_t) * 8);
				
				
				//missing_seq_num_set.bitmap[0] = 0xffffffff;
				//pp_rtps_cachechanges = rtps_writerproxy_missing_changes(p_rtps_writerproxy,  &i_size);
				//missing_seq_num_set.numbits = i_size;
				missing_seq_num_set = rtps_writerproxy_missing_changes_and_missing_sequencenumberset(p_rtps_writerproxy,  &i_size, missing_seq_num_set);
				

				
				//printf("send_acknack : %d\r\n", missing_seq_num_set.bitmap_base.low);
				send_acknack(p_statefulreader, p_rtps_writerproxy, missing_seq_num_set);

				

			}
			
			/*else if(p_statefulreader->heartbeatResponseDelay.sec == (current.sec - p_rtps_writerproxy->ackcheced_time.sec))
			{
				if(p_statefulreader->heartbeatResponseDelay.nanosec < (current.nanosec - p_rtps_writerproxy->ackcheced_time.nanosec))
				{
					//a
					missing_seq_num_set.bitmap_base = sequnce_number_inc(rtps_writerproxy_available_changes_max(p_rtps_writerproxy));
					missing_seq_num_set.numbits = 0;
					missing_seq_num_set.bitmap[0] = 0xffffffff;

					pp_rtps_cachechanges = rtps_writerproxy_missing_changes(p_rtps_writerproxy,  &i_size);
					missing_seq_num_set.numbits = i_size;
					if(i_size ==0 ){
					
						if(!sequnce_number_left_bigger_and_equal(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value))
						{
							//pp_rtps_cachechanges = rtps_writerproxy_missing_changes(p_rtps_writerproxy,  &i_size);
							//if( i_size == 0)
							//{
							//	if(!rtps_writerproxy_is_exist_cache(p_rtps_writerproxy, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value))
							//	{
							//		//writerproxy에 존재 하지 않는다. 그래서 보내달라고 요청함..
							//		missing_seq_num_set.bitmap_base = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
							//		if(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low == 0){
							//			missing_seq_num_set.numbits = 1;
							//		}else{
							//			missing_seq_num_set.numbits = SequnceNumberDifference(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);
							//		}
							//	}else{
							//		missing_seq_num_set.bitmap_base = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
							//	}
							//}


						}else if(!rtps_writerproxy_is_exist_cache(p_rtps_writerproxy, p_rtps_writerproxy->a_remote_heartbeat.last_sn.value))
						{
							//writerproxy에 존재 하지 않는다. 그래서 보내달라고 요청함..
							missing_seq_num_set.bitmap_base = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
							if(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low == 0){
								missing_seq_num_set.numbits = 1;
							}else{
								missing_seq_num_set.numbits = SequnceNumberDifference(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);
							}

							send_acknack(p_statefulreader, p_rtps_writerproxy, missing_seq_num_set);
						}else{
							send_acknack(p_statefulreader, p_rtps_writerproxy, missing_seq_num_set);
						}
					}else{

						send_acknack(p_statefulreader, p_rtps_writerproxy, missing_seq_num_set);
					}
				}
			}*/
			p_rtps_writerproxy->i_for_reader_state = WAITING_STATE;

		}
	}

	READER_UNLOCK((rtps_reader_t*)p_statefulreader);
}


void instanceset_dispose(Entity* p_entity, GUID_t key_guid);
void instanceset_unregister(Entity* p_entity, GUID_t key_guid);


rtps_cachechange_t	*check_key_guid(rtps_statefulreader_t* p_statefulreader,  GUID_t writer_guid, DataFull *p_datafull, GUID_t *key_guid
									, bool *found_key_guid, bool *found_state_info)
{
	int32_t info_value = 0;

	rtps_cachechange_t	*p_rtps_cachechange = NULL;

	ParameterWithValue *p_atom_parameter;

	p_atom_parameter = get_real_parameter(p_datafull->inline_qos.p_head_first);

	while(p_atom_parameter)
	{

		if(*found_key_guid && *found_state_info)
		{
			break;
		}else if(p_atom_parameter->parameter_id == PID_KEY_HASH && p_atom_parameter->length == 16)
		{
			*found_key_guid = true;
			*key_guid = *((GUID_t*)p_atom_parameter->p_value);

		}else if(p_atom_parameter->parameter_id == PID_STATUS_INFO && p_atom_parameter->length == 4)
		{
			const uint8_t *p = p_atom_parameter->p_value;
			info_value = ((int32_t)p[0] << 24) |
								((int32_t)p[1] << 16) |
								((int32_t)p[2] << 8) |
								((int32_t)p[3]);

			*found_state_info = true;
				if(info_value == 3)
			{
			}
		}

		p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
	}

	if(*found_key_guid && *found_state_info)
	{
		if(info_value == 1)
		{
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_DISPOSED, writer_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
		}else if(info_value == 2){
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, writer_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
		}else if(info_value == 3){
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, writer_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
		}else{
			p_rtps_cachechange = rtps_cachechange_new(ALIVE, writer_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
		}
	}else{
		p_rtps_cachechange = rtps_cachechange_new(ALIVE, writer_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
	}

	if(*found_key_guid && p_statefulreader->topic_kind == WITH_KEY
		&& p_rtps_cachechange != NULL)
	{
		memcpy(p_rtps_cachechange->key_guid_for_reader, key_guid, 16);
	}


	return p_rtps_cachechange;
}


rtps_cachechange_t	*check_key_guid_for_reliability(rtps_statefulreader_t* p_statefulreader,  GUID_t writer_guid, DataFull *p_datafull, SerializedPayload* p_serialized_data, GUID_t *key_guid
									, bool *found_key_guid, bool *found_state_info)
{
	int32_t info_value = 0;

	rtps_cachechange_t	*p_rtps_cachechange = NULL;

	ParameterWithValue *p_atom_parameter;
	SequenceNumber_t* p_seq;
	bool	is_found_coherent_set = false;

	p_atom_parameter = get_real_parameter(p_datafull->inline_qos.p_head_first);
	

	while(p_atom_parameter)
	{

		if(*found_key_guid && *found_state_info)
		{
			break;
		}else if(p_atom_parameter->parameter_id == PID_KEY_HASH && p_atom_parameter->length == 16)
		{
			*found_key_guid = true;
			*key_guid = *((GUID_t*)p_atom_parameter->p_value);

		}else if(p_atom_parameter->parameter_id == PID_STATUS_INFO && p_atom_parameter->length == 4)
		{
			const uint8_t *p = p_atom_parameter->p_value;
			info_value = ((int32_t)p[0] << 24) |
								((int32_t)p[1] << 16) |
								((int32_t)p[2] << 8) |
								((int32_t)p[3]);

			*found_state_info = true;
				if(info_value == 3)
			{
			}
		}else if(p_atom_parameter->parameter_id == PID_COHERENT_SET && p_atom_parameter->length == sizeof(SequenceNumber_t))
		{

			is_found_coherent_set = true;

			p_seq = (SequenceNumber_t*)p_atom_parameter->p_value;
			
					
		}

		p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
	}

	if(*found_key_guid && *found_state_info)
	{
		if(info_value == 1)
		{
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_DISPOSED, writer_guid, p_datafull->writer_sn.value, p_serialized_data, 0);
		}else if(info_value == 2){
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, writer_guid, p_datafull->writer_sn.value, p_serialized_data, 0);
		}else if(info_value == 3){
			p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, writer_guid, p_datafull->writer_sn.value, p_serialized_data, 0);
		}else{
			p_rtps_cachechange = rtps_cachechange_new(ALIVE, writer_guid, p_datafull->writer_sn.value, p_serialized_data, 0);
		}
	}else{
		p_rtps_cachechange = rtps_cachechange_new(ALIVE, writer_guid, p_datafull->writer_sn.value, p_serialized_data, 0);
	}

	if(*found_key_guid && p_statefulreader->topic_kind == WITH_KEY
		&& p_rtps_cachechange != NULL)
	{
		memcpy(p_rtps_cachechange->key_guid_for_reader, key_guid, 16);
	}


	if(is_found_coherent_set)
	{
		p_rtps_cachechange->coherent_set_number.high = p_seq->high;
		p_rtps_cachechange->coherent_set_number.low = p_seq->low;
		//printf("Coherent Set Number = %d, %d\n",p_seq->high,p_seq->low);
		//printf("Coherent Set 수집중 Coh_Number = %d\n",p_seq->low);
		if(p_seq->high == -1 && p_seq->low == 0)
		{
			//추후 이 메시지 이전 Seq# 검색하여 모두 받았는지 확인한 후 coherent_set을 false로 하는 것이 필요(02.12)
//							int coh_set;
//							coh_set = p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->sequence_number.low;
//							coh_set = p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->coherentStartNumber.low;
//							printf("%d \n",coh_set);
//							for (coh_set; coh_set < p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->sequence_number.low; coh_set++)
//							{
//								if(p_rtpsReader->reader_cache->pp_changes[coh_set]->sequence_number.low != coh_set)
//									printf("ERROR!! %d \n",coh_set);
//							}
			p_rtps_cachechange->is_coherent_set = false;
			p_statefulreader->p_datareader->last_coh_number = p_rtps_cachechange->sequence_number.low;
		}
		else if(p_rtps_cachechange->sequence_number.low == p_rtps_cachechange->coherent_set_number.low)
		{							
			p_rtps_cachechange->is_coherent_set = true;
			p_statefulreader->p_datareader->first_coh_number = p_rtps_cachechange->sequence_number.low;
			//printf("Coherent Set이 재설정 되었습니다. Coh_Number = %d\n",p_statefulreader->p_datareader->first_coh_number);
		}
		else
		{
			p_rtps_cachechange->is_coherent_set = true;
		}
	}


	return p_rtps_cachechange;
}


void best_effort_statefulreader_behavior( rtps_statefulreader_t* p_statefulreader, DataFull** pp_datafull, data_t* p_data )
{
	rtps_cachechange_t	*p_rtps_cachechange;
	GUID_t writer_guid;
	rtps_writerproxy_t *p_rtps_writerproxy;
	SequenceNumber_t expected_seq_num;

	/////added by kyy//////////////////////////
	Duration_t			receive_time;
	bool				correct_time_based_filter;
	bool				correct_deadline;

	GUID_t key_guid;
	bool found_key_guid = false;
	bool found_state_info = false;
	DataFull *p_datafull = *pp_datafull;

	//bool				correct_deadline;
	///////////////////////////////////////////
	//added by kyy
	receive_time = current_duration();
	correct_time_based_filter = false;
	/////////////////////////////////////////////
	
	

	if (p_datafull == NULL)
	{
		return;
	}

	//trace_msg(NULL, TRACE_LOG, "best_Effort_StatefulReader_Behavior");

	READER_LOCK((rtps_reader_t*)p_statefulreader);

	{
		module_t* p_module = current_object(get_domain_participant_factory_module_id());
		if (p_statefulreader->p_reader_cache)
		{
			trace_msg(OBJECT(p_module), TRACE_TRACE3, "history cache size : %d, %d", p_statefulreader->i_matched_writers, p_statefulreader->p_reader_cache->i_linked_size);
		}
	}


	//if(p_statefulreader->p_datareader->test_for_reliablity_division == 0) {
	//	destory_datafull_all(p_datafull);
	//	READER_UNLOCK((rtps_reader_t*)p_statefulreader);
	//	return;
	//}

	//if( p_datafull->writer_sn.value.low % p_statefulreader->p_datareader->test_for_reliablity_division != 0)
	////if( p_datafull->writer_sn.value.low % 2 != 0)
	//{
	//	destory_datafull_all(p_datafull);
	//	READER_UNLOCK((rtps_reader_t*)p_statefulreader);
	//	return;
	//}

	if(p_statefulreader->pp_matched_writers == NULL || p_statefulreader->i_matched_writers == 0) {
		destory_datafull_all(p_datafull);
		*pp_datafull = NULL;
		READER_UNLOCK((rtps_reader_t*)p_statefulreader);
		return;
	}
	
	if(p_data->p_address)
	{
		writer_guid.guid_prefix = *(GuidPrefix_t*)p_data->p_address;
	}
	writer_guid.entity_id = p_datafull->writer_id.value;
	p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, writer_guid);

	if(p_rtps_writerproxy == NULL || p_rtps_writerproxy->is_ignore == true)
	{
		destory_datafull(p_datafull);
		p_datafull = NULL;
		*pp_datafull = NULL;


		return;
	}

	//added by kyy(Timebasedfilter)
	//add BestEffort Deadline and Timebasedfilter QoS//


	if(p_rtps_writerproxy){
		if(p_rtps_writerproxy->start == 0)
		{
			p_rtps_writerproxy->base_time.sec = receive_time.sec;
			p_rtps_writerproxy->base_time.nanosec = receive_time.nanosec;
			p_rtps_writerproxy->deadline_base_time.sec = receive_time.sec;
			p_rtps_writerproxy->deadline_base_time.nanosec = receive_time.nanosec;
	//		printf("Timebasedfilter first basetime : %d\n", p_rtps_writerproxy->base_time.sec);
	//		printf("Timebasedfilter first basetime : %d\n", p_rtps_writerproxy->base_time.nanosec);
			p_rtps_writerproxy->start++;


			/// 
			/// 이것을 안넣으면 처음 데이터는 무조건 버림.....
			
			correct_time_based_filter = true;
		}
		else
		{
			//correct_time_based_filter = compare_timebasedfilter(receive_time, &(p_rtps_writerproxy->base_time), p_statefulreader->p_datareader->datareader_qos.time_based_filter.minimum_separation);
			correct_time_based_filter = qos_compare_timebasedfilter(receive_time, &(p_rtps_writerproxy->base_time), p_statefulreader->p_datareader->datareader_qos.time_based_filter.minimum_separation);
		}
	
		//////End Time Based Filter/////////////////////////////////////////////////////////////

		//Deadline
		//Deadline 검사 후 base_time 수정
		//correct_deadline = compare_deadline(receive_time, &(p_rtps_writerproxy->base_time), p_statefulreader->p_datareader->datareader_qos.deadline.period);
		correct_deadline = qos_compare_deadline(receive_time, &(p_rtps_writerproxy->deadline_base_time), p_statefulreader->p_datareader->datareader_qos.deadline.period);
		p_rtps_writerproxy->b_is_deadline_on = correct_deadline;//by kki (ownership)
		//status by jun 
		if (correct_deadline == false)
		{
			qos_change_owner_writer((rtps_reader_t*)p_statefulreader, p_rtps_writerproxy, false);
			change_RequestedDeadlineMissed_status(p_statefulreader->p_datareader, 0);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	if(p_rtps_writerproxy && correct_time_based_filter){
		SerializedPayloadForReader *p_serializedForReader = NULL;
		message_t *p_message = NULL;
		bool b_expected = false;
//by kki...(ownership)
		// 1) reader 가 exclusive 인지 확인
		// 2) writer 가 owner 인지 확인
		// 3) DATA 를 historycache 에 저장
		if (qos_has_exclusive_ownership((rtps_reader_t*)p_statefulreader))//by kki
		{
			rtps_reader_t* p_rtps_reader = (rtps_reader_t*)p_statefulreader;
			int32_t index = qos_set_exclusive_writer_key(p_rtps_reader, p_rtps_writerproxy, (SerializedPayloadForReader*)p_datafull->p_serialized_data);//by kki (ownership)
			if (index == -1 || p_rtps_reader->pp_exclusive_writers[index]->b_is_owner == false)
			{
				destory_datafull(p_datafull);
				p_datafull = NULL;
				*pp_datafull = NULL;

				READER_UNLOCK((rtps_reader_t*)p_statefulreader);
				if (p_datafull) destory_datafull_except_serialized_data(p_datafull);
				return;
			}
		}

		memset(&key_guid, 0, sizeof(GUID_t));
		
		p_rtps_cachechange = check_key_guid(p_statefulreader, writer_guid, p_datafull, &key_guid, &found_key_guid, &found_state_info);

		//added by kyy(Lifespan QoS)
		qos_input_lifespan(p_rtps_cachechange, p_statefulreader, p_rtps_writerproxy);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		p_rtps_cachechange->p_rtps_writerproxy = p_rtps_writerproxy;

		expected_seq_num = sequnce_number_inc(rtps_writerproxy_available_changes_max(p_rtps_writerproxy));

		b_expected = sequnce_number_left_bigger_and_equal(p_rtps_cachechange->sequence_number, expected_seq_num);

		//if(b_expected)
		{
			if (qos_check_historycache_for_reader(p_statefulreader->p_reader_cache, p_rtps_cachechange))//by kki (history)
			{
				p_rtps_cachechange = rtps_historycache_add_change(p_statefulreader->p_reader_cache, p_rtps_cachechange);

				if(found_key_guid && p_rtps_cachechange != NULL && p_rtps_cachechange->p_org_message != NULL
					&& p_statefulreader->topic_kind == WITH_KEY
					&& p_rtps_cachechange->kind == ALIVE)
				{
					memcpy(p_rtps_cachechange->p_org_message->key_guid, &key_guid, 16);
				}
				else if (found_key_guid && p_rtps_cachechange != NULL 
						&& p_statefulreader->topic_kind == WITH_KEY
						&& p_rtps_cachechange->kind == NOT_ALIVE_DISPOSED)
				{
					rtps_historycache_change_dispose(p_statefulreader->p_reader_cache, p_rtps_cachechange, key_guid);
					instanceset_dispose((Entity *)p_statefulreader->p_datareader, key_guid);
				}
				else if (found_key_guid && p_rtps_cachechange != NULL 
						&& p_statefulreader->topic_kind == WITH_KEY
						&& p_rtps_cachechange->kind == NOT_ALIVE_UNREGISTERED)
				{
					rtps_historycache_change_unregister(p_statefulreader->p_reader_cache, p_rtps_cachechange, key_guid);
					instanceset_unregister((Entity *)p_statefulreader->p_datareader, key_guid);
				}


				if (p_rtps_cachechange->is_same == false)
				{
					rtps_writerproxy_received_change_set(p_rtps_writerproxy, p_rtps_cachechange->sequence_number);

					if (sequnce_number_left_bigger(p_rtps_cachechange->sequence_number, expected_seq_num))
					{
						rtps_writerproxy_lost_changes_update(p_rtps_writerproxy,  p_rtps_cachechange->sequence_number);
					}
					// get_change대신 하는 역할을 할 거임...
					//dataWriterWriteFromRTPS(p_statefulreader->p_datareader, &p_datafull->p_serialized_data->value[4], NULL);
					p_serializedForReader = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
					if (p_rtps_cachechange->p_data_value)
					{
						/*p_message = messageNewForBultinReader(p_statefulreader->p_datareader, &p_rtps_cachechange->p_data_value->value[4], p_serializedForReader->i_size-4);
						if(p_message) messageWrite(p_message);*/
					}
				}
			}
			else
			{

				printf("remove %d\r\n",p_rtps_cachechange->sequence_number.low);
			}
		}
		//else
		//{

		//	printf("remove %d\r\n",p_rtps_cachechange->sequence_number.low);
		//	//일단은 데이터가 오면은 DCPS에 보내자.. 
		//	if(p_datafull && p_datafull->p_serialized_data){
		//		p_serializedForReader = (SerializedPayloadForReader *)p_datafull->p_serialized_data;
		//		rtps_cachechange_ref(p_rtps_cachechange, true, false);
		//		rtps_cachechange_destory(p_rtps_cachechange);
		//	}
		//}

		

		//p_message = messageNewForBultinReader(p_statefulreader->p_datareader, &p_datafull->p_serialized_data->value[4]);
		//message_write(p_message);
	}else{
		destory_datafull(p_datafull);
		p_datafull = NULL;
		*pp_datafull = NULL;
	}

	//added by kyy(Lifespan QoS Process)
	
	//qosim_process_lifespan_for_reader_historycache_stateful(p_statefulreader);

	READER_UNLOCK((rtps_reader_t*)p_statefulreader);

	if(*pp_datafull) {
		destory_datafull_except_serialized_data(*pp_datafull);
		*pp_datafull = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//added by kyy
//time_based_filter 비교 함수
bool compare_timebasedfilter(Duration_t receive_time, Duration_t* base_time, Duration_t minimum_separation)
{

	if(minimum_separation.nanosec / 1000000000 >= 1)
	{
		minimum_separation.sec += (minimum_separation.nanosec / 1000000000);
		minimum_separation.nanosec -= 1000000000;
	}

	if(minimum_separation.sec == 0 && minimum_separation.nanosec == 0)
	{
		base_time->sec = receive_time.sec;
		base_time->nanosec = receive_time.nanosec;

	}
/*	if(receive_time.sec < base_time->sec + minimum_separation.sec ) 
	{
		return false;
	}
	else if(receive_time.sec == base_time->sec && receive_time.nanosec < base_time->nanosec + minimum_separation.nanosec)
	{
		return false;
	}*/
	if(time_left_bigger_d(time_addition_d(*base_time,minimum_separation),receive_time))
	{
		return false;
	}
	else
	{
		base_time->sec = receive_time.sec;
		base_time->nanosec = receive_time.nanosec;
		//printf("Timebasedfilter next basetime : %d\n", base_time->sec);
		//printf("Timebasedfilter next basetime : %d\n", base_time->nanosec);
	}

	return true;
}
/*
//added by kyy
void process_lifespan_for_historycache(rtps_statefulreader_t *p_statefulreader)
{
	int i;
	Duration_t receive_time = current_duration();
	int i_cache_size;

	HISTORYCACHE_LOCK(p_statefulreader->reader_cache);


	i_cache_size = p_statefulreader->reader_cache->i_changes;

	for(i = i_cache_size-1; i >= 0; i--)
	{

		if(receive_time.sec - p_statefulreader->reader_cache->changes[i]->source_timestamp.sec > p_statefulreader->reader_cache->changes[i]->lifespan_duration.sec)
		{
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->changes[i]);
			rtps_cachechange_ref(p_statefulreader->reader_cache->changes[i], true, false);
			rtps_cachechange_destory(p_statefulreader->reader_cache->changes[i]);
			REMOVE_ELEM( p_statefulreader->reader_cache->changes, p_statefulreader->reader_cache->i_changes, i);
		}
		else if(receive_time.sec - p_statefulreader->reader_cache->changes[i]->source_timestamp.sec == p_statefulreader->reader_cache->changes[i]->lifespan_duration.sec)
		{
//			rtps_historycache_remove_change(p_statefulreader->reader_cache,p_statefulreader->reader_cache->changes[i]);
			rtps_cachechange_ref(p_statefulreader->reader_cache->changes[i], true, false);
			rtps_cachechange_destory(p_statefulreader->reader_cache->changes[i]);
			REMOVE_ELEM( p_statefulreader->reader_cache->changes, p_statefulreader->reader_cache->i_changes, i);
		}

//		printf("source[%d] : %d\n",i, p_statefulreader->reader_cache->changes[i]->source_timestamp.sec);
//		printf("%d\n",receive_time.sec - p_statefulreader->reader_cache->changes[i]->source_timestamp.sec);

	}

	HISTORYCACHE_UNLOCK(p_statefulreader->reader_cache);

}
//added by kyy
void input_lifespan(rtps_cachechange_t *p_rtps_cachechange, rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_rtps_writerproxy)
{
	p_rtps_cachechange->source_timestamp.sec = p_statefulreader->source_timestamp.sec;
	p_rtps_cachechange->source_timestamp.nanosec = p_statefulreader->source_timestamp.nanosec;
	p_rtps_cachechange->lifespan_duration.sec = p_rtps_writerproxy->lifespan.sec;
	p_rtps_cachechange->lifespan_duration.nanosec = p_rtps_writerproxy->lifespan.nanosec;
//	printf("source_time = %d , %d\n", p_rtps_cachechange->source_timestamp.sec,p_rtps_cachechange->source_timestamp.nanosec );

}
*/
//added by kyy
/*
bool compare_deadline(Duration_t receive_time, Duration_t *base_time, Duration_t deadline_period)
{
	if(deadline_period.sec == TIME_INFINITE.sec && deadline_period.nanosec == TIME_INFINITE.nanosec)
	{

	}
	else if(receive_time.sec > base_time->sec + deadline_period.sec) 
	{
		printf("Receive sec : BaseTime sec = %d : %d\n",receive_time.sec , base_time->sec + deadline_period.sec);
		printf("DataReader Deadline Sec Missed\n");
		//DataReaderListener의 on_requested_deadline_missed() 호출
	}

	else if(receive_time.sec == base_time->sec + deadline_period.sec 
		 && receive_time.nanosec > base_time->nanosec + deadline_period.nanosec)
	{
		printf("Receive nanosec : BaseTime nanosec = %d : %d\n" , base_time->nanosec + deadline_period.nanosec);
		printf("DataReader Deadline NanoSec Missed\n");
		//DataReaderListener의 on_requested_deadline_missed() 호출
	}

	//Deadline 검사 후 base_time 수정
	base_time->sec = receive_time.sec;
	base_time->nanosec = receive_time.nanosec;	

	return true;
}
*/