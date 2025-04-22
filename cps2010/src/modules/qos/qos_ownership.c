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
	Ownership QoS Policy�� ���� ���.

	�̷�
	2012-10-16
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


/************************************************************************/
/* Ownership QoS Policy�� kind == EXCLUSIVE �ΰ�?                       */
/************************************************************************/
static bool is_exclusive_ownership(const OwnershipQosPolicy* const p_ownership_qos)
{
	if (p_ownership_qos == NULL)
	{
		return false;
	}

	if (p_ownership_qos->kind == EXCLUSIVE_OWNERSHIP_QOS)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* p_rtpsReader �� EXCLUSIVE Ownership �� �����°�?                     */
/************************************************************************/
bool qosim_has_exclusive_ownership(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader)
{
	if (p_rtps_reader == NULL)
	{
		return false;
	}

	return is_exclusive_ownership(&p_rtps_reader->p_datareader->datareader_qos.ownership);
}


/************************************************************************/
/*  �� �Ķ���� p_guid_prefix_1 �� p_guid_prefix_2 �� ���� ���ΰ�?      */
/************************************************************************/
bool is_same_guid_prefix(const GuidPrefix_t* const p_guid_prefix_1, const GuidPrefix_t* const p_guid_prefix_2)
{
	if (p_guid_prefix_1 == NULL || p_guid_prefix_2 == NULL)
	{
		return false;
	}

	if (p_guid_prefix_1 == p_guid_prefix_2)
	{
		return true;
	}

	if (memcmp(&p_guid_prefix_1->_guidprefix, &p_guid_prefix_2->_guidprefix, sizeof(octet)*12) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* �� �Ķ���� p_entityid_1 �� p_entityid_2 �� ���� ���ΰ�?             */
/************************************************************************/
static bool is_same_entityid(const EntityId_t* const p_entityid_1, const EntityId_t* const p_entityid_2)
{
	if (p_entityid_1 == NULL || p_entityid_2 == NULL)
	{
		return false;
	}

	if (p_entityid_1 == p_entityid_2)
	{
		return true;
	}

	if (memcmp(&p_entityid_1->entity_key, &p_entityid_2->entity_key, sizeof(octet)*3) == 0
		&& p_entityid_1->entity_kind == p_entityid_2->entity_kind)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* �� �Ķ���� p_guid_1 �� p_guid_2 �� ���� ���ΰ�?                     */
/************************************************************************/
/*static bool is_same_guid(const GUID_t* const p_guid_1, const GUID_t* const p_guid_2)
{
	if (p_guid_1 == NULL || p_guid_2 == NULL)
	{
		return false;
	}

	if (p_guid_1 == p_guid_2)
	{
		return true;
	}

	if (is_same_guid_prefix(&p_guid_1->guid_prefix, &p_guid_2->guid_prefix)
		&& is_same_entityid(&p_guid_1->entity_id, &p_guid_2->entity_id))
	{
		return true;
	}
	else
	{
		return false;
	}
}*/


/************************************************************************/
/* RxO ���� ���� ���� �� ȣ���ϴ� �Լ�                                  */
/* ownership == exclusive �̸�,                                         */
/* �� writer �� ���� owner_writer �� ownership_strength ���� ū�� ��  */
/* Ŭ ���, �� writer �� owner_writer �� ��ü                           */
/************************************************************************/
void qosim_insert_exclusive_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, rtps_writerproxy_t* const p_rtps_writerproxy)
{
	rtps_exclusive_writer_t* p_exclusive_writer = NULL;

	if (!qosim_has_exclusive_ownership(p_this, p_rtps_reader))
	{
		return;
	}

	return;
	p_exclusive_writer = malloc(sizeof(rtps_exclusive_writer_t));
	memset(p_exclusive_writer, 0, sizeof(rtps_exclusive_writer_t));

	mutex_lock(&p_rtps_reader->object_lock);

	p_exclusive_writer->p_writerproxy = p_rtps_writerproxy;
	p_exclusive_writer->i_key = 0;
	p_exclusive_writer->b_has_key = false;
	p_exclusive_writer->b_is_owner = false;

	INSERT_ELEM(p_rtps_reader->pp_exclusive_writers, p_rtps_reader->i_exclusive_writers, p_rtps_reader->i_exclusive_writers, p_exclusive_writer);

	mutex_unlock(&p_rtps_reader->object_lock);
}


/************************************************************************/
/* <p_rtps_reader>�� exclusive_writer ��Ͽ��� <p_writer_guid>�� ����   */
/* exclusive_writer�� �ε����� ã�´�.                                  */
/* ������, -1�� �����Ѵ�.                                               */
/************************************************************************/
int32_t qosim_find_exclusive_writer(qos_policy_t* const p_this, const rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy)
{
	int32_t index;

	if (qosim_has_exclusive_ownership(p_this, p_rtps_reader) == false || p_rtps_writerproxy == NULL)
	{
		return -1;
	}

	for (index = 0; index < p_rtps_reader->i_exclusive_writers; index++)
	{
		if (p_rtps_reader->pp_exclusive_writers[index]->p_writerproxy == p_rtps_writerproxy)
		{
			return index;
		}
	}

	return -1;
}


/************************************************************************/
/* <i_key>�� <keylist>�� �ʱ�ȭ�Ѵ�.                                    */
/************************************************************************/
static void keylist_reset_key(uint32_t* i_key, cpskey_t keylist[])
{
	uint32_t index;

	for (index = 0; index < *i_key; index++)
	{
		keylist[index].offset = 0;
		keylist[index].len = 0;
		FREE(keylist[index].p_value);
	}
	*i_key = 0;
}


/************************************************************************/
/* <source_i_key>�� <source_keylist>�� <target_i_key>��                 */
/* <target_keylist>�� �����Ѵ�.                                         */
/************************************************************************/
static void keylist_copy_key(uint32_t* target_i_key, cpskey_t target_keylist[], const uint32_t source_i_key, const cpskey_t source_keylist[])
{
	uint32_t index;

	(*target_i_key) = source_i_key;

	for (index = 0; index < source_i_key; index++)
	{
		target_keylist[index].offset = source_keylist[index].offset;
		target_keylist[index].len = source_keylist[index].len;
		target_keylist[index].p_value = malloc(source_keylist[index].len);
		memset(target_keylist[index].p_value, 0, source_keylist[index].len);

		memcpy(target_keylist[index].p_value, source_keylist[index].p_value, source_keylist[index].len);
	}
}


/************************************************************************/
/* <p_exclusive_writer>�� Ű�� <p_message>�� Ű�� �����Ѵ�.             */
/************************************************************************/
static void exclusive_writer_set_key(rtps_exclusive_writer_t* const p_exclusive_writer, const message_t* const p_message)
{
	keylist_reset_key(&p_exclusive_writer->i_key, p_exclusive_writer->keylist);
	keylist_copy_key(&p_exclusive_writer->i_key, p_exclusive_writer->keylist, p_message->i_key, p_message->keylist);
	p_exclusive_writer->b_has_key = true;
}


/************************************************************************/
/* <i_key_1>�� <keylist_1>�ְ� <i_key_2>�� <keylist_2>���� ������       */
/* ���Ѵ�.                                                            */
/************************************************************************/
static bool keylist_has_same_key(const uint32_t i_key_1, const cpskey_t keylist_1[], const uint32_t i_key_2, const cpskey_t keylist_2[])
{
	uint32_t index;

	if (i_key_1 != i_key_2)
	{
		return false;
	}

	for (index = 0; index < i_key_1; index++)
	{
		if (keylist_1[index].len != keylist_2[index].len ||
			memcmp(keylist_1[index].p_value, keylist_2[index].p_value, keylist_1[index].len))
		{
			return false;
		}
	}

	return true;
}


/************************************************************************/
/* <p_exclusive_writer_1>�� <p_exclusive_writer_2>�� ���� Ű�� �������� */
/* ���Ѵ�.                                                            */
/************************************************************************/
static bool exclusive_writer_has_same_key(const rtps_exclusive_writer_t* const p_exclusive_writer_1, const rtps_exclusive_writer_t* const p_exclusive_writer_2)
{
	return keylist_has_same_key(p_exclusive_writer_1->i_key, p_exclusive_writer_1->keylist, p_exclusive_writer_2->i_key, p_exclusive_writer_2->keylist);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
static bool stronger_than(const rtps_exclusive_writer_t* const p_exclusive_writer, const int32_t before_ownership_strength)
{
	return (p_exclusive_writer->p_writerproxy->ownership_strength >= before_ownership_strength && 
			p_exclusive_writer->p_writerproxy->b_liveliness_alive == true &&
			p_exclusive_writer->p_writerproxy->b_is_deadline_on == true);
}


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
int32_t qosim_set_exclusive_writer_key(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const SerializedPayloadForReader* const p_serialized)
{
	int32_t index;
	message_t* p_message;
	int i;
	bool b_same_key_exist = false;

	if ((index = qosim_find_exclusive_writer(p_this, p_rtps_reader, p_rtps_writerproxy)) > -1)
	{
		rtps_exclusive_writer_t* p_exclusive_writer = p_rtps_reader->pp_exclusive_writers[index];
		if (p_exclusive_writer->b_has_key == false)
		{
			p_message = message_new_for_builtin_reader(p_rtps_reader->p_datareader, &p_serialized->p_value[4], p_serialized->i_size);
			exclusive_writer_set_key(p_exclusive_writer, p_message);

			// ������ owner�� strength�� ���Ͽ� �� ���� live �����̸� owner�� �ٲ۴�.
			for (i = 0; i < p_rtps_reader->i_exclusive_writers; i++)
			{
				if (i != index && exclusive_writer_has_same_key(p_rtps_reader->pp_exclusive_writers[i], p_exclusive_writer))
				{
					b_same_key_exist = true;
					if (p_rtps_reader->pp_exclusive_writers[i]->b_is_owner == true && 
						stronger_than(p_exclusive_writer, p_rtps_reader->pp_exclusive_writers[i]->p_writerproxy->ownership_strength))
					{
						p_exclusive_writer->b_is_owner = true;
						p_rtps_reader->pp_exclusive_writers[i]->b_is_owner = false;
						break;
					}
				}
			}
			if (b_same_key_exist == false)
			{
				p_exclusive_writer->b_is_owner = true;
			}
		}
	}

	return index;
}


/************************************************************************/
/************************************************************************/
void change_owner_writer(rtps_reader_t* const p_rtps_reader, rtps_exclusive_writer_t* const p_exclusive_writer, const bool b_is_include)
{
	int32_t i;
	uint32_t backup_i_key;
	cpskey_t backup_keylist[MAX_KEY_NUM];
	long temp_ownership_strength = -1;
	rtps_exclusive_writer_t* p_temp_exclusive_writer = NULL;

	keylist_copy_key(&backup_i_key, backup_keylist, p_exclusive_writer->i_key, p_exclusive_writer->keylist);

	for (i = 0; i < p_rtps_reader->i_exclusive_writers; i++)
	{
		if ((b_is_include || p_rtps_reader->pp_exclusive_writers[i] != p_exclusive_writer) && 
			keylist_has_same_key(backup_i_key, backup_keylist, p_rtps_reader->pp_exclusive_writers[i]->i_key, p_rtps_reader->pp_exclusive_writers[i]->keylist) &&
			stronger_than(p_rtps_reader->pp_exclusive_writers[i], temp_ownership_strength))
		{
			temp_ownership_strength = p_rtps_reader->pp_exclusive_writers[i]->p_writerproxy->ownership_strength;
			p_temp_exclusive_writer = p_rtps_reader->pp_exclusive_writers[i];
		}
	}

	if (p_temp_exclusive_writer)
	{
		p_exclusive_writer->b_is_owner = false;
		p_temp_exclusive_writer->b_is_owner = true;
	}
}


/************************************************************************/
/* �Ķ���ͷ� ���� p_rtps_writerproxy�� �����ϰ� ���ο� owner�� ����. */
/* p_rtps_writerproxy�� �ε��� ���� �����Ѵ�.                           */
/************************************************************************/
int qosim_change_owner_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const bool b_is_include)
{
	int32_t index;

	mutex_lock(&p_rtps_reader->object_lock);

	if ((index = qosim_find_exclusive_writer(p_this, p_rtps_reader, p_rtps_writerproxy)) > -1)
	{
		if (p_rtps_reader->pp_exclusive_writers[index]->b_is_owner == true)
		{
			change_owner_writer(p_rtps_reader, p_rtps_reader->pp_exclusive_writers[index], b_is_include);
		}
	}

	mutex_unlock(&p_rtps_reader->object_lock);

	return index;
}


/************************************************************************/
/* exclusive_writer�� �����Ѵ�.                                         */
/* ������ excluseive_writer�� owner_writer �� ��쿣 ���ο�             */
/* owner_writer�� �����Ѵ�.                                             */
/************************************************************************/
void qosim_remove_exclusive_writer(qos_policy_t* const p_this, rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy)
{
	int32_t index = qosim_change_owner_writer(p_this, p_rtps_reader, p_rtps_writerproxy, false);

	if (index > -1)
	{
		mutex_lock(&p_rtps_reader->object_lock);

		FREE(p_rtps_reader->pp_exclusive_writers[index]);
		REMOVE_ELEM(p_rtps_reader->pp_exclusive_writers, p_rtps_reader->i_exclusive_writers, index);

		mutex_unlock(&p_rtps_reader->object_lock);
	}
}
