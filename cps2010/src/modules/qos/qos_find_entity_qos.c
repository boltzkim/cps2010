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
Entity QoS Policy를 찾기 위한 모듈.

이력
2012-11-26
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


//added by zbmon
void qosim_find_topic_qos_from_parameterlist(qos_policy_t* const p_this, TopicBuiltinTopicData* p_topic_data, char* p_value, int32_t i_size)
{
	char* p_temp_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;

	//find Topic Name
	find_parameter_list(p_value,i_size, PID_TOPIC_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Type Name
	find_parameter_list(p_value,i_size, PID_TYPE_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->type_name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Durability kind
	find_parameter_list(p_value,i_size, PID_DURABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->durability.kind = *p_temp_value;
	}
	else
	{
		//발견 못했을 경우...
		p_topic_data->durability.kind = VOLATILE_DURABILITY_QOS;
	}
	i_found = 0;

	//find Durability Service (현재 미지원)
	find_parameter_list(p_value,i_size, PID_DURABILITY_SERVICE, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->durability_service.service_cleanup_delay = *(Duration_t*)p_temp_value;
		p_topic_data->durability_service.history_kind = *(p_temp_value+8);
		p_topic_data->durability_service.history_depth = *(p_temp_value+12);
		p_topic_data->durability_service.max_instances = *(p_temp_value+16);
		p_topic_data->durability_service.max_samples = *(p_temp_value+20);
		p_topic_data->durability_service.max_samples_per_instance = *(p_temp_value+24);

	}
	else
	{
		p_topic_data->durability_service.service_cleanup_delay = *(Duration_t*)p_temp_value;
		p_topic_data->durability_service.history_kind = LENGTH_UNLIMITED;
		p_topic_data->durability_service.history_depth = LENGTH_UNLIMITED;
		p_topic_data->durability_service.max_instances = LENGTH_UNLIMITED;
		p_topic_data->durability_service.max_samples = LENGTH_UNLIMITED;
		p_topic_data->durability_service.max_samples_per_instance = LENGTH_UNLIMITED;
	}
	i_found = 0;

	//find deadline period
	find_parameter_list(p_value,i_size, PID_DEADINE, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->deadline.period = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find latency_budget
	find_parameter_list(p_value,i_size, PID_LATENCY_BUDGET, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->latency_budget.duration = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find liveliness
	find_parameter_list(p_value,i_size, PID_LIVELINESS, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->liveliness.kind = *p_temp_value;
		p_topic_data->liveliness.lease_duration = *(Duration_t*)(p_temp_value+4);
	}
	i_found = 0;

	//find Reliability
	find_parameter_list(p_value,i_size, PID_RELIABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->reliability.kind = *(int*)p_temp_value;
	}
	i_found = 0;

	//find Transport Priority
	find_parameter_list(p_value,i_size, PID_TRANSPORT_PRIORITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->transport_priority.value = *(long*)p_temp_value;
	}
	i_found = 0;

	//find Lifespan
	find_parameter_list(p_value,i_size, PID_LIFESPAN, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->lifespan.duration = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find destination_order
	find_parameter_list(p_value,i_size, PID_DESTINATION_ORDER, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->destination_order.kind = *p_temp_value;
	}
	i_found = 0;



	//find History
	find_parameter_list(p_value,i_size, PID_HISTORY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->history.kind = *(int*)p_temp_value;
		p_topic_data->history.depth = *(int32_t*)(p_temp_value+sizeof(HistoryQosPolicyKind));
	}
	i_found = 0;

	//find Resource Limits
	find_parameter_list(p_value,i_size, PID_RESOURCE_LIMITS, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->resource_limits.max_samples = *(int32_t*)p_temp_value;
		p_topic_data->resource_limits.max_instances = *(int32_t*)(p_temp_value+sizeof(int32_t));
		p_topic_data->resource_limits.max_samples_per_instance = *(int32_t*)(p_temp_value+sizeof(int32_t)+sizeof(int32_t));
	}
	i_found = 0;



	//find ownership
	find_parameter_list(p_value,i_size, PID_OWNERSHIP, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_topic_data->ownership.kind = *p_temp_value;
	}
	else
	{
		p_topic_data->ownership.kind = SHARED_OWNERSHIP_QOS;
	}
	i_found = 0;

	//find TopicData
	qosim_find_topic_data_qos_from_parameterlist(p_this, &(p_topic_data->topic_data), p_value, i_size);
}


//added by kyy
void qosim_find_publication_qos_from_parameterlist(qos_policy_t* const p_this, PublicationBuiltinTopicData* p_publication_data, char* p_value, int32_t i_size)
{
	char* p_temp_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;


	//find Topic Name
	find_parameter_list(p_value, i_size, PID_TOPIC_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->topic_name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Type Name
	find_parameter_list(p_value, i_size, PID_TYPE_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->type_name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Durability kind
	find_parameter_list(p_value, i_size, PID_DURABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->durability.kind = *p_temp_value;
	}
	else
	{
		//발견 못했을 경우...
		p_publication_data->durability.kind = VOLATILE_DURABILITY_QOS;
	}
	i_found = 0;

	//find Durability Service (현재 미지원)
	find_parameter_list(p_value, i_size, PID_DURABILITY_SERVICE, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->durability_service.service_cleanup_delay = *(Duration_t*)p_temp_value;
		p_publication_data->durability_service.history_kind = *(p_temp_value+8);
		p_publication_data->durability_service.history_depth = *(p_temp_value+12);
		p_publication_data->durability_service.max_instances = *(p_temp_value+16);
		p_publication_data->durability_service.max_samples = *(p_temp_value+20);
		p_publication_data->durability_service.max_samples_per_instance = *(p_temp_value+24);

	}
	else
	{
		p_publication_data->durability_service.service_cleanup_delay = *(Duration_t*)p_temp_value;
		p_publication_data->durability_service.history_kind = LENGTH_UNLIMITED;
		p_publication_data->durability_service.history_depth = LENGTH_UNLIMITED;
		p_publication_data->durability_service.max_instances = LENGTH_UNLIMITED;
		p_publication_data->durability_service.max_samples = LENGTH_UNLIMITED;
		p_publication_data->durability_service.max_samples_per_instance = LENGTH_UNLIMITED;
	}
	i_found = 0;

	//find deadline period
	find_parameter_list(p_value, i_size, PID_DEADINE, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->deadline.period = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find latency_budget
	find_parameter_list(p_value, i_size, PID_LATENCY_BUDGET, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->latency_budget.duration = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find liveliness
	find_parameter_list(p_value, i_size, PID_LIVELINESS, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->liveliness.kind = *p_temp_value;
		p_publication_data->liveliness.lease_duration = *(Duration_t*)(p_temp_value+4);
	}
	i_found = 0;

	//find Reliability
	find_parameter_list(p_value, i_size, PID_RELIABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->reliability.kind = *(int*)p_temp_value;
	}
	i_found = 0;

	//find Lifespan
	find_parameter_list(p_value, i_size, PID_LIFESPAN, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->lifespan.duration = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find UserData
	qosim_find_user_data_qos_from_parameterlist(NULL, &(p_publication_data->user_data), p_value, i_size);
	qosim_print_user_data_qos(NULL, p_publication_data->user_data);

	//find ownership
	find_parameter_list(p_value, i_size, PID_OWNERSHIP, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->ownership.kind = *p_temp_value;
	}
	else
	{
		p_publication_data->ownership.kind = SHARED_OWNERSHIP_QOS;
	}
	i_found = 0;

	//find ownership strength
	find_parameter_list(p_value, i_size, PID_OWNERSHIP_STRENGTH, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->ownership_strength.value = *(long*)p_temp_value;
	}
	i_found = 0;

	//compare_destination_order
	find_parameter_list(p_value, i_size, PID_DESTINATION_ORDER, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->destination_order.kind = *p_temp_value;
	}
	i_found = 0;

	//Compare Presentation kind
	find_parameter_list(p_value, i_size, PID_PRESENTATION, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_publication_data->presentation.access_scope = *p_temp_value;
		p_publication_data->presentation.coherent_access = *(p_temp_value+4);
		p_publication_data->presentation.ordered_access = *(p_temp_value+5);
	}
	i_found = 0;


	//find Partition
	qosim_find_partition_qos_from_parameterlist(p_this, &(p_publication_data->partition), p_value, i_size);
	i_found = 0;

	//find TopicData
	qosim_find_topic_data_qos_from_parameterlist(p_this, &(p_publication_data->topic_data), p_value, i_size);

	//find GroupData
	qosim_find_group_data_qos_from_parameterlist(p_this, &(p_publication_data->group_data), p_value, i_size);
}


//added by kyy
void qosim_find_subscription_qos_from_parameterlist(qos_policy_t* const p_this, SubscriptionBuiltinTopicData* p_subscription_data, char* p_value, int32_t i_size)
{
	char* p_temp_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;

	//find Topic Name
	find_parameter_list(p_value, i_size, PID_TOPIC_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->topic_name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Type Name
	find_parameter_list(p_value, i_size, PID_TYPE_NAME, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->type_name = strdup(p_temp_value+4);
	}
	i_found = 0;

	//find Durability kind
	find_parameter_list(p_value, i_size, PID_DURABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->durability.kind = *p_temp_value;
	}
	else
	{
		//발견 못했을 경우...
		p_subscription_data->durability.kind = VOLATILE_DURABILITY_QOS;
	}
	i_found = 0;

	//find Deadline period
	find_parameter_list(p_value, i_size, PID_DEADINE, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->deadline.period = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find Latency_budget
	find_parameter_list(p_value, i_size, PID_LATENCY_BUDGET, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->latency_budget.duration = *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//find Liveliness
	find_parameter_list(p_value, i_size, PID_LIVELINESS, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->liveliness.kind = *p_temp_value;
		p_subscription_data->liveliness.lease_duration = *(Duration_t*)(p_temp_value+4);
	}
	i_found = 0;

	//find Reliability
	find_parameter_list(p_value, i_size, PID_RELIABILITY, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->reliability.kind = *(int*)p_temp_value;
	}
	i_found = 0;

	//find ownership
	find_parameter_list(p_value,i_size, PID_OWNERSHIP, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->ownership.kind = *p_temp_value;
	}
	else
	{
		p_subscription_data->ownership.kind = SHARED_OWNERSHIP_QOS;
	}
	i_found = 0;

	//compare_destination_order
	find_parameter_list(p_value, i_size, PID_DESTINATION_ORDER, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->destination_order.kind = *p_temp_value;
	}
	i_found = 0;

	//find UserData
	qosim_find_user_data_qos_from_parameterlist(NULL, &(p_subscription_data->user_data), p_value, i_size);
//	qosim_print_user_data_qos(NULL, p_subscription_data->user_data);

	//find Timebased Filter
	find_parameter_list(p_value, i_size, PID_TIME_BASED_FILTER, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->time_based_filter.minimum_separation= *(Duration_t*)p_temp_value;
	}
	i_found = 0;

	//Compare Presentation kind
	find_parameter_list(p_value, i_size, PID_PRESENTATION, &p_temp_value, &i_found);
	if (i_found && p_temp_value)
	{
		p_subscription_data->presentation.access_scope = *p_temp_value;
		p_subscription_data->presentation.coherent_access = *(p_temp_value+4);
		p_subscription_data->presentation.ordered_access = *(p_temp_value+5);
	}
	i_found = 0;


	//find Partition
	qosim_find_partition_qos_from_parameterlist(p_this, &(p_subscription_data->partition), p_value, i_size);
	i_found = 0;

	//find TopicData
	qosim_find_topic_data_qos_from_parameterlist(p_this, &(p_subscription_data->topic_data), p_value, i_size);

	//find GroupData
	qosim_find_group_data_qos_from_parameterlist(p_this, &(p_subscription_data->group_data), p_value, i_size);
	//print_group_data_qos(p_subscription_data->group_data);
}
