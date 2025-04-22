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
	UserData QoS Policy를 위한 모듈.

	이력
	2012-11-20
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


/************************************************************************/
/* UserData QoS Policy                                                 */
/* compare 하는 부분 필요                                               */
/* 원격지의 Data SubMessage에서 user_data에 대한 parsing 필요           */
/************************************************************************/

#define QOS_USER_DATA_STRING_SIZE 128

typedef struct dds_user_data_string
{
	int32_t size;
	char value[QOS_USER_DATA_STRING_SIZE];
} dds_user_data_string;


////int get_dds_user_data_string(dds_user_data_string* p_string, char* p_data, int string_size, int i_all_data_size)
////{
////	int32_t i_string = 0;
////
////	memset(p_string,0, sizeof(dds_user_data_string));
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


void qosim_find_user_data_qos_from_parameterlist(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data_qos, char* p_value, int32_t i_size)
{
	char* p_user_data_value = NULL;
	int32_t	i_found = 0;
	int32_t i_num = 0;
	int32_t i_len = 0;
	dds_user_data_string user_data_string;

	find_parameter_list(p_value, i_size, PID_USER_DATA, &p_user_data_value, &i_found);

	if (i_found && p_user_data_value)
	{
		int32_t i_user_data_length = *(int32_t*)p_user_data_value;
		i_num += sizeof(int32_t);

		p_user_data_qos->value.i_string = 0;

		memset(&user_data_string, 0, sizeof(dds_user_data_string));
		memcpy(&user_data_string.value, p_user_data_value+4, i_user_data_length);

		INSERT_ELEM(p_user_data_qos->value.pp_string, p_user_data_qos->value.i_string, p_user_data_qos->value.i_string, strdup(user_data_string.value));
	}
	i_found = 0;
}


////void find_datawriter_user_data_qos_from_parameterlist(DataWriterQos* p_datawriter_qos, char* value, int32_t i_size)
////{
////	qosim_find_user_data_qos_from_parameterlist((&p_datawriter_qos->user_data), value, i_size);
////}


////void find_datareader_user_data_qos_from_parameterlist(DataReaderQos* p_datareader_qos, char* value, int32_t i_size)
////{
////	qosim_find_user_data_qos_from_parameterlist((&p_datareader_qos->user_data), value, i_size);
////}


////void find_participant_user_data_qos_from_parameterlist(DomainParticipantQos* p_domainparticipant_qos, char* value, int32_t i_size)
////{
////	qosim_find_user_data_qos_from_parameterlist((&p_domainparticipant_qos->user_data), value, i_size);
////}


////bool compare_user_data_qos(UserDataQosPolicy a_user_data_qos, UserDataQosPolicy b_user_data_qos)
////{
////	int32_t i,j;
////
////	if (a_user_data_qos.value.i_string == 0 && b_user_data_qos.value.i_string == 0)
////	{
////		return true;
////	}
////	else
////	{
////		for (i = 0; i < a_user_data_qos.value.i_string; i++)
////		{
////			//if (strlen(a_user_data_qos.value.pp_string[i]) == 0) return true;
////
////			//if (a_user_data_qos.value.pp_string[i][0] == '*') return true;
////
////			for (j = 0; j < b_user_data_qos.value.i_string; j++)
////			{
////				//if(strlen(b_user_data_qos.value.pp_string[j]) == 0) return true;
////
////				//if(b_user_data_qos.value.pp_string[j][0] == '*') return true;
////
////				if (strcmp(a_user_data_qos.value.pp_string[i], b_user_data_qos.value.pp_string[j]) == 0)
////				{
////					return true;
////				}
////			}
////		}
////
////		return false;
////	}
////
////	return true;
////}


////UserDataQosPolicy get_user_data_qos_from_datawriter(DataWriterQos* p_datawriter_qos)
////{
////	return p_datawriter_qos->user_data;
////}


////UserDataQosPolicy get_user_data_qos_from_datareader(DataReaderQos* p_datareader_qos)
////{
////	return p_datareader_qos->user_data;
////}


////UserDataQosPolicy get_user_data_qos_from_participant(DomainParticipantQos* p_domainparticipant_qos)
////{
////	return p_domainparticipant_qos->user_data;
////}


////UserDataQosPolicy get_user_data_qos_from_publication_builtin_topic_data(PublicationBuiltinTopicData* p_publication_builtin_topic_data)
////{
////	return p_publication_builtin_topic_data->user_data;
////}


////UserDataQosPolicy get_user_data_qos_from_subscription_builtin_topic_data(SubscriptionBuiltinTopicData* p_subscription_builtin_topic_data)
////{
////	return p_subscription_builtin_topic_data->user_data;
////}


//////////////////////////////////////////////////////////////////////////////////////////////
void qosim_init_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy* p_user_data)
{
	p_user_data->value.i_string = 0;
	p_user_data->value.pp_string = NULL;
}


void qosim_copy_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy* p_dst_user_data, UserDataQosPolicy* p_src_user_data)
{
	int i;
	//added by kyy (user_data)
	qosim_init_user_data_qos(p_this, p_dst_user_data);

	for (i = 0; i < p_src_user_data->value.i_string; i++)
	{
		string str = strdup(p_src_user_data->value.pp_string[i]);
		INSERT_ELEM(p_dst_user_data->value.pp_string, p_dst_user_data->value.i_string, p_dst_user_data->value.i_string, str);
	}
}


void qosim_set_qos_about_user_data_qos_from_datareader(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos)
{
	//int i;

	//qosim_init_user_data_qos(p_this, &p_datareader->datareader_qos.user_data);

	//for (i = 0; i < p_qos->user_data.value.i_string; i++)
	//{
	//	string str = strdup(p_qos->user_data.value.pp_string[i]);
	//	INSERT_ELEM(p_datareader->datareader_qos.user_data.value.pp_string, p_datareader->datareader_qos.user_data.value.i_string, p_datareader->datareader_qos.user_data.value.i_string, str);
	//}

	qosim_copy_user_data_qos(p_this, &p_datareader->datareader_qos.user_data, &p_qos->user_data);
}


void qosim_get_qos_about_user_data_qos_from_datareader(qos_policy_t* const p_this, DataReader* p_datareader, DataReaderQos* p_qos)
{
	//int i;

	//qosim_init_user_data_qos(p_this, &p_qos->user_data);

	//for (i = 0; i < p_datareader->datareader_qos.user_data.value.i_string; i++)
	//{
	//	string str = strdup(p_datareader->datareader_qos.user_data.value.pp_string[i]);
	//	INSERT_ELEM(p_qos->user_data.value.pp_string, p_qos->user_data.value.i_string, p_qos->user_data.value.i_string, str);
	//}

	qosim_copy_user_data_qos(p_this, &p_qos->user_data, &p_datareader->datareader_qos.user_data);
}


void qosim_set_qos_about_user_data_qos_from_datawriter(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos)
{
	//int i;

	//qosim_init_user_data_qos(p_this, &p_datawriter->datawriter_qos.user_data);

	//for (i = 0; i < p_qos->user_data.value.i_string; i++)
	//{
	//	string str = strdup(p_qos->user_data.value.pp_string[i]);
	//	INSERT_ELEM(p_datawriter->datawriter_qos.user_data.value.pp_string, p_datawriter->datawriter_qos.user_data.value.i_string, p_datawriter->datawriter_qos.user_data.value.i_string, str);
	//}

	qosim_copy_user_data_qos(p_this, &p_datawriter->datawriter_qos.user_data, &p_qos->user_data);
}


void qosim_get_qos_about_user_data_qos_from_datawriter(qos_policy_t* const p_this, DataWriter* p_datawriter, DataWriterQos* p_qos)
{
	//int i;

	//qosim_init_user_data_qos(p_this, &p_qos->user_data);

	//for (i = 0; i < p_qos->user_data.value.i_string; i++)
	//{
	//	string str = strdup(p_datawriter->datawriter_qos.user_data.value.pp_string[i]);
	//	INSERT_ELEM(p_qos->user_data.value.pp_string, p_qos->user_data.value.i_string, p_qos->user_data.value.i_string, str);
	//}

	qosim_copy_user_data_qos(p_this, &p_qos->user_data, &p_datawriter->datawriter_qos.user_data);
}


////void generate_user_data_for_datareader(DataReader* p_datareader, char* p_serialized, int i_size)
////{
////	if (p_datareader->datareader_qos.user_data.value.i_string > 0)
////	{
////		int32_t i;
////		char userdata[1024];
////		int32_t	size = 0;
////		int32_t	i_len = 0;
////		int32_t j = 0;
////		int32_t i_tempstring = 0;
////		i_len = sizeof(int32_t);
////
////		memset(userdata, 0, 1024);
////
////		for (i = 0; i < p_datareader->datareader_qos.user_data.value.i_string; i++)
////		{
////			size += strlen(p_datareader->datareader_qos.user_data.value.pp_string[i]);
////		}
////
////		memcpy(userdata, &size, i_len);
////
////		for (j = 0; j < p_datareader->datareader_qos.user_data.value.i_string; j++)
////		{
////			memcpy(userdata+i_len, p_datareader->datareader_qos.user_data.value.pp_string[j], strlen(p_datareader->datareader_qos.user_data.value.pp_string[j]));
////			i_len += strlen(p_datareader->datareader_qos.user_data.value.pp_string[j]);
////		}
////
////		if (i_len > 0)
////		{
////			gererate_parameter(p_serialized+i_size, &i_size, PID_USER_DATA, i_len, userdata);
////		}
////	}
////}


void qosim_set_qos_about_user_data_qos_from_domainparticipant(qos_policy_t* const p_this, DomainParticipant* p_domainparticipant, DomainParticipantQos* p_qos)
{
	//int i;

	//qosim_init_user_data_qos(p_this, &p_domainparticipant->qos.user_data);

	//for (i = 0; i < p_qos->user_data.value.i_string; i++)
	//{
	//	string str = strdup(p_qos->user_data.value.pp_string[i]);
	//	INSERT_ELEM(p_domainparticipant->qos.user_data.value.pp_string, p_domainparticipant->qos.user_data.value.i_string, p_domainparticipant->qos.user_data.value.i_string, str);
	//}

	qosim_copy_user_data_qos(p_this, &p_domainparticipant->qos.user_data, &p_qos->user_data);
}


////int generate_user_data_for_domainparticipant(DomainParticipant* p_participant, char* p_userdata)
////{
////	if (p_participant->qos.user_data.value.i_string > 0)
////	{
////		int i;
//////		char userdata[1024];
////		int32_t	size = 0;
////		int32_t	i_len = 0;
////		int32_t j = 0;
////		int32_t i_tempstring = 0;
////		i_len = sizeof(int32_t);
////
////		memset(p_userdata, 0, 1024);
////
////		for (i = 0; i < p_participant->qos.user_data.value.i_string; i++)
////		{
////			size += strlen(p_participant->qos.user_data.value.pp_string[i]);
////		}
////
////		memcpy(p_userdata, &size, i_len);
////		//			printf("Size = %d \n",size);
////
////		for (j = 0; j < p_participant->qos.user_data.value.i_string; j++)
////		{
////			memcpy(p_userdata+i_len, p_participant->qos.user_data.value.pp_string[j], strlen(p_participant->qos.user_data.value.pp_string[j]));
////			i_len += strlen(p_participant->qos.user_data.value.pp_string[j]);
////		}
////
////		return i_len;
////	}
////	else
////	{
////		return 0;
////	}
////}


void qosim_print_user_data_qos(qos_policy_t* const p_this, UserDataQosPolicy user_data_qos)
{
	int i;

	if (user_data_qos.value.i_string > 0)
	{
		printf("UserData Size = %d, ", user_data_qos.value.i_string);
	}

	for (i = 0; i < user_data_qos.value.i_string; i++)
	{
		printf("UserData Value = %s\n", user_data_qos.value.pp_string[i]);
	}
}


int qosim_generate_user_data(qos_policy_t* const p_this, char* p_serialized, UserDataQosPolicy a_user_data_qos, int i_size)
{
	if (a_user_data_qos.value.i_string > 0)
	{
		int32_t i;
		char userdata[1024];
		int32_t	size = 0;
		int32_t	i_len = 0;
		int32_t j = 0;
		int32_t i_tempstring = 0;
		i_len = sizeof(int32_t);

		memset(userdata, 0, 1024);

		for (i = 0; i < a_user_data_qos.value.i_string; i++)
		{
			size += strlen(a_user_data_qos.value.pp_string[i]);
		}

		memcpy(userdata, &size, i_len);

		for (j = 0; j < a_user_data_qos.value.i_string; j++)
		{
			memcpy(userdata+i_len, a_user_data_qos.value.pp_string[j], strlen(a_user_data_qos.value.pp_string[j]));
			i_len += strlen(a_user_data_qos.value.pp_string[j]);
		}

		gererate_parameter(p_serialized+i_size, &i_size, PID_USER_DATA, i_len, userdata);

		return i_size;
	}
	else
	{
		return i_size;
	}
}
