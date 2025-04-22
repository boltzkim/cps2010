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
	Reader Data Lifecycle QoS Policy�� ���� ���.

	�̷�
	2012-11-06
*/
#include "../rtps/rtps.h"


/************************************************************************/
/* �� �Ķ���� duration_1 �� duration_2 �� ���� ���ΰ�?                 */
/************************************************************************/
//static bool is_same_duration(const Duration_t duration_1, 
//							 const Duration_t duration_2)
//{
//	return (duration_1.sec == duration_2.sec 
//		&& duration_1.nanosec == duration_2.nanosec);
//}


/************************************************************************/
/* Reader Data Lifecycle QoS Policy��                                   */
/* autopurge_nowriter_samples_delay != infinite �̰�                    */
/* �ν��Ͻ��� instance_state ���� NOT_ALIVE_NO_WRITERS �ΰ�?            */
/************************************************************************/
//static bool is_not_alive_no_writers(
//	const ReaderDataLifecycleQosPolicy* const p_reader_data_lifecycle_qos, 
//	const SampleInfo* const p_sample_info)
//{
//	if (p_reader_data_lifecycle_qos == NULL || p_sample_info == NULL)
//	{
//		return false;
//	}
//
//	if (!is_same_duration(p_reader_data_lifecycle_qos->autopurge_nowriter_samples_delay, TIME_INFINITE)
//		&& p_sample_info->instance_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}


/************************************************************************/
/* Reader Data Lifecycle QoS Policy��                                   */
/* autopurge_disposed_samples_delay != infinite �̰�                    */
/* �ν��Ͻ��� instance_state ���� NOT_ALIVE_DISPOSED �ΰ�?              */
/************************************************************************/
//static bool is_not_alive_disposed(
//	const ReaderDataLifecycleQosPolicy* const p_reader_data_lifecycle_qos, 
//	const SampleInfo* const p_sample_info)
//{
//	if (p_reader_data_lifecycle_qos == NULL || p_sample_info == NULL)
//	{
//		return false;
//	}
//
//	if (!is_same_duration(p_reader_data_lifecycle_qos->autopurge_disposed_samples_delay, TIME_INFINITE) 
//		&& p_sample_info->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}


/************************************************************************/
/*                                                                      */
/************************************************************************/
static bool time_is_infinite(const Time_t time)
{
	return (time.sec == TIME_INFINITE.sec && time.nanosec == TIME_INFINITE.nanosec);
}

static bool instanceset_is_disposed_state(const instanceset_t* const p_instanceset)
{
	if (p_instanceset == NULL) return false;
	//else return (p_instanceset->pp_messages[0]->sampleInfo.instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE);
	else return (p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE);
}


extern Time_t to_time(const Duration_t duration);
extern Time_t time_addition(const Time_t augend_time, const Time_t addend_time);
extern bool time_left_bigger(const Time_t time_1, const Time_t time_2);

static instanceset_has_timed_out(const instanceset_t* const p_instanceset, const Time_t autopurge_disposed_samples_delay)
{
	const Time_t current_time = currenTime();
	message_t* p_message = NULL;
	int i;

	for (i = p_instanceset->i_messages-1; i >= 0; i--)
	{
		p_message = p_instanceset->pp_messages[i];
		if (time_left_bigger(current_time, time_addition(autopurge_disposed_samples_delay, p_message->sampleInfo.disposed_timestamp)))
		{
			return true;
		}
	}

	return false;
}


//[KKI] despose �����̸鼭 autopurge_disposed_samples_delay �ð��� ��������, �� �ν��Ͻ��� �����Ѵ�.
//[KKI] Read()�� Take() �Լ� �ʹݿ� �����ؼ� �����ϵ��� �Ѵ�.
void qosim_remove_disposed_instance(qos_policy_t* const p_this, DataReader* p_datareader)
{
	const Time_t autopurge_disposed_samples_delay = to_time(p_datareader->datareader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay);
	instanceset_t* p_instanceset;
	int i;

	//[KKI] delay �� TIME_INFINITE �̸�, ���̻� ������ �ʿ䰡 ����.
	if (time_is_infinite(autopurge_disposed_samples_delay))
	{
		return;
	}

	for (i = p_datareader->i_instanceset-1; i >= 0; i--)
	{
		p_instanceset = p_datareader->pp_instanceset[i];
		if (instanceset_is_disposed_state(p_instanceset) && 
			instanceset_has_timed_out(p_instanceset, autopurge_disposed_samples_delay))
		{
			// �� �κ� �ٽ� ���ּ���..
			// �׳� �����ϸ� ��ü �� �ش� ���� �� �����...
			// key�� �����Ǵ� instance�� ���� �ؾ� �˴ϴ�.
//			instanceset_remove((Entity*)p_datareader, p_instanceset);
//			REMOVE_ELEM(p_datareader->pp_instanceset, p_datareader->i_instanceset, i);

		}
	}
}


/************************************************************************/
/* <guid> Writer �� ���� cachechange �� sampleInfo �� instance_state �� */
/* <instance_state> �� �ٲ۴�.                                          */
/************************************************************************/
void qosim_rtps_reader_set_instance_state_of_cachechanges_with_guid(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, InstanceStateKind instance_state, const GUID_t* const p_guid)
{
	int i = 0;
	rtps_cachechange_t* p_change;
	rtps_cachechange_t *p_change_atom = NULL;
	Time_t current_time = currenTime();

	p_change_atom = (rtps_cachechange_t *)p_rtps_reader->p_reader_cache->p_head_first;

	while(p_change_atom)
	{
		p_change = p_change_atom;
		if (memcmp(&p_change->writer_guid, p_guid, sizeof(GUID_t)) == 0)
		{
			p_change->p_org_message->sampleInfo.instance_state = instance_state;
			p_change->p_org_message->sampleInfo.disposed_timestamp = current_time;
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
}