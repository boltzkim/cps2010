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
	Liveliness QoS Policy를 위한 모듈.

	이력
	2012-10-16
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


extern bool remote_initialize;


/************************************************************************/
/* 두 파라미터 duration_1 과 duration_2 는 같은 값인가?                 */
/************************************************************************/
static bool is_same_duration(const Duration_t duration_1, const Duration_t duration_2)
{
	return (duration_1.sec == duration_2.sec && duration_1.nanosec == duration_2.nanosec);
}


/************************************************************************/
/* livelinessQos.kind 가 AUTOMATIC_LIVELINESS_QOS 인가?                 */
/************************************************************************/
static bool is_automatic_liveliness_writer(const DataWriter* const p_datawriter)
{
	if (p_datawriter == NULL)
	{
		return false;
	}

	if (p_datawriter->datawriter_qos.liveliness.kind == AUTOMATIC_LIVELINESS_QOS &&
		!is_same_duration(p_datawriter->datawriter_qos.liveliness.lease_duration, TIME_INFINITE))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* livelinessQos.kind 가 MANUAL_BY_PARTICIPANT_LIVELINESS_QOS 인가?     */
/************************************************************************/
static bool is_manual_by_participant_liveliness_writer(const DataWriter* const p_datawriter)
{
	if (p_datawriter == NULL)
	{
		return false;
	}

	if (p_datawriter->datawriter_qos.liveliness.kind == MANUAL_BY_PARTICIPANT_LIVELINESS_QOS &&
		!is_same_duration(p_datawriter->datawriter_qos.liveliness.lease_duration, TIME_INFINITE))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* livelinessQos.kind 가 MANUAL_BY_TOPIC_LIVELINESS_QOS 인가?           */
/************************************************************************/
static bool is_manual_by_topic_liveliness_writer(const DataWriter* const p_datawriter)
{
	if (p_datawriter == NULL)
	{
		return false;
	}

	if (p_datawriter->datawriter_qos.liveliness.kind == MANUAL_BY_TOPIC_LIVELINESS_QOS &&
		!is_same_duration(p_datawriter->datawriter_qos.liveliness.lease_duration, TIME_INFINITE))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* 두 파라미터 duration_1 과 duration_2 를 비교하여 작은 값을 리턴한다. */
/************************************************************************/
static Duration_t get_smaller_duration(const Duration_t duration_1, const Duration_t duration_2)
{
	if (duration_1.sec != duration_2.sec) 
	{
		if (duration_1.sec <= duration_2.sec)
		{
			return duration_1;
		}
		else
		{
			return duration_2;
		}
	}
	else
	{
		if (duration_1.sec >= 0)
		{
			if (duration_1.nanosec <= duration_2.nanosec)
			{
				return duration_1;
			}
			else
			{
				return duration_2;
			}
		}
		else
		{
			if (duration_1.nanosec <= duration_2.nanosec)
			{
				return duration_2;
			}
			else
			{
				return duration_1;
			}
		}
	}
}


void init_operation_called_for_liveliness_qos(qos_policy_t* const p_this)
{
	if (!p_this->b_operation_called_lock_initialize)
	{
		p_this->b_operation_called_lock_initialize = true;
		mutex_init(&p_this->operation_called_lock);
	}
}


/************************************************************************/
/* MANUAL_BY_PARTICIPANT_LIVELINESS_QOS 를 위한 함수                    */
/*                                                                      */ 
/* write(), assert_liveliness(), dispose(), unregister_instance() 가    */
/* 호출되었음을 알리기 위한 함수                                        */
/*                                                                      */ 
/* write(), assert_liveliness(), dispose(), unregister_instance() 에서  */
/* 호출해야만 한다.                                                     */
/************************************************************************/
void qosim_operation_called_for_liveliness_qos(qos_policy_t* const p_this, const DataWriter* const p_data_writer)
{
	if (is_manual_by_participant_liveliness_writer(p_data_writer))
	{
		p_this->b_operation_called = true;
	}
}


/************************************************************************/
/* MANUAL_BY_PARTICIPANT_LIVELINESS_QOS 를 위한 함수                    */
/*                                                                      */ 
/* write(), assert_liveliness(), dispose(), unregister_instance() 가    */
/* 호출되었는지 확인하기 위한 함수                                      */
/*                                                                      */ 
/************************************************************************/
static bool check_operation_called_for_liveliness_qos(qos_policy_t* const p_this)
{
	bool b_check;

	mutex_lock(&p_this->operation_called_lock);

	b_check = p_this->b_operation_called;
	p_this->b_operation_called = false;

	mutex_unlock(&p_this->operation_called_lock);

	return b_check;
}


typedef struct RemoteParticipant_t
{
	GUID_t		a_remoteParticipantGuid;
	Locator_t	a_remotelocator;
	Locator_t	a_defaultLocator;
	Locator_t	a_remoteMulticastlocator;
	Locator_t	a_defaultMulticastLocator;
	bool		is_remoteAlive;
	Duration_t	remoteDuration;
	Time_t		a_lastsendTime;
	uint32_t	count;
} RemoteParticipant_t;


extern RemoteParticipant_t**	pp_remoteParticipant;
extern int32_t					i_remoteParticipant;
extern mutex_t					remote_lock;
extern cond_t					remote_cond;


void qosim_set_stop_liveliness(qos_policy_t* const p_this, const bool state)
{
	p_this->b_end_liveliness = state;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static Duration_t get_minimum_lease_duration(liveliness_writer_t** const pp_liveliness_writers, const int32_t i_liveliness_writers)
{
	Duration_t lease_duration = TIME_INFINITE;
	int i;

	for (i = 0; i < i_liveliness_writers; i++)
	{
		lease_duration = get_smaller_duration(lease_duration, pp_liveliness_writers[i]->lease_duration);
	}

	return lease_duration;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static bool exist_automatic_liveliness_writer(qos_policy_t* const p_this)
{
	return (p_this->i_automatic_liveliness_writers > 0);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static bool exist_manual_liveliness_writer(qos_policy_t* const p_this)
{
	return (p_this->i_manual_liveliness_writers > 0);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static bool is_exist_liveliness_writer(qos_policy_t* const p_this)
{
	return (exist_automatic_liveliness_writer(p_this) || exist_manual_liveliness_writer(p_this));
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static void send_liveliness_message(rtps_writer_t* const p_rtps_writer, const bool b_is_automatic)
{
	int i;
//time by jun
	Time_t message_time = currenTime();

	mutex_lock(&remote_lock);

	for (i = 0; i < i_remoteParticipant; i++)
	{
		DataFull* p_data = malloc(sizeof(DataFull));
		Heartbeat a_heartBeatData;
		ParticipantMessageData* p_participantMessageData = NULL;
		SerializedPayloadForWriter* p_serializedPlayloadwriter = malloc(sizeof(SerializedPayloadForWriter));

		memset(p_data, 0, sizeof(DataFull));
		memset(p_serializedPlayloadwriter, 0, sizeof(SerializedPayloadForWriter));

		memcpy(&p_data->writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, 4);
		memcpy(&p_data->reader_id, &ENTITYID_UNKNOWN, 4);
///		memcpy(&p_data->reader_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, 4);//[120306,김경일]
		//TODO[120112,김경일] writerSN값은 계속 증가되어야 하는 것 같음.
		p_data->writer_sn.value.high = p_rtps_writer->last_change_sequence_number.high;
		p_data->writer_sn.value.low  = p_rtps_writer->last_change_sequence_number.low;
		p_data->p_serialized_data = (SerializedPayload *)p_serializedPlayloadwriter;
		p_data->inline_qos.i_linked_size = 0;
		p_data->inline_qos.p_head_first = NULL;
		p_data->inline_qos.p_head_last = NULL;
		p_data->octets_to_inline_qos = 0;

		{
			ParameterWithValue*	p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_rtps_writer->guid);

			insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
//			INSERT_ELEM(p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters, p_data->inline_qos.i_parameters, p_parameter);

			p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);
			insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
			//INSERT_ELEM(p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters, p_data->inline_qos.i_parameters, p_parameter);

			p_data->octets_to_inline_qos = 16;
		}

		p_serializedPlayloadwriter->i_size = sizeof(ParticipantMessageData)+4;
		p_serializedPlayloadwriter->p_value = (uint8_t*)malloc(p_serializedPlayloadwriter->i_size);
		memset(p_serializedPlayloadwriter->p_value, 0, p_serializedPlayloadwriter->i_size);
		p_serializedPlayloadwriter->p_value[0] = 0x00;
		p_serializedPlayloadwriter->p_value[1] = 0x01;
		p_participantMessageData = (ParticipantMessageData*)(p_serializedPlayloadwriter->p_value+4);

		p_participantMessageData->participant_guid_prefix = p_rtps_writer->p_rtps_participant->guid.guid_prefix;
		if (b_is_automatic)
		{
			memcpy(p_participantMessageData->kind, PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE, 4);
		}
		else
		{
			memcpy(p_participantMessageData->kind, PARTICIPANT_MESSAGE_DATA_KIND_MANUAL_LIVELINESS_UPDATE, 4);
		}
		memset(p_participantMessageData->sequence_length, 0, 4);
		memset(p_participantMessageData->data, 0, sizeof(p_participantMessageData->data));
		p_data->extra_flags = 0;

		//send Data......................
		rtps_send_data_with_dst_to(p_rtps_writer, pp_remoteParticipant[i]->a_remoteParticipantGuid, pp_remoteParticipant[i]->a_remotelocator, p_data, false, message_time);

		//send heartBeat......................

		memcpy(&a_heartBeatData.writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, 4);
		memcpy(&a_heartBeatData.reader_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, 4);
		a_heartBeatData.first_sn.value.high = p_rtps_writer->last_change_sequence_number.high;
		a_heartBeatData.first_sn.value.low  = p_rtps_writer->last_change_sequence_number.low;
		a_heartBeatData.last_sn.value.high = p_rtps_writer->last_change_sequence_number.high;
		a_heartBeatData.last_sn.value.low  = p_rtps_writer->last_change_sequence_number.low;
		p_rtps_writer->last_change_sequence_number = sequnce_number_inc(p_rtps_writer->last_change_sequence_number);
		a_heartBeatData.count.value = ++pp_remoteParticipant[i]->count;
		rtps_send_heartbeat_with_dst_to(p_rtps_writer, pp_remoteParticipant[i]->a_remoteParticipantGuid, pp_remoteParticipant[i]->a_remotelocator, a_heartBeatData, false);
	}

	mutex_unlock(&remote_lock);
}


/************************************************************************/
/* Duration의 값을 millisecond 단위의 값으로 변환한다.                  */
/************************************************************************/
uint32_t get_millisecond(const Duration_t duration)
{
	return (duration.sec*1000 + duration.nanosec/1000000);
}


extern Time_t time_addition(const Time_t augend_time, const Time_t addend_time);
static data_t* remove_liveliness_job_from(data_fifo_t* const p_fifo, const Duration_t lease_duration, const void* const v_endpoint);
void liveliness_writer_set_alive(rtps_writer_t* const p_rtps_writer, const Time_t current_time)
{
	data_t* p_job = NULL;

	p_rtps_writer->b_liveliness_alive = true;
	p_rtps_writer->liveliness_base_time = current_time;
	p_rtps_writer->p_liveliness_job->next_wakeup_time = time_addition(current_time, p_rtps_writer->liveliness_lease_duration);
	//Job 을 꺼내서 다시 넣어야 한다.
	p_job = remove_liveliness_job_from(p_rtps_writer->p_datawriter->p_publisher->p_domain_participant->p_writer_thread_fifo, TIME_ZERO, p_rtps_writer);
	if (p_job) 
	{
		timed_job_queue_time_compare_data_fifo_put(p_rtps_writer->p_datawriter->p_publisher->p_domain_participant->p_writer_thread_fifo, p_job);
	}
///	trace_msg(NULL, TRACE_LOG, "writer_set_alive+++ \t(%d) %d --> %d", (int)p_rtps_writer%10, current_time.sec, p_rtps_writer->p_liveliness_job->next_wakeup_time.sec);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void qosim_liveliness_thread(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer)
{
	Duration_t	automatic_minimum_lease_duration;
	Duration_t	manual_minimum_lease_duration;
	uint32_t	automatic_wake_time;
	uint32_t	manual_wake_time;
	uint32_t	time[3] = {0, 0, 0};
	static const uint32_t WAIT      = 0;
	static const uint32_t AUTOMATIC = 1;
	static const uint32_t MANUAL    = 2;
	int i = 0;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}
	else
	{
		return;
	}

	init_operation_called_for_liveliness_qos(p_this);


	while (p_this->b_end_liveliness == false)
	{
		if (!is_exist_liveliness_writer(p_this))
		{
			if(p_this->i_automatic_liveliness_writers == 0 
				&& p_this->i_manual_liveliness_writers == 0)
			{
				msleep(10*1000);
			}else{
				msleep(10000);
			}
			continue;
		}

		check_remote_participant_leaseDuration();

		mutex_lock(&remote_lock);

		if (time[AUTOMATIC] <= 0)
		{
			automatic_minimum_lease_duration = get_minimum_lease_duration(p_this->pp_automatic_liveliness_writers, p_this->i_automatic_liveliness_writers);
			automatic_wake_time = get_millisecond(automatic_minimum_lease_duration);
			time[AUTOMATIC] = automatic_wake_time-100;
		}
		if (time[MANUAL] <= 0)
		{
			manual_minimum_lease_duration = get_minimum_lease_duration(p_this->pp_manual_liveliness_writers, p_this->i_manual_liveliness_writers);
			manual_wake_time    = get_millisecond(manual_minimum_lease_duration);
			time[MANUAL] = manual_wake_time-100;
		}


		if (!is_same_duration(automatic_minimum_lease_duration, TIME_INFINITE) && !is_same_duration(manual_minimum_lease_duration, TIME_INFINITE))
		{
			time[WAIT] = (time[AUTOMATIC] <= time[MANUAL]) ? time[AUTOMATIC] : time[MANUAL];
		}
		else if (!is_same_duration(automatic_minimum_lease_duration, TIME_INFINITE))
		{
			time[WAIT] = time[AUTOMATIC];
		}
		else if (!is_same_duration(manual_minimum_lease_duration, TIME_INFINITE))
		{
			time[WAIT] = time[MANUAL];
		}

		if(time[WAIT] == 0)
		{
			time[WAIT] = 1;
		}


		cond_waittimed(&remote_cond, &remote_lock, time[WAIT]);

		mutex_unlock(&remote_lock);

		if (p_this->b_end_liveliness) break;

		time[AUTOMATIC] -= time[WAIT];
		time[MANUAL   ] -= time[WAIT];

		if (time[AUTOMATIC] <= 0 && exist_automatic_liveliness_writer(p_this))
		{
			Time_t current_time = currenTime();
			for (i = 0; i < p_this->i_automatic_liveliness_writers; i++)
			{
				liveliness_writer_set_alive((rtps_writer_t*)p_this->pp_automatic_liveliness_writers[i]->p_datawriter->p_related_rtps_writer, current_time);
			}

			send_liveliness_message(p_rtps_writer, true);
			trace_msg(NULL, TRACE_LOG, "\tsend_liveliness_message _ _ _ _ _ _ _ _ %d", current_time.sec);
		}
		if (time[MANUAL] <= 0 && exist_manual_liveliness_writer(p_this) && check_operation_called_for_liveliness_qos(p_this))
		{
			Time_t current_time = currenTime();
			for (i = 0; i < p_this->i_manual_liveliness_writers; i++)
			{
				liveliness_writer_set_alive((rtps_writer_t*)p_this->pp_manual_liveliness_writers[i]->p_datawriter->p_related_rtps_writer, current_time);
			}

			send_liveliness_message(p_rtps_writer, false);
			trace_msg(NULL, TRACE_LOG, "\tsend_liveliness_message _ _ _ _ _ _ _ _ %d", current_time.sec);
		}
	}
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void qosim_add_liveliness_writer(qos_policy_t* const p_this, const DataWriter* const p_datawriter)
{
	liveliness_writer_t* p_liveliness_writer = NULL;

	if (is_manual_by_topic_liveliness_writer(p_datawriter))
	{
		return;
	}

	p_liveliness_writer = malloc(sizeof(liveliness_writer_t));
	memset(p_liveliness_writer, 0, sizeof(liveliness_writer_t));

	p_liveliness_writer->p_datawriter = (DataWriter*)p_datawriter;
	// No Changeable 이기 때문에 복사해놓고 사용하기로 함.
	p_liveliness_writer->lease_duration = p_datawriter->datawriter_qos.liveliness.lease_duration;

	if (is_automatic_liveliness_writer(p_datawriter))
	{
		INSERT_ELEM(p_this->pp_automatic_liveliness_writers, p_this->i_automatic_liveliness_writers, p_this->i_automatic_liveliness_writers, p_liveliness_writer);
	}
	else
	{
		INSERT_ELEM(p_this->pp_manual_liveliness_writers, p_this->i_manual_liveliness_writers, p_this->i_manual_liveliness_writers, p_liveliness_writer);
	}
}


extern int BoundarySize(int i_size);
extern void Mapping_header(rtps_writer_t *p_rtps_writer, Header *p_header);
extern void Mapping_SubmessageHeader(rtps_writer_t *p_rtps_writer, SubmessageHeader *p_header, SubmessageKind kind, octet flags, ushort submessageLength);


static int send_heartbeat_with_final_and_liveliness_flag(rtps_writer_t* const p_rtps_writer, const Locator_t locator, const Heartbeat heartbeat)
{
	int i_size = 0;
	int i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_send_data = NULL;


	if (p_rtps_writer->p_accessout == NULL || p_rtps_writer->p_accessout->pf_write == NULL)
	{
		return MODULE_ERROR_RTPS;
	}

	i_size = BoundarySize(sizeof(Header));

	//// INFO_DST
	//i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));

	// HEARTBEAT
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));


	p_send_data = data_new(i_size);
	p_send_data->i_port = locator.port;
	if (locator.kind == LOCATOR_KIND_UDPv4)
	{
		asprintf((char**)&p_send_data->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_send_data->p_data);
	i_count += BoundarySize(sizeof(Header));


	//// INFO_DST
	//flags = 0x01;
	//Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_send_data->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	//i_temp = i_count+(sizeof(SubmessageHeader));
	//memcpy(p_send_data->p_data+i_temp,&remoteReaderGuid.guidPrefix, sizeof(GuidPrefix_t));
	//i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// HEARTBEAT
	flags = 0x07;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_send_data->p_data[i_count], HEARTBEAT , flags, sizeof(Heartbeat));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_send_data->p_data+i_temp,&heartbeat, sizeof(Heartbeat));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout, p_send_data);

 

	///////////////////////////

	//if (!is_topic_with_key(p_topic) && !is_single_instance(p_resource_limits_qos))
	//{
	//	return RETCODE_UNSUPPORTED;
	//}

	return RETCODE_OK;
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void qosim_send_heartbeat_for_topic_liveliness(qos_policy_t* const p_this, const DataWriter* const p_data_writer)
{
	int i;
	static Count_t count = 0;
	rtps_writer_t* p_rtps_writer = (rtps_writer_t*)p_data_writer->p_related_rtps_writer;
	Heartbeat heartbeat;
	memcpy(&heartbeat.writer_id, &p_rtps_writer->guid.entity_id, 4);
	memcpy(&heartbeat.reader_id, &ENTITYID_UNKNOWN, 4);
	// 마지막에 write 했던 번호(writerSeqNumber)
	heartbeat.first_sn.value.high = 0;
	heartbeat.first_sn.value.low = p_data_writer->last_sequencenum;
	// 마지막에 write 했던 번호(writerSeqNumber)
	heartbeat.last_sn.value.high = 0;
	heartbeat.last_sn.value.low = p_data_writer->last_sequencenum;
	//// send a Heartbeat Message with final flag and liveliness flag set
	heartbeat.count.value = ++count; //TODO[KKI] 아래에서 재설정하고 있으니, 이 라인은 무의미하다고 생각됨.

	{
		rtps_statefulwriter_t* p_rtps_stateful_writer = (rtps_statefulwriter_t*)p_rtps_writer;
		heartbeat.count.value = ++p_rtps_stateful_writer->heatbeat_count;
		for (i = 0; i < p_rtps_stateful_writer->i_matched_readers; i++)
		{
			send_heartbeat_with_final_and_liveliness_flag(p_rtps_writer, *p_rtps_stateful_writer->pp_matched_readers[i]->pp_unicast_locator_list[0], heartbeat);
		}
	}
	liveliness_writer_set_alive(p_rtps_writer, currenTime());
}


extern bool is_same_guid_prefix(const GuidPrefix_t* const p_guid_prefix_1, const GuidPrefix_t* const p_guid_prefix_2);



extern ReturnCode_t change_LivelinessChanged_status(DataReader * p_datareader, InstanceHandle_t handle, const bool b_is_alive);
/************************************************************************/
/*                                                                      */
/************************************************************************/
static void writerproxy_set_alive(rtps_writerproxy_t* const p_writerproxy, const bool b_is_alive)
{
	//[KKI] writerproxy가 살았는지 죽었는지 설정한다.
	if (p_writerproxy->b_liveliness_alive != b_is_alive)
	{
		change_LivelinessChanged_status(p_writerproxy->p_rtps_reader->p_datareader, (InstanceHandle_t)p_writerproxy, b_is_alive);
		p_writerproxy->b_liveliness_alive = b_is_alive;

		if (p_writerproxy->b_liveliness_alive == false)
		{
			qos_change_owner_writer(p_writerproxy->p_rtps_reader, p_writerproxy, false);
		}
	}
}


static void writerproxy_set_base_time(rtps_writerproxy_t* const p_writerproxy, Time_t base_time);
/************************************************************************/
/* ParticipantMessageData 를 수신했을 때...                             */
/* <p_participant_message_data> 의 participantGuidPrefix 를 가진        */
/* WriterProxy 를 찾아서 b_is_alive를 설정한다.                         */
/************************************************************************/
static void writerproxy_set_alive_by_participant(const GuidPrefix_t* const p_participant_guid_prefix, const LivelinessQosPolicyKind liveliness_kind)
{
	int p, e, w;
	int i_participant;
	rtps_participant_t** pp_participants;
	rtps_participant_t* p_participant;
	rtps_endpoint_t* p_endpoint;
	rtps_statefulreader_t* p_statefulreader;
	rtps_writerproxy_t* p_writerproxy;
	int i_size = 0;
	Time_t current_time = currenTime();

	rtps_participant_lock();
	pp_participants = get_rtps_participants(&i_participant);

	//[KKI] 전체 Participant 리스트를 돌면서
	for (p = 0; p < i_participant; p++)
	{
		p_participant = pp_participants[p];
		//[KKI] 각 Participant 의 전체 endpoint 리스트를 돌면서
		for (e = 0; e < p_participant->i_endpoint; e++)
		{
			p_endpoint = p_participant->pp_endpoint[e];
			//[KKI] endpoint 가 statefulreader 일 경우에
			if (p_endpoint->i_entity_type == READER_ENTITY && ((rtps_reader_t *)p_endpoint)->behavior_type == STATEFUL_TYPE)
			{
				p_statefulreader = (rtps_statefulreader_t*)p_endpoint;

				mutex_lock(&p_statefulreader->object_lock);

				i_size = p_statefulreader->i_matched_writers;
				//[KKI] 각 statefulreader 와 매칭된 writerproxy 리스트를 돌면서
				for (w = i_size-1; w >= 0; w--)
				{
					p_writerproxy = p_statefulreader->pp_matched_writers[w];
					//[KKI] participant_guid_prefix 와 같은 guidPrefix 를 가졌으면
					if (is_same_guid_prefix(&p_writerproxy->remote_writer_guid.guid_prefix, p_participant_guid_prefix) && p_writerproxy->liveliness_kind == liveliness_kind)
					{
						//[KKI] writerproxy를 alive로 세팅한다.
						trace_msg(NULL, TRACE_LOG, "\tALIVE__%-20s", p_writerproxy->p_rtps_reader->p_datareader->p_topic->topic_name);
						writerproxy_set_alive(p_writerproxy, true);

						//[KKI] writerproxy의 base_time을 현재시간으로 세팅한다.
						writerproxy_set_base_time(p_writerproxy, current_time);
					}
				}

				mutex_unlock(&p_statefulreader->object_lock);
			}
		}
	}
	rtps_participant_unlock();
}


static Time_t ZERO_TIME = {0, 0};
static Time_t INFINITE_TIME = {0x7fffffff, 0xffffffff};


static LivelinessQosPolicyKind get_liveliness_kind(octet kind[4])
{
	if (memcmp(kind, PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE, 4) == 0)
	{
		return AUTOMATIC_LIVELINESS_QOS;
	}
	else if (memcmp(kind, PARTICIPANT_MESSAGE_DATA_KIND_MANUAL_LIVELINESS_UPDATE, 4) == 0)
	{
		return MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
	}
	else
	{
		return MANUAL_BY_TOPIC_LIVELINESS_QOS;
	}
}


extern bool time_left_bigger(const Time_t time_1, const Time_t time_2);


/************************************************************************/
/*                                                                      */
/************************************************************************/
static Time_t writerproxy_get_base_time(const rtps_writerproxy_t* const p_writerproxy)
{
	return p_writerproxy->liveliness_base_time;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
static void writerproxy_set_base_time(rtps_writerproxy_t* const p_writerproxy, Time_t base_time)
{
	p_writerproxy->liveliness_base_time = base_time;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
extern Time_t to_time(const Duration_t duration);
void qosim_writerproxy_set_liveliness_kind_and_lease_duration(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy, const RxOQos rxo_qos)
{
	p_writerproxy->liveliness_kind = rxo_qos.liveliness.kind;
	p_writerproxy->liveliness_lease_duration = to_time(rxo_qos.liveliness.lease_duration);
	p_writerproxy->liveliness_base_time = ZERO_TIME;
	p_writerproxy->b_liveliness_alive = true;

	p_writerproxy->b_is_deadline_on = true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void qosim_writer_set_liveliness_kind_and_lease_duration(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer)
{
	p_rtps_writer->liveliness_kind = p_rtps_writer->p_datawriter->datawriter_qos.liveliness.kind;
	p_rtps_writer->liveliness_lease_duration = to_time(p_rtps_writer->p_datawriter->datawriter_qos.liveliness.lease_duration);
	p_rtps_writer->liveliness_base_time = ZERO_TIME;
	p_rtps_writer->b_liveliness_alive = true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void qosim_participant_liveliness_set_alive(qos_policy_t* const p_this, const SerializedPayloadForReader* const p_serialized)
{
	ParticipantMessageData* p_participant_message_data = NULL;
	GuidPrefix_t participant_guid_prefix;
	octet kind[4];
	LivelinessQosPolicyKind liveliness_kind;

	p_participant_message_data = (ParticipantMessageData*)(p_serialized->p_value+4);
	participant_guid_prefix = p_participant_message_data->participant_guid_prefix;

	memcpy(kind, p_participant_message_data->kind, 4);
	liveliness_kind = get_liveliness_kind(kind);
	//[KKI] participant_guid_prefix 를 갖는 writerproxy들을 alive로 세팅한다.
	trace_msg(NULL, TRACE_LOG, "\tALIVE______________________");
	writerproxy_set_alive_by_participant(&participant_guid_prefix, liveliness_kind);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void qosim_manual_by_topic_liveliness_set_alive(qos_policy_t* const p_this, rtps_writerproxy_t* const p_writerproxy)
{
	if (p_writerproxy->liveliness_kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
	{
		Time_t current_time = currenTime();

		//[KKI] writerproxy를 alive로 세팅한다.
		trace_msg(NULL, TRACE_LOG, "\tALIVE__%-20s", p_writerproxy->p_rtps_reader->p_datareader->p_topic->topic_name);
		writerproxy_set_alive(p_writerproxy, true);

		//[KKI] writerproxy의 base_time을 현재시간으로 세팅한다.
		writerproxy_set_base_time(p_writerproxy, current_time);
	}
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void qosim_liveliness_writer_set_alive(qos_policy_t* const p_this, rtps_writer_t* const p_rtps_writer)
{
	if (p_rtps_writer->liveliness_kind == MANUAL_BY_TOPIC_LIVELINESS_QOS)
	{
		liveliness_writer_set_alive(p_rtps_writer, currenTime());
	}
}


/************************************************************************/
/* DataReader 의 lease_duration 이 지났을 경우
/* DataReader 와 매칭되는 WriterProxy 중에 alive 상태인 것을 찾아낸다.
/* WriterProxy 의 base_time 과 DataReader 의 lease_duration 의 합이 current_time 보다 작으면 not_alive 로 세팅한다.
/* true -> false 일 경우엔 change_LivelinessChanged_status() 를 호출한다.
/* Liveliness Job 을 reader_thread 의 Job 큐에 넣는다.
/* DataReader 의 lease_duration 이 지나지 않았을 경우
/* 깨어날 시간을 조정하여 Liveliness Job 을 reader_thread 의 Job 큐에 넣는다. */
/************************************************************************/
void qosim_liveliness_check_wakeup_time_for_reader(qos_policy_t* const p_this, data_t* const p_job)
{
	Time_t current_time = currenTime();

	rtps_statefulreader_t* p_rtps_reader = NULL;
	Time_t lease_duration = INFINITE_TIME;
	int i = 0;
	rtps_writerproxy_t* p_writerproxy = NULL;

	if (!p_job) return;

	p_rtps_reader = (rtps_statefulreader_t *)p_job->v_rtps_reader;
	lease_duration = to_time(p_rtps_reader->p_datareader->datareader_qos.liveliness.lease_duration);

	//[KKI] next_wakeup_time 이 지났으면
	if (time_left_bigger(current_time, p_job->next_wakeup_time))
	{
		char alive[100];
		memset(alive, 0, 100);
		for (i = 0; i < p_rtps_reader->i_matched_writers; i++)
		{
			alive[i] = (p_rtps_reader->pp_matched_writers[i]->b_liveliness_alive) ? 'L' : '_';
		}
		trace_msg(NULL, TRACE_LOG, "\tCHECK_LIVELINESS START [%s]----------------------------- ", alive);

		for (i = 0; i < p_rtps_reader->i_matched_writers; i++)
		{
			p_writerproxy = p_rtps_reader->pp_matched_writers[i];

			if (p_writerproxy->b_liveliness_alive && time_left_bigger(current_time, time_addition(p_writerproxy->liveliness_base_time, lease_duration)))
			{
				//[KKI] writerproxy를 not_alive로 세팅한다.
				trace_msg(NULL, TRACE_LOG, "\tNOT_ALIVE__%-20s", p_writerproxy->p_rtps_reader->p_datareader->p_topic->topic_name);
				writerproxy_set_alive(p_writerproxy, false);
			}
		}

		p_job->next_wakeup_time = time_addition(p_job->next_wakeup_time, lease_duration);
		for (i = 0; i < p_rtps_reader->i_matched_writers; i++)
		{
			alive[i] = (p_rtps_reader->pp_matched_writers[i]->b_liveliness_alive) ? 'L' : '_';
		}
		trace_msg(NULL, TRACE_LOG, "\t------------------------------- END CHECK_LIVELINESS [%s] ", alive);
	}
	//[KKI] Liveliness Job 을 다시 p_data_fifo 큐에 추가한다.
	data_fifo_put(p_rtps_reader->p_data_fifo, p_job);
}


extern ReturnCode_t change_LivelinessLost_status(DataWriter * p_datawriter);
extern Time_t time_subtraction(const Time_t minuend_time, const Time_t subtrahend_time);
/************************************************************************/
/*                                                                      */
/************************************************************************/
void qosim_liveliness_check_wakeup_time_for_writer(qos_policy_t* const p_this, data_t* const p_job)
{
	rtps_writer_t* p_rtps_writer = NULL;
	Time_t current_time = currenTime();

	if (!p_job) return;

	p_rtps_writer = (rtps_writer_t *)p_job->p_rtps_writer;

///	trace_msg(NULL, TRACE_LOG, "check_wakeup_time--- \t(%d) %d", (int)p_rtps_writer%10, current_time.sec);
	if (p_rtps_writer->b_liveliness_alive)
	{
		//[KKI] p_rtps_writer 가 살았는지 죽었는지 설정한다.
		change_LivelinessLost_status(p_rtps_writer->p_datawriter);
		//[KKI] p_rtps_writer 를 not_alive 로 세팅한다.
		//trace_msg(NULL, TRACE_LOG, "\tNOT_ALIVE__%-20s", p_rtps_writer->p_datawriter->p_topic->topic_name);
		p_rtps_writer->b_liveliness_alive = false;
	}
	p_job->next_wakeup_time = time_addition(current_time, p_rtps_writer->liveliness_lease_duration);
///	trace_msg(NULL, TRACE_LOG, "next__wakeup_time--- \t(%d) %d", (int)p_rtps_writer%10, p_job->next_wakeup_time.sec);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
int get_size_of_liveliness_job(void)
{
	return sizeof(ParticipantMessageData)+11;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool qosim_is_liveliness_job(qos_policy_t* const p_this, const data_t* const p_job)
{
	return (p_job->i_size == get_size_of_liveliness_job());
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
static data_t* add_liveliness_job_to(data_fifo_t* const p_fifo, const Duration_t lease_duration, rtps_writer_t* const p_rtps_writer, void* const p_rtps_reader)
{
	data_t* p_job = NULL;
	Time_t current_time = currenTime();

	if (!is_same_duration(lease_duration, TIME_INFINITE))
	{
		p_job = data_new(get_size_of_liveliness_job());

		p_job->initial_time = current_time;
		p_job->next_wakeup_time = time_addition(current_time, to_time(lease_duration));
		p_job->priority = 4;
		p_job->p_rtps_writer = p_rtps_writer;
		p_job->v_rtps_reader = p_rtps_reader;

		if (p_rtps_writer)
		{
			p_job->p_rtps_writer->p_liveliness_job = p_job;
			timed_job_queue_time_compare_data_fifo_put(p_fifo, p_job);
		}
		else if (p_rtps_reader)
		{
			data_fifo_put(p_fifo, p_job);
		}
	}

	return p_job;
}


/************************************************************************/
/* reader_thread 의 Job 큐에 Liveliness Job 을 만들어 넣는다.           */
/************************************************************************/
data_t* qosim_add_liveliness_job_for_datareader(qos_policy_t* const p_this, const DataReader* const p_datareader)
{
	data_t* p_job = NULL;

	if (p_datareader && p_datareader->p_related_rtps_reader)
	{
		p_job = add_liveliness_job_to(((rtps_reader_t*)p_datareader->p_related_rtps_reader)->p_data_fifo, p_datareader->datareader_qos.liveliness.lease_duration, NULL, p_datareader->p_related_rtps_reader);
	}

	return p_job;
}


/************************************************************************/
/* writer_thread 의 Job 큐에 Liveliness Job 을 만들어 넣는다.           */
/************************************************************************/
data_t* qosim_add_liveliness_job_for_datawriter(qos_policy_t* const p_this, const DataWriter* const p_datawriter)
{
	data_t* p_job = NULL;

	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		p_job = add_liveliness_job_to(p_datawriter->p_publisher->p_domain_participant->p_writer_thread_fifo, p_datawriter->datawriter_qos.liveliness.lease_duration, p_datawriter->p_related_rtps_writer, NULL);
		cond_signal(&p_datawriter->p_publisher->p_domain_participant->writer_thread_wait);
	}

	return p_job;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
static data_t* remove_liveliness_job_from(data_fifo_t* const p_fifo, const Duration_t lease_duration, const void* const v_endpoint)
{
	data_t** pp_prev_s_next = NULL;
	data_t* p_curr_job = NULL;

	if (!is_same_duration(lease_duration, TIME_INFINITE))
	{
		mutex_lock(&p_fifo->lock);

		pp_prev_s_next = &p_fifo->p_first;
		for (p_curr_job = p_fifo->p_first; p_curr_job != NULL; p_curr_job = p_curr_job->p_next)
		{
			if (qosim_is_liveliness_job(NULL, p_curr_job) && (p_curr_job->v_rtps_reader == v_endpoint || p_curr_job->p_rtps_writer == v_endpoint))
			{
				*pp_prev_s_next = p_curr_job->p_next;
				if (*pp_prev_s_next == NULL)
				{
					p_fifo->pp_last = pp_prev_s_next;
				}

				p_fifo->i_depth--;
				p_fifo->i_size -= p_curr_job->i_size;

				p_curr_job->p_next = NULL;
				break;
			}
			pp_prev_s_next = &p_curr_job->p_next;
		}

		mutex_unlock(&p_fifo->lock);
	}

	return p_curr_job;
}


/************************************************************************/
/* DataReader 를 삭제할 때                                              */
/* LivelinessQoS 가 설정되어 있을 경우                                  */
/* reader_thread 의 Job 큐에서 Liveliness Job 을 찾아서 지운다.         */
/************************************************************************/
data_t* qosim_remove_liveliness_job_for_datareader(qos_policy_t* const p_this, const DataReader* const p_datareader)
{
	data_t* p_job = NULL;

	if (p_datareader && p_datareader->p_related_rtps_reader)
	{
		p_job = remove_liveliness_job_from(((rtps_reader_t*)p_datareader->p_related_rtps_reader)->p_data_fifo, p_datareader->datareader_qos.liveliness.lease_duration, p_datareader->p_related_rtps_reader);
	}

	return p_job;
}


/************************************************************************/
/* DataWriter 를 삭제할 때                                              */
/* LivelinessQoS 가 설정되어 있을 경우                                  */
/* reader_thread 의 Job 큐에서 Liveliness Job 을 찾아서 지운다.         */
/************************************************************************/
data_t* qosim_remove_liveliness_job_for_datawriter(qos_policy_t* const p_this, const DataWriter* const p_datawriter)
{
	data_t* p_job = NULL;

	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		p_job = remove_liveliness_job_from(p_datawriter->p_publisher->p_domain_participant->p_writer_thread_fifo, p_datawriter->datawriter_qos.liveliness.lease_duration, p_datawriter->p_related_rtps_writer);
	}

	return p_job;
}
