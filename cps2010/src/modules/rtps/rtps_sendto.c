/*
	RTSP Sendto class
	작성자 : 
	이력
	2010-10-16 : 처음 시작
*/

#include "rtps.h"

static char RTPS[] = "RTPS";


void check_HeatBeat_Count(rtps_readerproxy_t *p_rtps_readerproxy)
{
	if(p_rtps_readerproxy->is_heartbeatsend_and_wait_for_ack == true)
	{
		p_rtps_readerproxy->not_ack_count++;
	}



	/*if (p_rtps_readerproxy->not_ack_count > 10)
	{
		printf("p_rtps_readerproxy->not_ack_count : %d\r\n", p_rtps_readerproxy->not_ack_count);
	}*/
}

// RRPS 메세지 전송시 4바이트씩 boundary가 되어야 된다. 스펙에 있음
int BoundarySize(int i_size)
{
	int i_ = i_size % 4;

	if(i_ != 0){
		return i_size += 4-i_;
	}

	return i_size;
}



void Mapping_header(rtps_writer_t *p_rtps_writer, Header *p_header)
{
	memcpy(p_header->protocol._rtps, RTPS, 4);
	p_header->version.major = 2;
	p_header->version.minor = 1;
	p_header->vendor_id.vendor_id[0] = VENDORID_ETRI.vendor_id[0];
	p_header->vendor_id.vendor_id[1] = VENDORID_ETRI.vendor_id[1];
	//memcpy(p_header->guidPrefix._guidprefix ,p_rtps_writer->p_rtpsParticipant->guid.guidPrefix._guidprefix, 12);
	memcpy(p_header->guid_prefix._guidprefix ,p_rtps_writer->guid.guid_prefix._guidprefix, 12);
	
}


void Mapping_SubmessageHeader(rtps_writer_t *p_rtps_writer, SubmessageHeader *p_header, SubmessageKind kind, octet flags, ushort submessageLength)
{
	p_header->submessage_id = kind;
	p_header->flags = flags;
	p_header->submessage_length = submessageLength;
}

int Mapping_InlineQos(rtps_writer_t *p_rtps_writer, char *p_buf, ParameterList inlineQos)
{
	int i_count = 0;
	ParameterWithValue *p_atom_parameter;
	p_atom_parameter = get_real_parameter(inlineQos.p_head_first);

	while(p_atom_parameter)
	{
		memcpy(p_buf+i_count, p_atom_parameter, sizeof(Parameter));
		i_count += sizeof(Parameter);
		if(p_atom_parameter->length > 0){
			memcpy(p_buf+i_count, p_atom_parameter->p_value, p_atom_parameter->length);
			i_count += p_atom_parameter->length;
		}

		p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

	}

	return i_count;
}

int count_inlineQos(ParameterList inlineQos)
{
	int i_count = 0;

	ParameterWithValue *p_atom_parameter;
	p_atom_parameter = get_real_parameter(inlineQos.p_head_first);

	while(p_atom_parameter)
	{
		i_count += sizeof(Parameter);
		i_count += p_atom_parameter->length;

		p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
	}

	return i_count;
}

int32_t rtps_send_data_to( rtps_writer_t* p_rtps_writer, Locator_t locator, DataFull* p_data, Time_t message_time)
{
	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;

//time by jun
//	Duration_t duration = current_duration();
	Time_t duration = message_time;

	bool remove_data = false;

	


	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}else{
		p_data->reader_id.value = ENTITYID_UNKNOWN;
	}

	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	i_size = BoundarySize(sizeof(Header));
	// INFO_TS
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));

	
	// DATA
	i_inlineQos_count = count_inlineQos(p_data->inline_qos);
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Data) + i_inlineQos_count + p_serializedPlayloadwriter->i_size);

	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	

	//INFO_TS
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, sizeof(Duration_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&duration, sizeof(Duration_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (p_rtps_writer->is_builtin)
	{
		char *p_value = NULL;
		int i_found = 0;
		find_ParameterList_for_remove((char*)p_serializedPlayloadwriter->p_value,p_serializedPlayloadwriter->i_size, PID_STATUS_INFO, &p_value, &i_found);
		if (i_found && p_value && i_found == 4)
		{
			if (p_value[3] == 0x03)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}
		}
	}

	if(remove_data)
	{
		flags = 0x03;  //Q
	}else if (i_inlineQos_count){
		flags = 0x07; //remove
	}else{
		flags = 0x05; //add
	}


	if(!remove_data){
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data));
		p_senddata->i_size -= i_inlineQos_count;
	}

	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp, p_data, sizeof(Data));
	i_temp += sizeof(Data);

	//inline_qos
	if(!remove_data){
		i_temp += Mapping_InlineQos(p_rtps_writer, p_senddata->p_data+i_temp,p_data->inline_qos);
	}

	memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, p_serializedPlayloadwriter->i_size);
	i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;
	p_rtps_writer->send_sample_count++;
	p_rtps_writer->send_sample_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
	p_rtps_writer->send_sample_count_throughput++;
	p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;


	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	///////////////////////////
	destory_datafull(p_data);

	return MODULE_SUCCESS;
}


int32_t rtps_send_data_frag_to( rtps_writer_t* p_rtps_writer, Locator_t locator, DataFull* p_data, Time_t message_time )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;

	//time by jun
	//	Duration_t duration = current_duration();
	Time_t duration = message_time;

	bool is_frag = false;
	ushort fragmentsInSubmessage;
	ushort fragmentSize;
	int i;
	DataFrag dataFrag;

	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PARTICIPANTINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_UNKNOWN;
	}
	else
	{
		p_data->reader_id.value = ENTITYID_UNKNOWN;
		is_frag = true;
	}

	if (!is_frag)
	{
		return rtps_send_data_to(p_rtps_writer, locator, p_data, message_time);
	}

	if (p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	fragmentsInSubmessage = (p_serializedPlayloadwriter->i_size - 4) / p_rtps_writer->fragment_size;
	fragmentSize = p_serializedPlayloadwriter->i_size % p_rtps_writer->fragment_size;

	memcpy(&dataFrag,p_data,sizeof(Data));

	fragmentsInSubmessage = fragmentSize == 0 ? fragmentsInSubmessage : fragmentsInSubmessage + 1;

	for (i = 0; i < fragmentsInSubmessage; i++){
		i_count = 0;
		fragmentSize = fragmentsInSubmessage == i + 1 ? (p_serializedPlayloadwriter->i_size- 4) % p_rtps_writer->fragment_size : p_rtps_writer->fragment_size;

		i_size = BoundarySize(sizeof(Header));
		dataFrag.fragment_starting_num.value = 1;
		dataFrag.fragment_size = fragmentSize;
		dataFrag.data_size = p_serializedPlayloadwriter->i_size - 4; // 4바이트는 encapsulation kind and option 값임.
		dataFrag.fragments_in_submessage = i + 1;

		
		// INFO_TS
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
		// DATA_FRAG
		i_inlineQos_count = count_inlineQos(p_data->inline_qos);
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(DataFrag) + i_inlineQos_count) + 4 +fragmentSize;

		p_senddata = data_new(i_size);
		p_senddata->i_port = locator.port;
		if(locator.kind == LOCATOR_KIND_UDPv4){
			asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
		}

		Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
		i_count += BoundarySize(sizeof(Header));


		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//INFO_TS
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(i_inlineQos_count)
		{
			flags = 0x07;  //Q
		}else{
			flags = 0x05;
		}

		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA_FRAG, flags, 4+fragmentSize+sizeof(DataFrag)+i_inlineQos_count);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp, &dataFrag, sizeof(DataFrag));
		i_temp += sizeof(DataFrag);
		//inline_qos
		i_temp += Mapping_InlineQos(p_rtps_writer, p_senddata->p_data+i_temp,p_data->inline_qos);
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, 4); // 4바이트는 encapsulation kind and option 값임.
		i_temp += 4;
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value+4+p_rtps_writer->fragment_size*i, fragmentSize);
		i_count += BoundarySize(sizeof(SubmessageHeader)+fragmentSize);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		p_rtps_writer->send_count++;
		p_rtps_writer->send_bytes += p_senddata->i_size;
		p_rtps_writer->send_sample_count++;
		p_rtps_writer->send_sample_bytes += p_senddata->i_size;

		p_rtps_writer->send_count_throughput++;
		p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
		p_rtps_writer->send_sample_count_throughput++;
		p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;

		p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	


	///////////////////////////
	destory_datafull(p_data);

	return MODULE_SUCCESS;
}


int32_t rtps_send_data_with_dst_to_for_remove( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
	Duration_t duration = current_duration();
	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	bool is_send_InfoDst = true;

	is_send_InfoDst = false;

	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;

	i_size = BoundarySize(sizeof(Header));

	if(is_send_InfoDst)
	{
		// INFO_DST
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	// INFO_TS
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
	// DATA
	i_inlineQos_count = count_inlineQos(p_data->inline_qos);
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Data) + i_inlineQos_count + p_serializedPlayloadwriter->i_size);

	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	if(is_send_InfoDst)
	{ 
		//INFO_DST
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//INFO_TS
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(i_inlineQos_count)
	{
		flags = 0x07;  //Q
	}else{
		flags = 0x05; //add
	}

	flags = 0x03;// remove

	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp, p_data, sizeof(Data));
	i_temp += sizeof(Data);
	//inline_qos
	i_temp += Mapping_InlineQos(p_rtps_writer, p_senddata->p_data+i_temp,p_data->inline_qos);
	memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, p_serializedPlayloadwriter->i_size);
	i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;
	p_rtps_writer->send_sample_count++;
	p_rtps_writer->send_sample_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
	p_rtps_writer->send_sample_count_throughput++;
	p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	///////////////////////////
	destory_datafull(p_data);

	return MODULE_SUCCESS;
}



int32_t rtps_send_data_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data, bool is_multicast, Time_t message_time)
{
	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
	//time by jun
	//	Duration_t duration = current_duration();
	Time_t duration = message_time;

	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	bool is_send_InfoDst = true;
	bool remove_data = false;

	//added by kyy(Presentation QoS Writer Side)////////////////////////////////
	//Data가 비어있는지 검사
	bool is_data_empty = false;
	if(p_serializedPlayloadwriter->p_value == NULL)
	{
		is_data_empty = true;
		p_serializedPlayloadwriter->i_size = 0;
	}
	////////////////////////////////////////////////////////////

	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		is_send_InfoDst = false;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}else{
		//p_data->reader_id.value = ENTITYID_UNKNOWN;
		//p_data->reader_id.value = remoteReaderGuid.entityId;
		is_send_InfoDst = false;
	}

	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;

	i_size = BoundarySize(sizeof(Header));
/*
	if(is_send_InfoDst && is_multicast == false)
	{ 
		// INFO_DST
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	// INFO_TS
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
*/
	// DATA
	i_inlineQos_count = count_inlineQos(p_data->inline_qos);

	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Data) + i_inlineQos_count + p_serializedPlayloadwriter->i_size);

	p_senddata = data_new(i_size);
	p_senddata->p_rtps_writer = p_rtps_writer;
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));

/*
	if(is_send_InfoDst && is_multicast == false)
	{ 
		//INFO_DST
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//INFO_TS
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//if(p_rtps_writer->is_bultin)
	{
		char *p_value = NULL;
		int i_found = 0;
		find_ParameterList_for_remove((char*)p_serializedPlayloadwriter->p_value,p_serializedPlayloadwriter->i_size, PID_STATUS_INFO, &p_value, &i_found);
		if(i_found && p_value && i_found == 4)
		{
			if(p_value[3] == 0x03)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}else if(p_value[3] & 0x01)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}else if(p_value[3] & 0x02)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}
		}
	}

	//added by kyy(Presentation QoS Writer Side)
	// is_data_empty 추가
	if(remove_data || is_data_empty==true)
	{
		flags = 0x03;  //Q
	}else if (i_inlineQos_count){
		flags = 0x07; //remove
	}else{
		flags = 0x05; //add
	}

	if(!remove_data)
	{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data));
		p_senddata->i_size -= i_inlineQos_count;
	}
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp, p_data, sizeof(Data));
	i_temp += sizeof(Data);

	//inline_qos
	if(!remove_data)
		i_temp += Mapping_InlineQos(p_rtps_writer, (char*)p_senddata->p_data+i_temp,p_data->inline_qos);

	//added by kyy(Presentation QoS Writer Side) 이부분 주석 처리하면 데이터가 저장되지 않음
	if(p_serializedPlayloadwriter->p_value != NULL)
	{
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, p_serializedPlayloadwriter->i_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size);
	}
	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;
	p_rtps_writer->send_sample_count++;
	p_rtps_writer->send_sample_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
	p_rtps_writer->send_sample_count_throughput++;
	p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;


	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);

	///////////////////////////
	destory_datafull(p_data);

	return MODULE_SUCCESS;
}


int32_t rtps_send_data_and_heartbeat_with_dst_to(rtps_writer_t* p_rtps_writer, rtps_readerproxy_t *p_rtps_readerproxy, Locator_t locator, DataFull* p_data, bool is_multicast, bool is_final, Time_t message_time)
{

	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
	//time by jun
	//	Duration_t duration = current_duration();
	Time_t duration = message_time;

	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	bool is_send_InfoDst = true;
	bool remove_data = false;
	GUID_t remote_reader_guid = p_rtps_readerproxy->remote_reader_guid;

	//added by kyy(Presentation QoS Writer Side)////////////////////////////////
	//Data가 비어있는지 검사
	bool is_data_empty = false;
	if(p_serializedPlayloadwriter->p_value == NULL)
	{
		is_data_empty = true;
		p_serializedPlayloadwriter->i_size = 0;
	}
	////////////////////////////////////////////////////////////

	p_rtps_readerproxy->is_heartbeatsend_and_wait_for_ack = true;



	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		is_send_InfoDst = false;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}else{
		//p_data->reader_id.value = ENTITYID_UNKNOWN;
		//p_data->reader_id.value = remoteReaderGuid.entityId;
		is_send_InfoDst = false;
	}

	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;

	i_size = BoundarySize(sizeof(Header));

/*
	if(is_send_InfoDst && is_multicast == false)
	{ 
		// INFO_DST
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	// INFO_TS
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
*/


	// DATA
	i_inlineQos_count = count_inlineQos(p_data->inline_qos);

	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Data) + i_inlineQos_count + p_serializedPlayloadwriter->i_size);




	// HEARTBEAT
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));

	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));

/*
	if(is_send_InfoDst && is_multicast == false)
	{ 
		//INFO_DST
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//INFO_TS
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//if(p_rtps_writer->is_bultin)
	{
		char *p_value = NULL;
		int i_found = 0;
		find_ParameterList_for_remove((char*)p_serializedPlayloadwriter->p_value,p_serializedPlayloadwriter->i_size, PID_STATUS_INFO, &p_value, &i_found);
		if(i_found && p_value && i_found == 4)
		{
			if(p_value[3] == 0x03)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}else if(p_value[3] & 0x01)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}else if(p_value[3] & 0x02)
			{
				p_data->reader_id.value = ENTITYID_UNKNOWN;
				remove_data = true;
			}
		}
	}

	//added by kyy(Presentation QoS Writer Side)
	// is_data_empty 추가
	if(remove_data || is_data_empty==true)
	{
		flags = 0x03;  //Q
	}else if (i_inlineQos_count){
		flags = 0x07; //remove
	}else{
		flags = 0x05; //add
	}

	if(!remove_data)
	{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA, flags, p_serializedPlayloadwriter->i_size+sizeof(Data));
		p_senddata->i_size -= i_inlineQos_count;
	}
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp, p_data, sizeof(Data));
	i_temp += sizeof(Data);

	//inline_qos
	if(!remove_data)
		i_temp += Mapping_InlineQos(p_rtps_writer, (char*)p_senddata->p_data+i_temp,p_data->inline_qos);

	//added by kyy(Presentation QoS Writer Side) 이부분 주석 처리하면 데이터가 저장되지 않음
	if(p_serializedPlayloadwriter->p_value != NULL)
	{
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, p_serializedPlayloadwriter->i_size);
		//i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	}

	if(!remove_data)
	{
		i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size+sizeof(Data)+i_inlineQos_count);
	}else{
		i_count += BoundarySize(sizeof(SubmessageHeader)+p_serializedPlayloadwriter->i_size+sizeof(Data));
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////HEARTBEAT//////////////////////////////////
	{

		Heartbeat a_heartBeatData;
		DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
		//send


		check_HeatBeat_Count(p_rtps_readerproxy);
		p_rtps_readerproxy->is_heartbeatsend_and_wait_for_ack = true;

		if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
		{
			a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
		{
			a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
		}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
		{
			a_heartBeatData.reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
		}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
		{
			a_heartBeatData.reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
		}else{
			//a_heartBeatData.reader_id.value = ENTITYID_UNKNOWN;
			//[120212,김경일] INFO_DST와 함께 보내는 HeartBeat는 ReaderId값을 채워서 보내야 하는 것 같다. (RTI 패킷분석을 참고)
			a_heartBeatData.reader_id.value = p_rtps_readerproxy->remote_reader_guid.entity_id;
		}

		memcpy(&a_heartBeatData.writer_id, &p_rtps_writer->guid.entity_id, 4);
		
		a_heartBeatData.first_sn.value = rtps_historycache_get_seq_num_min(p_rtps_writer->p_writer_cache);

		//by kki...(reliability)
		// VOLATILE_DURABILITY인 경우에는 연결되기 전에 보냈던 data는 보낼 필요가 없다.
		{//by kki(130917)
			if (p_rtps_readerproxy->durability_kind == VOLATILE_DURABILITY_QOS)
			{
				int i_size, i;
				cache_for_guid* p_cache_for_guid;
				rtps_cachechange_t *p_change_atom = NULL;
				linked_list_head_t linked_list_head = rtps_readerproxy_unacked_changes(p_rtps_readerproxy, &i_size, false);

				p_change_atom = (rtps_cachechange_t *)linked_list_head.p_head_first;

				i=0;
				i_size = linked_list_head.i_linked_size;

				while (p_change_atom)
				{					
					p_cache_for_guid = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change_atom);
					if (p_cache_for_guid->is_relevant == true) break;

					i++;
					p_change_atom = (rtps_cachechange_t *)p_change_atom->p_extra;
				}

				if (i_size && i < i_size) a_heartBeatData.first_sn.value = ((rtps_cachechange_t *)linked_list_head.p_head_last)->sequence_number;

//				FREE(pp_cachechanges);
			}
		}

		if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
		{
			a_heartBeatData.last_sn.value.high = 0;
			a_heartBeatData.last_sn.value.low = 0;
		}
		else
		{
			a_heartBeatData.last_sn.value = rtps_historycache_get_seq_num_max(p_rtps_writer->p_writer_cache);
		}

		a_heartBeatData.count.value = ++((rtps_statefulwriter_t *)p_rtps_writer)->heatbeat_count;


		// HEARTBEAT

		if(is_final){
			flags = 0x03;
		}else{
			flags = 0x01;
		}


		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], HEARTBEAT , flags, sizeof(Heartbeat));
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_heartBeatData, sizeof(Heartbeat));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		p_rtps_writer->send_count++;
		p_rtps_writer->send_bytes += p_senddata->i_size;

		p_rtps_writer->send_count_throughput++;
		p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

		//printf("1 write...... \r\n");
		p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);
		//printf("2 write...... \r\n");


		p_rtps_readerproxy->is_zero = false;

	}

	///////////////////////////
	destory_datafull(p_data);


	////////////////////////////////////////////////////////

	return MODULE_SUCCESS;
}


int32_t rtps_send_data_frag_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data, Time_t message_time )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	int i_inlineQos_count;
	octet flags;
	data_t *p_senddata = NULL;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
	//time by jun
	//	Duration_t duration = current_duration();
	Time_t duration = message_time;

	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	bool is_send_InfoDst = true;
	ushort fragmentsInSubmessage;
	ushort fragmentSize;
	int i;
	DataFrag dataFrag;
	bool is_frag = false;



	if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
		is_send_InfoDst = false;
	}
	else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}
	else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}
	else if (strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		p_data->reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}
	else
	{
		//p_data->reader_id.value = ENTITYID_UNKNOWN;
		//p_data->reader_id.value = remoteReaderGuid.entityId;
		is_send_InfoDst = false;
		is_frag = true;
	}

	if (!is_frag)
	{
		return rtps_send_data_with_dst_to(p_rtps_writer, remote_reader_guid, locator, p_data, false, message_time);
	}

	if (p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;


	fragmentsInSubmessage = (p_serializedPlayloadwriter->i_size - 4) / p_rtps_writer->fragment_size;
	fragmentSize = p_serializedPlayloadwriter->i_size % p_rtps_writer->fragment_size;

	memcpy(&dataFrag,p_data,sizeof(Data));

	fragmentsInSubmessage = fragmentSize == 0 ? fragmentsInSubmessage : fragmentsInSubmessage + 1;

	for (i = 0; i < fragmentsInSubmessage; i++)
	{
		i_count = 0;
		fragmentSize = fragmentsInSubmessage == i + 1 ? (p_serializedPlayloadwriter->i_size- 4) % p_rtps_writer->fragment_size : p_rtps_writer->fragment_size;

		i_size = BoundarySize(sizeof(Header));
		dataFrag.fragment_starting_num.value = 1;
		dataFrag.fragment_size = fragmentSize;
		dataFrag.data_size = p_serializedPlayloadwriter->i_size - 4; // 4바이트는 encapsulation kind and option 값임.
		dataFrag.fragments_in_submessage = i + 1;

		if (is_send_InfoDst)
		{ 
			// INFO_DST
			i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
		}

		
		// INFO_TS
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
		// DATA_FRAG
		i_inlineQos_count = count_inlineQos(p_data->inline_qos);
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(DataFrag) + i_inlineQos_count) + 4 +fragmentSize;

		p_senddata = data_new(i_size);
		p_senddata->i_port = locator.port;
		if(locator.kind == LOCATOR_KIND_UDPv4){
			asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
		}

		Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
		i_count += BoundarySize(sizeof(Header));


		if(is_send_InfoDst)
		{ 
			//INFO_DST
			flags = 0x01;
			Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
			i_temp = i_count+(sizeof(SubmessageHeader));
			memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
			i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
		}

		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//INFO_TS
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(i_inlineQos_count)
		{
			flags = 0x07;  //Q
		}else{
			flags = 0x05;
		}

		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], DATA_FRAG, flags, 4+fragmentSize+sizeof(DataFrag)+i_inlineQos_count);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp, &dataFrag, sizeof(DataFrag));
		i_temp += sizeof(DataFrag);
		//inline_qos
		i_temp += Mapping_InlineQos(p_rtps_writer, p_senddata->p_data+i_temp,p_data->inline_qos);
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value, 4); // 4바이트는 encapsulation kind and option 값임.
		i_temp += 4;
		memcpy(p_senddata->p_data+i_temp, p_serializedPlayloadwriter->p_value+4+p_rtps_writer->fragment_size*i, fragmentSize);
		i_count += BoundarySize(sizeof(SubmessageHeader)+fragmentSize);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		p_rtps_writer->send_count++;
		p_rtps_writer->send_bytes += p_senddata->i_size;
		p_rtps_writer->send_sample_count++;
		p_rtps_writer->send_sample_bytes += p_senddata->i_size;

		p_rtps_writer->send_count_throughput++;
		p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
		p_rtps_writer->send_sample_count_throughput++;
		p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;

		p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);
	}

	
	if (p_rtps_writer->behavior_type == STATEFUL_TYPE && p_rtps_writer->reliability_level == RELIABLE)
	{
		HeartbeatFrag a_heartbeatFrag;
		a_heartbeatFrag.reader_id = p_data->reader_id;
		a_heartbeatFrag.writer_id = p_data->writer_id;
		a_heartbeatFrag.writer_sn = p_data->writer_sn;
		a_heartbeatFrag.last_fragment_num.value = fragmentsInSubmessage;
		a_heartbeatFrag.count.value = ++p_rtps_writer->i_heartbeat_frag_count;

		p_rtps_writer->send_count++;
		p_rtps_writer->send_bytes += p_senddata->i_size;


		p_rtps_writer->send_count_throughput++;
		p_rtps_writer->send_bytes_throughput += p_senddata->i_size;


		rtps_send_heartbeat_frag_with_dst_to(p_rtps_writer, remote_reader_guid, locator, a_heartbeatFrag);
	}


	///////////////////////////
	destory_datafull(p_data);

	return MODULE_SUCCESS;
}

int32_t rtps_send_heartbeat_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, Heartbeat a_heartbeat_data, bool is_final )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;


	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	i_size = BoundarySize(sizeof(Header));

	// INFO_DST
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));

	// HEARTBEAT
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	// INFO_DST
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// HEARTBEAT

	/*if(p_rtps_writer->writer_cache->i_changes){
		flags = 0x01;
	}else{
		flags = 0x03;
	}*/

	if(is_final){
		flags = 0x03;
	}else{
		flags = 0x01;
	}

	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], HEARTBEAT , flags, sizeof(Heartbeat));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&a_heartbeat_data, sizeof(Heartbeat));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);



	///////////////////////////

	return MODULE_SUCCESS;
}


int32_t rtps_send_acknack_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, AckNack a_acknack )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;
	int i_ackNack_size = 0;


	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	i_size = BoundarySize(sizeof(Header));

	// INFO_DST
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));


	// ACKNACK
	if(a_acknack.reader_sn_state.numbits == 0)
	{
		i_ackNack_size = sizeof(AckNack) - (8*sizeof(int32_t));
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}else{
		i_ackNack_size = sizeof(AckNack) - ((7-(a_acknack.reader_sn_state.numbits-1)/32)*sizeof(int32_t));
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	// INFO_DST
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// ACKNACK

	if(a_acknack.reader_sn_state.numbits == 0)
	{
		flags = 0x03;
	}else{
		flags = 0x01;
	}
	
	if(a_acknack.reader_sn_state.numbits == 0)
	{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], ACKNACK , flags, i_ackNack_size);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_acknack, i_ackNack_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], ACKNACK , flags, i_ackNack_size);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_acknack, i_ackNack_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}


	memcpy(p_senddata->p_data+i_count-4,&a_acknack.count, sizeof(int32_t));
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	///////////////////////////

	return MODULE_SUCCESS;
}


int32_t rtps_send_gap_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, Gap a_gap, SequenceNumber_t a_sequence_number )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;
	int i_gap_size = 0;
	DataWriter *p_dataWriter = NULL;


	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;

	p_dataWriter = p_rtps_writer->p_datawriter;
	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		a_gap.reader_id.value = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		a_gap.reader_id.value = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		a_gap.reader_id.value = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		a_gap.reader_id.value = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	}
	
	i_size = BoundarySize(sizeof(Header));

	// INFO_DST
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));


	// ACKNACK
	if(a_gap.gap_list.numbits == 0)
	{
		i_gap_size = sizeof(Gap) - 4;
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_gap_size);
	}else{
		i_gap_size = sizeof(Gap) - ((7-a_gap.gap_list.numbits/32)*sizeof(int32_t));
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_gap_size);
	}


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	// INFO_DST
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// GAP
	flags = 0x01;
	
	if(a_gap.gap_list.numbits == 0)
	{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], GAP , flags, sizeof(Gap)-4);
		i_temp = i_count+(sizeof(SubmessageHeader));
		a_gap.gap_list.numbits = 0;
		memcpy(p_senddata->p_data+i_temp,&a_gap, sizeof(Gap)-4);
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Gap)-4);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], GAP , flags, i_gap_size);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_gap, i_gap_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+i_gap_size);
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	///////////////////////////

	return MODULE_SUCCESS;
}


int32_t rtps_send_heartbeat_frag_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, HeartbeatFrag a_heartbeat_frag )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;


	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	i_size = BoundarySize(sizeof(Header));

	// INFO_DST
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));

	// HEARTBEAT
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Heartbeat));


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	// INFO_DST
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// HEARTBEAT_FRAG
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], HEARTBEAT_FRAG , flags, sizeof(Heartbeat));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&a_heartbeat_frag, sizeof(HeartbeatFrag));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(HeartbeatFrag));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);



	///////////////////////////

	return MODULE_SUCCESS;
}

int32_t rtps_send_nack_frag_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, NackFrag a_nack )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;
	int i_ackNack_size = 0;


	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;
	
	i_size = BoundarySize(sizeof(Header));

	// INFO_DST
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));


	// NACK
	if(a_nack.fragment_number_state.numbits == 0)
	{
		i_ackNack_size = sizeof(NackFrag) - (8*sizeof(int32_t));
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}else{
		i_ackNack_size = sizeof(NackFrag) - ((7-(a_nack.fragment_number_state.numbits-1)/32)*sizeof(int32_t));
		i_size += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	// INFO_DST
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// NACK_FRAG
	flags = 0x01;
	
	if(a_nack.fragment_number_state.numbits == 0)
	{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], NACK_FRAG , flags, i_ackNack_size);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_nack, i_ackNack_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}else{
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], NACK_FRAG , flags, i_ackNack_size);
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&a_nack, i_ackNack_size);
		i_count += BoundarySize(sizeof(SubmessageHeader)+i_ackNack_size);
	}

	memcpy(p_senddata->p_data+i_count-4,&a_nack.count, sizeof(int32_t));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;

	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;

	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	///////////////////////////

	return MODULE_SUCCESS;
}

int rtps_send_dispose_unregister_with_dst_to( rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, bool is_dispose, bool is_unregister )
{
	int i_size = 0, i_count = 0;
	int i_temp;
	octet flags;
	data_t *p_senddata = NULL;
	Duration_t duration = current_duration();
	DataWriter *p_dataWriter = p_rtps_writer->p_datawriter;
	bool is_send_InfoDst = true;
	bool remove_data = false;

	

	if(p_rtps_writer->p_accessout == NULL
		|| p_rtps_writer->p_accessout->pf_write == NULL) return MODULE_ERROR_RTPS;



	

	
	i_size = BoundarySize(sizeof(Header));

	if(is_send_InfoDst)
	{ 
		// INFO_DST
		i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	// INFO_TS
	i_size += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
	// DATA


	p_senddata = data_new(i_size);
	p_senddata->i_port = locator.port;
	if(locator.kind == LOCATOR_KIND_UDPv4){
		asprintf((char**)&p_senddata->p_address,"%d.%d.%d.%d",locator.address[12],locator.address[13],locator.address[14],locator.address[15]);
	}

	Mapping_header(p_rtps_writer, ( Header *)p_senddata->p_data);
	i_count += BoundarySize(sizeof(Header));


	if(is_send_InfoDst)
	{ 
		//INFO_DST
		flags = 0x01;
		Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_DST, flags, sizeof(GuidPrefix_t));
		i_temp = i_count+(sizeof(SubmessageHeader));
		memcpy(p_senddata->p_data+i_temp,&remote_reader_guid.guid_prefix, sizeof(GuidPrefix_t));
		i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t));
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//INFO_TS
	flags = 0x01;
	Mapping_SubmessageHeader(p_rtps_writer, (SubmessageHeader *)&p_senddata->p_data[i_count], INFO_TS, flags, 8);
	i_temp = i_count+(sizeof(SubmessageHeader));
	memcpy(p_senddata->p_data+i_temp,&duration,sizeof(Duration_t));
	i_count += BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	p_rtps_writer->send_count++;
	p_rtps_writer->send_bytes += p_senddata->i_size;
	p_rtps_writer->send_sample_count++;
	p_rtps_writer->send_sample_bytes += p_senddata->i_size;


	p_rtps_writer->send_count_throughput++;
	p_rtps_writer->send_bytes_throughput += p_senddata->i_size;
	p_rtps_writer->send_sample_count_throughput++;
	p_rtps_writer->send_sample_bytes_throughput += p_senddata->i_size;


	p_rtps_writer->p_accessout->pf_write(p_rtps_writer->p_accessout,p_senddata);


	return MODULE_SUCCESS;
}


