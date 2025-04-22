/*
	RTSP Serialized class
	작성자 : 
	이력
	2010-09-28 : 처음 시작
*/


/*
	10.1.1 Standard Data Encapsulation Schemes 참조
*/

#include "rtps.h"



/////////////////////////////////////

static uint16_t Pre_defined_data_encapsulation_schemes(encapsulation_schemes_t a_encapsulation_schemes)
{
	if(a_encapsulation_schemes.Encapsulation_Scheme_Identifier1 != 0x00)
	{
		return -1;
	}

	if(a_encapsulation_schemes.Encapsulation_Scheme_Identifier2 == CDR_BE)
	{
		return CDR_BE;
	}else if(a_encapsulation_schemes.Encapsulation_Scheme_Identifier2 == CDR_LE)
	{
		return CDR_LE;
	}else if(a_encapsulation_schemes.Encapsulation_Scheme_Identifier2 == PL_CDR_BE)
	{
		return PL_CDR_BE;
	}else if(a_encapsulation_schemes.Encapsulation_Scheme_Identifier2 == PL_CDR_LE)
	{
		return PL_CDR_LE;
	}

	return -1;
}




static int32_t Parse_ParameterList_PL_CDR_LE(module_object_t * p_this, uint8_t *p_parameterList, int32_t i_size)
{

	char	*p_started_parameter = p_parameterList;
	int32_t		i_parameter_size = sizeof(Parameter);
	int32_t		ret = 0;
	int32_t		i_remind = i_size;
	int32_t		i_used = 0;

	do{
		//ret = rtps_parse_Parameter(p_this,*(Parameter*)&p_started_parameter[i_used],&p_started_parameter[i_used]);
		i_remind -= ret;
		i_used += ret;
	}while(ret != i_parameter_size && i_used < i_size);

	return 0;
}

int rtps_parse_serializedData(uint8_t *p_serializedData,int i_size, DataFull *p_retData)
{
	int32_t		i_remind = i_size;
	int32_t		i_used = 0;
	uint16_t	i_Encapsulation_Scheme_Identifier;
	SerializedPayloadForReader *p_serial;

	if(i_size < sizeof(encapsulation_schemes_t))
	{
		return MESSAGE_ERROR_SIZE;
	}

	/*if(i_size> 1000)
	{
		i_size = 1000;
	}*/


	p_serial = malloc(sizeof(SerializedPayloadForReader));

	memset(p_serial, 0, sizeof(SerializedPayloadForReader));

	p_retData->p_serialized_data = (SerializedPayload *)p_serial;
	p_retData->p_serialized_data->p_value = malloc(i_size);
	memcpy(p_retData->p_serialized_data->p_value, p_serializedData, i_size);
	p_serial->i_size = i_size;

	i_Encapsulation_Scheme_Identifier = Pre_defined_data_encapsulation_schemes(*(encapsulation_schemes_t*)p_serializedData);

	if(i_Encapsulation_Scheme_Identifier == -1)
	{
		return MESSAGE_ERROR_INVALID_ENCAPSULATION_SCHEME_IDENTIFIER;
	}

	i_remind -= sizeof(encapsulation_schemes_t);
	i_used += sizeof(encapsulation_schemes_t);

	if(i_Encapsulation_Scheme_Identifier == CDR_BE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == CDR_LE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == PL_CDR_BE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == PL_CDR_LE)
	{
		//Parse_ParameterList_PL_CDR_LE(p_this, p_serializedData+i_used,i_remind);
	}

	return 0;
}


int32_t rtps_parse_serializedDataFrag(uint8_t *p_serializedData,int32_t i_size, DataFragFull *p_retData)
{
	int32_t		i_remind = i_size;
	int32_t		i_used = 0;
	uint16_t	i_Encapsulation_Scheme_Identifier;
	SerializedPayloadForReader *p_serial;

	if(i_size < sizeof(encapsulation_schemes_t))
	{
		return MESSAGE_ERROR_SIZE;
	}


	p_serial = malloc(sizeof(SerializedPayloadForReader));
	memset(p_serial, 0, sizeof(SerializedPayloadForReader));

	p_retData->p_serialized_data = (SerializedPayload *)p_serial;
	p_retData->p_serialized_data->p_value = malloc(i_size);
	memcpy(p_retData->p_serialized_data->p_value, p_serializedData, i_size);
	p_serial->i_size = i_size;

	i_Encapsulation_Scheme_Identifier = Pre_defined_data_encapsulation_schemes(*(encapsulation_schemes_t*)p_serializedData);

	if(i_Encapsulation_Scheme_Identifier == -1)
	{
		return MESSAGE_ERROR_INVALID_ENCAPSULATION_SCHEME_IDENTIFIER;
	}

	i_remind -= sizeof(encapsulation_schemes_t);
	i_used += sizeof(encapsulation_schemes_t);

	if(i_Encapsulation_Scheme_Identifier == CDR_BE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == CDR_LE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == PL_CDR_BE)
	{
		
	}else if(i_Encapsulation_Scheme_Identifier == PL_CDR_LE)
	{
		//Parse_ParameterList_PL_CDR_LE(p_this, p_serializedData+i_used,i_remind);
	}

	return 0;
}

////////////////////////////////////////////////////


void gererate_parameter( char* p_serialized, int32_t* p_size, int16_t pid, int16_t length, void* v_parameter_data )
{
	int32_t i_size = 0;
	int32_t i_temp = 0;
	Parameter *p_parameter = (Parameter*)p_serialized;

	
	memcpy(p_serialized,&pid, 2);
	i_size += 2;

	if (length > 0)
	{
		memcpy(p_serialized+i_size, &length, 2);
		i_size += 2;
		memcpy(p_serialized+i_size, v_parameter_data, length);
		i_size += length;
	}

	//보정 4바이트 단위로...
	i_temp = i_size % 4;

	if (i_temp)
	{
		int32_t i;

		for (i = 0; i < 4-i_temp; i++)
		{ 
			p_serialized[i_size++];
		}
	}

	p_parameter->length = i_size-sizeof(Parameter);

	*p_size += i_size;
}


static SerializedPayloadForWriter *Generate_ParticipantInfo(rtps_writer_t *p_writer, message_t *p_message)
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	uint16_t vendor_id = 0x0000;
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;

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
	endpoint_set |= BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER;
	endpoint_set |= BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER;

	gererate_parameter(p_serialized+i_size, &i_size, PID_BUILTIN_ENDPOINT_SET, 4, &endpoint_set);

	//PID_PROTOCOL_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PROTOCOL_VERSION, 4, &protocol);

	//PID_VENDORID
	gererate_parameter(p_serialized+i_size, &i_size, PID_VENDORID, 4, &vendor_id);

	//PID_DEFAULT_UNICAST_LOCATOR
	if (&p_writer->pp_unicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_DEFAULT_UNICAST_LOCATOR, 24, p_writer->pp_unicast_locator_list[0]);

	//PID_DEFAULT_MULTICAST_LOCATOR
	if (&p_writer->pp_multicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_DEFAULT_MULTICAST_LOCATOR, 24, p_writer->pp_multicast_locator_list[0]);

	//PID_METATRAFFIC_UNICAST_LOCATOR
	if (&p_writer->pp_unicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_METATRAFFIC_UNICAST_LOCATOR, 24, p_writer->pp_unicast_locator_list[0]);

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

	gererate_parameter(p_serialized+i_size, &i_size, PID_ENTITY_NAME, 16, &entity_name);

	gererate_parameter(p_serialized+i_size, &i_size, PID_SENTINEL, 0, NULL);
	//p_serialized[i_size] = 0;
	//p_serialized[i_size] = 0;
	//p_serialized_data->i_size = i_size+2;

	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}else{

		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;
}


static SerializedPayloadForWriter *Generate_Default(rtps_writer_t *p_writer, message_t *p_message)
{
	int i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	uint16_t vendor_id = 0x0000;
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int i_temp = 0;

	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));
	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	if(p_message->i_datasize > mtu)
	{
		p_serialized = p_serialized_data->p_value = malloc(p_message->i_datasize + 20);
		memset(p_serialized, 0, p_message->i_datasize + 20);

	}else{
		p_serialized = p_serialized_data->p_value = malloc(mtu);
	}

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x01;
	i_size += 2;
	//option skip 2byte
	i_size +=2;

	//added by kyy(Presentation QoS Writer Side)/////////////////////////////////
	if(p_message->v_data == NULL)
	{
		p_serialized_data->i_size = 0;
		if(p_serialized_data->p_value != NULL)
		{
			FREE(p_serialized_data->p_value);
		}
		p_serialized_data->p_value = NULL;

	}
	////////////////////////////////////////////////////////////////
	

	memcpy(p_serialized+i_size, p_message->v_data, p_message->i_datasize);
	i_size+= p_message->i_datasize;

	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}else{

		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;
}

static SerializedPayloadForWriter *Generate_PublicationInfo(rtps_writer_t *p_writer, message_t *p_message)
{
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int	mtu = DEFAULT_MTU;
	char *p_serialized;
	int i_size = 0;
	int i_temp;

	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}

	if(p_writer->behavior_type == STATEFUL_TYPE)
	{
		
	}

	p_serialized = p_serialized_data->p_value = malloc(mtu);
	memset(p_serialized, 0, mtu);
	memcpy(p_serialized, &p_writer->guid.guid_prefix, sizeof(GuidPrefix_t));
	i_size += sizeof(GuidPrefix_t);


	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}else{

		p_serialized_data->i_size = i_size;
	}


	return p_serialized_data;
}

///////////////////////////////////////////////////

SerializedPayloadForWriter* rtps_generate_serialized_payload( rtps_writer_t* p_rtps_writer, message_t* p_message )
{
	if(p_message == NULL) return NULL;

	if(!strcmp(p_message->p_type_name, PARTICIPANTINFO_TYPE))
	{
		return Generate_ParticipantInfo(p_rtps_writer, p_message);

	}else if(!strcmp(p_message->p_type_name, TOPICINFO_TYPE))
	{

	}else if(!strcmp(p_message->p_type_name, PUBLICATIONINFO_TYPE))
	{
		return Generate_PublicationInfo(p_rtps_writer, p_message);
	}else if(!strcmp(p_message->p_type_name, SUBSCRIPTIONINFO_TYPE))
	{

	}else{
		return Generate_Default(p_rtps_writer, p_message);
	}

	return NULL;
}

