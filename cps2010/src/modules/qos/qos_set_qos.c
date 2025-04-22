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
	set_qos를 위한 모듈.

	이력
	2012-10-15
*/
#include "../rtps/rtps.h"


/************************************************************************/
/* Subscriber Entity 의 set_qos() 함수에서 호출하기 위한 함수           */
/* 만일 엔티티가 enable된 이후에 No Changeable QoS 값을 변경할 경우,    */
/* IMMUTABLE_POLICY를 리턴함                                            */
/************************************************************************/
ReturnCode_t qosim_check_immutable_policy_before_change_publisher_qos(qos_policy_t* const p_this, const Publisher* const p_publisher, const PublisherQos* const p_new_qos)
{
	const PublisherQos* const p_old_qos = &p_publisher->publisher_qos;

	if (p_publisher->b_enable == false)
	{
		return RETCODE_OK;
	}

	if ((p_old_qos->presentation.access_scope    != p_new_qos->presentation.access_scope)    || 
		(p_old_qos->presentation.coherent_access != p_new_qos->presentation.coherent_access) || 
		(p_old_qos->presentation.ordered_access  != p_new_qos->presentation.ordered_access))
	{
		return RETCODE_IMMUTABLE_POLICY;
	}
	else
	{
		return RETCODE_OK;
	}
}


/************************************************************************/
/* Subscriber Entity 의 set_qos() 함수에서 호출하기 위한 함수           */
/* 만일 엔티티가 enable된 이후에 No Changeable QoS 값을 변경할 경우,    */
/* IMMUTABLE_POLICY를 리턴함                                            */
/************************************************************************/
ReturnCode_t qosim_check_immutable_policy_before_change_subscriber_qos(qos_policy_t* const p_this, const Subscriber* const p_subscriber, const SubscriberQos* const p_new_qos)
{
	const SubscriberQos* const p_old_qos = &p_subscriber->subscriber_qos;

	if (p_subscriber->b_enable == false)
	{
		return RETCODE_OK;
	}

	if ((p_old_qos->presentation.access_scope    != p_new_qos->presentation.access_scope)    || 
		(p_old_qos->presentation.coherent_access != p_new_qos->presentation.coherent_access) || 
		(p_old_qos->presentation.ordered_access  != p_new_qos->presentation.ordered_access))
	{
		return RETCODE_IMMUTABLE_POLICY;
	}
	else
	{
		return RETCODE_OK;
	}
}


/************************************************************************/
/* Topic Entity 의 set_qos() 함수에서 호출하기 위한 함수                */
/* 만일 엔티티가 enable된 이후에 No Changeable QoS 값을 변경할 경우,    */
/* IMMUTABLE_POLICY를 리턴함                                            */
/************************************************************************/
ReturnCode_t qosim_check_immutable_policy_before_change_topic_qos(qos_policy_t* const p_this, const Topic* const p_topic, const TopicQos* const p_new_qos)
{
	const TopicQos* const p_old_qos = &p_topic->topic_qos;

	if (p_topic->b_enable == false)
	{
		return RETCODE_OK;
	}

	if ((p_old_qos->durability.kind                                  != p_new_qos->durability.kind)                                  || 
		(p_old_qos->durability_service.history_depth                 != p_new_qos->durability_service.history_depth)                 ||
		(p_old_qos->durability_service.history_kind                  != p_new_qos->durability_service.history_kind)                  ||
		(p_old_qos->durability_service.max_instances                 != p_new_qos->durability_service.max_instances)                 ||
		(p_old_qos->durability_service.max_samples                   != p_new_qos->durability_service.max_samples)                   ||
		(p_old_qos->durability_service.max_samples_per_instance      != p_new_qos->durability_service.max_samples_per_instance)      ||
		(p_old_qos->durability_service.service_cleanup_delay.sec     != p_new_qos->durability_service.service_cleanup_delay.sec)     ||
		(p_old_qos->durability_service.service_cleanup_delay.nanosec != p_new_qos->durability_service.service_cleanup_delay.nanosec) ||
		(p_old_qos->ownership.kind                                   != p_new_qos->ownership.kind)                                   || 
		(p_old_qos->liveliness.kind                                  != p_new_qos->liveliness.kind)                                  || 
		(p_old_qos->liveliness.lease_duration.sec                    != p_new_qos->liveliness.lease_duration.sec)                    || 
		(p_old_qos->liveliness.lease_duration.nanosec                != p_new_qos->liveliness.lease_duration.nanosec)                || 
		(p_old_qos->reliability.kind                                 != p_new_qos->reliability.kind)                                 || 
		(p_old_qos->reliability.max_blocking_time.sec                != p_new_qos->reliability.max_blocking_time.sec)                || 
		(p_old_qos->reliability.max_blocking_time.nanosec            != p_new_qos->reliability.max_blocking_time.nanosec)            || 
		(p_old_qos->destination_order.kind                           != p_new_qos->destination_order.kind)                           || 
		(p_old_qos->history.kind                                     != p_new_qos->history.kind)                                     ||
		(p_old_qos->history.depth                                    != p_new_qos->history.depth)                                    ||
		(p_old_qos->resource_limits.max_samples                      != p_new_qos->resource_limits.max_samples)                      ||
		(p_old_qos->resource_limits.max_instances                    != p_new_qos->resource_limits.max_instances)                    ||
		(p_old_qos->resource_limits.max_samples_per_instance         != p_new_qos->resource_limits.max_samples_per_instance))
	{
		return RETCODE_IMMUTABLE_POLICY;
	}
	else
	{
		return RETCODE_OK;
	}
}


/************************************************************************/
/* DataWriter Entity 의 set_qos() 함수에서 호출하기 위한 함수           */
/* 만일 엔티티가 enable된 이후에 No Changeable QoS 값을 변경할 경우,    */
/* IMMUTABLE_POLICY를 리턴함                                            */
/************************************************************************/
ReturnCode_t qosim_check_immutable_policy_before_change_datawriter_qos(qos_policy_t* const p_this, const DataWriter* const p_datawriter, const DataWriterQos* const p_new_qos)
{
	const DataWriterQos* const p_old_qos = &p_datawriter->datawriter_qos;

	if (p_datawriter->b_enable == false)
	{
		return RETCODE_OK;
	}

	if ((p_old_qos->durability.kind                                  != p_new_qos->durability.kind)                                  || 
		(p_old_qos->durability_service.history_depth                 != p_new_qos->durability_service.history_depth)                 ||
		(p_old_qos->durability_service.history_kind                  != p_new_qos->durability_service.history_kind)                  ||
		(p_old_qos->durability_service.max_instances                 != p_new_qos->durability_service.max_instances)                 ||
		(p_old_qos->durability_service.max_samples                   != p_new_qos->durability_service.max_samples)                   ||
		(p_old_qos->durability_service.max_samples_per_instance      != p_new_qos->durability_service.max_samples_per_instance)      ||
		(p_old_qos->durability_service.service_cleanup_delay.sec     != p_new_qos->durability_service.service_cleanup_delay.sec)     ||
		(p_old_qos->durability_service.service_cleanup_delay.nanosec != p_new_qos->durability_service.service_cleanup_delay.nanosec) ||
		(p_old_qos->ownership.kind                                   != p_new_qos->ownership.kind)                                   || 
		(p_old_qos->liveliness.kind                                  != p_new_qos->liveliness.kind)                                  || 
		(p_old_qos->liveliness.lease_duration.sec                    != p_new_qos->liveliness.lease_duration.sec)                    || 
		(p_old_qos->liveliness.lease_duration.nanosec                != p_new_qos->liveliness.lease_duration.nanosec)                || 
		(p_old_qos->reliability.kind                                 != p_new_qos->reliability.kind)                                 || 
		(p_old_qos->reliability.max_blocking_time.sec                != p_new_qos->reliability.max_blocking_time.sec)                || 
		(p_old_qos->reliability.max_blocking_time.nanosec            != p_new_qos->reliability.max_blocking_time.nanosec)            || 
		(p_old_qos->destination_order.kind                           != p_new_qos->destination_order.kind)                           || 
		(p_old_qos->history.kind                                     != p_new_qos->history.kind)                                     ||
		(p_old_qos->history.depth                                    != p_new_qos->history.depth)                                    ||
		(p_old_qos->resource_limits.max_samples                      != p_new_qos->resource_limits.max_samples)                      ||
		(p_old_qos->resource_limits.max_instances                    != p_new_qos->resource_limits.max_instances)                    ||
		(p_old_qos->resource_limits.max_samples_per_instance         != p_new_qos->resource_limits.max_samples_per_instance))
	{
		return RETCODE_IMMUTABLE_POLICY;
	}
	else
	{
		return RETCODE_OK;
	}
}


/************************************************************************/
/* DataReader Entity 의 set_qos() 함수에서 호출하기 위한 함수           */
/* 만일 엔티티가 enable된 이후에 No Changeable QoS 값을 변경할 경우,    */
/* IMMUTABLE_POLICY를 리턴함                                            */
/************************************************************************/
ReturnCode_t qosim_check_immutable_policy_before_change_datareader_qos(qos_policy_t* const p_this, const DataReader* const p_datareader, const DataReaderQos* const p_new_qos)
{
	const DataReaderQos* const p_old_qos = &p_datareader->datareader_qos;

	if (p_datareader->b_enable == false)
	{
		return RETCODE_OK;
	}

	// IMMUTABLE_POLICY 를 검사 
	if ((p_old_qos->durability.kind                          != p_new_qos->durability.kind)                       || 
		(p_old_qos->ownership.kind                           != p_new_qos->ownership.kind)                        || 
		(p_old_qos->liveliness.kind                          != p_new_qos->liveliness.kind)                       || 
		(p_old_qos->liveliness.lease_duration.sec            != p_new_qos->liveliness.lease_duration.sec)         || 
		(p_old_qos->liveliness.lease_duration.nanosec        != p_new_qos->liveliness.lease_duration.nanosec)     || 
		(p_old_qos->reliability.kind                         != p_new_qos->reliability.kind)                      || 
		(p_old_qos->reliability.max_blocking_time.sec        != p_new_qos->reliability.max_blocking_time.sec)     || 
		(p_old_qos->reliability.max_blocking_time.nanosec    != p_new_qos->reliability.max_blocking_time.nanosec) || 
		(p_old_qos->destination_order.kind                   != p_new_qos->destination_order.kind)                || 
		(p_old_qos->history.kind                             != p_new_qos->history.kind)                          ||
		(p_old_qos->history.depth                            != p_new_qos->history.depth)                         ||
		(p_old_qos->resource_limits.max_samples              != p_new_qos->resource_limits.max_samples)           ||
		(p_old_qos->resource_limits.max_instances            != p_new_qos->resource_limits.max_instances)         ||
		(p_old_qos->resource_limits.max_samples_per_instance != p_new_qos->resource_limits.max_samples_per_instance))
	{
		return RETCODE_IMMUTABLE_POLICY;
	}
	else
	{
		return RETCODE_OK;
	}
}


/************************************************************************/
/************************************************************************/
ReturnCode_t qosim_check_inconsistent_policy_before_change_topic_qos(qos_policy_t* const p_this, const TopicQos* const p_new_qos)
{
	if (p_new_qos == NULL) return RETCODE_OK;

	// INCONSISTENT_POLICY 를 검사 
	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples != LENGTH_UNLIMITED) && 
		(p_new_qos->resource_limits.max_samples_per_instance > p_new_qos->resource_limits.max_samples))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples_per_instance < p_new_qos->history.depth))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	return RETCODE_OK;
}


/************************************************************************/
/************************************************************************/
ReturnCode_t qosim_check_inconsistent_policy_before_change_datawriter_qos(qos_policy_t* const p_this, const DataWriterQos* const p_new_qos)
{
	if (p_new_qos == NULL) return RETCODE_OK;

	// INCONSISTENT_POLICY 를 검사 
	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples != LENGTH_UNLIMITED) && 
		(p_new_qos->resource_limits.max_samples_per_instance > p_new_qos->resource_limits.max_samples))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples_per_instance < p_new_qos->history.depth))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	return RETCODE_OK;
}


/************************************************************************/
/************************************************************************/
extern bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2);
ReturnCode_t qosim_check_inconsistent_policy_before_change_datareader_qos(qos_policy_t* const p_this, const DataReaderQos* const p_new_qos)
{
	if (p_new_qos == NULL) return RETCODE_OK;

	// INCONSISTENT_POLICY 를 검사 
	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples != LENGTH_UNLIMITED) && 
		(p_new_qos->resource_limits.max_samples_per_instance > p_new_qos->resource_limits.max_samples))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	if ((p_new_qos->resource_limits.max_samples_per_instance != LENGTH_UNLIMITED) &&
		(p_new_qos->resource_limits.max_samples_per_instance < p_new_qos->history.depth))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	if ((time_left_bigger_d(p_new_qos->time_based_filter.minimum_separation, p_new_qos->deadline.period)))
	{
		return RETCODE_INCONSISTENT_POLICY;
	}

	return RETCODE_OK;
}


extern void rtps_added_user_defined_writer(DataWriter *p_dataWriter, rtps_writer_t *p_rtps_writer);
extern void rtps_add_readerproxies_to_writer(rtps_writer_t* p_rtps_writer);
void qosim_send_publication_info_after_change_datawriter_qos(qos_policy_t* const p_this, DataWriter* const p_datawriter)
{
	if (p_datawriter->p_related_rtps_writer != NULL)
	{
		rtps_added_user_defined_writer(p_datawriter, (rtps_writer_t*)p_datawriter->p_related_rtps_writer);
		rtps_add_readerproxies_to_writer((rtps_writer_t*)p_datawriter->p_related_rtps_writer);
	}
}


extern void rtps_added_user_defined_reader(DataReader* p_datareader, rtps_reader_t* p_rtps_reader);
extern void rtps_add_writerproxies_to_reader(rtps_reader_t* p_rtps_reader);
void qosim_send_subscription_info_after_change_datareader_qos(qos_policy_t* const p_this, DataReader* const p_datareader)
{
	if (p_datareader->p_related_rtps_reader != NULL)
	{
		rtps_added_user_defined_reader(p_datareader, (rtps_reader_t*)p_datareader->p_related_rtps_reader);
		rtps_add_writerproxies_to_reader((rtps_reader_t*)p_datareader->p_related_rtps_reader);
	}
}
