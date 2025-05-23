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
	GroupData QoS Policy를 위한 모듈.

	이력
	2012-12-20
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


/************************************************************************/
/* GroupData QoS Policy                                                 */
/* compare 하는 부분 필요                                               */
/* 원격지의 Data SubMessage에서 group_data에 대한 parsing 필요           */
/************************************************************************/

#define QOS_GROUP_DATA_STRING_SIZE 128

typedef struct dds_group_data_string
{
	int32_t size;
	char value[QOS_GROUP_DATA_STRING_SIZE];
} dds_group_data_string;


////int get_dds_group_data_string(dds_group_data_string* p_string, char* p_data, int string_size, int i_all_data_size)
////{
////	int32_t i_string = 0;
////
////	memset(p_string,0, sizeof(dds_group_data_string));
////	memcpy(p_string, p_data, i_all_data_size);
////	
////	if (p_string->size <= string_size)
////	{
////		i_string = p_string->size / 4;
////		if ((p_string->size % 4)!=0)
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


void qosim_find_group_data_qos_from_parameterlist(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data_qos, char* p_value, int32_t i_size)
{
	char* p_group_data_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;
	int32_t i_len = 0;
	dds_group_data_string group_data_string;

	find_parameter_list(p_value, i_size, PID_GROUP_DATA, &p_group_data_value, &i_found);

	if (i_found && p_group_data_value)
	{
		int32_t i_group_data_length = *(int32_t*)p_group_data_value;
		i_num += sizeof(int32_t);

		p_group_data_qos->value.i_string = 0;

		memset(&group_data_string,0, sizeof(dds_group_data_string));
		memcpy(&group_data_string.value, p_group_data_value+4, i_group_data_length);

		INSERT_ELEM(p_group_data_qos->value.pp_string, p_group_data_qos->value.i_string, p_group_data_qos->value.i_string, strdup(group_data_string.value));
	}

	i_found = 0;
}


void qosim_print_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy group_data_qos)
{
	int i;

	if (group_data_qos.value.i_string > 0)
	{
		printf("GroupData = %d\n",group_data_qos.value.i_string);
	}

	for (i = 0; i < group_data_qos.value.i_string; i++)
	{
		printf("GroupData = %s\n", group_data_qos.value.pp_string[i]);
	}
}


void qosim_init_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy* p_group_data)
{
	p_group_data->value.i_string = 0;
	p_group_data->value.pp_string = NULL;
}


////void set_init_group_data_qos(GroupDataQosPolicy p_group_data)
////{
////	p_group_data.value.i_string = 0;
////	p_group_data.value.pp_string = NULL;
////}


void qosim_copy_group_data_qos(qos_policy_t* const p_this, GroupDataQosPolicy* p_dst_group_data, GroupDataQosPolicy* p_src_group_data)
{
	int i;

	qosim_init_group_data_qos(p_this, p_dst_group_data);

	for (i = 0; i < p_src_group_data->value.i_string; i++)
	{
		string str = strdup(p_src_group_data->value.pp_string[i]);
		INSERT_ELEM(p_dst_group_data->value.pp_string, p_dst_group_data->value.i_string, p_dst_group_data->value.i_string, str);
	}
}


void qosim_set_qos_about_group_data_qos_from_publisher(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos)
{
	int i;

	for (i = 0; i < p_qos->group_data.value.i_string; i++)
	{
		string str = strdup(p_qos->group_data.value.pp_string[i]);
		INSERT_ELEM(p_publisher->publisher_qos.group_data.value.pp_string, p_publisher->publisher_qos.group_data.value.i_string, p_publisher->publisher_qos.group_data.value.i_string, str);
	}
}


void qosim_get_qos_about_group_data_qos_from_publisher(qos_policy_t* const p_this, Publisher* p_publisher, PublisherQos* p_qos)
{
	int i;

	for (i = 0; i < p_publisher->publisher_qos.group_data.value.i_string; i++)
	{
		string str = strdup(p_publisher->publisher_qos.group_data.value.pp_string[i]);
		INSERT_ELEM(p_qos->group_data.value.pp_string, p_qos->group_data.value.i_string, p_qos->group_data.value.i_string, str);
	}
}


void qosim_set_qos_about_group_data_qos_from_subscriber(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos* p_qos)
{
	int i;

	for (i = 0; i < p_qos->group_data.value.i_string; i++)
	{
		string str = strdup(p_qos->group_data.value.pp_string[i]);
		INSERT_ELEM(p_subscriber->subscriber_qos.group_data.value.pp_string, p_subscriber->subscriber_qos.group_data.value.i_string, p_subscriber->subscriber_qos.group_data.value.i_string, str);
	}
}


void qosim_get_qos_about_group_data_qos_from_subscriber(qos_policy_t* const p_this, Subscriber* p_subscriber, SubscriberQos *p_qos)
{
	int i;

	for (i = 0 ;i < p_subscriber->subscriber_qos.group_data.value.i_string; i++)
	{
		string str = strdup(p_subscriber->subscriber_qos.group_data.value.pp_string[i]);
		INSERT_ELEM(p_qos->group_data.value.pp_string, p_qos->group_data.value.i_string, p_qos->group_data.value.i_string, str);
	}
}


int qosim_generate_group_data(qos_policy_t* const p_this, char* p_serialized, GroupDataQosPolicy a_group_data_qos, int i_size)
{
	if (a_group_data_qos.value.i_string > 0)
	{
		int i;
		char groupdata[1024];
		int32_t	size = 0;
		int32_t	i_len = 0;
		int32_t j = 0;
		int32_t i_tempstring = 0;
		i_len = sizeof(int32_t);			

		memset(groupdata, 0, 1024);

		for (i = 0; i < a_group_data_qos.value.i_string; i++)
		{
			size += strlen(a_group_data_qos.value.pp_string[i]);
		}

		memcpy(groupdata, &size, i_len);
		//printf("Size = %d \n", size);

		for (j = 0; j < a_group_data_qos.value.i_string; j++)
		{
			memcpy(groupdata+i_len, a_group_data_qos.value.pp_string[j], strlen(a_group_data_qos.value.pp_string[j]));
			i_len += strlen(a_group_data_qos.value.pp_string[j]);
		}

		gererate_parameter(p_serialized+i_size, &i_size, PID_GROUP_DATA, i_len, groupdata);
		return i_size;
	}
	else
	{
		return i_size;
	}
}
