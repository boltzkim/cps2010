#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include "../../../src/modules/rtps/rtps.h"


extern void qosim_dcps_message_ordering(message_t **pp_message, int i_count);

instanceset_t *instaceset_new(message_t* p_message, uint32_t depth, uint32_t max_samples_per_instance)
{
	uint32_t i;
	instanceset_t* p_instaceset = (instanceset_t*)malloc(sizeof(instanceset_t));
	memset(p_instaceset, '\0', sizeof(instanceset_t));
	
	p_instaceset->i_key = p_message->i_key;

	for (i = 0; i < p_instaceset->i_key; i++)
	{
		p_instaceset->keylist[i].len = p_message->keylist[i].len;
		p_instaceset->keylist[i].p_value = malloc(p_message->keylist[i].len);
		memset(p_instaceset->keylist[i].p_value, '\0', p_message->keylist[i].len);

		memcpy(p_instaceset->keylist[i].p_value, p_message->keylist[i].p_value, p_message->keylist[i].len);

	}

	p_instaceset->max_samples_per_instance = max_samples_per_instance;
	p_instaceset->i_history_depth = depth;
	p_instaceset->i_messages = 0;
	p_instaceset->pp_messages = NULL;


	p_instaceset->i_key_guid = 0;
	p_instaceset->pp_key_guid = NULL;

//ignore by jun
	p_instaceset->ignore_id = 0;

	//readcondition by jun	
	p_instaceset->instance_state = ALIVE_INSTANCE_STATE;
	p_instaceset->view_state = NEW_VIEW_STATE;
	p_instaceset->sample_state = NOT_READ_SAMPLE_STATE;


	p_instaceset->register_instance_timestamp_set = false;
	p_instaceset->register_instance_timestamp_value.sec = 0;
	p_instaceset->register_instance_timestamp_value.nanosec = 0;
	
	return p_instaceset;
}


extern void rtps_cachechange_destory(rtps_cachechange_t* p_rtps_cachechange);
extern void rtps_cachechange_ref(rtps_cachechange_t* p_rtps_cachechange, bool isrtps, bool isref);
void remove_in_order(Entity* p_entity, message_t* p_message)
{
	uint32_t i;
	DataReader* p_datareader = NULL;
	DataWriter* p_datawriter = NULL;
	uint32_t size = 0;
	bool removed_order = false;

	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;
		size = p_datawriter->i_message_order;

		for (i = 0; i < size; i++)
		{
			if (p_datawriter->pp_message_order[i] == p_message)
			{
				REMOVE_ELEM(p_datawriter->pp_message_order, p_datawriter->i_message_order, i);
				break;
			}
		}

	}
	else if(p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader*) p_entity;
		size = p_datareader->i_message_order;

		for (i = 0; i < size; i++)
		{
			if (p_datareader->pp_message_order[i] == p_message)
			{
				REMOVE_ELEM(p_datareader->pp_message_order, p_datareader->i_message_order, i);
				removed_order = true;
				break;
			}
		}
	}

	if (p_entity->i_entity_type == DATAREADER_ENTITY && removed_order == false)
	{
		removed_order = false;
	}
}


void instanceset_remove(Entity* p_entity, instanceset_t* p_instaceset)
{
	uint32_t i;

	while (p_instaceset->i_messages)
	{		
		remove_in_order(p_entity, p_instaceset->pp_messages[0]);
		rtps_cachechange_ref((rtps_cachechange_t*)p_instaceset->pp_messages[0]->v_related_cachechange, false, false);
		rtps_cachechange_destory((rtps_cachechange_t*)p_instaceset->pp_messages[0]->v_related_cachechange);
		REMOVE_ELEM(p_instaceset->pp_messages, p_instaceset->i_messages, 0);
	}

	for (i = 0; i < p_instaceset->i_key; i++)
	{
		FREE(p_instaceset->keylist[i].p_value);
	}

	while (p_instaceset->i_key_guid)
	{
		FREE(p_instaceset->pp_key_guid[0]);
		REMOVE_ELEM(p_instaceset->pp_key_guid, p_instaceset->i_key_guid, 0);
	}

	FREE(p_instaceset->pp_messages);
	FREE(p_instaceset);
}


extern void register_instance_timestamp_set_to_cache_for_writer(void *related_cache, Time_t value);
void add_to_instanceset(Entity* p_entity, instanceset_t* p_instanceset, message_t* p_message)
{
	int i;
	DataReader* p_datareader = NULL;
	DataWriter* p_datawriter = NULL;
	ReliabilityQosPolicyKind reliablity_kind = BEST_EFFORT_RELIABILITY_QOS;
	rtps_reader_t* p_rtps_reader =NULL;

	p_message->sampleInfo.instance_handle = (InstanceHandle_t)p_instanceset;
	//readcondition by jun
	p_message->sampleInfo.instance_state = ALIVE_INSTANCE_STATE;
	p_message->sampleInfo.sample_state = NOT_READ_SAMPLE_STATE;
	p_message->sampleInfo.view_state = NEW_VIEW_STATE;
	p_message->sampleInfo.valid_data = true;
//	p_message->sampleInfo.source_timestamp = p_message->current_time;


	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;
		reliablity_kind = p_datawriter->datawriter_qos.reliability.kind;

		if (p_instanceset->register_instance_timestamp_set == true)
		{
			if (p_message != NULL && p_message->v_related_cachechange != NULL)
			{
				register_instance_timestamp_set_to_cache_for_writer(p_message->v_related_cachechange, p_instanceset->register_instance_timestamp_value);
			}
		}
	}
	else if (p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader *) p_entity;
//ignore by jun

		p_rtps_reader = (rtps_reader_t *) p_datareader->p_related_rtps_reader;
		if (!memcmp(&p_rtps_reader->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)))
		{
			p_message->sampleInfo.instance_handle = (InstanceHandle_t)p_instanceset + p_instanceset->ignore_id++;
//			printf(" PDPPDP ->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       %d \n",(InstanceHandle_t)p_instanceset + p_instanceset->ignore_id );
		}

		if (!memcmp(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)))
		{
			p_message->sampleInfo.instance_handle = (InstanceHandle_t)p_instanceset + p_instanceset->ignore_id++;
//			printf(" EDPEDP ->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       %d \n",(InstanceHandle_t)p_instanceset + p_instanceset->ignore_id );
		}

		if (!memcmp(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)))
		{
			p_message->sampleInfo.instance_handle = (InstanceHandle_t)p_instanceset + p_instanceset->ignore_id++;
//			printf(" EDPEDP ->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       %d \n",(InstanceHandle_t)p_instanceset + p_instanceset->ignore_id );
		}
		
		if (!memcmp(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)))
		{
			p_message->sampleInfo.instance_handle = (InstanceHandle_t)p_instanceset + p_instanceset->ignore_id++;
//			printf(" EDPEDP ->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 	  %d \n",(InstanceHandle_t)p_instanceset + p_instanceset->ignore_id );
		}

		reliablity_kind = p_datareader->datareader_qos.reliability.kind;
	}

	//[KKI] instanceset 의 max_samples_per_instance 보다 메시지를 더 많이 가지고 있으면 삭제한다.
	if (p_instanceset->i_messages >= p_instanceset->max_samples_per_instance  && p_instanceset->max_samples_per_instance != LENGTH_UNLIMITED)
	{
		// [KKI] 우선 READ_SAMPLE_STATE 인 것부터 삭제한다.
	

		int i_size = p_instanceset->i_messages;

		for (i = i_size - 1; i >= 0 && p_instanceset->i_messages >= p_instanceset->max_samples_per_instance; i--) 
		{

			if (p_entity->i_entity_type == DATAWRITER_ENTITY || p_instanceset->pp_messages[i]->sampleInfo.sample_state == READ_SAMPLE_STATE)
			{
				//remove cache
				remove_in_order(p_entity, p_instanceset->pp_messages[i]);
				rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange, false, false);
				rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange);

				REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
	
			}
		}

		//[KKI] 아직도 max_samples_per_instance 보다 많은데 BEST EFFORT 이면 첫번째 메시지를 삭제한다.
		if (reliablity_kind == BEST_EFFORT_RELIABILITY_QOS && p_instanceset->i_messages > p_instanceset->max_samples_per_instance)
		{
			p_instanceset->i_history_depth = p_instanceset->i_history_depth;
			remove_in_order(p_entity, p_instanceset->pp_messages[0]);
			rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[0]->v_related_cachechange, false, false);
			rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[0]->v_related_cachechange);

			REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, 0);
		}
	}
	//[KKI] instanceset 의 i_history_depth 보다 메시지를 더 많이 가지고 있으면 삭제한다.
	else if (p_instanceset->i_messages >= p_instanceset->i_history_depth && p_instanceset->max_samples_per_instance == LENGTH_UNLIMITED)
	{
		//[KKI] 우선 READ_SAMPLE_STATE 인 것부터 삭제한다.

		int i_size = p_instanceset->i_messages;


		//printf("1 remove_in_order[%d,%d] : %p\r\n", 0, p_instanceset->i_messages, p_instanceset->pp_messages[0]);

		for (i = i_size - 1; i >= 0 && p_instanceset->i_messages >= p_instanceset->i_history_depth; i--)
		{
			if (p_instanceset->pp_messages[i]->sampleInfo.sample_state == READ_SAMPLE_STATE)
			{
				//remove cache
				//printf("remove_in_order[%d,%d] : %p\r\n", p_instanceset->i_messages, i, p_instanceset->pp_messages[i]);
				remove_in_order(p_entity, p_instanceset->pp_messages[i]);
				rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange, false, false);
				rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange);

				REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
	
			}else if (p_entity->i_entity_type == DATAWRITER_ENTITY)
			{
				//remove cache
				remove_in_order(p_entity, p_instanceset->pp_messages[i]);
				rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange, false, false);
				rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[i]->v_related_cachechange);

				REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
			}
		}

		//[KKI] 아직도 i_history_depth 보다 많은데 BEST_EFFORT 이면 첫번째 메시지를 삭제한다.
		if (reliablity_kind == BEST_EFFORT_RELIABILITY_QOS && p_instanceset->i_messages > p_instanceset->i_history_depth)
		{
			p_instanceset->i_history_depth = p_instanceset->i_history_depth;
			remove_in_order(p_entity, p_instanceset->pp_messages[0]);
			rtps_cachechange_ref((rtps_cachechange_t *)p_instanceset->pp_messages[0]->v_related_cachechange, false, false);
			rtps_cachechange_destory((rtps_cachechange_t *)p_instanceset->pp_messages[0]->v_related_cachechange);

			REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, 0);
		}
	}

	if (p_message->handle == 0 && p_instanceset->i_key != 0)
	{
		p_message->handle = (InstanceHandle_t)p_instanceset;
	}
	else if (p_message->handle == 0)
	{
		p_message->handle = (InstanceHandle_t)p_instanceset;
	}

	rtps_cachechange_ref((rtps_cachechange_t *)p_message->v_related_cachechange, false, true);

	//[KKI] instanceset 에 메시지의 key_hash 가 존재하는지 확인하고, 없으면 instanceset 에 key_hash 를 추가한다.
	{
		bool b_is_found_key_hash = false;

		for (i = 0; i < p_instanceset->i_key_guid; i++)
		{
			if (memcmp(p_instanceset->pp_key_guid[i], p_message->key_guid, 16) == 0)
			{
				b_is_found_key_hash = true;
				break;
			}
		}

		if (b_is_found_key_hash == false)
		{
			key_guid_t* p_key_guid = malloc(16);
			memcpy(p_key_guid, p_message->key_guid, 16);
			if (p_key_guid->key[0] != 0)
			{
				p_key_guid->key[0] = p_key_guid->key[0];
			}

			INSERT_ELEM(p_instanceset->pp_key_guid, p_instanceset->i_key_guid,  p_instanceset->i_key_guid, p_key_guid);
		}
	}

	//[KKI] instanceset 에 메시지를 추가한다.
	INSERT_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, p_instanceset->i_messages, p_message);

	//printf("p_instanceset->i_messages : %d:%p\r\n", p_instanceset->i_messages-1, p_message);

	if (p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		//readcondtion by jun
		//	sample
		check_new_data_sample_status(p_entity, p_instanceset,p_message);

		//	instance	
		check_new_data_instance_stautus(p_entity, p_instanceset,p_message);

		//	view
		check_newData_viewStautus(p_entity, p_instanceset, p_message);
	}
}


bool cpskey__is_same(const cpskey_t key_1, const cpskey_t key_2)
{
	return (key_1.offset == key_2.offset && key_1.len == key_2.len && memcmp(key_1.p_value, key_2.p_value, key_1.len) == 0);
}


static module_t* p_static_module = NULL;

void insert_instanceset_datareader(DataReader* p_datareader, rtps_cachechange_t* p_rtps_cachechange, message_t* p_message)
{
	uint32_t i_size = p_datareader->i_instanceset;
	uint32_t i2, j;
	bool found_key_instanceset = false;
	module_t* p_module;
	uint32_t i_found_key_num;
	
	if (p_static_module == NULL)
	{
		p_module = p_static_module = current_object(get_domain_participant_factory_module_id());
	}
	else
	{
		p_module = p_static_module;
	}

	//pp_message_order queue에 p_message 집어 넣음
	INSERT_ELEM(p_datareader->pp_message_order, p_datareader->i_message_order, p_datareader->i_message_order, p_message);

	//added by kyy(Destination Order)
	//이후 pp_message_order를 Destination Order에 따라 ordering 해야 함
/*	if (p_datareader->datareader_qos.destination_order.kind == BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)
	{
		qosim_dcps_message_ordering_change(p_datareader);
	}
*/
	//[KKI] key 가 있는 경우, 해당 instanceset 을 찾아서 메시지를 삽입한다.
	if (p_message->i_key)
	{
		for (i2 = 0; i2 < i_size; i2++)
		{
			if (p_datareader->pp_instanceset[i2]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j], p_datareader->pp_instanceset[i2]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{
				//기존의 instanceset에 넣기
				add_to_instanceset((Entity*)p_datareader, p_datareader->pp_instanceset[i2], p_message);
				found_key_instanceset = true;
				i_found_key_num = i2;
				break;
			}
		}
	}

	//[KKI] key 가 없고 instanceset 이 1개이면 그 instanceset 에 추가한다.
	if (p_message->i_key == 0 && i_size == 1)
	{
		add_to_instanceset((Entity*)p_datareader, p_datareader->pp_instanceset[0], p_message);
	}
	else
	{
		//[KKI] instanceset 에서 해당 key 를 못찾았거나, 메시지의 key 가 없으면서 instanceset 도 없는 경우 새로운 instanceset 을 생성하고 추가한다.
		if (found_key_instanceset == false || (p_message->i_key == 0 && i_size == 0))
		{
			instanceset_t* p_instanceset = NULL;

			//[KKI] instanceset 개수가 max_instaces 를 넘으면 에러 출력
			if (p_datareader->i_instanceset > p_datareader->datareader_qos.resource_limits.max_instances && p_datareader->datareader_qos.resource_limits.max_instances != LENGTH_UNLIMITED)
			{
				//여기에서 SampleRejectedStatus 를 처리해야 함.
				InstanceHandle_t last_instance_handle; // 마지막 거부된 샘플이 업데이트하는 인스턴스에 대한 핸들.
				last_instance_handle = 0; // 거부된 샘플과 관련된 인스턴스가 존재하지 않기 때문에 0
				change_SampleRejected_status(p_datareader, REJECTED_BY_INSTANCES_LIMIT, last_instance_handle);

				trace_msg(OBJECT(p_module), TRACE_ERROR, "max_instances is over.");
			}
			else
			{
				p_instanceset = instaceset_new(p_message, p_datareader->datareader_qos.history.depth, p_datareader->datareader_qos.resource_limits.max_samples_per_instance);
				INSERT_ELEM(p_datareader->pp_instanceset, p_datareader->i_instanceset, p_datareader->i_instanceset, p_instanceset);
			}

			if (p_instanceset) add_to_instanceset((Entity*)p_datareader, p_instanceset, p_message);
		}
		else {
			//add_to_instanceset((Entity*)p_datareader, p_datareader->pp_instanceset[i_found_key_num], p_message);
		}
	}
}


void insert_instanceset_datawriter(DataWriter* p_datawriter, rtps_cachechange_t* p_rtps_cachechange, message_t* p_message)
{
	uint32_t i_size = p_datawriter->i_instanceset;
	uint32_t i, j;
	bool found_key_instanceset = false;
	module_t* p_module;

	if (p_static_module == NULL)
	{
		p_module = p_static_module = current_object(get_domain_participant_factory_module_id());
	}
	else
	{
		p_module = p_static_module;
	}

	INSERT_ELEM(p_datawriter->pp_message_order, p_datawriter->i_message_order, p_datawriter->i_message_order, p_message);

	//[KKI] key 가 있는 경우, 해당 instanceset 을 찾아서 메시지를 삽입한다.
	if (p_message->i_key)
	{
		for (i = 0; i < i_size; i++)
		{
			if (p_datawriter->pp_instanceset[i]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j],  p_datawriter->pp_instanceset[i]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{
				//기존의 instanceset에 넣기
				add_to_instanceset((Entity*)p_datawriter, p_datawriter->pp_instanceset[i], p_message);
				found_key_instanceset = true;
				break;
			}
		}
	}

	//[KKI] key 가 없고 instanceset 이 1개이면 그 instanceset 에 추가한다.
	if (p_message->i_key == 0 && i_size == 1)
	{
		add_to_instanceset((Entity*)p_datawriter, p_datawriter->pp_instanceset[0], p_message);
	}
	else
	{
		//[KKI] instanceset 에서 해당 key 를 못찾았거나,  메시지의 key 가 없으면서 instanceset 도 없는 경우
		if (found_key_instanceset == false || (p_message->i_key == 0 && i_size == 0))
		{
			instanceset_t* p_instanceset = NULL;

			//[KKI] instanceset 개수가 max_instaces 를 넘으면 에러 출력
			if (p_datawriter->i_instanceset > p_datawriter->datawriter_qos.resource_limits.max_instances && p_datawriter->datawriter_qos.resource_limits.max_instances != LENGTH_UNLIMITED)
			{
				trace_msg(OBJECT(p_module), TRACE_ERROR, "max_instances is over.");
			}
			else
			{
				p_instanceset = instaceset_new(p_message, p_datawriter->datawriter_qos.history.depth, p_datawriter->datawriter_qos.resource_limits.max_samples_per_instance);
				INSERT_ELEM(p_datawriter->pp_instanceset, p_datawriter->i_instanceset, p_datawriter->i_instanceset, p_instanceset);
			}

			if (p_instanceset) add_to_instanceset((Entity*)p_datawriter, p_instanceset, p_message);
		}
	}

	//if(p_rtps_cachechange->handle == 0)
	//{
	//	p_rtps_cachechange->handle = p_message->handle;
	//}
}


typedef uint8_t	octet;
/*
typedef struct	GuidPrefix_t
{
	octet _guidprefix[12];
}GuidPrefix_t;

typedef struct	EntityId_t
{
	octet	entityKey[3];
	octet	entityKind;
}EntityId_t;

typedef  struct GUID_t{
	GuidPrefix_t guidPrefix;
	EntityId_t	 entityId;
}GUID_t;
*/

void instanceset_dispose(Entity* p_entity, GUID_t key_guid)
{
	DataReader* p_datareader = NULL;
	DataWriter* p_datawriter = NULL;
	int i, j;

	instanceset_t *p_instanceset = NULL;

	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;

		for (i = 0; i < p_datawriter->i_instanceset; i++)
		{
			for (j = 0; j < p_datawriter->pp_instanceset[i]->i_key_guid; j++)
			{
				if (memcmp(p_datawriter->pp_instanceset[i]->pp_key_guid[j], &key_guid, 16)!=0)
				{
					p_instanceset = p_datawriter->pp_instanceset[i];
					break;
				}
			}
		}
	}
	else if (p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader *) p_entity;

		for (i = 0; i < p_datareader->i_instanceset; i++)
		{
			for (j = 0; j < p_datareader->pp_instanceset[i]->i_key_guid; j++)
			{
				if (memcmp(p_datareader->pp_instanceset[i]->pp_key_guid[j], &key_guid, 16)!=0)
				{
					p_instanceset = p_datareader->pp_instanceset[i];
					break;
				}
			}
		}

		//readcondition by jun
		if (p_instanceset != NULL)
		{
			check_instanceStatus_disposed(p_entity, p_instanceset);
		}
	}

	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;
	}
	else if(p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader *) p_entity;

		//readcondition by jun
		if (p_instanceset != NULL)
		{
			check_instanceStatus_disposed(p_entity, p_instanceset);
		}

		//for (i = 0; i < p_datareader->i_message_order; i++)
		//{
		//	if (memcmp(p_datareader->pp_message_order[i]->key_guid, &key_guid, sizeof(GUID_t)) == 0)
		//	{
		//		p_datareader->pp_message_order[i]->sampleInfo.instance_state = NOT_ALIVE_DISPOSED_INSTANCE_STATE;
		//	}
		//}
	}
}


void instanceset_unregister(Entity* p_entity, GUID_t key_guid)
{
	DataReader* p_datareader = NULL;
	DataWriter* p_datawriter = NULL;
	int i, j;

	instanceset_t* p_instanceset = NULL;

	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;

		for (i = 0; i < p_datawriter->i_instanceset; i++)
		{
			for (j = 0; j < p_datawriter->pp_instanceset[i]->i_key_guid; j++)
			{
				if (memcmp(p_datawriter->pp_instanceset[i]->pp_key_guid[j], &key_guid, 16) != 0)
				{
					p_instanceset = p_datawriter->pp_instanceset[i];
					break;
				}
			}
		}
	}
	else if(p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader *) p_entity;

		for (i = 0; i < p_datareader->i_instanceset; i++)
		{
			for (j = 0; j < p_datareader->pp_instanceset[i]->i_key_guid; j++)
			{
				if (memcmp(p_datareader->pp_instanceset[i]->pp_key_guid[j], &key_guid, 16) != 0)
				{
					//readcondition by jun
					p_instanceset = p_datareader->pp_instanceset[i];
					REMOVE_ELEM(p_datareader->pp_instanceset[i]->pp_key_guid, p_datareader->pp_instanceset[i]->i_key_guid, j);
					check_instanceStatus_unregister(p_entity, p_instanceset);
					break;
				}
			}
		}
	}

	if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_datawriter = (DataWriter *) p_entity;
	}
	else if(p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_datareader = (DataReader *) p_entity;

		/*for (i = 0; i < p_datareader->i_message_order; i++)
		{
			if (memcmp(p_datareader->pp_message_order[i]->key_guid, &key_guid, sizeof(GUID_t)) == 0)
			{
				p_datareader->pp_message_order[i]->sampleInfo.instance_state = NOT_ALIVE_NO_WRITERS_INSTANCE_STATE;
			}
		}*/
	}
}


void remove_in_instanceset(message_t* p_message)
{
	int i;
	instanceset_t* p_instanceset = (instanceset_t*)p_message->handle;

	for (i = 0; i < p_instanceset->i_messages; i++)
	{
		if (p_instanceset->pp_messages[i] == p_message)
		{
			REMOVE_ELEM(p_instanceset->pp_messages, p_instanceset->i_messages, i);
			break;
		}
	}
}
