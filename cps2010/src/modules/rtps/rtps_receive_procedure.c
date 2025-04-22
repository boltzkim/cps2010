/*
	RTSP Receive Procedure class
	작성자 : 
	이력
	2010-10-1 : 처음 시작
*/


#include "rtps.h"

/*
	8.3.7.5.5 Logical Interpretation

	The Heartbeat message serves two purposes:
		- It informs the Reader of the sequence numbers that are available in the writer’s HistoryCache so that the Reader may
		  request (using an AckNack) any that it has missed.

		- It requests the Reader to send an acknowledgement for the CacheChange changes that have been entered into the
		  reader’s HistoryCache such that the Writer knows the state of the reader.

	All Heartbeat messages serve the first purpose. That is, the Reader will always find out the state of the writer’s
	HistoryCache and may request what it has missed. Normally, the RTPS Reader would only send an AckNack message if
	it is missing a CacheChange.

	The Writer uses the FinalFlag to request the Reader to send an acknowledgment for the sequence numbers it has
	received. If the Heartbeat has the FinalFlag set, then the Reader is not required to send an AckNack message back.
	However, if the FinalFlag is not set, then the Reader must send an AckNack message indicating which CacheChange
	changes it has received, even if the AckNack indicates it has received all CacheChange changes in the writer’s
	HistoryCache.

	The Writer sets the LivelinessFlag to indicate that the DDS DataWriter associated with the RTPS Writer of the message
	has manually asserted its liveliness using the appropriate DDS operation (see the DDS Specification). The RTPS Reader
	should therefore renew the manual liveliness lease of the corresponding remote DDS DataWriter.

	The Writer is identified uniquely by its GUID. The Writer GUID is obtained using the state of the Receiver:
		writerGUID = { Receiver.sourceGuidPrefix, Heartbeat.writer_id }

	The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver:
		readerGUID = { Receiver.destGuidPrefix, Heartbeat.reader_id }
	
	The Heartbeat.reader_id can be ENTITYID_UNKNOWN, in which case the Heartbeat applies to all Readers of that
	writerGUID within the Participant.
*/

/*
	9.4.5.6.1 Flags in the Submessage Header

	In addition to the EndiannessFlag, the HeartBeat Submessage introduces the FinalFlag and the LivelinessFlag
	(“Content” on page 46). The PSM maps the FinalFlag flag into the 2nd least-significant bit (LSB) of the flags and the
	LivelinessFlag into the 3rd least-significant bit (LSB) of the flags.

	The FinalFlag is represented with the literal ‘F’. F=1 means the Writer does not require a response from the Reader. F=0
	means the Reader must respond to the HeartBeat message.
	
	The value of the FinalFlag can be obtained from the expression:
		F = SubmessageHeader.flags & 0x02

	The LivelinessFlag is represented with the literal ‘L’. L=1 means the DDS DataReader associated with the RTPS Reader
	should refresh the ‘manual’ liveliness of the DDS DataWriter associated with the RTPS Writer of the message.
	
	The value of the LivelinessFlag can be obtained from the expression:
		L = SubmessageHeader.flags & 0x04

*/
//


static void WakeUpBuiltinPub_and_Sub();

void rtps_receive_heartbeat_procedure2( InfoDestination a_info_descript, Heartbeat a_heartbeat, octet flags )
{
	rtps_entity_t *p_rtps_entity = rtps_find_all_entity(a_info_descript.guid_prefix.value);
	octet F = flags & 0x02;
	octet L = flags & 0x04;
	AckNack a_ackNack;

	if (p_rtps_entity)
	{
		if (memcmp(&a_heartbeat.reader_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId)) == 0 
			&& memcmp(&a_heartbeat.writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId)) == 0)
		{
			heartbeat_wakeup_participant(a_info_descript.guid_prefix.value);
		}

		if (F == 0)
		{
			//must
			memcpy(&a_ackNack.writer_id, &a_heartbeat.writer_id, sizeof(EntityId_t));
			memcpy(&a_ackNack.reader_id, &a_heartbeat.reader_id, sizeof(EntityId_t));
			a_ackNack.reader_sn_state.bitmap_base.high = a_heartbeat.first_sn.value.high;
			a_ackNack.reader_sn_state.bitmap_base.low = a_heartbeat.first_sn.value.low;
			a_ackNack.reader_sn_state.numbits = a_heartbeat.last_sn.value.low;
			a_ackNack.reader_sn_state.bitmap[0] = 0xffffffff;
			//a_ackNack.count.value = ++p_rtpsReader->count;
			
			//rtps_sendAckNAckWithDstTo((rtps_writer_t *)p_statefulwriter, a_reader_guid, *p_rtps_readerproxy->unicastLocatorList[0],a_ackNack);
		}

	}
}


// 이부분 다시 작업...
//void rtps_receive_heartbeat_procedure(rtps_reader_t *p_rtpsReader, GuidPrefix_t a_guidPrefix, InfoDestination a_infoDescript, Heartbeat a_heartbeat, octet flags)
//{
//	AckNack a_ackNack;
//	GUID_t a_reader_guid;
//	GUID_t a_findGuid;
//	rtps_readerproxy_t *p_rtps_readerproxy;
//
//	octet F = flags & 0x02;
//	octet L = flags & 0x04;
//	rtps_statefulwriter_t *p_statefulwriter;
//
//	a_findGuid.guidPrefix = a_infoDescript.guidPrefix.value;
//	a_findGuid.entityId = a_heartbeat.writer_id.value;
//	
//	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_Endpoint(a_findGuid);
//
//	if(p_statefulwriter == NULL) return;
//
//	memcpy(&a_reader_guid.guidPrefix, &a_guidPrefix, sizeof(GuidPrefix_t));
//	memcpy(&a_reader_guid.entityId, &a_heartbeat.reader_id, sizeof(EntityId_t));
//
//	p_rtps_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, a_reader_guid);
//
//
//	if(p_rtps_readerproxy == NULL) return;
//	//send heartBeat......................
//
//
//	/*if(p_statefulwriter->behavior_type == STATEFUL_TYPE)
//	{
//		if(((rtps_statefulwriter_t*)p_statefulwriter)->pushMode)
//		{
//			return;
//		}
//	}*/
//
//	if(F == 0)
//	{
//		//must
//		memcpy(&a_ackNack.writer_id, &a_heartbeat.writer_id, sizeof(EntityId_t));
//		memcpy(&a_ackNack.reader_id, &a_heartbeat.reader_id, sizeof(EntityId_t));
//		a_ackNack.reader_sn_state.bitmap_base.high = a_heartbeat.first_sn.value.high;
//		a_ackNack.reader_sn_state.bitmap_base.low = a_heartbeat.first_sn.value.low == 0 ? 1 : a_heartbeat.first_sn.value.low;
//		a_ackNack.reader_sn_state.numbits = a_heartbeat.last_sn.value.low == 0 ? 1 :a_heartbeat.last_sn.value.low;
//		a_ackNack.reader_sn_state.bitmap = 0xffffffff;
//		a_ackNack.count.value = ++p_rtpsReader->count;
//		
//		rtps_sendAckNAckWithDstTo((rtps_writer_t *)p_statefulwriter, a_reader_guid, *p_rtps_readerproxy->unicastLocatorList[0],a_ackNack);
//		p_rtps_readerproxy->i_behavior_state = MUST_SEND_ACK_STATE;
//	}else if(L == 0)
//	{
//		p_rtps_readerproxy->i_behavior_state = MAY_SEND_ACK_STATE;
//	}else{
//		p_rtpsReader->i_behavior_state = WAITING_STATE;
//	}
//}


static void Request_acknack_for_publication(rtps_statefulreader_t * p_rtps_reader, GUID_t a_findGuid, GUID_t a_reader_guid, Heartbeat a_heartbeat)
{

	rtps_writerproxy_t a_trick_rtps_writerproxy;
	rtps_writerproxy_t *p_rtps_writerproxy = NULL;

	SequenceNumberSet missing_seq_num_set;
	SequenceNumber_t _first;
	rtps_reader_t *p_rtpsBultinReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();
	int32_t i_diff = 255;
	int i;

	a_trick_rtps_writerproxy.remote_writer_guid = a_findGuid;

	

	a_findGuid.entity_id = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
	p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtpsBultinReader, a_findGuid);

	if(p_rtps_writerproxy == NULL)
	{
		printf("can't find edp\r\n");
	}


	if (p_rtps_writerproxy->a_remote_heartbeat.first_sn.value.low == 0
		|| p_rtps_writerproxy->a_remote_heartbeat.last_sn.value.low == 0)
	{
		_first.high = 0;
		_first.low = 1;

		missing_seq_num_set.bitmap_base = _first;
		missing_seq_num_set.numbits = 255;
		i_diff = 256;
		
	}
	else {
		missing_seq_num_set.bitmap_base = p_rtps_writerproxy->a_remote_heartbeat.first_sn.value;
		i_diff = sequnce_number_difference2(p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, p_rtps_writerproxy->a_remote_heartbeat.first_sn.value);

		missing_seq_num_set.numbits = i_diff;
		
	}


	
	memset(missing_seq_num_set.bitmap, '\0', sizeof(uint32_t) * 8);

	for (i = 0; i < i_diff && i < 256; i++)
	{
		missing_seq_num_set.bitmap[i / 32] |= 1 << (31 - (i % 32));
	}

	memset(&a_trick_rtps_writerproxy, '\0', sizeof(rtps_writerproxy_t));

	


	

	if (p_rtps_writerproxy != NULL)
	{

		a_trick_rtps_writerproxy.remote_writer_guid = a_findGuid;
		a_trick_rtps_writerproxy.pp_multicast_locator_list = p_rtps_writerproxy->pp_multicast_locator_list;
		a_trick_rtps_writerproxy.i_multicast_locator_list = p_rtps_writerproxy->i_multicast_locator_list;

		a_trick_rtps_writerproxy.pp_unicast_locator_list = p_rtps_writerproxy->pp_unicast_locator_list;
		a_trick_rtps_writerproxy.i_unicast_locator_list = p_rtps_writerproxy->i_unicast_locator_list;

		p_rtps_writerproxy->a_remote_heartbeat.writer_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
		p_rtps_writerproxy->a_remote_heartbeat.reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		send_acknack((rtps_statefulreader_t *)p_rtpsBultinReader, &a_trick_rtps_writerproxy, missing_seq_num_set);
		trace_msg(NULL, TRACE_LOG, "Request_acknack_for_publication.....to %d.%d.%d.%d:%d (%d - %d) \r\n", a_trick_rtps_writerproxy.pp_unicast_locator_list[0]->address[12] 
			, a_trick_rtps_writerproxy.pp_unicast_locator_list[0]->address[13]
			, a_trick_rtps_writerproxy.pp_unicast_locator_list[0]->address[14]
			, a_trick_rtps_writerproxy.pp_unicast_locator_list[0]->address[15]
			, a_trick_rtps_writerproxy.pp_unicast_locator_list[0]->port
			, missing_seq_num_set.bitmap_base.low, missing_seq_num_set.numbits);
	}


}



extern int32_t					i_remoteParticipant;

void rtps_receive_heartbeat_procedure( rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, /*InfoDestination a_infoDescript,*/ Heartbeat a_heartbeat, octet flags )
{
	GUID_t a_reader_guid;
	GUID_t a_findGuid;
	rtps_writerproxy_t *p_rtps_writerproxy=NULL;

	octet F = flags & 0x02;
	octet L = flags & 0x04;
	int i;

	a_findGuid.guid_prefix = a_guid_prefix;
	a_findGuid.entity_id = a_heartbeat.writer_id.value;


	memcpy(&a_reader_guid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t));
	memcpy(&a_reader_guid.entity_id, &a_heartbeat.reader_id, sizeof(EntityId_t));

	//if (p_rtps_reader->is_builtin)
	//{
	//	p_rtps_reader->is_builtin = p_rtps_reader->is_builtin;

	//	if ((memcmp(&a_heartbeat.writer_id.value, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
	//	{
	//		int a = 0;
	//		a++;
	//		printf("receive heartbeat... pub\r\n");
	//	}
	//}
 //
	p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtps_reader, a_findGuid);


	/*if(p_rtps_writerproxy == NULL) {
		int i_wait_count = 0;
		while(i_wait_count <= 5){
			msleep(40000);
			p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtpsReader, a_findGuid);
			if(p_rtps_writerproxy != NULL) {
				break;
			}

			i_wait_count++;
		}
	}*/

	if (p_rtps_writerproxy == NULL)
	{
		rtps_reader_t *p_rtpsBultinReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();

		trace_msg(NULL, TRACE_LOG2, "[%d]p_rtps_writerproxy is null(%s : %d)...\r\n" , i_remoteParticipant, p_rtps_reader->p_datareader->p_topic->topic_name,  p_rtpsBultinReader->p_reader_cache->i_linked_size);
		//trace_msg(NULL, TRACE_LOG, "p_rtps_writerproxy is null(%s : %d)...\r\n", p_rtps_reader->p_datareader->p_topic->topic_name, p_rtpsBultinReader->p_reader_cache->i_linked_size);
		//writerproxy is null이면, 강제로 요청

		/*if (p_rtps_reader->p_datareader->builtin_type == NORMAL_TYPE)
		{
			Request_acknack_for_publication((rtps_statefulreader_t *)p_rtps_reader, a_findGuid, a_reader_guid, a_heartbeat);
			WakeUpBuiltinPub_and_Sub();
		}*/
		return;
	}

	
	//send heartBeat......................

	p_rtps_writerproxy->a_remote_heartbeat = a_heartbeat;
	if(F == 0)
	{
		//must
		p_rtps_writerproxy->i_for_reader_state = MUST_SEND_ACK_STATE;
		p_rtps_writerproxy->i_for_reader_state_for_ack = MUST_SEND_ACK_STATE;
	}else if(L == 0)
	{
		p_rtps_writerproxy->i_for_reader_state = MAY_SEND_ACK_STATE;
		p_rtps_writerproxy->i_for_reader_state_for_ack = MAY_SEND_ACK_STATE;
	}else{
		p_rtps_writerproxy->i_for_reader_state = WAITING_STATE;
		p_rtps_writerproxy->i_for_reader_state_for_ack = WAITING_STATE;
		qos_manual_by_topic_liveliness_set_alive(p_rtps_writerproxy);//by kki (liveliness)
	}


	/*if (p_rtps_reader->is_builtin)
	{
		p_rtps_reader->is_builtin = p_rtps_reader->is_builtin;
	}*/
	
	if(p_rtps_reader->reliability_level == BEST_EFFORT)
	{
		SequenceNumberSet missing_seq_num_set;
		missing_seq_num_set.bitmap_base = sequnce_number_inc(a_heartbeat.last_sn.value);
		missing_seq_num_set.numbits = 0;
		memset(missing_seq_num_set.bitmap, '\0', sizeof(uint32_t) * 8);
				
		
				

		send_acknack((rtps_statefulreader_t *)p_rtps_reader, p_rtps_writerproxy, missing_seq_num_set);

		return;
	}



	if (p_rtps_writerproxy->received_first_packet == true && sequnce_number_left_bigger(p_rtps_writerproxy->min_seq_num, a_heartbeat.first_sn.value))
	{
		int j;
		//insert missing
		int32_t i_diff = sequnce_number_difference2(p_rtps_writerproxy->min_seq_num, a_heartbeat.first_sn.value);
		SequenceNumber_t base_seq = sequnce_number_dec(a_heartbeat.first_sn.value);


		for (j = 0; j < i_diff; j++) {
			SequenceNumber_t *p_missig = malloc(sizeof(SequenceNumber_t));
			memset(p_missig, 0, sizeof(SequenceNumber_t));

			*p_missig = base_seq = sequnce_number_inc(base_seq);
			INSERT_ELEM(p_rtps_writerproxy->pp_missing_seq_num, p_rtps_writerproxy->i_missing_seq_num
				, p_rtps_writerproxy->i_missing_seq_num, p_missig);

			base_seq = sequnce_number_inc(base_seq);
			insert_missing_num(p_rtps_writerproxy, base_seq);
		}
	}


	for(i=0; i < p_rtps_writerproxy->i_missing_seq_num; i++)
	{
		if(sequnce_number_left_bigger(a_heartbeat.first_sn.value, *p_rtps_writerproxy->pp_missing_seq_num[i]))
		{
			FREE(p_rtps_writerproxy->pp_missing_seq_num[i]);
			REMOVE_ELEM(p_rtps_writerproxy->pp_missing_seq_num, p_rtps_writerproxy->i_missing_seq_num,i);
			i--;
		}
	}


	rtps_writerproxy_missing_changes_update(p_rtps_writerproxy, a_heartbeat.last_sn.value);
	rtps_writerproxy_lost_changes_update(p_rtps_writerproxy, a_heartbeat.first_sn.value);

	if (p_rtps_reader->is_builtin == false)
	{
		Locator_t* remoteLocator = get_default_remote_participant_locator(p_rtps_writerproxy->remote_writer_guid);
		if(remoteLocator) trace_msg(OBJECT(current_object(0)), TRACE_TRACE3, "(%d.%d.%d.%d) >>> H_B[%5d]:([%5u]~[%5u]) ----------", remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15], a_heartbeat.count.value, a_heartbeat.first_sn.value.low, a_heartbeat.last_sn.value.low);
	}


	///////////////////////////////////////////
	if(p_rtps_writerproxy->is_ignore == false && p_rtps_writerproxy->i_for_reader_state != WAITING_STATE)
	{
		SequenceNumberSet missing_seq_num_set;
		int i_size;

		p_rtps_writerproxy->i_for_reader_state_for_ack = WAITING_STATE;

		{


				missing_seq_num_set.bitmap_base = sequnce_number_inc(rtps_writerproxy_available_changes_max_for_acknack(p_rtps_writerproxy));
				missing_seq_num_set.numbits = 0;
				memset(missing_seq_num_set.bitmap, '\0', sizeof(uint32_t) * 8);
				
				missing_seq_num_set = rtps_writerproxy_missing_changes_and_missing_sequencenumberset(p_rtps_writerproxy,  &i_size, missing_seq_num_set);
				

				
				//printf("send_acknack : %d\r\n", missing_seq_num_set.bitmap_base.low);
				send_acknack((rtps_statefulreader_t *)p_rtps_reader, p_rtps_writerproxy, missing_seq_num_set);

				

			}

	}



	/////////////////////////////////////////

	cond_signal(&p_rtps_reader->object_wait);
}

void rtps_receive_acknack_procedure( rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, AckNack a_acknack, octet flags )
{
	GUID_t guid;
	GUID_t myguid;
	rtps_writer_t *p_rtps_writer = NULL;
	rtps_statefulwriter_t *p_rtps_statefulwriter = NULL;
	bool matched_reader_find = false;
	int size=0;
	int i;
	rtps_endpoint_t **pp_endpoint = NULL;
	bool is_ok_numbit = false;

	

	//if(p_rtpsReader->behavior_type == STATEFUL_TYPE)
	{
		rtps_statefulreader_t *p_statefulreader = (rtps_statefulreader_t *)p_rtps_reader;
		rtps_writerproxy_t *p_rtps_writerproxy=NULL;

		memcpy(&guid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t));
		memcpy(&guid.entity_id, &a_acknack.reader_id, sizeof(EntityId_t));

		memcpy(&myguid.guid_prefix, &a_info_descript.guid_prefix, sizeof(GuidPrefix_t));
		memcpy(&myguid.entity_id, &a_acknack.writer_id, sizeof(EntityId_t));

		///// 관계되는 Writer한테 알려준다.....
		//p_rtps_writer = (rtps_writer_t *)rtps_get_Endpoint(myguid);

		
		pp_endpoint = rtps_matched_readerproxy_writer(guid, a_acknack.writer_id.value, &size);
		
		for(i=0; i < size && pp_endpoint; i++)
		{
			p_rtps_writer = (rtps_writer_t *)pp_endpoint[i];
		

			if(p_rtps_writer && p_rtps_writer->behavior_type == STATEFUL_TYPE)
			{
				int i_matched_reader = 0;
				int i = 0;
				p_rtps_statefulwriter = (rtps_statefulwriter_t*)p_rtps_writer;
				//먼저 remote의 리더에 대응되는지 판별함.

				mutex_lock(&p_rtps_statefulwriter->object_lock);

				i_matched_reader = p_rtps_statefulwriter->i_matched_readers;

				for(i=0; i < i_matched_reader; i++)
				{
					if(memcmp(&p_rtps_statefulwriter->pp_matched_readers[i]->remote_reader_guid, &guid, sizeof(GUID_t)) == 0)
					{
						// 같은게 맞음.
						/*
							the_rtps_writer.acked_changes_set(ACKNACK.reader_sn_state.base - 1);
							the_reader_proxy.requested_changes_set(ACKNACK.reader_sn_state.set);
						*/

						if(a_acknack.reader_sn_state.bitmap_base.high == 0 && a_acknack.reader_sn_state.bitmap_base.low == 0)
						{
							p_rtps_statefulwriter->pp_matched_readers[i]->is_zero = true;
						}
						
						if(a_acknack.reader_sn_state.numbits == 0)
						{
							is_ok_numbit = true;
							p_rtps_statefulwriter->pp_matched_readers[i]->is_heartbeatsend_and_wait_for_ack = false;
						}else{
							is_ok_numbit = is_ok_numbit;
						}


						p_rtps_statefulwriter->pp_matched_readers[i]->not_ack_count = 0;

						a_acknack.reader_sn_state.bitmap_base = sequnce_number_dec(a_acknack.reader_sn_state.bitmap_base);
						//rtps_readerproxy_acked_changes_set(p_rtps_statefulwriter->pp_matched_readers[i], sequnce_number_dec(a_AckNack.reader_sn_state.bitmap_base));
						rtps_readerproxy_acked_changes_set(p_rtps_statefulwriter->pp_matched_readers[i], a_acknack.reader_sn_state.bitmap_base);

						a_acknack.reader_sn_state.bitmap_base = sequnce_number_inc(a_acknack.reader_sn_state.bitmap_base);
						rtps_readerproxy_requested_chanages_set(p_rtps_statefulwriter->pp_matched_readers[i],a_acknack.reader_sn_state);
						

						p_rtps_statefulwriter->pp_matched_readers[i]->i_for_writer_fromReader_state = WAITING_STATE;
						p_rtps_statefulwriter->pp_matched_readers[i]->flags = flags;
						matched_reader_find = true;

					

						
						
						break;
					}
				}

				mutex_unlock(&p_rtps_statefulwriter->object_lock);

				//heartbeat의 영향으로 인해서, acknack에 대한 응답을 빨리하기 위해서
				//Signal처리
				cond_signal(&p_rtps_writer->object_wait);
			}

		}

		FREE(pp_endpoint);



		cond_signal(&p_rtps_statefulwriter->p_datawriter->entity_wait);

		//if(is_ok_numbit)
		//{
		//	bool is_writer_block = false;
		//	for(i=0 ;i < p_rtps_statefulwriter->i_matched_readers; i++)
		//	{
		//		// 이 부분을 넣은 이유는 remote reader가 ack에 대한 응답이
		//		// 안오기 때문에 계속 기다릴 경우, 속도가 늦어지기 때문이다.
		//		// 그리고 remote reader의 qos가 RELIABLE_RELIABILITY_QOS일 경우만, block 시킨다.
		//		//
		//		if(p_rtps_statefulwriter->pp_matched_readers[i]->reliablility_kind == RELIABLE_RELIABILITY_QOS
		//			&& p_rtps_statefulwriter->pp_matched_readers[i]->not_ack_count < NOT_ACK_COUNT)
		//		{
		//			if(p_rtps_statefulwriter->pp_matched_readers[i]->is_heartbeatsend_and_wait_for_ack == true)
		//			{
		//				is_writer_block = true;

		//				printf("NOT_ACK_COUNT : %d\r\n", p_rtps_statefulwriter->pp_matched_readers[i]->not_ack_count);
		//				break;
		//			}
		//		}
		//	}

		//	if(is_writer_block == false)
		//	{

		//		//printf("ack all : %d\n", a_acknack.reader_sn_state.bitmap_base.low);
		//		 cond_signal(&p_rtps_statefulwriter->p_datawriter->entity_wait);
		//		 //cond_signal(&p_rtps_statefulwriter->p_datawriter->ack_wait);
		//	}
		//	else {

		//		cond_signal(&p_rtps_statefulwriter->p_datawriter->entity_wait);
		//	}
		//	
		//}else{

		//	cond_signal(&p_rtps_statefulwriter->p_datawriter->entity_wait);
		//}


	}

	/*if(matched_reader_find)
	{
		cond_signal(&p_rtpsReader->object_wait);
	}*/


	


}


//일단 둘다 깨우기..
//연관된 새로운 Entity가 들어왔기 때문에..
static void WakeUpBuiltinPub_and_Sub()
{
	rtps_statefulwriter_t *p_statefulwriter = NULL;

	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_publication_writer();

	if (p_statefulwriter)
	{
		p_statefulwriter->heartbeat_period.sec = 1;
		cond_signal(&p_statefulwriter->object_wait);
	}

	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_subscription_writer();

	if (p_statefulwriter)
	{
		p_statefulwriter->heartbeat_period.sec = 1;
		cond_signal(&p_statefulwriter->object_wait);
	}
}


////int get_dds_partition_string(dds_partition_string* p_string, char* p_data, int string_size, int i_all_data_size)
////{
////	int32_t i_string = 0;
////
////	memset(p_string, 0, sizeof(dds_partition_string));
////	memcpy(p_string, p_data, i_all_data_size);
////	
////	if (p_string->size <= string_size)
////	{
////		i_string = p_string->size / 4;
////		if ((p_string->size % 4) != 0)
////		{
////			i_string++;
////		}
////		i_string *= 4;
////	}
////
////	i_string += sizeof(int32_t); // i_size
////
////	return i_string;
////}


void find_publisher_qos_from_parameterlist(PublisherQos *p_pqos, char *value, int32_t i_size)
{
	char *p_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;

	find_parameter_list(value,i_size, PID_PARTITION, &p_value, &i_found);
	if (i_found && p_value)
	{
		int32_t i;
		int32_t i_partion_length = *(int32_t*)p_value;
		i_num += sizeof(int32_t);

		for (i = 0; i < i_partion_length; i++)
		{
			dds_partition_string partition_string;
			i_num += get_dds_partition_string(&partition_string, &p_value[i_num], QOS_PARTITION_STRING_SIZE, i_found-i_num);
			i_found = i_found;
			INSERT_ELEM(p_pqos->partition.name.pp_string, p_pqos->partition.name.i_string, p_pqos->partition.name.i_string, strdup(partition_string.value));
		}
	}
	i_found = 0;
}


void find_subscriber_qos_from_parameterlist(SubscriberQos *p_sqos, char *value, int32_t i_size)
{
	char *p_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;

	find_parameter_list(value,i_size, PID_PARTITION, &p_value, &i_found);
	if(i_found && p_value)
	{
		int32_t i;
		int32_t i_partion_length = *(int32_t*)p_value;
		i_num += sizeof(int32_t);

		for(i=0 ; i < i_partion_length; i++)
		{
			dds_partition_string partition_string;
			i_num += get_dds_partition_string(&partition_string, &p_value[i_num], QOS_PARTITION_STRING_SIZE, i_found-i_num);
			i_found = i_found;
			INSERT_ELEM( p_sqos->partition.name.pp_string, p_sqos->partition.name.i_string,
				p_sqos->partition.name.i_string, strdup(partition_string.value) );
		}
	}
	i_found = 0;
}


bool rtps_remove_remote_entity(DataFull* p_datafull)
{
	bool is_builtin = false;
	bool is_remove = false;
	//remove remote entity........


	if (p_datafull->inline_qos.i_linked_size)
	{
		int i = 0;
		GUID_t remote_pariticpant_guid;
		GUID_t key_guid;
		bool found_key_guid = false;
		bool found_state_info = false;
		int32_t info_value = 0;

		rtps_statefulreader_t *p_removeReader = NULL;
		rtps_writerproxy_t *p_remove_writerproxy = NULL;

		rtps_statefulwriter_t *p_removeWriter = NULL;
		rtps_readerproxy_t *p_remove_readerproxy = NULL;


		ParameterWithValue *p_atom_parameter;

		memset(&key_guid, 0, sizeof(GUID_t));


		p_atom_parameter = (ParameterWithValue *)p_datafull->inline_qos.p_head_first;

		while (p_atom_parameter)
		{
			//trace_msg(NULL, TRACE_LOG, "Dispose inline_qos 1");

			if (found_key_guid && found_state_info)
			{
				break;
			}
			else if (p_atom_parameter->parameter_id == PID_KEY_HASH && p_atom_parameter->length == 16)
			{
				found_key_guid = true;
				key_guid = *((GUID_t*)p_atom_parameter->p_value);

				//trace_msg(NULL, TRACE_LOG, "Dispose inline_qos 2");
			}
			else if (p_atom_parameter->parameter_id == PID_STATUS_INFO && p_atom_parameter->length == 4)
			{
				const uint8_t *p = p_atom_parameter->p_value;
				info_value = ((int32_t)p[0] << 24) |
									((int32_t)p[1] << 16) |
									((int32_t)p[2] << 8) |
									((int32_t)p[3]);

				found_state_info = true;

				//trace_msg(NULL, TRACE_LOG, "Dispose inline_qos 3");
				if (info_value == 3)
				{
					//trace_msg(NULL, TRACE_LOG, "Dispose inline_qos 5");

					is_remove = true;
					if (memcmp(&p_datafull->writer_id.value, &ENTITYID_UNKNOWN, sizeof(EntityId_t)) != 0)
					{
						int size = 0;
						int i;
						rtps_endpoint_t **pp_endpoint = rtps_matched_readerproxy_writer(key_guid, ENTITYID_UNKNOWN, &size);
						
						for (i = 0; i < size && pp_endpoint; i++)
						{
							p_removeWriter = (rtps_statefulwriter_t *)pp_endpoint[i];
							p_remove_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_removeWriter, key_guid);
							rtps_statefulwriter_matched_reader_remove(p_removeWriter, p_remove_readerproxy);
							trace_msg(NULL, TRACE_LOG, "Dispose matched reader : %d", p_removeWriter->i_matched_readers);
						}

						FREE(pp_endpoint);
					}
					
					if (memcmp(&p_datafull->reader_id.value, &ENTITYID_UNKNOWN, sizeof(EntityId_t)) != 0)
					{
						int size = 0;
						int i;
						rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(key_guid, ENTITYID_UNKNOWN, &size);

						for (i = 0; i < size; i++)
						{
							p_removeReader = (rtps_statefulreader_t *) pp_endpoint[i];
							p_remove_writerproxy = rtps_statefulreader_matched_writer_lookup(p_removeReader, key_guid);
							rtps_statefulreader_matched_writer_remove(p_removeReader,p_remove_writerproxy);
							trace_msg(NULL, TRACE_LOG, "Dispose matched writer : %d", p_removeReader->i_matched_writers);

							//[KKI] dispose 메시지를 수신하여 WriterProxy 를 삭제할 때, <key_guid> 인 Writer 가 만든 cachechange 의 instance_state 를 NOT_ALIVE_DISPOSED_INSTANCE_STATE 로 바꾼다.
							qos_rtps_reader_set_instance_state_of_cachechanges_with_guid((rtps_reader_t*)p_removeReader, NOT_ALIVE_DISPOSED_INSTANCE_STATE, &key_guid);//by kki (reader data lifecycle)
						}

						FREE(pp_endpoint);
					}

					if (memcmp(&p_datafull->writer_id.value, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0)
					{
						int size = 0;
						int i;
						rtps_endpoint_t **pp_endpoint = rtps_matched_readerproxy_writer(key_guid, ENTITYID_UNKNOWN, &size);
						
						for (i = 0; i < size && pp_endpoint; i++)
						{
							p_removeWriter = (rtps_statefulwriter_t *)pp_endpoint[i];
							p_remove_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_removeWriter, key_guid);
							rtps_statefulwriter_matched_reader_remove(p_removeWriter, p_remove_readerproxy);

							trace_msg(NULL, TRACE_LOG, "Dispose matched reader : %d", p_removeWriter->i_matched_readers);
						}

						FREE(pp_endpoint);

						is_builtin = true;
					}
					else if (memcmp(&p_datafull->writer_id.value, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0)
					{
						int size = 0;
						int i;
						rtps_endpoint_t **pp_endpoint = rtps_matched_writerproxy_reader(key_guid, ENTITYID_UNKNOWN, &size);

						for (i = 0; i < size; i++)
						{
							p_removeReader = (rtps_statefulreader_t *) pp_endpoint[i];
							p_remove_writerproxy = rtps_statefulreader_matched_writer_lookup(p_removeReader, key_guid);
							rtps_statefulreader_matched_writer_remove(p_removeReader,p_remove_writerproxy);						
							trace_msg(NULL, TRACE_LOG, "Dispose matched writer : %d", p_removeReader->i_matched_writers);
						}


						FREE(pp_endpoint);

						is_builtin = true;
					}
					else if (memcmp(&p_datafull->writer_id.value, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0)
					{
						is_builtin = true;
					}				
				}				
			}
			else if (p_atom_parameter->parameter_id == PID_KEY_HASH && p_atom_parameter->length == 16)
			{
				remote_pariticpant_guid = *((GUID_t*)&p_atom_parameter->p_value[0]);
			}


			p_atom_parameter = (ParameterWithValue *)p_atom_parameter->a_tom.p_next;
		}

		if (found_key_guid && found_state_info)
		{
			//trace_msg(NULL, TRACE_LOG, "Dispose inline_qos 4");			
		}
	}

	return is_remove;
}


static bool is_define_user_entity_lock = false;

bool get_define_user_entity_lock()
{
	return is_define_user_entity_lock;
}

void instanceset_dispose(Entity* p_entity, GUID_t key_guid);
void instanceset_unregister(Entity* p_entity, GUID_t key_guid);


static SequenceNumber_t	sequence_number_test;
static bool is_first = true;


void rtps_receive_data_procedure( rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, Data a_data, SerializedPayload* p_serialized_data, octet flags, bool b_remove_serialized_data, DataFull* p_datafull )
{
	GUID_t guid;
	SerializedPayloadForReader *p_serialized = NULL;
	int32_t	i_size = 0;
	char *p_value = NULL;
	int32_t	i_found = 0;
	GUID_t	a_remoteEndpointGuid;
	char *p_TopicName = NULL;
	char *p_TypeName = NULL;
	rtps_reader_t *p_temp_rtps_reader = NULL;
	rtps_writer_t *p_temp_rtps_writer = NULL;
	bool is_builtin = false;
	rtps_cachechange_t	*p_rtps_cachechange = NULL;

	//added by kyy //////////////////
	//	Duration_t	duration;
	bool		compare_value = true;
	bool		partition_value = true;
	bool		correct_deadline = false;
	/*RxOQos *p_pub_rxo_qos = NULL;
	RxOQos *p_sub_rxo_qos = NULL;
	RxOQos *p_requested_qos = NULL;
	RxOQos *p_offered_qos = NULL;*/

	RxOQos a_pub_rxo_qos;
	RxOQos a_sub_rxo_qos;
	RxOQos a_requested_qos;
	RxOQos a_offered_qos;

	bool is_remove = false;
	static int PUBLISH = 1;
	static int SUBSCRIBE = 2;
	static int TOPIC = 3;
	int	builtintype = -1;


	/////////////////////////////////
	/////added by kyy//////////////////////////
	Duration_t			receive_time;
	///////////////////////////////////////////
	//added by kyy
	receive_time = current_duration();
	//p_pub_rxo_qos = malloc(sizeof(RxOQos));
	//p_sub_rxo_qos = malloc(sizeof(RxOQos));
	//p_requested_qos	= malloc(sizeof(RxOQos));
	//p_offered_qos	= malloc(sizeof(RxOQos));
	//////////////////////////////////////////////////

	memset(&a_pub_rxo_qos, 0, sizeof(RxOQos));
	memset(&a_sub_rxo_qos, 0, sizeof(RxOQos));
	memset(&a_requested_qos, 0, sizeof(RxOQos));
	memset(&a_offered_qos, 0, sizeof(RxOQos));

	//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 1(%d)", p_rtpsReader->behavior_type);
	if(p_serialized_data){
		p_serialized = (SerializedPayloadForReader *)p_serialized_data;
		i_size = p_serialized->i_size;
	}else{
		p_serialized_data = NULL;
	}

	p_rtps_reader->received_sample_count++;
	p_rtps_reader->received_sample_bytes += i_size;

	p_rtps_reader->received_sample_count_throughput++;
	p_rtps_reader->received_sample_bytes_throughput += i_size;


	

	if(p_rtps_reader->behavior_type == STATEFUL_TYPE)
	{
		rtps_statefulreader_t *p_statefulreader = (rtps_statefulreader_t *)p_rtps_reader;
		rtps_writerproxy_t *p_rtps_writerproxy=NULL;


		//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 2");

		memcpy(&guid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t));
		memcpy(&guid.entity_id, &a_data.writer_id, sizeof(EntityId_t));

		if(p_statefulreader->pp_matched_writers == NULL) {
			if(p_serialized_data && b_remove_serialized_data)
			{
				FREE(p_serialized_data->p_value);
				FREE(p_serialized_data);
			}

			//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 3");
			return;
		}

		if(p_datafull && p_datafull->p_serialized_data == NULL)
		{
			//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 4");
			is_remove = rtps_remove_remote_entity(p_datafull);


			
			
		}else{

			is_define_user_entity_lock = true;

			if(memcmp(&a_data.writer_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0)
			{
				PublisherQos pqos;
				UserDataQosPolicy p_user_data_qos;
				GroupDataQosPolicy p_group_data_qos;
				is_builtin = true;
				builtintype = PUBLISH;
				memcpy(&guid.entity_id, &a_data.writer_id, sizeof(EntityId_t));
				////////////////////////////////////////
				// rtps reader의 Topic 및 TypeName이 있는지 살펴본다.
				//rtps_compare_reader(guid, 
				//partition
				static_get_default_publisher_qos(&pqos);

				//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 5");

				find_publisher_qos_from_parameterlist(&pqos, (char*)p_serialized->p_value, i_size);

				//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////
				qos_find_user_data_qos_from_parameterlist(&p_user_data_qos, (char*)p_serialized->p_value, i_size);
				//printf("Builtin_pub_writer : ");
				//qos_print_user_data_qos(p_user_data_qos);
				////////////////////////////////////////////////////////////////////////////////////////////
				//added by kyy(GroupData QoS)///////////////////////////////////////////////////////////////////
				qos_find_group_data_qos_from_parameterlist(&p_group_data_qos,(char*)p_serialized->p_value, i_size);
				//printf("Builtin_pub_writer : ");
				//qos_print_group_data_qos(p_group_data_qos);
				////////////////////////////////////////////////////////////////////////////////////////////				

				find_parameter_list((char*)p_serialized->p_value, i_size, PID_ENDPOINT_GUID, &p_value, &i_found);
				if (i_found && p_value)
				{
					a_remoteEndpointGuid = *(GUID_t*)p_value;
					i_found = 0;

					//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 6");

					find_parameter_list((char*)p_serialized->p_value, i_size, PID_TOPIC_NAME, &p_value, &i_found);
					if (i_found && p_value)
					{
						i_found = 0;
						p_TopicName = p_value+4;

						//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 7");

						find_parameter_list((char*)p_serialized->p_value, i_size, PID_TYPE_NAME, &p_value, &i_found);
						if (i_found && p_value)
						{
							rtps_endpoint_t** pp_rtpsEndpoint = NULL;
							int ii_size = 0;
							int i;

							p_TypeName = p_value+4;

							//trace_msg(NULL,TRACE_LOG,"rtps_receive_data_procedure : 8");
							pp_rtpsEndpoint = rtps_compare_reader(guid, p_TopicName, p_TypeName, &ii_size);
							find_rxo_qos_from_parameterlist(&a_pub_rxo_qos, p_serialized->p_value, i_size);

							for (i = 0; i < ii_size; i++)
							{
								p_temp_rtps_reader = (rtps_reader_t *)pp_rtpsEndpoint[i];
								//printf("ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER : %s\r\n", p_TopicName);
								if (p_temp_rtps_reader && p_temp_rtps_reader->behavior_type == STATEFUL_TYPE)
								{
									Locator_t* remoteLocator = get_default_remote_participant_locator(a_remoteEndpointGuid);
									Locator_t* remoteMulticastLocator = get_default_remote_participant_multicast_locator(a_remoteEndpointGuid);
									rtps_writerproxy_t* p_rtpswriterproxy = NULL;
									bool equal_parition = true;

									//trace_msg(NULL, TRACE_LOG, "rtps_receive_data_procedure : 9");

									//////////////////////////////////////////////////////////////////////////////////////////////////
									//added by kyy (RxO Compare : requested vs offered)
									//RxO Compare
									/*p_pub_rxo_qos = malloc(sizeof(RxOQos));
									p_requested_qos	= malloc(sizeof(RxOQos));*/

									
									
									get_requested_qos(&a_requested_qos, p_temp_rtps_reader);
									compare_value = compare_rxo(&a_pub_rxo_qos, &a_requested_qos);
									/*FREE(p_pub_rxo_qos);
									FREE(p_requested_qos);*/
									///////////////////////////////////////////////////////////////////////////////////////////////
				

									if (p_temp_rtps_reader->p_datareader && p_temp_rtps_reader->p_datareader->p_subscriber)
									{
										equal_parition = qos_compare_partition(pqos.partition, p_temp_rtps_reader->p_datareader->p_subscriber->subscriber_qos.partition);
										if (equal_parition == false) trace_msg(NULL, TRACE_LOG, "Partition Mismatched");
									}

									REMOVE_STRING_SEQ(pqos.partition.name);

									if (equal_parition && remoteLocator && compare_value)
									{
										rtps_statefulwriter_t* p_statefulwriter = NULL;
										Locator_t* p_unicatlocator = (Locator_t *)malloc(sizeof(Locator_t));
										Locator_t* p_multicastlocator = (Locator_t *)malloc(sizeof(Locator_t));
										bool b_ret;

										i_found=0;

										memset(p_unicatlocator, 0, sizeof(Locator_t));
										memset(p_multicastlocator, 0, sizeof(Locator_t));

										find_parameter_list((char*)p_serialized->p_value, i_size, PID_UNICAST_LOCATOR, &p_value, &i_found);

										if (i_found == 24)
										{
											memcpy(p_unicatlocator, p_value, sizeof(Locator_t));
										}
										else
										{
											memcpy(p_unicatlocator, remoteLocator, sizeof(Locator_t));
										}

										i_found=0;
	
										find_parameter_list((char*)p_serialized->p_value,i_size, PID_MULTICAST_LOCATOR, &p_value, &i_found);

										if (i_found == 24)
										{
											memcpy(p_multicastlocator, p_value, sizeof(Locator_t));
										}
										else
										{
											memcpy(p_multicastlocator, remoteMulticastLocator, sizeof(Locator_t));
										}
										

										//만일 이미 연결되어 있는데, 새로 EDP를 받은 경우라면
										if ((p_rtpswriterproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_temp_rtps_reader, a_remoteEndpointGuid)) != NULL)
										{
											find_parameter_list(p_serialized->p_value, p_serialized->i_size, PID_OWNERSHIP_STRENGTH, &p_value, &i_found);
											if (i_found && p_value)
											{
												p_rtpswriterproxy->ownership_strength = *(int32_t*)p_value;
												qos_change_owner_writer(p_temp_rtps_reader, p_rtpswriterproxy, true);
											}
										}
										else
										{
											p_rtpswriterproxy = rtps_writerproxy_new(p_temp_rtps_reader, a_remoteEndpointGuid, p_unicatlocator, p_multicastlocator);

											qos_writerproxy_set_liveliness_kind_and_lease_duration(p_rtpswriterproxy, a_pub_rxo_qos);//by kki (liveliness)

											find_parameter_list(p_serialized->p_value, p_serialized->i_size, PID_OWNERSHIP_STRENGTH, &p_value, &i_found);
											if (i_found && p_value)
											{
												p_rtpswriterproxy->ownership_strength = *(int32_t*)p_value;
											}
											//added by kyy(Lifespan)
											qos_find_lifespan_qos_from_parameterlist(p_rtpswriterproxy, p_serialized->p_value, i_size);
											///////////////////////////////////////////////////////////////////////////////////////

											b_ret = rtps_statefulreader_matched_writer_add((rtps_statefulreader_t *)p_temp_rtps_reader, p_rtpswriterproxy);

											/// 연관된 Builtin writer 깨우기
											/*p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_publication_writer();

											if(p_statefulwriter){
												p_statefulwriter->heartbeatPeriod.sec = 1;
												cond_signal(&p_statefulwriter->object_wait);
											}*/
											//WakeUpBuiltinPub_and_Sub();

											if (b_ret)
											{
												//printf("Add WriterProxy\r\n");
												trace_msg(NULL, TRACE_LOG, "Add the WriterProxy[%s:%d] to a Reader.[%d.%d.%d.%d]",  p_TopicName, ((rtps_statefulreader_t *)p_temp_rtps_reader)->i_matched_writers, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
											}else
												trace_msg(NULL, TRACE_LOG, "Couldn't add the WriterProxy[%s] to a Reader.[%d.%d.%d.%d]", p_TopicName, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
											trace_msg(NULL, TRACE_LOG, "Current Matched Writers : [%d]", ((rtps_statefulreader_t *)p_temp_rtps_reader)->i_matched_writers);
										}
									}
									else
									{
										if (remoteLocator)
											trace_msg(NULL, TRACE_LOG, "Couldn't add the WriterProxy[%s] to a Reader.[%d.%d.%d.%d]", p_TopicName, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
										else
											trace_msg(NULL, TRACE_LOG, "Couldn't add the WriterProxy[%s] to a Reader. remoteLocator is null", p_TopicName);
									}
								}
								else
								{									
								}
							}

							if (ii_size == 0) trace_msg(NULL, TRACE_LOG, "Couldn't add the WriterProxy[%s] to a Reader. : No Matched proxy does not exist!!", p_TopicName);

							FREE(pp_rtpsEndpoint);
						}
					}
				}

				WakeUpBuiltinPub_and_Sub();			
			}

			if (memcmp(&a_data.reader_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
			{
				////////////////////////////////////////

			}

			if (memcmp(&a_data.writer_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0)
			{
				SubscriberQos sqos;
				UserDataQosPolicy p_user_data_qos;
				GroupDataQosPolicy p_group_data_qos;
				is_builtin = true;
				builtintype = SUBSCRIBE;
				////////////////////////////////////////
				////////////////////////////////////////
				// rtps reader의 Topic 및 TypeName이 있는지 살펴본다.
				//rtps_compare_reader(guid, 


				memset(&p_user_data_qos, 0, sizeof(UserDataQosPolicy));
				memset(&p_group_data_qos, 0, sizeof(GroupDataQosPolicy));

				static_get_default_subscriber_qos(&sqos);

				find_subscriber_qos_from_parameterlist(&sqos, (char*)p_serialized->p_value, i_size);

				//added by kyy(User QoS)///////////////////////////////////////////////////////////////////
				qos_find_user_data_qos_from_parameterlist(&p_user_data_qos, (char*)p_serialized->p_value, i_size);
				//printf("Builtin_sub_writer : ");
				qos_print_user_data_qos(p_user_data_qos);
				////////////////////////////////////////////////////////////////////////////////////////////
				//added by kyy(Group QoS)///////////////////////////////////////////////////////////////////
				qos_find_group_data_qos_from_parameterlist(&p_group_data_qos,(char*)p_serialized->p_value, i_size);
				//printf("Builtin_sub_writer : ");
				qos_print_group_data_qos(p_group_data_qos);
				////////////////////////////////////////////////////////////////////////////////////////////

				find_parameter_list((char*)p_serialized->p_value,i_size, PID_ENDPOINT_GUID, &p_value, &i_found);
				if(i_found && p_value)
				{
					a_remoteEndpointGuid = *(GUID_t*)p_value;
					i_found = 0;

					find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
					if(i_found && p_value)
					{
						i_found = 0;
						p_TopicName = p_value+4;

						find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);

						

						if(i_found && p_value)
						{
							rtps_endpoint_t		**pp_rtpsEndpoint =  NULL;
							int ii_size=0;
							int i;

							p_TypeName = p_value+4;

							pp_rtpsEndpoint = rtps_compare_writer(guid, p_TopicName, p_TypeName, &ii_size);

							find_rxo_qos_from_parameterlist(&a_sub_rxo_qos, p_serialized->p_value, i_size);
							
							for(i=0; i < ii_size; i++)
							{

								p_temp_rtps_writer = (rtps_writer_t *)pp_rtpsEndpoint[i];
								if(p_temp_rtps_writer && p_temp_rtps_writer->behavior_type == STATEFUL_TYPE)
								{
									Locator_t *remoteLocator = get_default_remote_participant_locator(a_remoteEndpointGuid);
									Locator_t *remoteMulticastLocator = get_default_remote_participant_multicast_locator(a_remoteEndpointGuid);
									rtps_readerproxy_t *p_rtps_readerproxy = NULL;
									bool equal_parition = true;

									//////////////////////////////////////////////////////////////////////////////////////////////////
									//added by kyy (RxO Compare : requested vs offered)
									//RxO Compare
									/*p_sub_rxo_qos = malloc(sizeof(RxOQos));
									p_offered_qos	= malloc(sizeof(RxOQos));*/
									
									get_offered_qos(&a_offered_qos, p_temp_rtps_writer);
									compare_value = compare_rxo(&a_offered_qos, &a_sub_rxo_qos);

									/*FREE(p_sub_rxo_qos);
									FREE(p_offered_qos);*/


									if (p_temp_rtps_writer->p_datawriter && p_temp_rtps_writer->p_datawriter->p_publisher)
									{
										equal_parition = qos_compare_partition(sqos.partition,  p_temp_rtps_writer->p_datawriter->p_publisher->publisher_qos.partition);
									}

									REMOVE_STRING_SEQ(sqos.partition.name);


									if (equal_parition && remoteLocator && compare_value)
									{
										rtps_statefulwriter_t *p_statefulwriter = NULL;
										Locator_t *p_unicatlocator = (Locator_t *)malloc(sizeof(Locator_t));
										Locator_t *p_multicastlocator = (Locator_t *)malloc(sizeof(Locator_t));
										bool b_ret;

										i_found=0;

										memset(p_unicatlocator, 0, sizeof(Locator_t));
										memset(p_multicastlocator, 0, sizeof(Locator_t));

										find_parameter_list((char*)p_serialized->p_value,i_size, PID_UNICAST_LOCATOR, &p_value, &i_found);

										if(i_found == 24){
											memcpy(p_unicatlocator, p_value, sizeof(Locator_t));
										}else{
											memcpy(p_unicatlocator, remoteLocator, sizeof(Locator_t));
										}

										i_found=0;
	
										find_parameter_list((char*)p_serialized->p_value,i_size, PID_MULTICAST_LOCATOR, &p_value, &i_found);

										if(i_found == 24){
											memcpy(p_multicastlocator, p_value, sizeof(Locator_t));
										}else{
											memcpy(p_multicastlocator, remoteMulticastLocator, sizeof(Locator_t));
										}

										p_rtps_readerproxy = rtps_readerproxy_new(p_temp_rtps_writer, a_remoteEndpointGuid, p_unicatlocator, p_multicastlocator, false, a_sub_rxo_qos.durability.kind);
										p_rtps_readerproxy->remote_endpoint_guid = a_remoteEndpointGuid;
										p_rtps_readerproxy->reliablility_kind = a_sub_rxo_qos.reliability.kind;

										b_ret = rtps_statefulwriter_matched_reader_add((rtps_statefulwriter_t *)p_temp_rtps_writer, p_rtps_readerproxy);
										/// 연관된 Builtin writer 깨우기
										/*p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_subscription_writer();

										if(p_statefulwriter){
											p_statefulwriter->heartbeatPeriod.sec = 1;
											cond_signal(&p_statefulwriter->object_wait);
										}*/

										//WakeUpBuiltinPub_and_Sub();

										

										if(b_ret) {
											//printf("Add ReaderProxy\r\n");
											trace_msg(NULL, TRACE_LOG, "Add ReaderProxy[%s:%d] to a Writer.[%d.%d.%d.%d]", p_TopicName, ((rtps_statefulwriter_t *)p_temp_rtps_writer)->i_matched_readers, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
										}else{
											//printf("Aleady ReaderProxy\r\n");
										}
									}else{
										if(remoteLocator == NULL)
											trace_msg(NULL, TRACE_LOG, "Couldn't add ReaderProxy[%s] to a Writer.May be Participant Not yet ready..", p_TopicName);
										else
											trace_msg(NULL, TRACE_LOG, "Couldn't add ReaderProxy[%s] to a Writer.[%d.%d.%d.%d]", p_TopicName, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
									}
								
								}else
								{

								}
							}

							if(ii_size == 0) trace_msg(NULL, TRACE_LOG, "Couldn't add ReaderProxy[%s] to a Writer : No Matched proxy does not exist!!", p_TopicName);

							FREE(pp_rtpsEndpoint);
						}
					}

				}
				WakeUpBuiltinPub_and_Sub();
			}

			if(memcmp(&a_data.reader_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
			{
				////////////////////////////////////////

			}


			if(memcmp(&a_data.writer_id, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0)
			{
				////////////////////////////////////////////
				is_builtin = true;
				builtintype = TOPIC;
				find_parameter_list((char*)p_serialized->p_value,i_size, PID_ENDPOINT_GUID, &p_value, &i_found);
				if(i_found && p_value)
				{
					a_remoteEndpointGuid = *(GUID_t*)p_value;
					i_found = 0;

					find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
					if(i_found && p_value)
					{
						rtps_endpoint_t		*p_rtpsEndpoint;
						i_found = 0;
						p_TopicName = p_value+4;


						p_rtpsEndpoint = rtps_find_topic(p_TopicName);

						if(p_rtpsEndpoint != NULL){
							rtps_topic_t *p_rtps_topic = (rtps_topic_t*)p_rtpsEndpoint;
							Topic *p_topic = p_rtps_topic->p_topic;

							find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
							if(i_found && p_value)
							{
								p_TypeName = p_value+4;

								if(have_topic_name_topic_type_topic(p_topic, p_TopicName, p_TypeName))
								{
									rtps_endpoint_t		**pp_rtpsEndpoint =  NULL;
									int ii_size=0;
									GUID_t guid;


									RxOQos a_pub_rxo_qos;
									RxOQos a_sub_rxo_qos;
									RxOQos a_requested_qos;
									RxOQos a_offered_qos;

									TopicQos tqos;

									memset(&a_pub_rxo_qos, 0, sizeof(RxOQos));
									memset(&a_sub_rxo_qos, 0, sizeof(RxOQos));
									memset(&a_requested_qos, 0, sizeof(RxOQos));
									memset(&a_offered_qos, 0, sizeof(RxOQos));


									memset(&guid,0,sizeof(GUID_t));

									static_get_default_topic_qos(&tqos);

									find_rxo_qos_from_parameterlist(&a_pub_rxo_qos, p_serialized->p_value, i_size);

									/// Qos 체크


								}else{
									change_topic_status(p_topic);
								}

							}else{
								change_topic_status(p_topic);
							}
						}
					}else{

					}
				}
			}

			//by kki...(liveliness)
			if (memcmp(&a_data.writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0)
			{
				qos_participant_liveliness_set_alive(p_serialized);

				is_builtin = true;
			}
		}

		

		//[KKI] p_rtps_reader 의 matched_writer 중에서 guid 를 가진 것을 찾아준다.
		p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtps_reader, guid);
		//[KKI] p_rtps_writerproxy 의 i_ref_count 를 하나 증가시킨다. 이유는 모른다.
		if (p_rtps_writerproxy) rtps_writerproxy_ref(p_rtps_writerproxy);

		if (!p_rtps_reader->is_builtin && p_rtps_writerproxy)
		{
			if (p_rtps_reader->reliability_level == RELIABLE 
				&& (p_rtps_reader->p_datareader->datareader_qos.durability.kind == TRANSIENT_LOCAL_DURABILITY_QOS 
					|| p_rtps_reader->p_datareader->datareader_qos.durability.kind == TRANSIENT_DURABILITY_QOS))
			{
				if (p_rtps_writerproxy->is_first_ack == true)
				{
					if (p_serialized_data && b_remove_serialized_data)
					{
						FREE(p_serialized_data->p_value);
						FREE(p_serialized_data);
					}
					if (p_rtps_writerproxy) rtps_writerproxy_unref(p_rtps_writerproxy);
					is_define_user_entity_lock = false;

					return;
				}

				/*if(!(SequnceNumberLeftBiggerAndEqual(a_data.writer_sn.value , p_rtps_writerproxy->a_remote_heartbeat.first_sn.value)
					&& SequnceNumberLeftBiggerAndEqual( p_rtps_writerproxy->a_remote_heartbeat.last_sn.value, a_data.writer_sn.value)))
				{
					if(p_serializedData && b_removeSerializedData)
					{
						FREE(p_serializedData->p_value);
						FREE(p_serializedData);
					}

					return;
				}*/

				/*mutex_lock(&p_rtps_writerproxy->object_lock);

				UpdateWriterProxyFromrtpsReader(p_rtps_writerproxy);

				if(p_rtps_writerproxy->i_changes_from_writer)
				{
					rtps_cachechange_t* p_rtps_cachechange = p_rtps_writerproxy->changes_from_writer[p_rtps_writerproxy->i_changes_from_writer-1];

					if(p_rtps_cachechange->sequence_number.low + 1 != a_data.writer_sn.value.low)
					{
						if(p_serializedData && b_removeSerializedData)
						{
							FREE(p_serializedData->p_value);
							FREE(p_serializedData);
						}
						mutex_unlock(&p_rtps_writerproxy->object_lock);
						return;
					}
				}
				
				mutex_unlock(&p_rtps_writerproxy->object_lock);*/
			}

			qos_manual_by_topic_liveliness_set_alive(p_rtps_writerproxy);//by kki (liveliness)

		}

		//if(p_rtps_writerproxy && p_rtps_writerproxy->p_rtpsReader->p_datareader->test_for_reliablity_division == 0)
		//{
		//	if(p_serializedData && b_removeSerializedData)
		//	{
		//		FREE(p_serializedData->p_value);
		//		FREE(p_serializedData);
		//	}
		//	return;
		//}

		//if( p_rtps_writerproxy && a_data.writer_sn.value.low % p_rtps_writerproxy->p_rtpsReader->p_datareader->test_for_reliablity_division != 0)
		////if( a_data.writer_sn.value.low % 2 != 0)
		//{
		//	if(p_serializedData && b_removeSerializedData)
		//	{
		//		FREE(p_serializedData->p_value);
		//		FREE(p_serializedData);
		//	}
		//	return;
		//}


		

		if (p_rtps_writerproxy && (p_rtps_reader->behavior_type == STATEFUL_TYPE && p_rtps_reader->reliability_level != BEST_EFFORT) )
		{
		//	GUID_t writer_guid;
			GUID_t key_guid;
			bool found_key_guid = false;
			bool found_state_info = false;


			memset(&key_guid,0, sizeof(GUID_t));
	//		memset(&writer_guid,0, sizeof(GUID_t));

			//added by kyy(Deadline QoS)
			//Deadline 검사 후 base_time 수정
			if(!is_builtin)
			{
				if(p_rtps_writerproxy->start == 0)
				{
					p_rtps_writerproxy->deadline_base_time.sec = receive_time.sec;
					p_rtps_writerproxy->deadline_base_time.nanosec = receive_time.nanosec;
					p_rtps_writerproxy->start++;
				}
				//correct_deadline = compare_deadline(receive_time, &(p_rtps_writerproxy->deadline_base_time), p_statefulreader->p_datareader->datareader_qos.deadline.period);
				correct_deadline = qos_compare_deadline(receive_time, &(p_rtps_writerproxy->deadline_base_time), p_statefulreader->p_datareader->datareader_qos.deadline.period);
				p_rtps_writerproxy->b_is_deadline_on = correct_deadline;//by kki (ownership)
				//status by jun 
				if (correct_deadline == false)
				{
					qos_change_owner_writer(p_rtps_reader, p_rtps_writerproxy,false);
					change_RequestedDeadlineMissed_status(p_rtps_reader->p_datareader, 0);
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//by kki...(ownership)
			// 1) reader 가 exclusive 인지 확인
			// 2) writer 가 owner 인지 확인
			// 3) DATA 를 historycache 에 저장
			p_temp_rtps_reader = (rtps_reader_t *)p_rtps_reader;

			if (p_serialized && qos_has_exclusive_ownership(p_temp_rtps_reader))
			{
				int32_t index = qos_set_exclusive_writer_key(p_temp_rtps_reader, p_rtps_writerproxy, p_serialized);
				if (index == -1 || p_temp_rtps_reader->pp_exclusive_writers[index]->b_is_owner == false)
				{
					if (p_serialized_data && b_remove_serialized_data)
					{
						FREE(p_serialized_data->p_value);
						FREE(p_serialized_data);
					}
					if(p_rtps_writerproxy) rtps_writerproxy_unref(p_rtps_writerproxy);
					is_define_user_entity_lock = false;
					return;
				}
			}


			
			


			/*if(p_statefulreader->p_reader_cache->i_linked_size >100 &&  p_statefulreader->p_datareader->id == 25)
			{
				return;

			}else{*/

				p_rtps_cachechange = check_key_guid_for_reliability(p_statefulreader, guid, p_datafull, p_serialized_data, &key_guid, &found_key_guid, &found_state_info);
			//}

			/*if(is_remove){
				p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, guid, a_data.writer_sn.value, p_serialized_data, (InstanceHandle_t)p_serialized_data);
			}else{
				p_rtps_cachechange = rtps_cachechange_new(ALIVE, guid, a_data.writer_sn.value, p_serialized_data, (InstanceHandle_t)p_serialized_data);
			}*/

#if 0
			if(p_statefulreader->p_datareader->id == 25)
			{
				if(is_first == true)
				{
					sequence_number_test.high = 0;
					sequence_number_test.low = 0;
					is_first = false;

					sequence_number_test = p_rtps_cachechange->sequence_number;
				}else{

					if(sequence_number_test.low+1 != p_rtps_cachechange->sequence_number.low)
					{
						is_first = false;
					}

					sequence_number_test = p_rtps_cachechange->sequence_number;
					
				}
			}
#endif

			if (is_builtin)
			{
				message_t *p_message = malloc(sizeof(message_t));
				memset(p_message,0, sizeof(message_t));

				if(builtintype == PUBLISH)
				{
					PublicationBuiltinTopicData* p_publish_data = malloc(sizeof(PublicationBuiltinTopicData));

					memset(p_publish_data, 0, sizeof(PublicationBuiltinTopicData));

					memcpy(&p_publish_data->key, &guid, sizeof(GUID_t));
					memcpy(&p_publish_data->participant_key, &guid, sizeof(GUID_t));
					qos_find_publication_qos_from_parameterlist(p_publish_data, (char*)p_serialized->p_value, i_size);

					p_message->i_datasize = sizeof(PublicationBuiltinTopicData);
					p_message->v_data = p_publish_data;

					/*SubscriptionBuiltinTopicData *p_subscrion_data = malloc(sizeof(SubscriptionBuiltinTopicData));

					memcpy(&p_subscrion_data->key, &guid, sizeof(GUID_t));

					p_message->i_datasize = sizeof(SubscriptionBuiltinTopicData);
					p_message->v_data = p_subscrion_data;*/
				}
				else if (builtintype == SUBSCRIBE)
				{
					SubscriptionBuiltinTopicData* p_subscription_data = malloc(sizeof(SubscriptionBuiltinTopicData));
					
					memcpy(&p_subscription_data->key, &guid, sizeof(GUID_t));
					memcpy(&p_subscription_data->participant_key, &guid, sizeof(GUID_t));
					qos_find_subscription_qos_from_parameterlist(p_subscription_data, (char*)p_serialized->p_value, i_size);

					p_message->i_datasize = sizeof(SubscriptionBuiltinTopicData);
					p_message->v_data = p_subscription_data;

					/*PublicationBuiltinTopicData *p_publish_data = malloc(sizeof(PublicationBuiltinTopicData));

					memcpy(&p_publish_data->key, &guid, sizeof(GUID_t));

					p_message->i_datasize = sizeof(PublicationBuiltinTopicData);
					p_message->v_data = p_publish_data;*/
				}
				else if (builtintype == TOPIC)
				{
					TopicBuiltinTopicData* p_topic_data = malloc(sizeof(TopicBuiltinTopicData));

					memcpy(&p_topic_data->key, &guid, sizeof(GUID_t));
					qos_find_topic_qos_from_parameterlist(p_topic_data, (char*)p_serialized->p_value, i_size);

					p_message->i_datasize = sizeof(TopicBuiltinTopicData);
					p_message->v_data = p_topic_data;
				}
				else // 여기는 수행 안됨(rtps_statelessreader.c 의 best_effort_statelessreader_behavior() 에서 처리함)
				{					
					ParticipantBuiltinTopicData* p_participant_data = malloc(sizeof(ParticipantBuiltinTopicData));

					memcpy(&p_participant_data->key, &guid, sizeof(GUID_t));

					p_message->i_datasize = sizeof(ParticipantBuiltinTopicData);
					p_message->v_data = p_participant_data;
				}

				p_rtps_cachechange->p_org_message = p_message;
			}

			//added by kyy(Presentation QoS)//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//			if(p_rtps_reader->is_builtin != true )//&& p_rtpsReader->p_datareader->p_subscriber->subscriber_qos.presentation.access_scope == TOPIC_PRESENTATION_QOS)
//			{
//				ParameterWithValue *p_atom_parameter;
//
//				p_atom_parameter = get_real_parameter(p_datafull->inline_qos.p_head_first);
//
//				while(p_atom_parameter)
//				{
//					if(p_atom_parameter->parameter_id == PID_COHERENT_SET && p_atom_parameter->length == sizeof(SequenceNumber_t))
//					{
//						SequenceNumber_t* p_seq;
//						p_seq = (SequenceNumber_t*)p_atom_parameter->p_value;
//						p_rtps_cachechange->coherent_set_number.high = p_seq->high;
//						p_rtps_cachechange->coherent_set_number.low = p_seq->low;
//						//printf("Coherent Set Number = %d, %d\n",p_seq->high,p_seq->low);
//						//printf("Coherent Set 수집중 Coh_Number = %d\n",p_seq->low);
//						if(p_seq->high == -1 && p_seq->low == 0)
//						{
//							//추후 이 메시지 이전 Seq# 검색하여 모두 받았는지 확인한 후 coherent_set을 false로 하는 것이 필요(02.12)
////							int coh_set;
////							coh_set = p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->sequence_number.low;
////							coh_set = p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->coherentStartNumber.low;
////							printf("%d \n",coh_set);
////							for (coh_set; coh_set < p_rtpsReader->reader_cache->pp_changes[p_rtpsReader->reader_cache->i_changes - 2 ]->sequence_number.low; coh_set++)
////							{
////								if(p_rtpsReader->reader_cache->pp_changes[coh_set]->sequence_number.low != coh_set)
////									printf("ERROR!! %d \n",coh_set);
////							}
//							p_rtps_cachechange->is_coherent_set = false;
//							p_statefulreader->p_datareader->last_coh_number = p_rtps_cachechange->sequence_number.low;
//						}
//						else if(p_rtps_cachechange->sequence_number.low == p_rtps_cachechange->coherent_set_number.low)
//						{							
//							p_rtps_cachechange->is_coherent_set = true;
//							p_statefulreader->p_datareader->first_coh_number = p_rtps_cachechange->sequence_number.low;
//							//printf("Coherent Set이 재설정 되었습니다. Coh_Number = %d\n",p_statefulreader->p_datareader->first_coh_number);
//						}
//						else
//						{
//							p_rtps_cachechange->is_coherent_set = true;
//						}
//					}
//
//					p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
//
//				}
//			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			if(p_rtps_writerproxy && p_rtps_cachechange != NULL)
			{
				//rtps_writerproxy_t *p_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t*)p_rtps_reader, p_rtps_cachechange->writer_guid);
				Is_exist_in_cachechange(p_rtps_writerproxy->remote_writer_guid, p_rtps_cachechange, -1);
			}

			//added by kyy(Lifespan QoS)//////////////////////////////////////
			qos_input_lifespan(p_rtps_cachechange, p_statefulreader, p_rtps_writerproxy);
			////////////////////////////////////////////////////////////////////////////
			//added by kyy(Destination Order QoS)//////////////////////////////////////
			p_rtps_cachechange->received_time.sec = currenTime().sec;
			p_rtps_cachechange->received_time.nanosec = currenTime().nanosec;
			////////////////////////////////////////////////////////////////////////////
			
			//by kki...(history)
			if (qos_check_historycache_for_reader(p_rtps_reader->p_reader_cache, p_rtps_cachechange))
			{
				p_rtps_cachechange->p_rtps_writerproxy = p_rtps_writerproxy;

				p_rtps_cachechange = rtps_historycache_add_change(p_rtps_reader->p_reader_cache, p_rtps_cachechange);
				
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

				// 이미 받아서 received 상태로 변경함/// 더이상 필요 없음...
				//rtps_writerproxy_received_change_set(p_rtps_writerproxy, a_data.writer_sn.value); 

			}
			else
			{
				// 추가하지 못한 cachechange 를 삭제
				//rtps_cachechange_destory(p_rtps_cachechange);
			}

			//////////////////////////////////////////////////////////////////////////////////

			//test_for_reliablity_division
			
			if(p_rtps_cachechange)
			{
				if(p_rtps_cachechange->is_blockcount != -1){
					p_rtps_cachechange->is_blockcount = 1;
				}else{

					if(p_rtps_reader->p_datareader->test_for_reliablity_bulk_start != -1
						&& p_rtps_reader->p_datareader->test_for_reliablity_bulk_last != -1
						&& p_rtps_reader->p_datareader->test_for_reliablity_bulk_last > p_rtps_reader->p_datareader->test_for_reliablity_bulk_start){
						int32_t div = p_rtps_cachechange->sequence_number.low % 20;

						if( div >= p_rtps_reader->p_datareader->test_for_reliablity_bulk_start && div <= p_rtps_reader->p_datareader->test_for_reliablity_bulk_last)
						{
							p_rtps_cachechange->is_blockcount = 0;
						}

					}else{

						if(p_rtps_reader->p_datareader->test_for_reliablity_division == 0)
						{
							p_rtps_cachechange->is_blockcount = 0;
						}else if((p_rtps_cachechange->sequence_number.low %p_rtps_reader->p_datareader->test_for_reliablity_division) != 0){
							p_rtps_cachechange->is_blockcount = 0;
						}else{
							p_rtps_cachechange->is_blockcount = 1;
						}
					}
				}

			}
			
		}else{
			if(p_serialized_data && b_remove_serialized_data)
			{
				FREE(p_serialized_data->p_value);
				FREE(p_serialized_data);
			}
		}

		if(p_rtps_writerproxy) rtps_writerproxy_unref(p_rtps_writerproxy);


		if(!is_builtin && p_rtps_cachechange && !p_rtps_cachechange->b_read)
		{
			/*if(p_rtps_cachechange->is_blockcount && p_rtps_cachechange->data_value != NULL){
				SerializedPayloadForReader *p_serializedForReader = (SerializedPayloadForReader *)p_rtps_cachechange->data_value;
				message_t *p_message = messageNewForBultinReader(p_statefulreader->p_datareader, &p_rtps_cachechange->data_value->value[4], p_serializedForReader->i_size);
				p_rtps_cachechange->b_read = true;
				message_write(p_message);
			}*/

		}else{
			//if(p_rtps_cachechange) p_rtps_cachechange->b_read = true;
		}
	}else{
		if(p_serialized_data && b_remove_serialized_data)
		{
			FREE(p_serialized_data->p_value);
			FREE(p_serialized_data);
		}
	}

	is_define_user_entity_lock = false;
}

// GAP은 나중에 다시 함.

//FOREACH seq_num IN [GAP.gap_start, GAP.gap_list.base-1] DO {
//	the_writer_proxy.irrelevant_change_set(seq_num);
//}
//FOREACH seq_num IN GAP.gap_list DO {
//	the_writer_proxy.irrelevant_change_set(seq_num);
//}
void rtps_receive_gap_procedure( rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, Gap a_gap, octet flags )
{
	GUID_t guid;
	
	if(p_rtps_reader->behavior_type == STATEFUL_TYPE)
	{
		rtps_statefulreader_t *p_statefulreader = (rtps_statefulreader_t *)p_rtps_reader;
		rtps_writerproxy_t *p_rtps_writerproxy=NULL;
		int32_t count = a_gap.gap_list.numbits;
		SequenceNumber_t check_gap_seq_num = a_gap.gap_list.bitmap_base;
		int32_t i;
		int32_t l_size = sequnce_number_difference(sequnce_number_dec(check_gap_seq_num), a_gap.gap_start.value)+1;
		SequenceNumber_t a_gapStart = a_gap.gap_start.value;

		memcpy(&guid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t));
		memcpy(&guid.entity_id, &a_gap.writer_id, sizeof(EntityId_t));

		if(p_statefulreader->pp_matched_writers == NULL) return;


		p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtps_reader, guid);

		

		if(p_rtps_writerproxy)
		{
			if (p_rtps_reader->is_builtin == false)
			{
				Locator_t* remoteLocator = get_default_remote_participant_locator(p_rtps_writerproxy->remote_writer_guid);
				trace_msg(OBJECT(current_object(0)), TRACE_TRACE3, "(%d.%d.%d.%d) >>> GAP:([%5u]) ??????????", remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15], a_gap.gap_start.value.low);
			}

			for(i = 0; i < l_size; i++)
			{

				rtps_writerproxy_irrelevant_change_set(p_rtps_writerproxy, a_gapStart);
				a_gapStart = sequnce_number_inc(a_gapStart);
			}
			

			for(i = 0; i < count; i++)
			{
				if((a_gap.gap_list.bitmap[i/32] & 1 << (31-(i % 32))))
				{
					
					rtps_writerproxy_irrelevant_change_set(p_rtps_writerproxy, check_gap_seq_num);
					
				}

				check_gap_seq_num = sequnce_number_inc(check_gap_seq_num);
			}
			
		}

	}
}

void send_nackFrag(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumberSet a_seq_num_set, SequenceNumber_t a_seq);

void rtps_receive_heartbeat_frag_procedure( rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, HeartbeatFrag a_heartbeat_frag, octet flags )
{
	GUID_t a_reader_guid;
	GUID_t a_findGuid;
	rtps_writerproxy_t *p_rtps_writerproxy=NULL;
	SequenceNumberSet a_seq_num_set;

	octet F = flags & 0x02;
	octet L = flags & 0x04;

	a_findGuid.guid_prefix = a_guid_prefix;
	a_findGuid.entity_id = a_heartbeat_frag.writer_id.value;


	memcpy(&a_reader_guid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t));
	memcpy(&a_reader_guid.entity_id, &a_heartbeat_frag.reader_id, sizeof(EntityId_t));

	p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtps_reader, a_findGuid);


	if(p_rtps_writerproxy == NULL) return;
	

	//send nackfrag
	if(rtps_writerproxy_is_exist_cache(p_rtps_writerproxy, a_heartbeat_frag.writer_sn.value))
	{
		memset(&a_seq_num_set, '\0', sizeof(SequenceNumberSet));
		a_seq_num_set.bitmap_base.high = 0;
		a_seq_num_set.bitmap_base.low = a_heartbeat_frag.last_fragment_num.value;
		a_seq_num_set.numbits = 0;
		
	}else
	{
		memset(&a_seq_num_set, 0xff, sizeof(SequenceNumberSet));
		a_seq_num_set.bitmap_base.high = 0;
		a_seq_num_set.bitmap_base.low = 1;
		a_seq_num_set.numbits = a_heartbeat_frag.last_fragment_num.value;
	}

	send_nackFrag((rtps_statefulreader_t *)p_rtps_reader, p_rtps_writerproxy, a_seq_num_set, a_heartbeat_frag.writer_sn.value);	
}

//added by kyy
void find_rxo_qos_from_parameterlist(RxOQos *p_rxo_qos, char *value, int32_t i_size)
{
	char *p_value = NULL;
	int32_t	i_found = 0;

	//compare_reliability
	find_parameter_list(value,i_size, PID_RELIABILITY, &p_value, &i_found);
	if(i_found && p_value)
		p_rxo_qos->reliability.kind = *(int*)p_value;
	i_found = 0;

	//Compare Durability kind
	find_parameter_list(value,i_size, PID_DURABILITY, &p_value, &i_found);

	if(i_found && p_value){
		p_rxo_qos->durability.kind = *p_value;
	}else{
		//발견 못했을 경우...
		p_rxo_qos->durability.kind = VOLATILE_DURABILITY_QOS;
	}
	i_found = 0;

	//Compare Presentation kind
	find_parameter_list(value,i_size, PID_PRESENTATION, &p_value, &i_found);
	if(i_found && p_value)
	{
		p_rxo_qos->presentation.access_scope = *p_value;
		p_rxo_qos->presentation.coherent_access = *(p_value+4);
		p_rxo_qos->presentation.ordered_access = *(p_value+5);
	}
	i_found = 0;

	//Compare deadline period
	find_parameter_list(value,i_size, PID_DEADINE, &p_value, &i_found);
	if(i_found && p_value)
		p_rxo_qos->deadline.period = *(Duration_t*)p_value;
	i_found = 0;

	//compare_latency_budget
	find_parameter_list(value,i_size, PID_LATENCY_BUDGET, &p_value, &i_found);
	if(i_found && p_value)
		p_rxo_qos->latency_budget.duration = *(Duration_t*)p_value;
	i_found = 0;

	//compare_ownership
	find_parameter_list(value,i_size, PID_OWNERSHIP, &p_value, &i_found);
	if(i_found && p_value){
		p_rxo_qos->ownership.kind = *p_value;
	}else{
		p_rxo_qos->ownership.kind = SHARED_OWNERSHIP_QOS;
	}
	i_found = 0;

	//compare_liveliness
	find_parameter_list(value,i_size, PID_LIVELINESS, &p_value, &i_found);
	if(i_found && p_value)
	{
		p_rxo_qos->liveliness.kind = *p_value;
		p_rxo_qos->liveliness.lease_duration = *(Duration_t*)(p_value+4);
	}
	i_found = 0;

	//compare_destination_order
	find_parameter_list(value,i_size, PID_DESTINATION_ORDER, &p_value, &i_found);
	if(i_found && p_value)
		p_rxo_qos->destination_order.kind = *p_value;
	i_found = 0;

}
//added by kyy
void get_requested_qos(RxOQos *p_requested_qos, rtps_reader_t *p_rtps_reader)
{
	p_requested_qos->reliability = p_rtps_reader->p_datareader->datareader_qos.reliability;
	p_requested_qos->durability = p_rtps_reader->p_datareader->datareader_qos.durability;
	p_requested_qos->presentation = p_rtps_reader->p_datareader->p_subscriber->subscriber_qos.presentation;
	p_requested_qos->deadline = p_rtps_reader->p_datareader->datareader_qos.deadline;
	p_requested_qos->latency_budget = p_rtps_reader->p_datareader->datareader_qos.latency_budget;
	p_requested_qos->ownership = p_rtps_reader->p_datareader->datareader_qos.ownership;
	p_requested_qos->liveliness = p_rtps_reader->p_datareader->datareader_qos.liveliness;
	p_requested_qos->destination_order = p_rtps_reader->p_datareader->datareader_qos.destination_order;


	p_requested_qos->p_rtps_reader = p_rtps_reader;
	p_requested_qos->p_rtps_writer = NULL;

}
//added by kyy
void get_offered_qos(RxOQos *p_offered_qos, rtps_writer_t *p_rtps_writer)
{
	p_offered_qos->reliability = p_rtps_writer->p_datawriter->datawriter_qos.reliability;
	p_offered_qos->durability = p_rtps_writer->p_datawriter->datawriter_qos.durability;
	p_offered_qos->presentation = p_rtps_writer->p_datawriter->p_publisher->publisher_qos.presentation;
	p_offered_qos->deadline = p_rtps_writer->p_datawriter->datawriter_qos.deadline;
	p_offered_qos->latency_budget = p_rtps_writer->p_datawriter->datawriter_qos.latency_budget;
	p_offered_qos->ownership = p_rtps_writer->p_datawriter->datawriter_qos.ownership;
	p_offered_qos->liveliness = p_rtps_writer->p_datawriter->datawriter_qos.liveliness;
	p_offered_qos->destination_order = p_rtps_writer->p_datawriter->datawriter_qos.destination_order;

	p_offered_qos->p_rtps_writer = p_rtps_writer;
	p_offered_qos->p_rtps_reader = NULL;

}
//added by kyy (modified by zbmon)
bool compare_rxo(RxOQos* p_offered_qos, RxOQos* p_requested_qos)
{
	bool compare_value;
	QosPolicyId_t qos_policy_id = INVALID_QOS_POLICY_ID;
	compare_value = true;

	if (p_requested_qos->reliability.kind > p_offered_qos->reliability.kind)
	{
		trace_msg(NULL, TRACE_LOG, "Reliability RxO Error");
		compare_value = false;
		qos_policy_id = RELIABILITY_QOS_POLICY_ID;
	}

	if (p_requested_qos->durability.kind > p_offered_qos->durability.kind)
	{
		trace_msg(NULL, TRACE_LOG, "Durability RxO Error");
		compare_value = false;
		qos_policy_id = DURABILITY_QOS_POLICY_ID;
	}

	if (p_requested_qos->presentation.access_scope > p_offered_qos->presentation.access_scope)
	{
		trace_msg(NULL, TRACE_LOG, "Presentation RxO Error");
		compare_value = false;
		qos_policy_id = PRESENTATION_QOS_POLICY_ID;
	}
	else if (p_requested_qos->presentation.coherent_access == false || p_offered_qos->presentation.coherent_access == p_requested_qos->presentation.coherent_access == true)
	{
	}
	else if (p_requested_qos->presentation.ordered_access == false || p_offered_qos->presentation.ordered_access == p_requested_qos->presentation.ordered_access == true)
	{
	}

	//offered period <= requested period
	if ((p_requested_qos->deadline.period.sec < p_offered_qos->deadline.period.sec) ||
		(p_requested_qos->deadline.period.sec == p_offered_qos->deadline.period.sec && p_requested_qos->deadline.period.nanosec < p_offered_qos->deadline.period.nanosec))
	{
		trace_msg(NULL, TRACE_LOG, "Deadline RxO Error");
		compare_value = false;
		qos_policy_id = DEADLINE_QOS_POLICY_ID;
	}

	//offered duration <= requested duration
	if ((p_requested_qos->latency_budget.duration.sec < p_offered_qos->latency_budget.duration.sec) ||
		(p_requested_qos->latency_budget.duration.sec == p_offered_qos->latency_budget.duration.sec && p_requested_qos->latency_budget.duration.nanosec < p_offered_qos->latency_budget.duration.nanosec))
	{
		trace_msg(NULL, TRACE_LOG, "Latency Budget RxO Error");
		compare_value = false;
		qos_policy_id = LATENCYBUDGET_QOS_POLICY_ID;
	}

	if (p_requested_qos->ownership.kind != p_offered_qos->ownership.kind)
	{
		trace_msg(NULL, TRACE_LOG, "Ownership RxO Error");
		compare_value = false;
		qos_policy_id = OWNERSHIP_QOS_POLICY_ID;
	}

	//offered kind >= requested kind
	if (p_requested_qos->liveliness.kind > p_offered_qos->liveliness.kind)
	{
		trace_msg(NULL, TRACE_LOG, "Liveliness Kind RxO Error");
		compare_value = false;
		qos_policy_id = LIVELINESS_QOS_POLICY_ID;
	}

	//offered duration <= requested duration
	if (p_requested_qos->liveliness.lease_duration.sec == TIME_INFINITE.sec && p_offered_qos->liveliness.lease_duration.sec == TIME_INFINITE.nanosec)
	{
		trace_msg(NULL, TRACE_LOG, "Liveliness is infinite");
	}
	else if ((p_requested_qos->liveliness.lease_duration.sec < p_offered_qos->liveliness.lease_duration.sec) ||
		(p_requested_qos->liveliness.lease_duration.sec == p_offered_qos->liveliness.lease_duration.sec && p_requested_qos->liveliness.lease_duration.nanosec < p_offered_qos->liveliness.lease_duration.nanosec))
	{
		trace_msg(NULL, TRACE_LOG, "Liveliness RxO Error");
		compare_value = false;
		qos_policy_id = LIVELINESS_QOS_POLICY_ID;
	}

	//offered kind >= requested kind
	if (p_requested_qos->destination_order.kind > p_offered_qos->destination_order.kind)
	{
		trace_msg(NULL, TRACE_LOG, "Destination Order RxO Error");
		compare_value = false;
		qos_policy_id = DESTINATIONORDER_QOS_POLICY_ID;
	}

	//status by jun (modified by zbmon)
	if (compare_value == false)
	{
		if (p_requested_qos->p_rtps_writer != NULL || p_requested_qos->p_rtps_reader != NULL)
		{
			if (p_requested_qos->p_rtps_writer == NULL)
			{
				change_RequestedIncompatibleQoS_status(p_requested_qos->p_rtps_reader->p_datareader, qos_policy_id);
			}
			else if (p_requested_qos->p_rtps_reader == NULL)
			{
				change_OfferedIncompatibleQoS_status(p_requested_qos->p_rtps_writer->p_datawriter, qos_policy_id);
			}
		}
		else if (p_offered_qos->p_rtps_writer != NULL || p_offered_qos->p_rtps_reader != NULL)
		{
			if (p_offered_qos->p_rtps_writer == NULL)
			{
				change_RequestedIncompatibleQoS_status(p_offered_qos->p_rtps_reader->p_datareader, qos_policy_id);
			}
			else if (p_offered_qos->p_rtps_reader == NULL)
			{
				change_OfferedIncompatibleQoS_status(p_offered_qos->p_rtps_writer->p_datawriter, qos_policy_id);
			}
		}
	}

	return compare_value;
}

/*
//added by kyy
void find_lifespan_qos_from_parameterlist(rtps_writerproxy_t *p_writerproxy, char *value, int32_t i_size)
{
	char *p_value = NULL;
	int32_t	i_found = 0;

	//compare_reliability
	find_parameter_list(value,i_size, PID_LIFESPAN, &p_value, &i_found);
	if(i_found && p_value)
		p_writerproxy->lifespan = *(Duration_t*)p_value;
	printf("%d\n",p_writerproxy->lifespan.sec);
	i_found = 0;
}
*/