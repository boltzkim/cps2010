/*
	RTSP Topic class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"


bool Check_to_topic(Topic *p_topic, SerializedPayload	*data_value);


static uint8_t entity_id_check = 0;
static mutex_t				structure_lock;
static bool structure_lock_initialize = false;

static uint8_t Default_Topic_Entity_id_get()
{
	int ret;
	if(!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	mutex_lock(&structure_lock);
	ret = ++entity_id_check;
	mutex_unlock(&structure_lock);
	
	return ret;
}


static SerializedPayloadForWriter *rtps_generate_topicInfo(Topic *p_topic, rtps_writer_t *p_writer)
{
	int i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
//	ProtocolVersion_t	protocol = p_rtps_participant->protocolVersion;

	uint32_t endpoint_set = 0x00000000;
	char *p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;
	char *p_temp;
	//TopicDescription *p_topicDescription = NULL;
	rtps_topic_t *p_rtps_topic = (rtps_topic_t *)p_topic->p_rtps_topic;

	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;
	i_size += 2;
	//option skip 2byte
	i_size +=2;
	//make ParameterList

	

	memcpy(&p_serialized_data->entity_key_guid, &p_rtps_topic->guid, sizeof(GUID_t));


	
	//PID_ENDPOINT_GUID
	gererate_parameter(p_serialized+i_size, &i_size, PID_ENDPOINT_GUID, 16, &p_rtps_topic->guid);

	//PID_TOPIC_NAME
	if(p_topic->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_topic;
		p_temp = get_string(p_contentFilteredTopic->get_related_topic(p_contentFilteredTopic)->topic_name, &i_temp);
	}else if(p_topic->topic_type == MULTITOPIC_TYPE)
	{
		assert(false);
	}else{
		p_temp = get_string(p_topic->topic_name, &i_temp);
	}

	
	
	gererate_parameter(p_serialized+i_size, &i_size, PID_TOPIC_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_TYPE_NAME
	p_temp = get_string(p_topic->type_name, &i_temp);
	
	gererate_parameter(p_serialized+i_size, &i_size, PID_TYPE_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_DURABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_DURABILITY, sizeof(DurabilityQosPolicy), &p_topic->topic_qos.durability);

	//PID_DEADINE
	gererate_parameter(p_serialized+i_size, &i_size, PID_DEADINE, sizeof(DeadlineQosPolicy), &p_topic->topic_qos.deadline);

	//PID_LATENCY_BUDGET
	gererate_parameter(p_serialized+i_size, &i_size, PID_LATENCY_BUDGET, sizeof(LatencyBudgetQosPolicy), &p_topic->topic_qos.latency_budget);

	//PID_LIVELINESS
	gererate_parameter(p_serialized+i_size, &i_size, PID_LIVELINESS, sizeof(LivelinessQosPolicy), &p_topic->topic_qos.liveliness);

	//PID_RELIABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_RELIABILITY, sizeof(ReliabilityQosPolicy), &p_topic->topic_qos.reliability);

	//PID_OWNERSHIP
	gererate_parameter(p_serialized+i_size, &i_size, PID_OWNERSHIP, sizeof(OwnershipQosPolicy), &p_topic->topic_qos.ownership);

	//added by kyy(Transport Priority)
	//PID_TRANSPORT_PRIORITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_TRANSPORT_PRIORITY, sizeof(TransportPriorityQosPolicy), &p_topic->topic_qos.transport_priority.value);

	
	//added by kyy(TopicData QoS)
	//PID_TOPIC_DATA///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		if(p_topic->topic_qos.topic_data.value.i_string > 0){
			int i;
			char topicdata[1024];
			int32_t	size=0;
			int32_t	i_len=0;
			int32_t j=0;
			int32_t i_tempstring=0;
			i_len = sizeof(int32_t);
			

			memset(topicdata, 0, 1024);

			for(i=0; i < p_topic->topic_qos.topic_data.value.i_string; i++)
			{
				size += strlen(p_topic->topic_qos.topic_data.value.pp_string[i]);
			}

			memcpy(topicdata, &size, i_len);
			//printf("Size = %d \n",size);
									
			for(j=0; j < p_topic->topic_qos.topic_data.value.i_string; j++)
			{
				memcpy(topicdata+i_len, p_topic->topic_qos.topic_data.value.pp_string[j], strlen(p_topic->topic_qos.topic_data.value.pp_string[j]));
				i_len += strlen(p_topic->topic_qos.topic_data.value.pp_string[j]);
			}

			gererate_parameter(p_serialized+i_size, &i_size, PID_TOPIC_DATA, i_len, topicdata);
		}
		
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	//PID_EXPECTS_INLINE_QOS
	gererate_parameter(p_serialized+i_size, &i_size, PID_EXPECTS_INLINE_QOS, 4, p_serialized+i_size+10);


	//PID_PROTOCOL_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PROTOCOL_VERSION, 4, &protocol);

	//PID_VENDORID
	gererate_parameter(p_serialized+i_size, &i_size, PID_VENDORID, 4, &p_writer->p_rtps_participant->vendor_id);

	//PID_PRODUCT_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PRODUCT_VERSION, 4, &product_version);




	//PID_SENTINEL
	gererate_parameter(p_serialized+i_size, &i_size, PID_SENTINEL, 0, NULL);

	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}else{

		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;
}


static SerializedPayloadForWriter *rtps_generate_topicInfo_remove(Topic *p_topic, rtps_writer_t *p_writer)
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
//	ProtocolVersion_t	protocol = p_rtps_participant->protocolVersion;

	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;
	TopicDescription *p_topicDescription = NULL;
	rtps_topic_t *p_rtps_topic = (rtps_topic_t *)p_topic->p_rtps_topic;

	char status_info[] = {0x00,0x00,0x00,0x03};
	
	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;

	//PID_KEY_HASH
	gererate_parameter(p_serialized+i_size, &i_size, PID_KEY_HASH, 16, &p_rtps_topic->guid);

	//PID_STATUS_INFO
	gererate_parameter(p_serialized+i_size, &i_size, PID_STATUS_INFO, 4, status_info);



	//PID_SENTINEL
	gererate_parameter(p_serialized+i_size, &i_size, PID_SENTINEL, 0, NULL);

	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}else{

		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;
}

void rtps_added_user_defined_Topic(Topic *p_topic)
{
	DataWriter *p_bultindataWriter = NULL;
	SerializedPayloadForWriter *p_serializedData = NULL;

	rtps_statefulwriter_t *p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_topic_writer();
	if(p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
	p_bultindataWriter = p_statefulwriter->p_datawriter;
	p_serializedData = rtps_generate_topicInfo(p_topic, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin((rtps_writer_t *)p_statefulwriter, p_serializedData, (InstanceHandle_t)p_serializedData);

	

}

void rtps_remove_user_defined_Topic(Topic *p_topic)
{
	DataWriter *p_bultindataWriter = NULL;
	SerializedPayloadForWriter *p_serializedData = NULL;
	
	rtps_statefulwriter_t *p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_topic_writer();
	if (p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
	p_bultindataWriter = p_statefulwriter->p_datawriter;
	p_serializedData = rtps_generate_topicInfo_remove(p_topic, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin_rapidly((rtps_writer_t *)p_statefulwriter, p_serializedData, (InstanceHandle_t)p_serializedData);
}


void rtps_topic_init(rtps_topic_t *p_rtps_topic)
{
	init_rtps_endpoint((rtps_endpoint_t *)p_rtps_topic);
	p_rtps_topic->i_entity_type = RTPS_TOPIC_ENTITY;
}


rtps_topic_t *rtps_topic_new()
{
	rtps_topic_t *p_rtps_topic = malloc(sizeof(rtps_topic_t));
	memset(p_rtps_topic, 0, sizeof(rtps_topic_t));
	rtps_topic_init(p_rtps_topic);
	return p_rtps_topic;
}

void rtps_topic_remove(rtps_topic_t *p_rtps_topic)
{
	destroy_endpoint((rtps_endpoint_t *)p_rtps_topic);
	FREE(p_rtps_topic);
}

int32_t rtps_add_topic( module_object_t* p_this, Topic* p_topic )
{

	rtps_reader_t *p_rtpsBultinReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();
	rtps_cachechange_t *p_change_atom = NULL;

	EntityId_t a_userEntityId;
	rtps_topic_t *p_rtps_topic = rtps_topic_new();
	p_rtps_topic->p_topic = p_topic;
	p_topic->p_rtps_topic = p_rtps_topic;


	a_userEntityId.entity_key[0] = 0;
	a_userEntityId.entity_key[1] = 0;
	a_userEntityId.entity_key[2] = Default_Topic_Entity_id_get();;
	a_userEntityId.entity_kind = KIND_OF_ENTITY_WRITER_WITH_KEY_USER_DEFINED;
	memcpy(&p_rtps_topic->guid.entity_id, &a_userEntityId, sizeof(EntityId_t));


	rtps_added_user_defined_Topic(p_topic);

	if (rtps_add_endpoint_to_participant(p_this, (rtps_endpoint_t*)p_rtps_topic, p_topic->p_domain_participant) != MODULE_SUCCESS)
	{
		trace_msg(p_this, TRACE_ERROR,"RTPS cannot add EndPoint(Topic) To Participant..");
		return MODULE_ERROR_RTPS;
	}


	if (p_rtpsBultinReader == NULL) return 0;

	//msleep(100000);

	while (get_define_user_entity_lock())
	{
		trace_msg(NULL, TRACE_LOG, "Define_UserEntity_Lock sleep reader");
		//msleep(10000);
	}

	HISTORYCACHE_LOCK(p_rtpsBultinReader->p_reader_cache);


	p_change_atom = (rtps_cachechange_t *)p_rtpsBultinReader->p_reader_cache->p_head_first;

	while(p_change_atom)
	{
		rtps_cachechange_t		*cachechange = p_change_atom;
		if (cachechange && cachechange->p_data_value)
		{
			if (cachechange->b_isdisposed == false)
			{
				Check_to_topic(p_topic, cachechange->p_data_value);
			}
		}

		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}


	HISTORYCACHE_UNLOCK(p_rtpsBultinReader->p_reader_cache);


	return 0;
}



int32_t rtps_remove_topic( module_object_t* p_this, Topic* p_topic )
{
	rtps_remove_user_defined_Topic(p_topic);
	return 0;
}


bool have_topic_name_topic_type_topic( Topic* p_topic, char* p_topic_name, char* p_topic_type )
{
	bool is_match = false;
	DataReader *p_datareader = NULL;
	TopicDescription *p_topicDescription = NULL;

	if (p_topic == NULL) return is_match;
	


	if (p_topic->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		 ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_topic;
		 assert(p_contentFilteredTopic != NULL);

		 if (!strcmp(p_contentFilteredTopic->p_related_topic->topic_name, p_topic_name)
			&& !strcmp(p_topicDescription->type_name, p_topic_type))
		{
			is_match = true;
		}

	}
	else if (p_topic->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		assert(false);
	}
	else
	{
		if (!strcmp(p_topic->topic_name, p_topic_name)
			&& !strcmp(p_topic->type_name, p_topic_type))
		{
			is_match = true;
		}
	}

	return is_match;
}


bool Check_to_topic(Topic *p_topic, SerializedPayload	*data_value)
{
	SerializedPayloadForReader *p_serialized = NULL;
	int32_t	i_size = 0;
	char *p_value = NULL;
	int32_t	i_found = 0;
	GUID_t	a_remoteEndpointGuid;
	char *p_TopicName = NULL;
	char *p_TypeName = NULL;
	bool is_bultin = false;
	bool is_added = false;

	

	//if(p_rtpsReader->behavior_type == STATEFUL_TYPE)
	{
		if(data_value){
			p_serialized = (SerializedPayloadForReader *)data_value;
			i_size = p_serialized->i_size;

		}else{
			// incosis
			return false;
		}

	

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

						


						p_TypeName = p_value+4;

						find_rxo_qos_from_parameterlist(&a_pub_rxo_qos, p_serialized->p_value, i_size);

											

						
					}else{
						//inconsistant_
						change_topic_status(p_topic);
						
					}
				}else{
					//inconsistant_
					change_topic_status(p_topic);
				}
			}else{
				
			}
		}else{
			
		}
	}

	return is_added;

}



void get_builtin_topic_data( TopicBuiltinTopicData* p_topic_data, void* p_cache )
{
	rtps_cachechange_t	*p_rtps_cachechange = (rtps_cachechange_t	*)p_cache;

	if(p_topic_data == NULL || p_cache == NULL) return;


	if(p_rtps_cachechange->p_data_value != NULL)
	{
		SerializedPayloadForReader *p_serialized = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
		char *p_value = NULL;
		int	i_found = 0;
		int	i_size = 0;

		char *p_TopicName = NULL;
		char *p_TypeName = NULL;
		
		if(p_serialized) i_size = p_serialized->i_size;

		find_parameter_list(p_serialized->p_value,i_size, PID_ENDPOINT_GUID, &p_value, &i_found);

		if(i_found){
			GUID_t a_keyguid;
			a_keyguid = *(GUID_t*)p_value;

			memcpy(&p_topic_data->key, &a_keyguid, sizeof(GUID_t));
		}

		i_found = 0;

		find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
		if(i_found && p_value)
		{
			i_found = 0;
			p_TopicName = p_value+4;
			p_topic_data->name = strdup(p_TopicName);




			find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
			if(i_found && p_value)
			{
				p_TypeName = p_value+4;

				p_topic_data->type_name = strdup(p_TypeName);


			}

		}



	}
}



void ignoreTopic(DomainParticipant *p_domainparticipant, GUID_t key, char *topic_name, char *type_name)
{
	int i;

	if(p_domainparticipant == NULL || topic_name == NULL || type_name == NULL) return;


	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_subscriber; i++)
	{
		Subscriber *p_subscriber = p_domainparticipant->pp_subscriber[i];
		int j;

		
		for(j=0; j < p_subscriber->i_datareaders; j++)
		{
			DataReader* p_datareader = p_subscriber->pp_datareaders[j];

			if(strcmp(p_datareader->p_topic->topic_name, topic_name) == 0
				&& strcmp(p_datareader->p_topic->type_name, type_name) == 0)
			{
				ignore_writerproxy_guid_prefix(p_datareader->p_related_rtps_reader, &key);
			}
		}
		
	}
	mutex_unlock(&p_domainparticipant->entity_lock);



	mutex_lock(&p_domainparticipant->entity_lock);
	for(i=0; i < p_domainparticipant->i_publisher; i++)
	{
		Publisher *p_publisher = p_domainparticipant->pp_publisher[i];
		int j;
	

		for(j=0; j < p_publisher->i_datawriters; j++)
		{
			DataWriter* p_dataWriter = p_publisher->pp_datawriters[j];

			if(strcmp(p_dataWriter->p_topic->topic_name, topic_name) == 0
				&& strcmp(p_dataWriter->p_topic->type_name, type_name) == 0)
			{
				ignore_readerproxy_guid_prefix(p_dataWriter->p_related_rtps_writer, &key);
			}
		}
		
	}
	mutex_unlock(&p_domainparticipant->entity_lock);


}