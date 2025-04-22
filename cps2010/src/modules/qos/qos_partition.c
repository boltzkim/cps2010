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
Partition QoS Policy를 위한 모듈.

이력
2012-11-26
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


bool qosim_compare_partition(qos_policy_t* const p_this, PartitionQosPolicy a_partition, PartitionQosPolicy b_partition)
{
	int32_t i, j;

	if (a_partition.name.i_string == 0 && b_partition.name.i_string == 0)
	{
		return true;
	}
	else
	{
		for (i = 0; i < a_partition.name.i_string; i++)
		{
			if (strlen(a_partition.name.pp_string[i]) == 0) return true;

			if (a_partition.name.pp_string[i][0] == '*') return true;

			for (j = 0; j < b_partition.name.i_string; j++)
			{
				if (strlen(b_partition.name.pp_string[j]) == 0) return true;

				if (b_partition.name.pp_string[j][0] == '*') return true;

				if (strcmp(a_partition.name.pp_string[i], b_partition.name.pp_string[j]) == 0) return true;
			}
		}

		return false;
	}

	return true;
}


int get_dds_partition_string(dds_partition_string* p_string, char* p_data, int string_size, int i_all_data_size)
{
	int32_t i_string = 0;

	memset(p_string,0, sizeof(dds_partition_string));
	memcpy(p_string, p_data, i_all_data_size);

	if (p_string->size <= string_size)
	{
		i_string = p_string->size / 4;
		if ((p_string->size % 4) != 0)
		{
			i_string++;
		}
		i_string *= 4;
	}

	i_string += sizeof(int32_t); // i_size

	return i_string;
}


void qosim_find_partition_qos_from_parameterlist(qos_policy_t* const p_this, PartitionQosPolicy* p_partition_qos, char* p_value, int32_t i_size)
{
	char* p_partition_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;

	find_parameter_list(p_value, i_size, PID_PARTITION, &p_partition_value, &i_found);
	if (i_found && p_partition_value)
	{
		int32_t i;
		int32_t i_partion_length = *(int32_t*)p_partition_value;
		i_num += sizeof(int32_t);

		for (i = 0 ; i < i_partion_length; i++)
		{
			dds_partition_string partition_string;
			i_num += get_dds_partition_string(&partition_string, &p_partition_value[i_num], QOS_PARTITION_STRING_SIZE, i_found-i_num);
			i_found = i_found;
			INSERT_ELEM(p_partition_qos->name.pp_string, p_partition_qos->name.i_string, p_partition_qos->name.i_string, strdup(partition_string.value));
		}
	}
	i_found = 0;
}


int qosim_generate_partition(qos_policy_t* const p_this, char* p_serialized, PartitionQosPolicy a_partition_qos, int i_size)
{
	if (a_partition_qos.name.i_string > 0)
	{
		char partition[1024];
		int32_t	size = 0;
		int32_t	i_len = 0;
		int32_t j = 0;
		int32_t i_tempstring = 0;
		char* p_temp;

		memset(partition, 0, 1024);
		i_len = sizeof(int32_t);

		size = a_partition_qos.name.i_string;

		memcpy(partition, &size, i_len);

		for (j = 0; j < a_partition_qos.name.i_string; j++)
		{
			p_temp = get_string(a_partition_qos.name.pp_string[j], &i_tempstring);
			memcpy(partition+i_len, p_temp, i_tempstring);
			i_len += i_tempstring;
			FREE(p_temp);
		}

		gererate_parameter(p_serialized+i_size, &i_size, PID_PARTITION, i_len, partition);

		return i_size;
	}
	else
	{
		return i_size;
	}
}
