/*
	RTSP participant class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Container of all RTPS entities that share common properties and are located in a single
address space.
*/

#include "rtps.h"


static rtps_participant_t**	pp_rtps_participants = NULL;
static int					i_rtps_participant = 0;
static mutex_t				structure_lock;
static bool structure_lock_initialize = false;
static int i_ref_participant = 0;
static bool is_participant_remove = false;

static void init_rtps_participant(rtps_participant_t* p_rtps_participant)
{
	init_rtps_entity((rtps_entity_t*)p_rtps_participant);

	p_rtps_participant->guid.entity_id.entity_kind = KIND_OF_ENTITY_PARTICIPANT_BUILTIN;
	memcpy(&p_rtps_participant->guid.entity_id, &ENTITYID_PARTICIPANT, sizeof(EntityId_t));
	p_rtps_participant->vendor_id = VENDORID_ETRI;
	p_rtps_participant->pp_default_unicast_locator_list = NULL;
	p_rtps_participant->pp_default_multicast_locator_list = NULL;
	//추가
	p_rtps_participant->p_participant = NULL;
	p_rtps_participant->i_endpoint = 0;
	p_rtps_participant->pp_endpoint = NULL;

	p_rtps_participant->p_spdp_builtin_participant_reader = NULL;
	p_rtps_participant->p_spdp_builtin_participant_writer = NULL;
}


static SerializedPayloadForWriter* rtps_generate_participantInfo(DomainParticipant* p_domain_participant, rtps_participant_t* p_rtps_participant, rtps_writer_t* p_writer)
{
	int i_size = 0;
//	ProtocolVersion_t	protocol = PROTOCOLVERSION;
	ProtocolVersion_t	protocol = p_rtps_participant->protocol_version;

	uint32_t endpoint_set = 0x00000000; 
	char* p_serialized;
	VendorId_t vendor_id = p_rtps_participant->vendor_id;
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int i_temp = 0;
	char expects_inlineqos[] = "0000";


	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	
	if (p_writer->p_accessout)
	{
		if (p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);
	memset(p_serialized, 0, mtu);

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;
	i_size += 2;
	//option skip 2byte
	i_size += 2;
	//make ParameterList

	memcpy(&p_serialized_data->entity_key_guid, &p_writer->p_rtps_participant->guid, sizeof(GUID_t));

	//PID_PARTICIPANT_GUID
	gererate_parameter(p_serialized+i_size, &i_size, PID_PARTICIPANT_GUID, 16, &p_writer->p_rtps_participant->guid);

	//PID_BUILTIN_ENDPOINT_SET
	endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER;
	endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR;
	endpoint_set |= DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER;
	endpoint_set |= DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR;
	endpoint_set |= DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER;
	endpoint_set |= DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR;
	//endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_ANNOUNCER;
	//endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_DETECTOR;
	//endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_ANNOUNCER;
	//endpoint_set |= DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_DETECTOR;
	//////////////
	
	
	/*endpoint_set |= BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER;
	endpoint_set |= BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER;*/

	gererate_parameter(p_serialized+i_size, &i_size, PID_BUILTIN_ENDPOINT_SET, 4, &endpoint_set);

	//PID_PROTOCOL_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PROTOCOL_VERSION, 4, &protocol);

	//PID_VENDORID
	gererate_parameter(p_serialized+i_size, &i_size, PID_VENDORID, 4, &vendor_id);


	//PID_EXPECTS_INLINE_QOS
	//gererate_parameter(p_serialized+i_size, &i_size, PID_EXPECTS_INLINE_QOS, 4, &expects_inlineqos);


	p_writer->pp_unicast_locator_list[0]->port = get_default_port();
	//PID_DEFAULT_UNICAST_LOCATOR
	if (&p_writer->pp_unicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_DEFAULT_UNICAST_LOCATOR, 24, p_writer->pp_unicast_locator_list[0]);

	p_writer->pp_multicast_locator_list[0]->port = get_default_port();
	//PID_DEFAULT_MULTICAST_LOCATOR
	if (&p_writer->pp_multicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_DEFAULT_MULTICAST_LOCATOR, 24, p_writer->pp_multicast_locator_list[0]);

	p_writer->pp_unicast_locator_list[0]->port = get_metatraffic_port();
	//PID_METATRAFFIC_UNICAST_LOCATOR
	if (&p_writer->pp_unicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_METATRAFFIC_UNICAST_LOCATOR, 24, p_writer->pp_unicast_locator_list[0]);

	p_writer->pp_multicast_locator_list[0]->port = get_metatraffic_port();
	//PID_METATRAFFIC_MULTICAST_LOCATOR
	if (&p_writer->pp_multicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, 24, p_writer->pp_multicast_locator_list[0]);

	if (p_writer->behavior_type == STATELESS_TYPE)
	{
		//Table 9.13 - ParameterId mapping and default values
		//Default {100, 0}
		Duration_t leaseDuration = {100, 0};

		//PID_PARTICIPANT_LEASE_DURATION
		gererate_parameter(p_serialized+i_size, &i_size, PID_PARTICIPANT_LEASE_DURATION, sizeof(Duration_t), &leaseDuration);
	}

	gererate_parameter(p_serialized+i_size, &i_size, PID_PRODUCT_VERSION, 4, &product_version);

	//gererate_parameter(p_serialized+i_size, &i_size, PID_ENTITY_NAME, 16, &entity_name);

	//added by kyy(UserData QoS)
	//PID_USER_DATA(DomainParticipant)///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_user_data(p_serialized, p_domain_participant->qos.user_data, i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


static SerializedPayloadForWriter* rtps_generate_participantInfo_remove(DomainParticipant* p_domain_participant, rtps_participant_t* p_rtps_participant, rtps_writer_t* p_writer)
{
	int i_size = 0;

	uint32_t endpoint_set = 0x00000000; 
	char* p_serialized = NULL;
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter*	p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;
	char status_info[] = {0x00,0x00,0x00,0x03};
	
	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if (p_writer->p_accessout)
	{
		if (p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;



	//PID_KEY_HASH
	gererate_parameter(p_serialized+i_size, &i_size, PID_KEY_HASH, 16, &p_rtps_participant->guid);

	//PID_STATUS_INFO
	gererate_parameter(p_serialized+i_size, &i_size, PID_STATUS_INFO, 4, status_info);



	//PID_SENTINEL
	gererate_parameter(p_serialized+i_size, &i_size, PID_SENTINEL, 0, NULL);

	i_temp = i_size % 4;

	if (i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}
	else
	{
		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;

}


static char ParticipantDataForGuid[] = {0x00,0x10,0x00, 0x00}; 
static SerializedPayloadForWriter* rtps_generate_participantData(DomainParticipant* p_domain_participant, rtps_participant_t* p_rtps_participant, rtps_writer_t* p_writer)
{
	ParticipantMessageData* p_participant_message_data = NULL;
	SerializedPayloadForWriter* p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));

	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	p_serialized_data->i_size = sizeof(ParticipantMessageData)+4;
	p_serialized_data->p_value = (uint8_t*)malloc(p_serialized_data->i_size);

	memset(p_serialized_data->p_value, 0, p_serialized_data->i_size);
	p_serialized_data->p_value[0] = 0x00;
	p_serialized_data->p_value[1] = 0x01;

	p_participant_message_data = (ParticipantMessageData*)(p_serialized_data->p_value+4);
	memcpy(&p_participant_message_data->participant_guid_prefix, &p_rtps_participant->guid.guid_prefix, 8);
	memcpy(&p_participant_message_data->participant_guid_prefix._guidprefix[8], ParticipantDataForGuid, 4);
	//p_participant_message_data->participant_guid_prefix = p_rtps_participant->guid.guidPrefix;
	memcpy(p_participant_message_data->kind , PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE, 4);
	memcpy(&p_serialized_data->entity_key_guid, &p_writer->p_rtps_participant->guid, sizeof(GUID_t));
	p_participant_message_data->sequence_length[0] = 0;
	p_participant_message_data->sequence_length[1] = 0;
	p_participant_message_data->sequence_length[2] = 0;
	p_participant_message_data->sequence_length[3] = 1;
	memset(p_participant_message_data->data, 0, sizeof(p_participant_message_data->data));
	p_participant_message_data->data[0] = 1;
	p_participant_message_data->data[1] = 0;

	
	return (SerializedPayloadForWriter *)p_serialized_data;
}


void rtps_added_user_defined_Participant(DomainParticipant* p_domain_participant, rtps_participant_t* p_rtps_participant)
{
	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//butinSubscripton을 찾자.
	rtps_statefulwriter_t* p_statefulwriter = NULL; 
	rtps_statelesswriter_t* p_statelesswriter = (rtps_statelesswriter_t*)rtps_get_spdp_builtin_participant_writer();
	if (p_statelesswriter == NULL || p_statelesswriter->p_datawriter == NULL) return;
	p_builtin_datawriter = p_statelesswriter->p_datawriter;

	p_serialized_data = rtps_generate_participantInfo(p_domain_participant, p_rtps_participant, (rtps_writer_t *)p_statelesswriter);


	////////////////
	rtps_write_from_builtin((rtps_writer_t *)p_statelesswriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);

	////////////////
	//[120307,김경일]필요없어진 것 같아서 주석처리함.
	//p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_LivelinessWriter();
	//if(p_statefulwriter == NULL) return;

	//p_serialized_data = rtps_generate_participantData(p_domain_participant, p_rtps_participant, (rtps_writer_t *)p_statelesswriter);

	//rtps_write_from_builtin((rtps_writer_t *)p_statefulwriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


void rtps_remove_user_defined_participant(rtps_participant_t* p_rtps_participant)
{
	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//butinSubscripton을 찾자.
	rtps_statefulwriter_t* p_statefulwriter = NULL; 
	rtps_statelesswriter_t* p_statelesswriter = (rtps_statelesswriter_t *)rtps_get_spdp_builtin_participant_writer();
	if (p_statelesswriter == NULL || p_statelesswriter->p_datawriter == NULL) return;
	p_builtin_datawriter = p_statelesswriter->p_datawriter;

	p_serialized_data = rtps_generate_participantInfo_remove(p_rtps_participant->p_participant, p_rtps_participant, (rtps_writer_t *)p_statelesswriter);


	////////////////
	rtps_write_from_builtin_rapidly((rtps_writer_t*)p_statelesswriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


int32_t rtps_add_participant(module_object_t* p_this, DomainParticipant* p_domain_participant)
{
	rtps_participant_t* p_rtps_participant = malloc(sizeof(rtps_participant_t));

	memset(p_rtps_participant, 0, sizeof(rtps_participant_t));

	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	init_rtps_participant(p_rtps_participant);
	

	p_rtps_participant->p_participant = p_domain_participant;

	rtps_participant_lock();
	INSERT_ELEM(pp_rtps_participants, i_rtps_participant, i_rtps_participant, p_rtps_participant);
	rtps_participant_unlock();


	//////////////////////////////////////////////////////////////////////////////////
	if (p_domain_participant->domain_id != BUILTIN_PARTICIPANT_DOMAIN_ID)
	{
		//user-defined Participant추가..
		rtps_added_user_defined_Participant(p_domain_participant, p_rtps_participant);
	}

	p_domain_participant->p_rtps_participant = p_rtps_participant;

	//////////////////////////////////////////////////////////////////////////////////

	trace_msg(p_this, TRACE_DEBUG,"RTPS addParticipant..");

	return MODULE_SUCCESS;
}


int32_t rtps_remove_participant(module_object_t* p_this, DomainParticipant* p_domain_participant)
{
	int i;

	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	while (i_ref_participant)
	{
		if (is_quit())
		{
			break;
		}

		trace_msg(p_this, TRACE_LOG, "i_ref_participant : %d [%s]", i_ref_participant, __FUNCTION__);
		msleep(1);
	}

	//rtps_remove_user_defined_participant(p_participant->p_rt)

	rtps_participant_lock();
	is_participant_remove = true;
	for (i = i_rtps_participant-1; i >= 0; i--)
	{
		if (pp_rtps_participants[i]->p_participant == p_domain_participant)
		{
			rtps_remove_user_defined_participant(pp_rtps_participants[i]);
			FREE(pp_rtps_participants[i]);
			REMOVE_ELEM( pp_rtps_participants, i_rtps_participant, i);
			trace_msg(p_this, TRACE_DEBUG,"RTPS remove a Participant..");
			is_participant_remove = false;
			rtps_participant_unlock();
			return MODULE_SUCCESS;
		}
	}
	is_participant_remove = false;
	rtps_participant_unlock();

	trace_msg(p_this, TRACE_ERROR,"RTPS cannot remove a Participant..");

	return MODULE_ERROR_RTPS;
}


int32_t rtps_add_endpoint_to_participant(module_object_t* p_this, rtps_endpoint_t* p_rtps_endpoint, DomainParticipant* p_domain_participant)
{
	int i;

	rtps_participant_lock();

	for (i = 0; i < i_rtps_participant; i++)
	{
		if (pp_rtps_participants[i]->p_participant == p_domain_participant)
		{
			///////////////////////////////////////////////////////////////////////////////
			rtps_participant_t *p_rtpsParticipant = pp_rtps_participants[i];
			INSERT_ELEM( p_rtpsParticipant->pp_endpoint, p_rtpsParticipant->i_endpoint,
                 p_rtpsParticipant->i_endpoint, p_rtps_endpoint );
			p_rtps_endpoint->p_rtps_participant = p_rtpsParticipant;
			///////////////////////////////////////////////////////////////////////////////
			trace_msg(p_this, TRACE_DEBUG,"RTPS add EndPoint to the rtps participant.");

			rtps_participant_unlock();

			return MODULE_SUCCESS;
		}
	}

	rtps_participant_unlock();

	//////////////////////////////////////////////////////////////////////////////////////////

	return MODULE_ERROR_RTPS;
}


int32_t rtps_remove_endpoint_from_participant(module_object_t* p_this, Entity* p_entity)
{
	int i;

	rtps_participant_t* p_rtps_participant = NULL;
	DomainParticipant* p_participant = NULL;

	if (!p_entity)
	{
		return MODULE_ERROR_RTPS;
	}

	if (p_entity->i_entity_type == DATAREADER_ENTITY)
	{
		p_participant = ((DataReader *)p_entity)->p_subscriber->p_domain_participant;

	}
	else if (p_entity->i_entity_type == DATAWRITER_ENTITY)
	{
		p_participant = ((DataWriter *)p_entity)->p_publisher->p_domain_participant;
	}
	else
	{
		trace_msg(p_this, TRACE_ERROR,"RTPS cannot remove a EndPoint from participant.. Entity Type unknown.");
		return MODULE_ERROR_RTPS;
	}

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		if (pp_rtps_participants[i]->p_participant == p_participant)
		{
			p_rtps_participant = pp_rtps_participants[i];
			break;
		}
	}
	rtps_participant_unlock();

	if (!p_rtps_participant)
	{
		trace_msg(p_this, TRACE_ERROR,"RTPS cannot remove a EndPoint from participant.. cannot find RTPS Participant.");
		return MODULE_ERROR_RTPS;
	}

	while (i_ref_participant)
	{

		if (is_quit())
		{
			break;
		}

		trace_msg(p_this, TRACE_LOG, "i_ref_participant : %d [%s]", i_ref_participant, __FUNCTION__);
		msleep(1);
	}

	rtps_participant_lock();
	is_participant_remove = true;
	for (i = p_rtps_participant->i_endpoint-1; i >=0; i--)
	{
		Entity *p_tmpEntity = (Entity *)(p_rtps_participant->pp_endpoint[i]+1);
		
		if ((Entity *)p_tmpEntity->enable == p_entity)
		{
			destroy_endpoint(p_rtps_participant->pp_endpoint[i]);
			REMOVE_ELEM( p_rtps_participant->pp_endpoint, p_rtps_participant->i_endpoint, i);
			trace_msg(p_this, TRACE_DEBUG,"RTPS remove a EndpPoint from participant..");
			is_participant_remove = false;
			rtps_participant_unlock();
			return MODULE_SUCCESS;
		}
	}
	is_participant_remove = false;
	rtps_participant_unlock();

	trace_msg(p_this, TRACE_ERROR,"RTPS cannot remove a EndPoint from participant..");

	return MODULE_ERROR_RTPS;
}


void rtps_participant_lock()
{
	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}
	
	//trace_msg(NULL, TRACE_LOG,"rtps_participant_lock");
	mutex_lock(&structure_lock);
	i_ref_participant++;
}

void rtps_participant_unlock()
{
	if (structure_lock_initialize == false) return;


	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	//trace_msg(NULL, TRACE_LOG,"rtpsParticipant_UNLock");

	mutex_unlock(&structure_lock);
	i_ref_participant--;
}


rtps_participant_t** get_rtps_participants(int* i_size)
{
	*i_size = i_rtps_participant;
	return pp_rtps_participants;
}


rtps_participant_t* get_rtps_builtin_participant()
{
	if (i_rtps_participant == 0) return NULL;

	return pp_rtps_participants[0];
}


rtps_endpoint_t* rtps_get_spdp_builtin_participant_reader()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_participant_writer()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_Builtin_publication_reader()
{
	int i, j;
	rtps_participant_t*	p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_publication_writer()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_subscription_reader()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_topic_reader()
{
	int i,j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_topic_writer()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_participant_message_reader()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_participant_message_writer()
{
	int i,j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_spdp_builtin_subscription_writer()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


bool have_topic_name_topic_type_reader(rtps_reader_t* p_rtps_reader, char* p_topic_name, char* p_topic_type)
{
	bool is_match = false;
	DataReader* p_datareader = NULL;
	TopicDescription* p_topic_description = NULL;

	if (p_rtps_reader == NULL || p_rtps_reader->p_datareader == NULL) return is_match;
	
	p_datareader = p_rtps_reader->p_datareader;
	p_topic_description = p_datareader->get_topicdescription(p_datareader);

	if (p_topic_description->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		 ContentFilteredTopic* p_content_filtered_topic = (ContentFilteredTopic *)p_datareader->p_topic;
		 assert(p_content_filtered_topic != NULL);

		 if (!strcmp(p_content_filtered_topic->p_related_topic->topic_name, p_topic_name)	&& !strcmp(p_topic_description->type_name, p_topic_type))
		{
			is_match = true;
		}
	}
	else if (p_topic_description->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		assert(false);
	}
	else
	{
		if (!strcmp(p_topic_description->topic_name, p_topic_name) && !strcmp(p_topic_description->type_name, p_topic_type))
		{
			is_match = true;
		}
	}

	return is_match;
}


rtps_endpoint_t** rtps_compare_reader(GUID_t a_guid, char* p_topic_name, char* p_topic_type, int* p_size)
{
	int i, j;
	int i_count = 0;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;
	rtps_endpoint_t**	pp_rtps_endpoints =  NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if ((p_rtps_endpoint->i_entity_type == READER_ENTITY))
			{
				if (have_topic_name_topic_type_reader((rtps_reader_t*)p_rtps_endpoint, p_topic_name,p_topic_type))
				{
					//rtps_participant_unlock();
					//return p_rtps_endpoint;
					INSERT_ELEM(pp_rtps_endpoints, i_count, i_count, p_rtps_endpoint);
				}
			}
		}
	}
	rtps_participant_unlock();

	*p_size = i_count;

	return pp_rtps_endpoints;
}


bool have_topic_name_topic_type_writer(rtps_writer_t* p_rtps_writer, char* p_topic_name, char* p_topic_type)
{
	bool is_match = false;
	DataWriter* p_datawriter = NULL;
	Topic* p_topic = NULL;

	if (p_rtps_writer == NULL || p_rtps_writer->p_datawriter == NULL) return is_match;
	
	p_datawriter = p_rtps_writer->p_datawriter;
	p_topic = p_datawriter->get_topic(p_datawriter);

	if (!strcmp(p_topic->topic_name, p_topic_name) && !strcmp(p_topic->type_name, p_topic_type))
	{
		is_match = true;
	}

	return is_match;
}


rtps_endpoint_t** rtps_compare_writer(GUID_t a_guid, char* p_topic_name, char* p_topic_type, int* p_size)
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;
	rtps_endpoint_t**	pp_rtps_endpoints =  NULL;
	int i_count = 0;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if ((p_rtps_endpoint->i_entity_type == WRITER_ENTITY))
			{
				if (have_topic_name_topic_type_writer((rtps_writer_t*)p_rtps_endpoint, p_topic_name,p_topic_type))
				{
					//rtps_participant_unlock();
					//return p_rtps_endpoint;
					INSERT_ELEM(pp_rtps_endpoints, i_count, i_count, p_rtps_endpoint);
				}
			}
		}
	}
	rtps_participant_unlock();

	*p_size = i_count;

	return pp_rtps_endpoints;
}


rtps_endpoint_t* rtps_find_topic(char* p_topic_name)
{
	rtps_topic_t *p_topic;
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;
///	rtps_endpoint_t**	pp_rtps_endpoints =  NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if ((p_rtps_endpoint->i_entity_type == RTPS_TOPIC_ENTITY))
			{
				p_topic = (rtps_topic_t*)p_rtps_endpoint;
				if (strcmp(p_topic->p_topic->topic_name, p_topic_name) == 0)
				{
					rtps_participant_unlock();
					return p_rtps_endpoint;
				}
			}
		}
	}

	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_liveliness_writer()
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_liveliness_reader()
{
	int i, j;
	rtps_participant_t*	p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)))
			{
				rtps_participant_unlock();
				return p_rtps_endpoint;
			}
		}
	}
	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t* rtps_get_endpoint(GUID_t a_matched_guid)
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint;

	if (!structure_lock_initialize)
	{
		return NULL;
	}

	rtps_participant_lock();

	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if (!memcmp(&p_rtps_endpoint->guid, &a_matched_guid, sizeof(GUID_t)))
			{
				rtps_participant_unlock();

				return p_rtps_endpoint;
			}
		}
	}

	rtps_participant_unlock();

	return NULL;
}


rtps_endpoint_t** rtps_matched_writerproxy_reader(GUID_t a_matched_guid, EntityId_t reader_id, int* p_size)
{
	int i, j;
	rtps_participant_t* p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;
	rtps_writerproxy_t* p_rtps_writerproxy = NULL;
	rtps_reader_t*		p_rtps_reader = NULL;
	rtps_endpoint_t**	pp_rtps_endpoint = NULL;
	int i_count = 0;

	if(!structure_lock_initialize) return NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if ((p_rtps_endpoint->i_entity_type == READER_ENTITY))
			{
				p_rtps_reader = (rtps_reader_t *)p_rtps_endpoint;

				if (memcmp(&reader_id, &ENTITYID_UNKNOWN, sizeof(EntityId_t)) == 0)
				{
					if (p_rtps_reader->behavior_type == STATEFUL_TYPE)
					{
						p_rtps_writerproxy = rtps_statefulreader_matched_writer_lookup((rtps_statefulreader_t *)p_rtps_endpoint, a_matched_guid);
						if (p_rtps_writerproxy)
						{
							INSERT_ELEM(pp_rtps_endpoint, i_count, i_count, p_rtps_endpoint);
							//rtps_participant_unlock();
							//return p_rtps_endpoint;
						}
					}
				}
				else if (memcmp(&p_rtps_reader->guid.entity_id, &reader_id, sizeof(EntityId_t)) == 0)
				{
					INSERT_ELEM(pp_rtps_endpoint, i_count, i_count, p_rtps_endpoint);
					//rtps_participant_unlock();
					//return p_rtps_endpoint;
				}
			}
		}
	}
	rtps_participant_unlock();

	*p_size = i_count;

	return pp_rtps_endpoint;
}


rtps_endpoint_t** rtps_matched_readerproxy_writer(GUID_t a_matched_guid, EntityId_t reader_id, int* p_size)
{
	int i,j;
	rtps_participant_t*	p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;
	rtps_writer_t*		p_rtps_writer = NULL;
	rtps_readerproxy_t*	p_rtps_readerproxy = NULL;
	rtps_endpoint_t**	pp_rtps_endpoints = NULL;
	int i_count = 0;

	if (!structure_lock_initialize) return NULL;

	rtps_participant_lock();
	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];
			if ((p_rtps_endpoint->i_entity_type == WRITER_ENTITY))
			{
				p_rtps_writer = (rtps_writer_t *)p_rtps_endpoint;

				if (memcmp(&reader_id, &ENTITYID_UNKNOWN, sizeof(EntityId_t)) == 0)
				{
					if (p_rtps_writer->behavior_type == STATEFUL_TYPE)
					{
						p_rtps_readerproxy = rtps_statefulwriter_matched_reader_lookup((rtps_statefulwriter_t *)p_rtps_endpoint, a_matched_guid);
						if (p_rtps_readerproxy)
						{
							INSERT_ELEM(pp_rtps_endpoints, i_count, i_count, p_rtps_endpoint);
						}
					}
				}
				else if (memcmp(&p_rtps_writer->guid.entity_id, &reader_id, sizeof(EntityId_t)) == 0)
				{
					INSERT_ELEM(pp_rtps_endpoints, i_count, i_count, p_rtps_endpoint);
				}
			}
		}
	}
	rtps_participant_unlock();

	*p_size = i_count;

	return pp_rtps_endpoints;
}


void remove_matched_endpoint_proxy(GUID_t remote_pariticpant_guid)
{
	int i, j, k;
	rtps_participant_t*	p_rtps_participant = NULL;
	rtps_endpoint_t*	p_rtps_endpoint = NULL;
	rtps_writerproxy_t* p_rtps_writerproxy = NULL;
	rtps_readerproxy_t*	p_rtps_readerproxy = NULL;
	rtps_reader_t*		p_rtps_reader = NULL;
	rtps_writer_t*		p_rtps_writer = NULL;
	rtps_statefulreader_t* p_statefulreader;
	rtps_statefulwriter_t *p_statefulwriter;

	if (!structure_lock_initialize) return;

	rtps_participant_lock();


	for (i = 0; i < i_rtps_participant; i++)
	{
		p_rtps_participant = pp_rtps_participants[i];
		for (j = 0; j < p_rtps_participant->i_endpoint; j++)
		{
			p_rtps_endpoint = p_rtps_participant->pp_endpoint[j];


			if ((p_rtps_endpoint->i_entity_type == READER_ENTITY))
			{
				p_rtps_reader = (rtps_reader_t *)p_rtps_endpoint;
				if (p_rtps_reader->behavior_type == STATEFUL_TYPE)
				{
					p_statefulreader = (rtps_statefulreader_t *)p_rtps_endpoint;

					for(k=0; k < p_statefulreader->i_matched_writers; k++)
					{

						if (memcmp(&p_statefulreader->pp_matched_writers[k]->remote_writer_guid.guid_prefix, &remote_pariticpant_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
						{
							rtps_statefulreader_matched_writer_remove(p_statefulreader, p_statefulreader->pp_matched_writers[k]);
							k--;
						}
					}
				}

			}else if ((p_rtps_endpoint->i_entity_type == WRITER_ENTITY))
			{
				p_rtps_writer = (rtps_writer_t *)p_rtps_endpoint;
				if (p_rtps_writer->behavior_type == STATEFUL_TYPE)
				{
					p_statefulwriter = (rtps_statefulwriter_t *)p_rtps_endpoint;

					for(k=0; k < p_statefulwriter->i_matched_readers; k++)
					{

						if(memcmp(&p_statefulwriter->pp_matched_readers[k]->remote_reader_guid.guid_prefix, &remote_pariticpant_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
						{
							rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_statefulwriter->pp_matched_readers[k]);
							k--;
						}
					}
				}
			}

		}


	}

	rtps_participant_unlock();
}