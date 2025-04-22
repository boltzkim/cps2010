/*
	RTSP Message parser class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"

static int EntityID_Kind_Parse(module_object_t * p_this, EntityId *p_entityID)
{
	char *p_parse = (char*)p_entityID;

	//if(IS_SAME_ENTITYID(p_parse, ENTITYID_PARTICIPANT))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_PARTICIPANT");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_TOPIC_WRITER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_TOPIC_WRITER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_TOPIC_READER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_TOPIC_READER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER");
	//}else if(IS_SAME_ENTITYID(p_parse, ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER))
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID : ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER");
	//}
	
	if(p_entityID->value.entity_kind == KIND_OF_ENTITY_UNKNOWN_USER_DEFINED)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_UNKNOWN_USER_DEFINED");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_WRITER_WITH_KEY_USER_DEFINED)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_WRITER_WITH_KEY_USER_DEFINED");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_WRITER_NO_KEY_USER_DEFINED)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_WRITER_NO_KEY_USER_DEFINED");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_READER_WITH_KEY_USER_DEFINED)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_READER_WITH_KEY_USER_DEFINED");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_READER_NO_KEY_USER_DEFINED)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_READER_NO_KEY_USER_DEFINED");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_UNKNOWN_BUILTIN)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_UNKNOWN_BUILTIN");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_WRITER_WITH_KEY_BUILTIN)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_WRITER_WITH_KEY_BUILTIN");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_WRITER_NO_KEY_BUILTIN)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_WRITER_NO_KEY_BUILTIN");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_READER_WITH_KEY_BUILTIN)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_WRITER_NO_KEY_BUILTIN");
	}else if(p_entityID->value.entity_kind == KIND_OF_ENTITY_READER_NO_KEY_BUILTIN)
	{
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS EntityID KIND: KIND_OF_ENTITY_READER_NO_KEY_BUILTIN");
	}


	return MESSAGE_OK;
}



int rtps_parse_Parameter(Parameter a_parameter, char* p_data, ParameterList *p_parameterList)
{
	int i_used = sizeof(Parameter);
	ParameterWithValue *p_parameter = malloc(sizeof(ParameterWithValue));
	memset(p_parameter, 0, sizeof(ParameterWithValue));

	p_parameter->parameter_id = a_parameter.parameter_id;
	p_parameter->length = a_parameter.length;

	if(a_parameter.parameter_id == PID_SENTINEL)
	{
		p_parameter->length  = 0;
		p_parameter->p_value = NULL;
		//i_used -= 2;
	}else{

		if(p_parameter->length >= 1024)
		{
			i_used += p_parameter->length;
			return i_used;
		}

		if(p_parameter->length)
		{
			p_parameter->p_value = malloc(p_parameter->length);
			memset(p_parameter->p_value, 0, p_parameter->length);

			memcpy(p_parameter->p_value,p_data+i_used, p_parameter->length);
		}else{
			p_parameter->p_value = NULL;
		}
	}

	if(p_parameterList) insert_linked_list((linked_list_head_t *)p_parameterList, &p_parameter->a_tom);

	

	i_used += p_parameter->length;

	///*if(a_parameter.parameter_id == PID_PAD)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PAD): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_SENTINEL)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_SENTINEL): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_USER_DATA)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_USER_DATA): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TOPIC_NAME)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TOPIC_NAME): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TYPE_NAME)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TYPE_NAME): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_GROUP_DATA)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_GROUP_DATA): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TOPIC_DATA)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TOPIC_DATA): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DURABILITY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DURABILITY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DURABILITY_SERVICE)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DURABILITY_SERVICE): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DEADINE)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DEADINE): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_LATENCY_BUDGET)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_LATENCY_BUDGET): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_LIVELINESS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_LIVELINESS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_RELIABILITY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_RELIABILITY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_LIFESPAN)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_LIFESPAN): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DESTINATION_ORDER)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DESTINATION_ORDER): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_HISTORY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_HISTORY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_RESOURCE_LIMITS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_RESOURCE_LIMITS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_OWNERSHIP)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_OWNERSHIP): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_OWNERSHIP_STRENGTH)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_OWNERSHIP_STRENGTH): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PRESENTATION)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PRESENTATION): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTITION)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTITION): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TIME_BASED_FILTER)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TIME_BASED_FILTER): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TRANSPORT_PRIORITY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TRANSPORT_PRIORITY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PROTOCOL_VERSION)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PROTOCOL_VERSION): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_VENDORID)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_VENDORID): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_UNICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_UNICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_MULTICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_MULTICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_MULTICAST_IPADDRESS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_MULTICAST_IPADDRESS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DEFAULT_UNICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DEFAULT_UNICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DEFAULT_MULTICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DEFAULT_MULTICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_UNICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_UNICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_MULTICAST_LOCATOR)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_MULTICAST_LOCATOR): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DEFAULT_UNICAST_IPADDRESS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DEFAULT_UNICAST_IPADDRESS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_DEFAULT_UNICAST_PORT)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_DEFAULT_UNICAST_PORT): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_UNICAST_IPADDRESS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_UNICAST_IPADDRESS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_UNICAST_PORT)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_UNICAST_PORT): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_MULTICAST_IPADDRESS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_MULTICAST_IPADDRESS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_METATRAFFIC_MULTICAST_PORT)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_METATRAFFIC_MULTICAST_PORT): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_EXPECTS_INLINE_QOS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_EXPECTS_INLINE_QOS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTICIPANT_MANUAL_LIVENESS_COUNT)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTICIPANT_MANUAL_LIVENESS_COUNT): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTICIPANT_BUILTIN_ENDPOINTS)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTICIPANT_BUILTIN_ENDPOINTS): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTICIPANT_LEASE_DURATION)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTICIPANT_LEASE_DURATION): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_CONTENT_FILTER_PROPERTY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_CONTENT_FILTER_PROPERTY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTICIPANT_GUID)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTICIPANT_GUID): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PARTICIPANT_ENTITYID)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PARTICIPANT_ENTITYID): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_GROUP_GUID)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_GROUP_GUID): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_GROUP_ENTITYID)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_GROUP_ENTITYID): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_BUILTIN_ENDPOINT_SET)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_BUILTIN_ENDPOINT_SET): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PROPERTY_LIST)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PROPERTY_LIST): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TYPE_MAX_SIZE_SERIALIZED)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TYPE_MAX_SIZE_SERIALIZED): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_ENTITY_NAME)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_ENTITY_NAME): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PROPERTY_LIST)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PROPERTY_LIST): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_KEY_HASH)
	//{
	//	char *p_guid = p_data+i_used;
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_KEY_HASH): %d, %s",a_parameter.length,p_guid);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_STATUS_INFO)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_STATUS_INFO): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_PERSISTENCE)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PERSISTENCE): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TYPE_CHECKSUM)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TYPE_CHECKSUM): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TYPE2_NAME)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TYPE2_NAME): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_TYPE2_CHECKSUM)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_TYPE2_CHECKSUM): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_EXPECTS_ACK)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_EXPECTS_ACK): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_MANAGER_KEY)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_MANAGER_KEY): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_SEND_QUEUE_SIZE)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_SEND_QUEUE_SIZE): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_RELIABILITY_ENABLED)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_RELIABILITY_ENABLED): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_VARGAPPS_SEQUENCE_NUMBER_LIST)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_VARGAPPS_SEQUENCE_NUMBER_LIST): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_RECV_QUEUE_SIZE)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_RECV_QUEUE_SIZE): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == PID_RELIABILITY_OFFERED)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_RELIABILITY_OFFERED): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else if(a_parameter.parameter_id == (short)PID_PRODUCT_VERSION)
	//{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(PID_PRODUCT_VERSION): %d",a_parameter.length);
	//	i_used += a_parameter.length;
	//}else{
	//	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS Parameter(UNKNOWN : %x): %d",a_parameter.parameter_id, a_parameter.length);
	//	i_used += a_parameter.length;
	//}*/

	return i_used;
}



int rtps_Header(char* p_data,int i_size, Header *p_header)
{

	if(i_size < sizeof(Header))
	{
		return MESSAGE_ERROR_HEADER;
	}
	
	if(!(p_data[0] == 'R' && p_data[1] == 'T' && p_data[2] == 'P' && p_data[3] == 'S'))
	{
		return MESSAGE_ERROR_HEADER;
	}
	
	return MESSAGE_OK;
}

int rtps_submessage_header(char* p_data,int i_size, SubmessageHeader *p_header)
{

	if(i_size < sizeof(SubmessageHeader))
	{
		return MESSAGE_ERROR_SUBMESSAGE_HEADER;
	}
	
	memcpy(p_header,p_data,sizeof(SubmessageHeader));
	
	return MESSAGE_OK;
}

/*
0...2...........8...............16.............24...............32
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| INFO_TS |X|X|X|X|X|X|I|E| octetsToNextHeader |
+---------------+---------------+---------------+---------------+
| |
+ Timestamp timestamp [ only if I==0 ] +
| |
+---------------+---------------+---------------+---------------+
*/

static int rtps_Submessage_InfoTimestamp(module_object_t * p_this, InfoTimestamp a_infoTimestamp, int i_size, uint8_t flag)
{
	uint8_t I;

	if(i_size != sizeof(InfoTimestamp))
	{
		return MESSAGE_ERROR_SIZE;
	}

	I = flag & 0x02;

	if(I){
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS InfoTimestamp : InvalidateFlag (%d)", I);
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS InfoTimestamp : (%ld, %ld)",a_infoTimestamp.timestamp.value.sec,a_infoTimestamp.timestamp.value.nanosec);
	}
	
	return MESSAGE_OK;
}


/*
0...2...........8...............16.............24...............32
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| DATA |X|X|X|X|X|D|Q|E| octetsToNextHeader |
+---------------+---------------+---------------+---------------+
| Flags extraFlags | octets_to_inline_qos |
+---------------+---------------+---------------+---------------+
| EntityId reader_id |
+---------------+---------------+---------------+---------------+
| EntityId writer_id |
+---------------+---------------+---------------+---------------+
+ SequenceNumber writer_sn +
+---------------+---------------+---------------+---------------+
~ ParameterList inline_qos [only if Q==1] ~
+---------------+---------------+---------------+---------------+
~ SerializedPayload serializedPayload [only if D==1 || K==1] ~
+---------------+---------------+---------------+---------------+
*/

DataFull *rtps_submessage_data(Data a_data, char *p_data, int i_size, uint8_t flag)
{
	uint8_t Q, D, K;
	int		i_remind = i_size;
	int		i_used = 0;
	DataFull *p_retData = malloc(sizeof(DataFull));
	memset(p_retData, 0, sizeof(DataFull));

	if(i_size < sizeof(Data))
	{
		return NULL;
	}

	Q = flag & 0x02;	//InlineQosFlag
	D = flag & 0x04;	//DataFlag
	K = flag & 0x08;	//KeyFlag

	/*
	- D=0 and K=0 means that there is no serializedPayload SubmessageElement.
	- D=1 and K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
	- D=0 and K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
	- D=1 and K=1 is an invalid combination in this version of the protocol.
	*/


	/////////////////////////////////
	memcpy(&p_retData->reader_id,&a_data.reader_id, sizeof(a_data.reader_id));
	memcpy(&p_retData->writer_id,&a_data.writer_id, sizeof(a_data.writer_id));
	p_retData->octets_to_inline_qos = a_data.octets_to_inline_qos;
	p_retData->extra_flags = a_data.extra_flags;
	p_retData->writer_sn.value.high = a_data.writer_sn.value.high;
	p_retData->writer_sn.value.low = a_data.writer_sn.value.low;

	p_retData->inline_qos.i_linked_size = 0;
	p_retData->inline_qos.p_head_first = NULL;
	p_retData->inline_qos.p_head_last = NULL;

	i_remind -= sizeof(Data);
	i_used += sizeof(Data);

	if(i_size == 72)
	{
		i_size = 72;
	}

	if(Q)
	{
		char *p_started_qos = p_data;
		int i_inline_qos_size = sizeof(Parameter);
		int ret = 0;
		//inline_qos
		do{
			
			ret = rtps_parse_Parameter(*(Parameter*)&p_started_qos[i_used],&p_started_qos[i_used], &p_retData->inline_qos);
			i_remind -= ret;
			i_used += ret;
		}while(ret != i_inline_qos_size && i_used < i_size && i_remind >= 0);
	}

	if( D == 0x00 && K == 0x00)
	{
		//No serializedData
		//added by kyy(Presentation QoS)////////////////////////////////////////////////////
		//Presentation QoS의 Coherent_Set이 SEQUENCENUMBER_UNKNOWN 일 때 no serializedData 임
		rtps_parse_serializedData(p_data+i_used,i_remind, p_retData);
		/////////////////////////////////////////////////////////////////////////////////////
		
	}else if( D && K == 0x00)
	{
		//serializedData

		if(i_remind < 0){
			if(p_retData)destory_datafull_all(p_retData);
			return NULL;
		}

		rtps_parse_serializedData(p_data+i_used,i_remind, p_retData);

	}else if( D  == 0x00 && K)
	{
		//serializedKey
	}else if( D && K)
	{
		//invalid combination
	}


	
	//a_data.reader_id.value.entity_kind;
	
	return p_retData;
}

/*
0...2...........8...............16.............24...............32
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| DATA_FRAG |X|X|X|X|X|K|Q|E| octetsToNextHeader |
+---------------+---------------+---------------+---------------+
| Flags extraFlags | octets_to_inline_qos |
+---------------+---------------+---------------+---------------+
| EntityId reader_id |
+---------------+---------------+---------------+---------------+
| EntityId writer_id |
+---------------+---------------+---------------+---------------+
+ SequenceNumber writer_sn +
+---------------+---------------+---------------+---------------+
| FragmentNumber fragmentStartingNum |
+---------------+---------------+---------------+---------------+
| ushort fragmentsInSubmessage | ushort fragmentSize |
+---------------+---------------+---------------+---------------+
| unsigned long sampleSize |
+---------------+---------------+---------------+---------------+
~ ParameterList inline_qos [only if Q==1] ~
+---------------+---------------+---------------+---------------+
~ SerializedPayload serializedPayload ~
+---------------+---------------+---------------+---------------+
*/

DataFragFull *rtps_submessage_datafrag(DataFrag a_datafrag, char *p_data, int i_size, uint8_t flag)
{
	uint8_t Q, D, K;
	int		i_remind = i_size;
	int		i_used = 0;
	DataFragFull *p_retData = malloc(sizeof(DataFragFull));

	memset(p_retData, 0, sizeof(DataFragFull));

	if(i_size < sizeof(DataFrag))
	{
		return NULL;
	}

	Q = flag & 0x02;	//InlineQosFlag
	D = flag & 0x04;	//DataFlag
	K = flag & 0x08;	//KeyFlag

	/*
	- D=0 and K=0 means that there is no serializedPayload SubmessageElement.
	- D=1 and K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
	- D=0 and K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
	- D=1 and K=1 is an invalid combination in this version of the protocol.
	*/


	/////////////////////////////////
	memcpy(&p_retData->reader_id,&a_datafrag.reader_id, sizeof(a_datafrag.reader_id));
	memcpy(&p_retData->writer_id,&a_datafrag.writer_id, sizeof(a_datafrag.writer_id));
	p_retData->octets_to_inline_qos = a_datafrag.octets_to_inline_qos;
	p_retData->extra_flags = a_datafrag.extra_flags;
	p_retData->writer_sn.value.high = a_datafrag.writer_sn.value.high;
	p_retData->writer_sn.value.low = a_datafrag.writer_sn.value.low;
	p_retData->data_size = a_datafrag.data_size;
	p_retData->fragments_in_submessage = a_datafrag.fragments_in_submessage;
	p_retData->fragment_size = a_datafrag.fragment_size;
	p_retData->fragment_starting_num = a_datafrag.fragment_starting_num;

	p_retData->inline_qos.i_linked_size = 0;
	p_retData->inline_qos.p_head_first = NULL;
	p_retData->inline_qos.p_head_last = NULL;

	i_remind -= sizeof(DataFrag);
	i_used += sizeof(DataFrag);

	if(Q)
	{
		char *p_started_qos = p_data;
		int i_inline_qos_size = sizeof(Parameter);
		int ret = 0;
		//inline_qos
		do{
			
			ret = rtps_parse_Parameter(*(Parameter*)&p_started_qos[i_used],&p_started_qos[i_used], &p_retData->inline_qos);
			i_remind -= ret;
			i_used += ret;
		}while(ret != i_inline_qos_size && i_used < i_size);
	}

	if( D == 0x00 && K == 0x00)
	{
		
	}else if( D && K == 0x00)
	{
		//serializedData

		rtps_parse_serializedDataFrag(p_data+i_used,i_remind, p_retData);

	}else if( D  == 0x00 && K)
	{
		//serializedKey
	}else if( D && K)
	{
		//invalid combination
	}


	
	//a_data.reader_id.value.entity_kind;
	
	return p_retData;
}



static void Parse_for_reader(Header	a_header, GuidPrefix_t a_remoteguidPrefix, EntityId_t a_readerid, EntityId_t a_writerid, char *p_subdata, int i_size)
{
	int size=0;
	int i;
	GUID_t a_matchedGuid;
	rtps_endpoint_t **pp_endpoint;
	rtps_reader_t *p_rtpsReader;
	data_t *p_data = data_new(i_size+sizeof(Header));
	

	memcpy(&a_matchedGuid.entity_id, &a_writerid, sizeof(EntityId_t));
	memcpy(&a_matchedGuid.guid_prefix, &a_remoteguidPrefix, sizeof(GuidPrefix_t));

	memcpy(p_data->p_data, &a_header, sizeof(Header));
	memcpy(&p_data->p_data[sizeof(Header)], p_subdata, i_size);

	if((memcmp(&a_readerid, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t)) == 0)
			|| (memcmp(&a_writerid, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(EntityId_t)) == 0))
	{
		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_reader();

		
		if(p_rtpsReader){
			//trace_msg(NULL, TRACE_LOG, "put data : rtps_get_spdp_builtin_participant_reader : %d", p_rtpsReader->p_data_fifo->i_depth);
			p_data->v_rtps_reader = p_rtpsReader;
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{
			if(p_data) data_release(p_data);
		}

	}else if((memcmp(&a_readerid, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t)) == 0)
		|| (memcmp(&a_writerid, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t)) == 0))
	{
		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();
		
		if(p_rtpsReader){
			//trace_msg(NULL, TRACE_LOG, "put data : rtps_get_spdp_Builtin_publication_reader : %d", p_rtpsReader->p_data_fifo->i_depth);
			p_data->v_rtps_reader = p_rtpsReader;
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{
			if(p_data) data_release(p_data);
		}

	}else if((memcmp(&a_readerid, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t)) == 0)
		|| (memcmp(&a_writerid, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t)) == 0))
	{
		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_subscription_reader();
		
		if(p_rtpsReader){
			//trace_msg(NULL, TRACE_LOG, "put data : rtps_get_spdp_builtin_subscription_reader : %d", p_rtpsReader->p_data_fifo->i_depth);
			p_data->v_rtps_reader = p_rtpsReader;
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{
			if(p_data) data_release(p_data);
		}

	}else if((memcmp(&a_readerid, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t)) == 0)
		|| (memcmp(&a_writerid, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t)) == 0))
	{
		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_topic_reader();
		
		if(p_rtpsReader){
			//trace_msg(NULL, TRACE_LOG, "put data : rtps_get_spdp_builtin_topic_reader : %d", p_rtpsReader->p_data_fifo->i_depth);
			p_data->v_rtps_reader = p_rtpsReader;
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{
			if(p_data) data_release(p_data);
		}

	}else if((memcmp(&a_readerid, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t)) == 0)
		|| (memcmp(&a_writerid, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t)) == 0))
	{
		p_rtpsReader = (rtps_reader_t *)rtps_get_spdp_builtin_participant_message_reader();
		
		if(p_rtpsReader){
			//trace_msg(NULL, TRACE_LOG, "put data : rtps_get_spdp_builtin_participant_message_reader");
			p_data->v_rtps_reader = p_rtpsReader;
			data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
		}else{
			if(p_data) data_release(p_data);
		}
	}else{

		

		pp_endpoint = rtps_matched_writerproxy_reader(a_matchedGuid, a_readerid, &size);


		for(i=0; i < size; i++)
		{
			p_rtpsReader = (rtps_reader_t *) pp_endpoint[i]; 

			p_data->p_address = malloc(sizeof(GuidPrefix_t));
			memcpy(p_data->p_address, &a_remoteguidPrefix, sizeof(GuidPrefix_t));

			if(i == size - 1){
				p_data->v_rtps_reader = p_rtpsReader;
				data_fifo_put(p_rtpsReader->p_data_fifo, p_data);
			}else{
				data_t *tmpdata = data_duplicate(p_data);
				tmpdata->p_address = malloc(sizeof(GuidPrefix_t));
				memcpy(tmpdata->p_address, &a_remoteguidPrefix, sizeof(GuidPrefix_t));
				p_data->v_rtps_reader = p_rtpsReader;
				tmpdata->v_rtps_reader = p_rtpsReader;
				data_fifo_put(p_rtpsReader->p_data_fifo, tmpdata);
			}
		}

		FREE(pp_endpoint);
	}
}


//처음에는 파싱하는거였으나 .. 이부분은 단지 readerEntityId및 writerEntityId만 가져오는것으로 한다.
DataFull* rtps_message_parser( module_object_t* p_this, char* p_data, int32_t i_size, GuidPrefix_t** pp_remote_guid_prefix, GuidPrefix_t** pp_dst_guid_prefix, EntityId_t** pp_reader_entity_id, EntityId_t** pp_writer_entity_id )
{
	int32_t ret;
	Header	a_header;
	DataFull	*p_ret = NULL;
	SubmessageHeader a_submessageHeader;
	int32_t		i_pos = 0;
	uint8_t E; // EndiannessFlag
	int32_t		i_remind = i_size;
	InfoDestination	a_InfoDestination;
	bool	is_find_InfoDescription = false;
	Heartbeat a_heartbeat;
	rtps_participant_t	*p_bultinParticipant;
	uint32_t submessage_types = 0;
	int32_t i_submessage_size = 0;
	Data a_data;
	DataFrag a_datafrag;
	AckNack a_AckNack;
	Pad a_pad;
	InfoSource a_infoSource;
	InfoReply a_infoReply;
	NackFrag a_nackFrag;
	Gap	a_gap;
	HeartbeatFrag a_heartBeatFrag;
	GuidPrefix_t *p_dstGuidPrefix = NULL;
	EntityId_t *p_readerEntityId = NULL;
	EntityId_t *p_writerEntityId = NULL;
	GuidPrefix_t *p_remoteGuidPrefix = NULL;

	//added by kyy(Destination Order)////////////////////////////////////////
	InfoTimestamp a_source_timestamp;
	memset(&a_source_timestamp,0,sizeof(InfoTimestamp));
	/////////////////////////////////////////////////////

	memset(&a_InfoDestination,0 , sizeof(InfoDestination));

	if(ret = rtps_Header(p_data,i_size,&a_header))
	{
		return NULL;
	}

	a_header = *(Header*)p_data;

	/*if(a_header.vendorId.vendorId[0] == 0 &&
		a_header.vendorId.vendorId[1] == 4)
	{
		return NULL;
	}*/

	i_pos += sizeof(Header);
	i_remind -= sizeof(Header);

	

	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS HEADER PARSE START");
	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS HEADER Protocol Version %d.%d",a_header.version.major,a_header.version.minor);
	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS HEADER Vendor ID %s",a_header.vendor_id.vendor_id);
	//trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS HEADER Guid Prefix %s",header.guidPrefix._guidprefix);
	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS HEADER PARSE END");

	p_bultinParticipant = get_rtps_builtin_participant();
	if(p_bultinParticipant)
	{
		if(memcmp(&p_bultinParticipant->guid.guid_prefix, &a_header.guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			return NULL;
		}
	}

	
	p_remoteGuidPrefix = malloc(sizeof(GuidPrefix_t));
	memcpy(p_remoteGuidPrefix, &a_header.guid_prefix, sizeof(GuidPrefix_t));
	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE HEADER START");

	while(i_remind > 0){

		a_submessageHeader = *(SubmessageHeader*)&p_data[i_pos];

		/*if(ret = rtps_submessage_header(&p_data[i_pos],i_size,&a_submessageHeader))
		{
			return NULL;
		}*/

		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Flag %d",a_submessageHeader.flags);
		trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE submessage_length %d",a_submessageHeader.submessage_length);

		E = a_submessageHeader.flags & 0x01;	// Endianness bit


		i_pos += sizeof(SubmessageHeader);
		i_remind -= sizeof(SubmessageHeader);

		switch(a_submessageHeader.submessage_id)
		{
			case PAD:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind PAD");
				submessage_types |= FOUND_PAD;
				a_pad = *((Pad*)&p_data[i_pos]);
				break;
			case GAP:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind GAP");
				submessage_types |= FOUND_GAP;
				a_gap = *((Gap*)&p_data[i_pos]);
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_gap.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_gap.writer_id.value, sizeof(EntityId_t));*/


				Parse_for_reader(a_header, a_header.guid_prefix, a_gap.reader_id.value, a_gap.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
				break;
			case ACKNACK:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind ACKNACK");
				//printf("ACKNACK \r\n");
				submessage_types |= FOUND_ACKNACK;
				a_AckNack = *((AckNack*)&p_data[i_pos]);
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_AckNack.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_AckNack.writer_id.value, sizeof(EntityId_t));*/
				rtps_receive_acknack_procedure(NULL, a_header.guid_prefix, a_InfoDestination, a_AckNack, a_submessageHeader.flags);
				break;
			case HEARTBEAT:
				trace_msg(OBJECT(p_this), TRACE_TRACE,"RTPS SUBMESSAGE Kind HEARTBEAT");
				submessage_types |= FOUND_HEARTBEAT;
				// 맞는 Writer를 찾아서, remote관련 refresh.... EndPoint를 찾음....
				//if(is_find_InfoDescription)
				//{
				//	memcpy(&a_heartbeat, &p_data[i_pos], sizeof(Heartbeat));
				//	rtps_receive_heartbeat_procedure2(a_InfoDestination, a_heartbeat, a_submessageHeader.flags);
				//}

				a_heartbeat = *(Heartbeat*)&p_data[i_pos];

				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_heartbeat.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_heartbeat.writer_id.value, sizeof(EntityId_t));*/


				Parse_for_reader(a_header, a_header.guid_prefix, a_heartbeat.reader_id.value, a_heartbeat.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);

				break;
			case INFO_TS:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind INFO_TS");
				submessage_types |= FOUND_INFOTIMESTAMP;
				rtps_Submessage_InfoTimestamp(p_this, *((InfoTimestamp*)&p_data[i_pos]),a_submessageHeader.submessage_length, a_submessageHeader.flags);

				//added by kyy(Destination Order : Source Timestamp)////////////////////////////////////////////////////////////////////////////////////////
				a_source_timestamp = *((InfoTimestamp*)&p_data[i_pos]);
				//InfoTimestamp 구조체에 저장 후 저장 위치 => rtps_reader->historycache->cachechange에 저장
//						printf("Source Timestamp : %d , %d \n",a_source_timestamp.timestamp.value.sec, a_source_timestamp.timestamp.value.nanosec);

				//이부분을 InfoTS에 맞게 수정해야 함
				a_data = *((Data*)&p_data[i_pos + 12]);
				//뒤의 DATA Message의 readerId, writerId 가져다 씀
				Parse_for_reader(a_header, a_header.guid_prefix, a_data.reader_id.value, a_data.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
				//				p_rtpsReader->source_timestamp.sec = a_source_timestamp.timestamp.value.sec;
				//				p_rtpsReader->source_timestamp.nanosec = a_source_timestamp.timestamp.value.nanosec;
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				break;
			case INFO_SRC:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind INFO_SRC");
				submessage_types |= FOUND_INFOSOURCE;
				a_infoSource = *((InfoSource*)&p_data[i_pos]);
				break;
			case INFO_REPLY_IP4:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind INFO_REPLY_IP4");
				submessage_types |= FOUND_INFOREPLY_IP4;
				break;
			case INFO_DST:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind INFO_DST");
				///
				a_InfoDestination = *(InfoDestination*)&p_data[i_pos];
				is_find_InfoDescription = true;
				submessage_types |= FOUND_INFODESTINATION;

				p_dstGuidPrefix = malloc(sizeof(GuidPrefix_t));
				memcpy(p_dstGuidPrefix, &a_InfoDestination.guid_prefix, sizeof(GuidPrefix_t));
				break;
			case INFO_REPLY:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind INFO_REPLY");
				submessage_types |= FOUND_INFOREPLY;
				a_infoReply = *((InfoReply*)&p_data[i_pos]);
				break;
			case NACK_FRAG:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind NACK_FRAG");
				submessage_types |= FOUND_NACKFRAG;
				a_nackFrag = *((NackFrag*)&p_data[i_pos]);
				
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_nackFrag.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_nackFrag.writer_id.value, sizeof(EntityId_t));*/

				

				Parse_for_reader(a_header, a_header.guid_prefix, a_nackFrag.reader_id.value, a_nackFrag.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
				break;
			case HEARTBEAT_FRAG:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind HEARTBEAT_FRAG");
				submessage_types |= FOUND_HEARTBEATFRAG;
				a_heartBeatFrag = *((HeartbeatFrag*)&p_data[i_pos]);
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_heartBeatFrag.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_heartBeatFrag.writer_id.value, sizeof(EntityId_t));*/

				Parse_for_reader(a_header, a_header.guid_prefix, a_heartBeatFrag.reader_id.value, a_heartBeatFrag.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
				break;
			case DATA:
				a_data = *((Data*)&p_data[i_pos]);
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind DATA");
				submessage_types |= FOUND_DATA;
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_data.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_data.writer_id.value, sizeof(EntityId_t));*/

				//if(a_submessageHeader.submessage_length == 48) 
				//{
				//	printf("first : %d, %d\r\n", a_data.writer_sn.value.low);
				//}

				//if(i_remind > 500)
				//{
				//	a_submessageHeader.submessage_length  = a_submessageHeader.submessage_length;
				//}

				//oci 땜시
				if(a_submessageHeader.submessage_length == 0)
				{
					a_submessageHeader.submessage_length = i_remind;
				}

				/*if(a_submessageHeader.submessage_length > 1000)
				{
					a_submessageHeader.submessage_length = i_remind;
				}*/

				Parse_for_reader(a_header, a_header.guid_prefix, a_data.reader_id.value, a_data.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
		
				break;
			case DATA_FRAG:
				trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE Kind DATA_FRAG");
				submessage_types |= FOUND_DATAFRAG;
				a_datafrag = *((DataFrag*)&p_data[i_pos]);
				/*p_readerEntityId = malloc(sizeof(EntityId_t));
				p_writerEntityId = malloc(sizeof(EntityId_t));
				memcpy(p_readerEntityId, &a_datafrag.reader_id.value, sizeof(EntityId_t));
				memcpy(p_writerEntityId, &a_datafrag.writer_id.value, sizeof(EntityId_t));*/

				Parse_for_reader(a_header, a_header.guid_prefix, a_datafrag.reader_id.value, a_datafrag.writer_id.value, &p_data[i_pos-sizeof(SubmessageHeader)], sizeof(SubmessageHeader) + a_submessageHeader.submessage_length);
				break;
			default:
				trace_msg(OBJECT(p_this),TRACE_ERROR,"Unknown RTPS SUBMESSAGE Kind....");
				break;
		}

		/*if(a_submessageHeader.submessage_id == INFO_SRC){
			i_pos		+= 16;
			i_remind	-= 16;
		}else{*/
			i_pos		+= a_submessageHeader.submessage_length;
			i_remind	-= a_submessageHeader.submessage_length;
		//}
	}	

	trace_msg(OBJECT(p_this),TRACE_TRACE,"RTPS SUBMESSAGE HEADER END");

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	*pp_dst_guid_prefix = p_dstGuidPrefix;
	*pp_reader_entity_id = p_readerEntityId;
	*pp_writer_entity_id = p_writerEntityId;
	*pp_remote_guid_prefix = p_remoteGuidPrefix;

	return p_ret;
}



int32_t find_parameter( Parameter a_parameter, char* p_data, ParameterId_t pid, char** pp_ret, int32_t* p_found )
{
	int32_t i_used = sizeof(Parameter);
	//added by kjh
	Locator_t* temp_addr;
	char* buff = p_data;

	if (a_parameter.parameter_id == PID_SENTINEL)
	{
		*p_found = -1;

	}
	else if (a_parameter.parameter_id == pid)
	{
		//added by kjh
		if (pid == PID_DEFAULT_UNICAST_LOCATOR)
		{
		//	printf("PID_DEFAULT_UNICAST_LOCATOR\n");
			buff += sizeof(Parameter);
			temp_addr = (Locator_t*)buff;			
			//buff -= sizeof(Parameter);
			if (temp_addr->kind == 1)
			{
				//printf("PID_DEFAULT_UNICAST_LOCATOR KIND : %d\n", temp_addr->kind);
				*p_found = a_parameter.length;
				*pp_ret = p_data + sizeof(Parameter);
			}			
			else if (temp_addr->kind == 2)
			{
				//printf("PID_DEFAULT_UNICAST_LOCATOR KIND : %d\n", temp_addr->kind);
			}
		}
		else if (pid == PID_METATRAFFIC_UNICAST_LOCATOR)
		{
		//	printf("PID_METATRAFFIC_UNICAST_LOCATOR\n");
			buff += sizeof(Parameter);
			temp_addr = (Locator_t*)buff;			
			//buff -= sizeof(Parameter);
			if (temp_addr->kind == 1)
			{
				//printf("PID_METATRAFFIC_UNICAST_LOCATOR KIND : %d\n", temp_addr->kind);
				*p_found = a_parameter.length;
				*pp_ret = p_data + sizeof(Parameter);
			}			
			else if (temp_addr->kind == 2)
			{
				//printf("PID_METATRAFFIC_UNICAST_LOCATOR KIND : %d\n", temp_addr->kind);
			}
		}else if (pid == PID_UNICAST_LOCATOR)
		{
			buff += sizeof(Parameter);
			temp_addr = (Locator_t*)buff;			
			if (temp_addr->kind == 1)
			{
				*p_found = a_parameter.length;
				*pp_ret = p_data + sizeof(Parameter);
			}			
			else if (temp_addr->kind == 2)
			{
			}
		}else if (pid == PID_MULTICAST_LOCATOR)
		{
			buff += sizeof(Parameter);
			temp_addr = (Locator_t*)buff;			
			if (temp_addr->kind == 1)
			{
				*p_found = a_parameter.length;
				*pp_ret = p_data + sizeof(Parameter);
			}			
			else if (temp_addr->kind == 2)
			{
			}
		}
		else 
		{
			*p_found = a_parameter.length;
			*pp_ret = p_data + sizeof(Parameter);
		}
	}

	i_used += a_parameter.length;

	return i_used;
}

void find_in_parameter_list_pl_cdr_le( char* p_buf, int32_t i_size, ParameterId_t pid, char** pp_ret, int32_t* p_found )
{
	char	*p_started_parameter = p_buf;
	int32_t		i_parameter_size = sizeof(Parameter);
	int32_t		ret = 0;
	int32_t		i_remind = i_size;
	int32_t		i_used = 0;

	do {
		ret = find_parameter(*(Parameter*)&p_started_parameter[i_used],&p_started_parameter[i_used], pid, pp_ret, p_found);
		i_remind -= ret;
		i_used += ret;
		if (*p_found)
		{
			if (*p_found == -1)
			{
				*p_found = 0;
			}

			break;
		}
	} while (ret != i_parameter_size && i_used < i_size);
}

void find_parameter_list(char *p_buf, int32_t i_size, ParameterId_t pid, char **pp_ret, int32_t *i_found)
{
	int i_count = 0;
	int16_t	i_Encapsulation_Scheme_Identifier;

	if (i_size < 4) return;

	i_Encapsulation_Scheme_Identifier = (p_buf[0] << 8 & 0xff00) + (p_buf[1] &0xff);
	i_count += 4;

	if (i_Encapsulation_Scheme_Identifier == PL_CDR_LE)
	{
		find_in_parameter_list_pl_cdr_le(p_buf+4 , i_size-4, pid, pp_ret, i_found);
	}
}

void find_ParameterList_for_remove(char *p_buf, int32_t i_size, ParameterId_t pid, char **pp_ret, int32_t *i_found)
{
	//added by kyy(Presentation QoS Writer Side)
	//remove 명령은 Data가 NULL이 아닐 때로 한정지어야 한다.
	if(p_buf != NULL)
	///////////////////////////////////////////////////////////////////////////
		find_in_parameter_list_pl_cdr_le(p_buf , i_size, pid, pp_ret, i_found);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


