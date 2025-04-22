/*
RTSP Reader class
작성자 : 
이력
2010-08-10 : 처음 시작
*/

/*
Specialization of RTPS Endpoint representing the objects that can be used to receive
messages communicating CacheChanges.
*/

#include "rtps.h"


static uint8_t entity_id_check = 0;
static mutex_t				structure_lock;
static bool structure_lock_initialize = false;

static uint8_t Default_Reader_Entity_id_get()
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


static int Reader_Thread(rtps_reader_t *p_rtpsReader);

void rtps_reader_init(rtps_reader_t* p_rtps_reader)
{
	init_rtps_endpoint((rtps_endpoint_t *)p_rtps_reader);

	p_rtps_reader->heartbeat_response_delay = TIME_ZERO;
	p_rtps_reader->heartbeat_suppression_duration = TIME_ZERO;
	p_rtps_reader->p_reader_cache = rtps_historycache_new();
	p_rtps_reader->p_reader_cache->p_rtps_reader = p_rtps_reader;
	p_rtps_reader->expects_inline_qos = false;
	p_rtps_reader->p_datareader = NULL;
	p_rtps_reader->heartbeat_response_delay.nanosec = 500 * 1000 * 1000; // 500 milliseconds
	//
	mutex_init(&p_rtps_reader->object_lock );
	cond_init(&p_rtps_reader->object_wait );
	p_rtps_reader->b_end = false;
	p_rtps_reader->behavior_type = DEFAULT_TYPE;
	p_rtps_reader->p_data_fifo = data_fifo_new();
	p_rtps_reader->count = 0;
	p_rtps_reader->i_entity_type = READER_ENTITY;

	p_rtps_reader->pp_datafragfull = NULL;
	p_rtps_reader->i_datafragfull = 0;

	p_rtps_reader->pp_exclusive_writers = NULL; //by kki (ownership)
	p_rtps_reader->i_exclusive_writers = 0; //by kki (ownership)

	p_rtps_reader->received_count = 0;
	p_rtps_reader->received_bytes = 0;
	p_rtps_reader->received_sample_count = 0;
	p_rtps_reader->received_sample_bytes = 0;

	p_rtps_reader->received_count_throughput = 0;
	p_rtps_reader->received_bytes_throughput = 0;
	p_rtps_reader->received_sample_count_throughput = 0;
	p_rtps_reader->received_sample_bytes_throughput = 0;
}


rtps_reader_t* rtps_reader_new()
{
	rtps_reader_t* p_rtpsReader = malloc(sizeof(rtps_reader_t));

	memset(p_rtpsReader, 0, sizeof(rtps_reader_t));

	init_rtps_endpoint((rtps_endpoint_t *)p_rtpsReader);
	rtps_reader_init(p_rtpsReader);

	return p_rtpsReader;
}


void rtps_reader_destory(rtps_reader_t* p_rtps_reader)
{
	//	data_t *p_data;

	if (p_rtps_reader == NULL) return;

	p_rtps_reader->b_end = true;

	/*
	p_data = malloc(sizeof(data_t));
	memset(p_data,'\0',sizeof(data_t));
	dataFifoPut(p_rtps_reader->p_data_fifo, p_data);

	thread_join2( p_rtps_reader->thread_id );
	*/

	if (p_rtps_reader->behavior_type == STATELESS_TYPE)
	{
		rtps_statelessreader_destory((rtps_statelessreader_t *)p_rtps_reader);
	}
	else if (p_rtps_reader->behavior_type == STATEFUL_TYPE)
	{
		rtps_statefulreader_destory((rtps_statefulreader_t *)p_rtps_reader);
	}

	/*

	if(p_rtps_reader->p_data_fifo)dataFifoRelease(p_rtps_reader->p_data_fifo);
	*/
	rtps_historycache_destory(p_rtps_reader->p_reader_cache);
	destroy_endpoint((rtps_endpoint_t *)p_rtps_reader);
	mutex_destroy(&p_rtps_reader->object_lock );
	cond_destroy(&p_rtps_reader->object_wait );
	//FREE(p_rtps_reader);
}


static SerializedPayloadForWriter* rtps_generate_subscriptionInfo(DataReader* p_datareader, rtps_reader_t* p_rtps_reader, rtps_writer_t* p_writer)
{
	int i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
	//	ProtocolVersion_t	protocol = p_rtps_participant->protocolVersion;

	uint32_t endpoint_set = 0x00000000; 
	char* p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter* p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;
	char* p_temp;
	TopicDescription* p_topic_description = NULL;
	Subscriber* p_subscriber = p_datareader->get_subscriber(p_datareader);

	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if (p_writer->p_accessout)
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

	p_topic_description = p_datareader->get_topicdescription(p_datareader);

	memcpy(&p_serialized_data->entity_key_guid, &p_rtps_reader->guid, sizeof(GUID_t));

	if (strcmp(p_topic_description->topic_name, PUBLICATIONINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_reader->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}
	else if (strcmp(p_topic_description->topic_name, SUBSCRIPTIONINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_reader->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}
	else if (strcmp(p_topic_description->topic_name, TOPICINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_reader->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}
	else if (strcmp(p_topic_description->topic_name, LIVELINESSP2P_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_reader->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}

	//PID_ENDPOINT_GUID
	gererate_parameter(p_serialized+i_size, &i_size, PID_ENDPOINT_GUID, 16, &p_rtps_reader->guid);

	//PID_TOPIC_NAME
	if (p_topic_description->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_datareader->p_topic;
		p_temp = get_string(p_contentFilteredTopic->get_related_topic(p_contentFilteredTopic)->topic_name, &i_temp);
	}
	else if(p_topic_description->topic_type == MULTITOPIC_TYPE)
	{
		assert(false);
	}
	else
	{
		p_temp = get_string(p_topic_description->topic_name, &i_temp);
	}

	gererate_parameter(p_serialized+i_size, &i_size, PID_TOPIC_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_TYPE_NAME
	p_temp = get_string(p_topic_description->type_name, &i_temp);

	gererate_parameter(p_serialized+i_size, &i_size, PID_TYPE_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_DURABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_DURABILITY, sizeof(DurabilityQosPolicy), &p_datareader->datareader_qos.durability);

	//PID_DEADINE
	gererate_parameter(p_serialized+i_size, &i_size, PID_DEADINE, sizeof(DeadlineQosPolicy), &p_datareader->datareader_qos.deadline);

	//PID_LATENCY_BUDGET
	gererate_parameter(p_serialized+i_size, &i_size, PID_LATENCY_BUDGET, sizeof(LatencyBudgetQosPolicy), &p_datareader->datareader_qos.latency_budget);

	//PID_LIVELINESS
	gererate_parameter(p_serialized+i_size, &i_size, PID_LIVELINESS, sizeof(LivelinessQosPolicy), &p_datareader->datareader_qos.liveliness);

	//PID_RELIABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_RELIABILITY, sizeof(ReliabilityQosPolicy), &p_datareader->datareader_qos.reliability);

	//PID_OWNERSHIP
	gererate_parameter(p_serialized+i_size, &i_size, PID_OWNERSHIP, sizeof(OwnershipQosPolicy), &p_datareader->datareader_qos.ownership);

	//PID_DESTINATION_ORDER
	gererate_parameter(p_serialized+i_size, &i_size, PID_DESTINATION_ORDER, sizeof(DestinationOrderQosPolicy), &p_datareader->datareader_qos.destination_order);

	//PID_PRESENTATION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PRESENTATION, sizeof(PresentationQosPolicy), &p_subscriber->subscriber_qos.presentation);

	//PID_EXPECTS_INLINE_QOS
	gererate_parameter(p_serialized+i_size, &i_size, PID_EXPECTS_INLINE_QOS, 4, p_serialized+i_size+10);


	//PID_PROTOCOL_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PROTOCOL_VERSION, 4, &protocol);

	//PID_VENDORID
	gererate_parameter(p_serialized+i_size, &i_size, PID_VENDORID, 4, &p_writer->p_rtps_participant->vendor_id);

	//PID_PRODUCT_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PRODUCT_VERSION, 4, &product_version);

	//added by kyy(Partition QoS)
	//PID_PARTITION(DataReader)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_partition(p_serialized,p_subscriber->subscriber_qos.partition,i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//added by kyy(UserData QoS)
	//PID_USER_DATA(DataReader)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_user_data(p_serialized, p_datareader->datareader_qos.user_data, i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//PID_TOPIC_DATA(DataReader)///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//added by kyy(Content based Filter)
	if (p_topic_description->topic_type == CONTENTFILTER_TOPIC_TYPE)
	{
		ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_datareader->p_topic;
		i_size = qos_generate_topic_data(p_serialized, p_contentFilteredTopic->p_related_topic->topic_qos.topic_data, i_size);
	}
	else if(p_topic_description->topic_type == MULTITOPIC_TYPE)
	{
		assert(false);
	}
	else
	{
		i_size = qos_generate_topic_data(p_serialized, p_datareader->p_topic->topic_qos.topic_data, i_size);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//PID_GROUP_DATA(DataReader)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_group_data(p_serialized, p_subscriber->subscriber_qos.group_data,i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//PID_TIME_BASED_FILTER(DataReader)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	gererate_parameter(p_serialized+i_size, &i_size, PID_TIME_BASED_FILTER, sizeof(TimeBasedFilterQosPolicy), &p_datareader->datareader_qos.time_based_filter);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


static SerializedPayloadForWriter* rtps_generate_subscriptionInfo_remove(DataReader* p_datareader, rtps_reader_t* p_rtpsReader, rtps_writer_t* p_writer)
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;
	//	ProtocolVersion_t	protocol = p_rtps_participant->protocolVersion;

	uint32_t endpoint_set = 0x00000000; 
	char* p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	int32_t i_temp = 0;
	TopicDescription *p_topicDescription = NULL;
	Subscriber *p_subscriber = p_datareader->get_subscriber(p_datareader);
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
	gererate_parameter(p_serialized+i_size, &i_size, PID_KEY_HASH, 16, &p_rtpsReader->guid);

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


void rtps_added_user_defined_reader(DataReader* p_datareader, rtps_reader_t* p_rtps_reader)
{
///	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//builtinSubscripton을 찾자.
	rtps_statefulwriter_t* p_statefulwriter = (rtps_statefulwriter_t*)rtps_get_spdp_builtin_subscription_writer();
	if (p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
///	p_builtin_datawriter = p_statefulwriter->p_datawriter;
	p_serialized_data = rtps_generate_subscriptionInfo(p_datareader, p_rtps_reader, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin((rtps_writer_t *)p_statefulwriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


//[KKI] EDP로 DATA(r[UD])메시지가 날아감.
void rtps_remove_user_defined_Reader(rtps_reader_t* p_rtps_reader)
{
///	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//butinSubscripton을 찾자.
	rtps_statefulwriter_t *p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_subscription_writer();
	if (p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
///	p_builtin_datawriter = p_statefulwriter->p_datawriter;
	p_serialized_data = rtps_generate_subscriptionInfo_remove(p_rtps_reader->p_datareader, p_rtps_reader, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin_rapidly((rtps_writer_t *)p_statefulwriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


bool add_writerproxy_to_reader(rtps_reader_t* p_rtpsReader, SerializedPayload* p_data_value)
{
	SerializedPayloadForReader* p_serialized = NULL;
	int32_t	i_size = 0;
	char* p_value = NULL;
	int32_t	i_found = 0;
	GUID_t	a_remote_endpoint_guid;
	char* p_topic_name = NULL;
	char* p_type_name = NULL;
	bool is_builtin = false;
	bool is_added = false;

	if (p_rtpsReader->behavior_type == STATEFUL_TYPE)
	{
		if (p_data_value)
		{
			p_serialized = (SerializedPayloadForReader *)p_data_value;
			i_size = p_serialized->i_size;
		}
		else
		{
			trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] p_data_value is null");
			return false;
		}

		find_parameter_list((char*)p_serialized->p_value,i_size, PID_ENDPOINT_GUID, &p_value, &i_found);
		if (i_found && p_value)
		{
			a_remote_endpoint_guid = *(GUID_t*)p_value;
			i_found = 0;

			find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
			if (i_found && p_value)
			{
				i_found = 0;
				p_topic_name = p_value+4;

				find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
				if (i_found && p_value)
				{
					p_type_name = p_value+4;

					if (have_topic_name_topic_type_reader(p_rtpsReader, p_topic_name, p_type_name))
					{
						rtps_endpoint_t** pp_rtps_endpoint =  NULL;
						int ii_size = 0;
						int i;
						GUID_t guid;
						rtps_reader_t* p_rtps_reader;

						RxOQos a_pub_rxo_qos;
						RxOQos a_sub_rxo_qos;
						RxOQos a_requested_qos;
						RxOQos a_offered_qos;

						PublisherQos pqos;

						memset(&a_pub_rxo_qos, 0, sizeof(RxOQos));
						memset(&a_sub_rxo_qos, 0, sizeof(RxOQos));
						memset(&a_requested_qos, 0, sizeof(RxOQos));
						memset(&a_offered_qos, 0, sizeof(RxOQos));


						memset(&guid,0,sizeof(GUID_t));

						static_get_default_publisher_qos(&pqos);

						find_publisher_qos_from_parameterlist(&pqos, (char*)p_serialized->p_value, i_size);


						p_type_name = p_value+4;

						pp_rtps_endpoint = rtps_compare_reader(guid, p_topic_name, p_type_name, &ii_size);
						find_rxo_qos_from_parameterlist(&a_pub_rxo_qos, p_serialized->p_value, i_size);

						for(i=0; i < ii_size; i++)
						{

							p_rtps_reader = (rtps_reader_t *)pp_rtps_endpoint[i];
							//printf("ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER : %s\r\n", p_topic_name);
							if(p_rtps_reader && p_rtps_reader->behavior_type == STATEFUL_TYPE)
							{
								Locator_t *remoteLocator = get_default_remote_participant_locator(a_remote_endpoint_guid);
								Locator_t *remoteMulticastLocator = get_default_remote_participant_multicast_locator(a_remote_endpoint_guid);
								rtps_writerproxy_t *p_rtpswriterproxy = NULL;
								bool equal_parition = true;
								bool compare_value;

								//////////////////////////////////////////////////////////////////////////////////////////////////
								//added by kyy (RxO Compare : requested vs offered)
								//RxO Compare
								/*p_pub_rxo_qos = malloc(sizeof(RxOQos));
								p_requested_qos	= malloc(sizeof(RxOQos));*/



								get_requested_qos(&a_requested_qos, p_rtps_reader);
								compare_value = compare_rxo(&a_pub_rxo_qos,&a_requested_qos);
								/*FREE(p_pub_rxo_qos);
								FREE(p_requested_qos);*/
								///////////////////////////////////////////////////////////////////////////////////////////////


								if(p_rtps_reader->p_datareader && p_rtps_reader->p_datareader->p_subscriber){
									equal_parition = qos_compare_partition(pqos.partition, p_rtps_reader->p_datareader->p_subscriber->subscriber_qos.partition);
									if (equal_parition == false) trace_msg(NULL, TRACE_LOG, "Partition Mismatched");
								}

								REMOVE_STRING_SEQ(pqos.partition.name);

								if(equal_parition && remoteLocator && compare_value)
								{
									rtps_statefulwriter_t *p_statefulwriter = NULL;
									Locator_t *p_unicatlocator = (Locator_t *)malloc(sizeof(Locator_t));
									Locator_t *p_multicastlocator = (Locator_t *)malloc(sizeof(Locator_t));
									bool b_ret;

									memset(p_unicatlocator, 0, sizeof(Locator_t));
									memset(p_multicastlocator, 0, sizeof(Locator_t));

									i_found=0;

									find_parameter_list((char*)p_serialized->p_value,i_size, PID_UNICAST_LOCATOR, &p_value, &i_found);

									if(i_found == 24){
										memcpy(p_unicatlocator, p_value, sizeof(Locator_t));
									}else{
										memcpy(p_unicatlocator, remoteLocator, sizeof(Locator_t));
									}

									i_found=0;

									find_parameter_list((char*)p_serialized->p_value,i_size, PID_MULTICAST_LOCATOR, &p_value, &i_found);

									if(i_found == 24){
										memcpy(p_multicastlocator, p_value, sizeof(Locator_t));
									}else{
										memcpy(p_multicastlocator, remoteMulticastLocator, sizeof(Locator_t));
									}


									p_rtpswriterproxy = rtps_writerproxy_new(p_rtps_reader, a_remote_endpoint_guid, p_unicatlocator, p_multicastlocator);
									qos_writerproxy_set_liveliness_kind_and_lease_duration(p_rtpswriterproxy, a_pub_rxo_qos);//by kki (liveliness)
									find_parameter_list(p_serialized->p_value, p_serialized->i_size, PID_OWNERSHIP_STRENGTH, &p_value, &i_found);
									if (i_found && p_value)
									{
										p_rtpswriterproxy->ownership_strength = *(int32_t*)p_value;
									}

									b_ret = rtps_statefulreader_matched_writer_add((rtps_statefulreader_t *)p_rtps_reader, p_rtpswriterproxy);

									/// 연관된 Builtin writer 깨우기
									/*p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_SPDPBultinPublicationWriter();

									if(p_statefulwriter){
									p_statefulwriter->heartbeatPeriod.sec = 1;
									cond_signal(&p_statefulwriter->object_wait);
									}*/
									//WakeUpBuiltinPub_and_Sub();
									if(b_ret){
										is_added = true;
										trace_msg(NULL, TRACE_LOG, "Define Add The WriterProxy(%d)[%s:%d] to a Reader.[%d.%d.%d.%d]", ii_size,  p_topic_name, ((rtps_statefulreader_t *)p_rtps_reader)->i_matched_writers , remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
									}else{
										trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] to a Reader.[%d.%d.%d.%d] proxy exist.", p_topic_name, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
									}


								}else{
									trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] to a Reader.[%d.%d.%d.%d]", p_topic_name, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
								}

							}else
							{

							}

						}

						if(ii_size ==0) trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] to a Reader: No Matched proxy does not exist!!", p_topic_name);

						FREE(pp_rtps_endpoint);
					}else{
						//trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] have_topic_name_topic_type_reader is false", p_topic_name);
					}
				}else{
					trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy[%s] PID_TYPE_NAME is null", p_topic_name);
				}
			}else{
				trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy PID_TOPIC_NAME is null");
			}
		}else{
			trace_msg(NULL, TRACE_LOG, "Define Couldn't add The WriterProxy end_point is null");
		}
	}

	return is_added;
}


////BultinPublicationReader에 있는 history캐쉬에서 연관되는 Entity를 찾아 rtps_writerproxy_new를 이용해서 추가 시키는 함수 
void rtps_add_writerproxies_to_reader(rtps_reader_t* p_rtps_reader)
{
	rtps_reader_t *p_rtps_builtin_reader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();
	rtps_cachechange_t *p_change_atom = NULL;
	int i_size;

	//msleep(100000);

	while(get_define_user_entity_lock())
	{
		trace_msg(NULL, TRACE_LOG, "Define_UserEntity_Lock sleep reader");
		msleep(1000);
	}

//	HISTORYCACHE_LOCK(p_rtps_builtin_reader->p_reader_cache);
	i_size = p_rtps_builtin_reader->p_reader_cache->i_linked_size;
	trace_msg(NULL, TRACE_LOG, "\r\nDefine before Add WriterProxy[%s] to a Reader: %d", p_rtps_reader->p_datareader->p_topic->topic_name, i_size);

	p_change_atom = (rtps_cachechange_t *)p_rtps_builtin_reader->p_reader_cache->p_head_first;

	while(p_change_atom)
	{
		rtps_cachechange_t* cachechange = p_change_atom;
		if (cachechange && cachechange->p_data_value)
		{
			if (cachechange->b_isdisposed == false)
			{
				add_writerproxy_to_reader(p_rtps_reader, cachechange->p_data_value);
			}
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
//	HISTORYCACHE_UNLOCK(p_rtps_builtin_reader->p_reader_cache);

	trace_msg(NULL, TRACE_LOG, "Define finish Add WriterProxy[%s] to a Reader: %d", p_rtps_reader->p_datareader->p_topic->topic_name, p_rtps_builtin_reader->p_reader_cache->i_linked_size);
}


bool get_define_user_entity_lock();

int32_t rtps_add_datareader(module_object_t* p_this, DataReader* p_datareader)
{
	rtps_reader_t *p_rtps_reader = NULL;
	bool is_user_defined = false;

	if (strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, PARTICIPANTINFO_NAME) == 0)
	{
		//일단 여기서 Builtin인지 판별 그러면 stateless로....

		p_rtps_reader = (rtps_reader_t *)rtps_statelessreader_new();
		p_rtps_reader->topic_kind = WITH_KEY;
		p_rtps_reader->expects_inline_qos = true;
		set_unicast_and_multicast_spdp(p_this,  (rtps_endpoint_t *)p_rtps_reader);
		memcpy(&p_rtps_reader->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(EntityId_t));


		set_participant_reader(p_rtps_reader);
	}
	else if (strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, PUBLICATIONINFO_NAME) == 0)
	{
		rtps_statefulreader_t *p_statefulreader = rtps_statefulreader_new(p_this);
		p_rtps_reader = (rtps_reader_t *)p_statefulreader;
		p_rtps_reader->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_reader);
		memcpy(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t));
		p_rtps_reader->expects_inline_qos = false;
		set_pub_reader(p_rtps_reader);
	}
	else if (strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, SUBSCRIPTIONINFO_NAME) == 0)
	{
		rtps_statefulreader_t *p_statefulreader = rtps_statefulreader_new(p_this);
		p_rtps_reader = (rtps_reader_t *)p_statefulreader;
		p_rtps_reader->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_reader);
		memcpy(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t));
		p_rtps_reader->expects_inline_qos = true;
		set_sub_reader(p_rtps_reader);
	}
	else if (strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, TOPICINFO_NAME) == 0)
	{
		rtps_statefulreader_t *p_statefulreader = rtps_statefulreader_new(p_this);
		p_rtps_reader = (rtps_reader_t *)p_statefulreader;
		p_rtps_reader->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_reader);
		memcpy(&p_rtps_reader->guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t));
		p_rtps_reader->expects_inline_qos = true;
		set_topic_reader(p_rtps_reader);
	}
	else if (strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, LIVELINESSP2P_NAME) == 0)
	{
		rtps_statefulreader_t *p_statefulreader = rtps_statefulreader_new(p_this);
		p_rtps_reader = (rtps_reader_t *)p_statefulreader;
		p_rtps_reader->topic_kind = WITH_KEY;
		set_unicast_and_multicast_spdp(p_this,  (rtps_endpoint_t *)p_rtps_reader);
		memcpy(&p_rtps_reader->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t));
		p_rtps_reader->expects_inline_qos = true;
		set_liveness_reader(p_rtps_reader);
	}
	else
	{
		EntityId_t a_userEntityId;
		int i;
		FooTypeSupport *p_typeSupport = NULL;
		Topic* p_topic = p_datareader->p_topic;
		bool is_foundkey = false;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		rtps_statefulreader_t* p_statefulreader = rtps_statefulreader_new(p_this);
		p_rtps_reader = (rtps_reader_t *)p_statefulreader;

		p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->p_domain_participant /*p_topic->get_participant(p_topic)*/, p_topic->type_name);

		for (i = 0; i < p_typeSupport->i_parameters; i++)
		{
			if (p_typeSupport->pp_parameters[i]->is_key)
			{
				is_foundkey = true;
				break;
			}
		}

		if (is_foundkey)
		{
			p_rtps_reader->topic_kind = WITH_KEY;
		}
		else
		{
			p_rtps_reader->topic_kind = NO_KEY;
		}

		a_userEntityId.entity_key[0] = 0; //Default_Reader_Entity_id_get();
		a_userEntityId.entity_key[1] = Default_Reader_Entity_id_get();
		a_userEntityId.entity_key[2] = 0;
		a_userEntityId.entity_kind = KIND_OF_ENTITY_READER_WITH_KEY_USER_DEFINED;
		memcpy(&p_rtps_reader->guid.entity_id, &a_userEntityId, sizeof(EntityId_t));

		//User-defined Reader가 생성되었기 때문에 BultinSubscription에게 알려주자..
		rtps_added_user_defined_reader(p_datareader, p_rtps_reader);

		p_rtps_reader->expects_inline_qos = false;
		is_user_defined = true;
	}

	p_rtps_reader->p_reader_cache->history_kind = p_datareader->datareader_qos.history.kind;

	if (p_datareader->datareader_qos.history.depth)
	{
		p_rtps_reader->p_reader_cache->history_depth = p_datareader->datareader_qos.history.depth;
	}
	//by kki...(history && reliability)
	p_rtps_reader->p_reader_cache->history_max_length = qos_get_history_max_length(&p_datareader->datareader_qos.history, &p_datareader->datareader_qos.resource_limits);
	p_rtps_reader->p_reader_cache->reliability_kind = p_datareader->datareader_qos.reliability.kind;

	if (is_user_defined)
	{
		p_rtps_reader->is_builtin = false;
	}
	else
	{
		p_rtps_reader->is_builtin = true;
	}

	if (p_rtps_reader)
	{
		if (p_datareader->datareader_qos.reliability.kind == BEST_EFFORT_RELIABILITY_QOS)
		{
			p_rtps_reader->reliability_level = BEST_EFFORT;
		}
		else if (p_datareader->datareader_qos.reliability.kind == RELIABLE_RELIABILITY_QOS)
		{
			p_rtps_reader->reliability_level = RELIABLE;
		}
		else
		{
			p_rtps_reader->reliability_level = RELIABLE;
		}
	}


	p_datareader->p_related_rtps_reader = p_rtps_reader;
	p_rtps_reader->p_datareader = p_datareader;

	if(p_datareader && p_datareader->p_subscriber && p_datareader->p_subscriber->p_domain_participant)
	{
		if(rtps_add_endpoint_to_participant(p_this, (rtps_endpoint_t*)p_rtps_reader, p_datareader->p_subscriber->p_domain_participant) != MODULE_SUCCESS)
		{
			trace_msg(p_this, TRACE_ERROR,"RTPS cannot add EndPoint(Reader) To Participant..");
			FREE(p_rtps_reader);
			return MODULE_ERROR_RTPS;
		}
	}

	if(is_user_defined)
	{
		////BultinPublicationReader에 있는 history캐쉬에서 연관되는 Entity를 찾아 rtps_writerproxy_new를 이용해서 추가 시키자.
		rtps_add_writerproxies_to_reader(p_rtps_reader);
		////{
		////	rtps_reader_t *p_rtpsBultinReader = (rtps_reader_t *)rtps_get_spdp_Builtin_publication_reader();
		////	int i;
		////	bool is_added = false;
		////	int i_size;

		////	//msleep(100000);

		////	while(get_define_user_entity_lock())
		////	{
		////		trace_msg(NULL, TRACE_LOG, "Define_UserEntity_Lock sleep reader");
		////		msleep(10000);
		////	}

		////	HISTORYCACHE_LOCK(p_rtpsBultinReader->p_reader_cache);
		////	i_size = p_rtpsBultinReader->p_reader_cache->i_changes;
		////	trace_msg(NULL, TRACE_LOG, "Define before Add WriterProxy[%s] to a Reader: %d", p_datareader->p_topic->topic_name, i_size);

		////	for (i = 0; i < i_size; i++)
		////	{
		////		rtps_cachechange_t		*cachechange = p_rtpsBultinReader->p_reader_cache->pp_changes[i];

		////		//trace_msg(NULL, TRACE_LOG, "------------------------- %d", i);
		////		if (cachechange && cachechange->p_data_value)
		////		{
		////			if (cachechange->b_isdisposed == false)
		////			{
		////				is_added = add_writerproxy_to_reader(p_rtps_reader, cachechange->p_data_value);
		////				//	if(is_added) break;
		////			}
		////		}
		////	}
		////	HISTORYCACHE_UNLOCK(p_rtpsBultinReader->p_reader_cache);

		////	if (is_added == false)
		////	{
		////		//trace_msg(NULL, TRACE_LOG, "Define after fail Add WriterProxy[%s] to a Reader: %d", p_datareader->p_topic->topic_name, p_rtpsBultinReader->reader_cache->i_changes);
		////	}

		////	trace_msg(NULL, TRACE_LOG, "Define finish Add WriterProxy[%s] to a Reader: %d:%d", p_datareader->p_topic->topic_name, p_rtpsBultinReader->p_reader_cache->i_changes,i);
		////}
	}

	//by jun
	//각 스레드의 큐를 p_domainParticipant의 큐에 연결해서 모든 네트워크 데이터를 하나의 스레드가 데이터를 처리할 수 잇게 
	p_rtps_reader->p_data_fifo = p_rtps_reader->p_datareader->p_subscriber->p_domain_participant->p_reader_thread_fifo;

	/*
	if(p_datareader && p_datareader->p_subscriber && p_datareader->p_subscriber->p_domain_participant)
	{
	if(rtps_addEndPointToParticipant(p_this, (rtps_endpoint_t*)p_rtpsReader, p_datareader->p_subscriber->p_domain_participant) != MODULE_SUCCESS)
	{
	trace_msg(p_this, TRACE_ERROR,"RTPS cannot add EndPoint(Reader) To Participant..");
	FREE(p_rtpsReader);
	return MODULE_ERROR_RTPS;
	}
	}*/

	/*
	if( (p_rtpsReader->thread_id = thread_create2(p_rtpsReader, &p_rtpsReader->object_wait, &p_rtpsReader->object_lock, "RTPS Reader", (void*)Reader_Thread, 0, false )) == 0 )
	{
	return MODULE_ERROR_CREATE;
	}
	*/
	qos_add_liveliness_job_for_datareader(p_datareader);//by kki (liveliness)

	return MODULE_SUCCESS;
}

int32_t rtps_remove_datareader(module_object_t* p_this, DataReader* p_datareader)
{
	qos_remove_liveliness_job_for_datareader(p_datareader);//by kki (liveliness)

	if (p_datareader->p_related_rtps_reader)
	{
		rtps_reader_t* p_rtps_reader = p_datareader->p_related_rtps_reader;

		//[KKI] Reader 의 삭제를 상대편 Writer 에게 알린다.
		rtps_remove_user_defined_Reader(p_rtps_reader);
	}

	if (p_datareader && p_datareader->p_subscriber && p_datareader->p_subscriber->p_domain_participant)
	{
		if (rtps_remove_endpoint_from_participant(p_this, (Entity *)p_datareader) != MODULE_SUCCESS)
		{
			return MODULE_ERROR_RTPS;
		}

		rtps_reader_destory(p_datareader->p_related_rtps_reader);
	}
	return MODULE_SUCCESS;
}

////////////////////////////////////////

DataFull *rtps_message_process(rtps_reader_t *p_rtpsReader, char* p_data, int i_size, int state, DataFragFull **pp_datafragfull);
DataFull *rtps_Datafrag_insert_and_check_data(rtps_reader_t *p_rtpsReader, DataFragFull *p_datafragfull);

////////////////////////////////////////





DataFull *rtps_message_process(rtps_reader_t *p_rtpsReader, char* p_data, int32_t i_size, int32_t state, DataFragFull **pp_datafragfull)
{
	DataFull		*p_ret = NULL;
	DataFragFull	*p_datafragfull = NULL;
	SubmessageHeader a_submessageHeader;
	int32_t		i_pos = 0;
	uint8_t E; // EndiannessFlag
	int32_t		i_remind = i_size;
	InfoDestination	a_InfoDestination;
	bool	is_find_InfoDescription = false;
	Heartbeat a_heartbeat;
	uint32_t submessage_types = 0;
	int32_t i_submessage_size = 0;
	Data a_data;
	Header a_Header = *(Header*)p_data;
	AckNack a_AckNack;
	Gap a_Gap;
	SerializedPayloadForReader *p_serializedData;
	DataFrag a_datafrag;
	HeartbeatFrag a_heartBeatFrag;

	//added by kyy
	//	Duration_t receive_time;
	InfoTimestamp	a_source_timestamp;

	//////////////////


	if(i_size <= sizeof(Header))
	{
		return NULL;
	}


	if(!(a_Header.protocol._rtps[0] == 'R' 
		&& a_Header.protocol._rtps[1] == 'T'
		&& a_Header.protocol._rtps[2] == 'P'
		&& a_Header.protocol._rtps[3] == 'S'))
	{
		return NULL;
	}

	if(is_ignore_participant(a_Header.guid_prefix))
	{
		return NULL;
	}


	


	//////////////////

	i_pos += sizeof(Header);
	i_remind -= sizeof(Header);

	memset(&a_InfoDestination, 0, sizeof(InfoDestination));


	while(i_remind > 0){

		a_submessageHeader = *(SubmessageHeader *)&p_data[i_pos];

		E = a_submessageHeader.flags & 0x01;	// Endianness bit


		i_pos += sizeof(SubmessageHeader);
		i_remind -= sizeof(SubmessageHeader);

		switch(a_submessageHeader.submessage_id)
		{
		case GAP:
			a_Gap = *(Gap*)&p_data[i_pos];
			//trace_msg(NULL,TRACE_LOG,"RTPS SUBMESSAGE Kind GAP");
			rtps_receive_gap_procedure(p_rtpsReader, a_Header.guid_prefix, a_InfoDestination, a_Gap, a_submessageHeader.flags);
			break;
		case PAD:
			submessage_types |= FOUND_PAD;
			break;
		case ACKNACK:
			submessage_types |= FOUND_ACKNACK;
			a_AckNack = *(AckNack*)&p_data[i_pos];
			//rtps_receive_acknnack_procedure(p_rtpsReader, a_Header.guidPrefix, a_InfoDestination, a_AckNack, a_submessageHeader.flags);
			break;
		case HEARTBEAT:
			submessage_types |= FOUND_HEARTBEAT;
			a_heartbeat = *(Heartbeat*)&p_data[i_pos];
			// 맞는 Writer를 찾아서, remote관련 refresh.... EndPoint를 찾자....

			/*if(p_rtpsReader->p_reader_cache->i_linked_size >100 &&  p_rtpsReader->p_datareader->id == 25)
			{
				return p_ret;

			}else{*/


				rtps_receive_heartbeat_procedure(p_rtpsReader, a_Header.guid_prefix, /*a_InfoDestination,*/ a_heartbeat, a_submessageHeader.flags);
			//}

			break;
		case INFO_TS:
			submessage_types |= FOUND_INFOTIMESTAMP;

			//added by kyy(Lifespan)
			a_source_timestamp = *(InfoTimestamp*)&p_data[i_pos];
			//				printf("Source Timestamp : %d , %d \n",a_source_timestamp.timestamp.value.sec, a_source_timestamp.timestamp.value.nanosec);
			p_rtpsReader->source_timestamp.sec = a_source_timestamp.timestamp.value.sec;
			p_rtpsReader->source_timestamp.nanosec = a_source_timestamp.timestamp.value.nanosec;
			//				printf("Source Timestamp : %d , %d \n",p_rtpsReader->source_timestamp.sec, p_rtpsReader->source_timestamp.nanosec);


			break;
		case INFO_SRC: 
			submessage_types |= FOUND_INFOSOURCE;
			break;
		case INFO_REPLY_IP4:
			submessage_types |= FOUND_INFOREPLY_IP4;
			break;
		case INFO_DST:
			///
			a_InfoDestination = *(InfoDestination*)&p_data[i_pos];
			is_find_InfoDescription = true;
			submessage_types |= FOUND_INFODESTINATION;
			break;
		case INFO_REPLY:
			submessage_types |= FOUND_INFOREPLY;
			break;
		case NACK_FRAG:
			submessage_types |= FOUND_NACKFRAG;
			break;
		case HEARTBEAT_FRAG:
			submessage_types |= FOUND_HEARTBEATFRAG;
			a_heartBeatFrag = *((HeartbeatFrag*)&p_data[i_pos]);
			rtps_receive_heartbeat_frag_procedure(p_rtpsReader, a_Header.guid_prefix, a_InfoDestination, a_heartBeatFrag, a_submessageHeader.flags);
			break;
		case DATA:
			a_data = *((Data*)&p_data[i_pos]);	
			submessage_types |= FOUND_DATA;
			//oci 땜시
			if(a_submessageHeader.submessage_length == 0)
			{
				a_submessageHeader.submessage_length = i_remind;
			}

			/*if(a_submessageHeader.submessage_length == 48) 
			{
			printf("second : %d, %d\r\n", a_data.writer_sn.value.low, p_rtpsReader->reader_cache->i_changes);
			}*/

			
			p_ret = rtps_submessage_data(*((Data*)&p_data[i_pos]), &p_data[i_pos],a_submessageHeader.submessage_length, a_submessageHeader.flags);
			
			if(p_ret){
				//if(p_ret->inline_qos.i_parameter)
				//{
				//	int i=0;
				//	GUID_t remote_pariticpant_guid;

				//	for(i=0; i < p_ret->inline_qos.i_parameter; i++)
				//	{
				//		if(p_ret->inline_qos.pp_parameter[i]->parameterId == PID_STATUS_INFO && p_ret->inline_qos.pp_parameter[i]->length == 4)
				//		{
				//			const uint8_t *p = p_ret->inline_qos.pp_parameter[i]->value;
				//			int info_value = ((int)p[0] << 24) |
				//							  ((int)p[1] << 16) |
				//							   ((int)p[2] << 8) |
				//							    ((int)p[3]);
				//			
				//			if(info_value == 3)
				//			{
				//				info_value = 3;
				//				//remove_remote_participant_and_etc(remote_pariticpant_guid);
				//			}

				//			info_value = 3;
				//		}else if(p_ret->inline_qos.pp_parameter[i]->parameterId == PID_KEY_HASH && p_ret->inline_qos.pp_parameter[i]->length == 16)
				//		{
				//			remote_pariticpant_guid = *((GUID_t*)&p_ret->inline_qos.pp_parameter[i]->value[0]);
				//		}
				//	}
				//}

				/*if(p_rtpsReader->p_reader_cache->i_linked_size >5000 &&  p_rtpsReader->p_datareader->id == 25)
				{
					return p_ret;

				}else{*/
					if(p_ret->p_serialized_data)
					{
						p_serializedData = malloc(sizeof(SerializedPayloadForReader));
						memset(p_serializedData, 0, sizeof(SerializedPayloadForReader));

						p_serializedData->i_size = ((SerializedPayloadForReader*)p_ret->p_serialized_data)->i_size;
						p_serializedData->p_value = malloc(p_serializedData->i_size);

						memset(p_serializedData->p_value, 0, p_serializedData->i_size);

						memcpy(p_serializedData->p_value, ((SerializedPayloadForReader*)p_ret->p_serialized_data)->p_value, p_serializedData->i_size);
					}else{
						p_serializedData = NULL;
					}

			
					rtps_receive_data_procedure(p_rtpsReader, a_Header.guid_prefix, a_data, (SerializedPayload *)p_serializedData, a_submessageHeader.flags, true, p_ret);
				//}

			}
			break;
		case DATA_FRAG:
			a_data = *((Data*)&p_data[i_pos]);	
			a_datafrag = *((DataFrag*)&p_data[i_pos]);
			submessage_types |= FOUND_DATAFRAG;
			p_datafragfull = rtps_submessage_datafrag(*((DataFrag*)&p_data[i_pos]), &p_data[i_pos],a_submessageHeader.submessage_length, a_submessageHeader.flags);
			*pp_datafragfull = p_datafragfull;
			p_ret = rtps_Datafrag_insert_and_check_data(p_rtpsReader, p_datafragfull);

			if(p_ret){
				p_serializedData = malloc(sizeof(SerializedPayloadForReader));
				memset(p_serializedData, 0, sizeof(SerializedPayloadForReader));

				p_serializedData->i_size = ((SerializedPayloadForReader*)p_ret->p_serialized_data)->i_size;
				p_serializedData->p_value = malloc(p_serializedData->i_size);
				memcpy(p_serializedData->p_value, ((SerializedPayloadForReader*)p_ret->p_serialized_data)->p_value, p_serializedData->i_size);
				rtps_receive_data_procedure(p_rtpsReader, a_Header.guid_prefix, a_data, (SerializedPayload *)p_serializedData, a_submessageHeader.flags, true, p_ret);
			}
			if(p_ret)destory_datafull_all(p_ret);
			p_ret = NULL;
			break;
		default:
			break;
		}
		i_pos		+= a_submessageHeader.submessage_length;
		i_remind	-= a_submessageHeader.submessage_length;
	}	

	////////////////////////////////////////////////////////////////////////////////////////////////////////


	return p_ret;
}



DataFull *rtps_Datafrag_insert_and_check_data(rtps_reader_t *p_rtpsReader, DataFragFull *p_datafragfull)
{
	int32_t i = 0;
	uint32_t dataSize;
	bool is_datafull = false;
	DataFull *p_datafull=NULL;
	int i_size;
	SerializedPayloadForReader *p_serializedData;
	ushort fragmentSize=0;

	if(p_datafragfull == NULL) return NULL;

	dataSize = p_datafragfull->fragment_size;

	for(i=0; i < p_rtpsReader->i_datafragfull; i++)
	{
		if(memcmp(&p_datafragfull->writer_sn, &p_rtpsReader->pp_datafragfull[i]->writer_sn, sizeof(SequenceNumber)) == 0
			&& memcmp(&p_datafragfull->reader_id, &p_rtpsReader->pp_datafragfull[i]->reader_id, sizeof(EntityId)) == 0
			&& memcmp(&p_datafragfull->writer_id, &p_rtpsReader->pp_datafragfull[i]->writer_id, sizeof(EntityId)) == 0)
		{
			if (p_datafragfull->fragments_in_submessage ==  p_rtpsReader->pp_datafragfull[i]->fragments_in_submessage) {
				//기존에 있기 때문에 삭제
				if(p_datafragfull) destory_data_frag_full_all(p_datafragfull);
				return NULL;
			}else{
				fragmentSize = fragmentSize > p_rtpsReader->pp_datafragfull[i]->fragment_size ? fragmentSize : p_rtpsReader->pp_datafragfull[i]->fragment_size;

				dataSize += p_rtpsReader->pp_datafragfull[i]->fragment_size;
				if(dataSize == p_rtpsReader->pp_datafragfull[i]->data_size)
				{
					is_datafull = true;
				}
			}

		}
	}

	if(is_datafull)
	{
		p_datafull = malloc(sizeof(DataFull));
		memset(p_datafull, 0, sizeof(DataFull));

		memcpy(p_datafull, p_datafragfull, sizeof(Data));
		p_datafull->inline_qos = p_datafragfull->inline_qos;
		i_size = p_rtpsReader->i_datafragfull;
		p_serializedData = malloc(sizeof(SerializedPayloadForReader));

		memset(p_serializedData, 0, sizeof(SerializedPayloadForReader));

		p_serializedData->i_size = p_datafragfull->data_size+4;
		p_serializedData->p_value = malloc(p_serializedData->i_size);

		memset(p_serializedData->p_value, 0, p_serializedData->i_size);

		memcpy(p_serializedData->p_value, ((SerializedPayloadForReader*)p_datafragfull->p_serialized_data)->p_value, 4);
		i_size = p_rtpsReader->i_datafragfull;
		for(i = i_size-1; i >=0; i--)
		{
			if(memcmp(&p_datafragfull->writer_sn, &p_rtpsReader->pp_datafragfull[i]->writer_sn, sizeof(SequenceNumber)) == 0
				&& memcmp(&p_datafragfull->reader_id, &p_rtpsReader->pp_datafragfull[i]->reader_id, sizeof(EntityId)) == 0
				&& memcmp(&p_datafragfull->writer_id, &p_rtpsReader->pp_datafragfull[i]->writer_id, sizeof(EntityId)) == 0)
			{
				memcpy(p_serializedData->p_value+4+fragmentSize*(p_rtpsReader->pp_datafragfull[i]->fragments_in_submessage-p_rtpsReader->pp_datafragfull[i]->fragment_starting_num.value), 
					((SerializedPayloadForReader*)p_rtpsReader->pp_datafragfull[i]->p_serialized_data)->p_value+4, p_rtpsReader->pp_datafragfull[i]->fragment_size);

				destory_data_frag_full_all(p_rtpsReader->pp_datafragfull[i]);
				REMOVE_ELEM( p_rtpsReader->pp_datafragfull, p_rtpsReader->i_datafragfull, i);
			}
		}

		memcpy(p_serializedData->p_value+4+fragmentSize*(p_datafragfull->fragments_in_submessage-p_datafragfull->fragment_starting_num.value), 
			((SerializedPayloadForReader*)p_datafragfull->p_serialized_data)->p_value+4, p_datafragfull->fragment_size);

		destory_data_frag_full_all_except_inline_qos(p_datafragfull);
		p_datafull->p_serialized_data = (SerializedPayload *)p_serializedData;
	}else{

		INSERT_ELEM( p_rtpsReader->pp_datafragfull, p_rtpsReader->i_datafragfull,
			p_rtpsReader->i_datafragfull, p_datafragfull);
	}

	return p_datafull;
}


static int Reader_Thread(rtps_reader_t* p_rtpsReader)
{

	int state = INITIAL_STATE;
	data_t*	p_data = NULL;
	DataFull* p_datafull = NULL;
	DataFragFull* p_dataFragfull = NULL;

	state = WAITING_STATE;


	while (!p_rtpsReader->b_end)
	{
		//by kki...(entity factory)
		if (!qos_is_enabled((Entity*) p_rtpsReader->p_datareader))
		{
			msleep(1);
			continue;
		}

		p_data = data_fifo_get(p_rtpsReader->p_data_fifo);
		if (p_rtpsReader->b_end)
		{
			if (p_data)
			{
				data_release(p_data);
			}
			break;
		}



		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///				이부분에서 파싱을 다시 시작하자.....
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (p_data != NULL)
		{
			p_rtpsReader->received_count++;
			p_rtpsReader->received_bytes += p_data->i_size;

			p_rtpsReader->received_count_throughput++;
			p_rtpsReader->received_bytes_throughput += p_data->i_size;
		}

		p_datafull = rtps_message_process(p_rtpsReader, (char*)p_data->p_data, p_data->i_size, state, &p_dataFragfull);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/*if(p_dataFragfull)
		{
		if(p_dataFragfull) destory_data_frag_full_all(p_dataFragfull);
		p_dataFragfull = NULL;
		}*/




		if (!p_rtpsReader->is_builtin)
		{
			p_rtpsReader->is_builtin = p_rtpsReader->is_builtin;
		}

		//[120822,전형국] User_xx는 모두 Stateful로 만들고, EDP_xx는 Stateless로 만든다.
		if (p_rtpsReader->behavior_type == STATELESS_TYPE && p_rtpsReader->reliability_level == BEST_EFFORT)
		{
			best_effort_statelessreader_behavior((rtps_statelessreader_t *)p_rtpsReader, state, &p_datafull);
		}
		else
		{
			if (p_rtpsReader->reliability_level == BEST_EFFORT)
			{
				best_effort_statefulreader_behavior((rtps_statefulreader_t *)p_rtpsReader, &p_datafull, p_data);
			}
			else
			{
				reliable_statefulreader_behavior((rtps_statefulreader_t *)p_rtpsReader, p_datafull);
			}
		}


		if (p_rtpsReader->behavior_type == STATEFUL_TYPE && p_rtpsReader->reliability_level == BEST_EFFORT)
		{

		}
		else if(p_rtpsReader->behavior_type == STATEFUL_TYPE && p_rtpsReader->reliability_level == RELIABLE)
		{
			if (p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		}
		else
		{
			if (p_datafull) destory_datafull_all(p_datafull);
			//if(p_dataFragfull) destory_data_frag_full_all(p_dataFragfull);
		}
		p_datafull = NULL;
		//p_dataFragfull = NULL;

		if (p_data)
		{
			data_release(p_data);
		}

	}

	return MODULE_SUCCESS;
}


static int kkkk = 0;


////////////////////////////////////////
//by jun
//behavior가 실행되는 스레드 Reader_Thread를 대신해서 하나의 스레드로 동작
//다른 점은 각 스레드가 자신의 data queue에서 데이터를 가져오는 대신 
//domainparticipant에 reader 스레드를 위한 데이터 큐(p_domainparticipant->p_reader_thread_fifo(를 만들고 
//모든 네트워크 데이터를 받아서 처리한다. 
//각 데이터에는 rtps writer를 위한 (p_data->p_rtps_reader;) 구조체를 가지고 잇어
//데이터가 누구 것이고 누구를 통해서 데이터를 처리해야 할지를 알 수 잇다. 
static bool is_created_thread = false;
int reader_thread2( DomainParticipant *p_domainparticipant )
{
	int state = INITIAL_STATE;
	data_t	*p_data = NULL;
	DataFull *p_datafull = NULL;
	DataFragFull *p_dataFragfull = NULL;
	rtps_reader_t *p_rtpsReader = NULL;

	state = WAITING_STATE;

	/*if(is_created_thread == true)
	{
		return -1;
	}

	is_created_thread = true;*/


	while (!p_domainparticipant->b_end)
	{
		p_data = data_fifo_get(p_domainparticipant->p_reader_thread_fifo);

		if (is_quit())
		{
			p_domainparticipant->b_end = true;
		}

		if (all_standby() == false)
		{
			while (all_standby() == false)
			{
				Sleep(1);
			}

		}

		/*if(++kkkk % 500000 == 0)
		{
			printf("depth : %d,%d\n",p_domainparticipant->p_reader_thread_fifo->i_depth, p_domainparticipant->p_reader_thread_fifo->i_size);
		}*/

		//trace_msg(NULL,TRACE_LOG,"data_fifo_get(p_domainparticipant->p_reader_thread_fifo)");
		if (p_domainparticipant->b_end)
		{
			if (p_data)
			{
				data_release(p_data);
			}
			//trace_msg(NULL,TRACE_LOG,"data_fifo_get(p_domainparticipant->p_reader_thread_fifo break;)");
			break;
		}

		p_rtpsReader = (rtps_reader_t *)p_data->v_rtps_reader;

		if(p_rtpsReader->b_end)
		{
			FREE(p_rtpsReader);
			continue;
		}

		if (!qos_is_enabled((Entity*) p_rtpsReader->p_datareader))//by kki
		{
			msleep(1);
			if(p_data)
			{
				data_release(p_data);
			}
			//trace_msg(NULL,TRACE_LOG,"data_fifo_get(p_domainparticipant->p_reader_thread_fifo continue; %p)", p_rtpsReader->b_enable);
			continue;
		}/*else{

		 if(p_data->i_size == 76){
		 msleep(1000);
		 if(p_data) {
		 data_release(p_data);
		 }
		 continue;
			}
			}*/



		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///				이부분에서 파싱을 다시 시작하자.....
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (p_data != NULL)
		{
			p_rtpsReader->received_count++;
			p_rtpsReader->received_bytes += p_data->i_size;

			p_rtpsReader->received_count_throughput++;
			p_rtpsReader->received_bytes_throughput += p_data->i_size;
		}

		//trace_msg(NULL,TRACE_LOG,"Reader_Thread2 : rtps_message_process start");
		//by kki...(liveliness)
		if (!qos_is_liveliness_job(p_data))
		{
			
			/*if( p_rtpsReader->p_reader_cache->i_linked_size >100 && p_rtpsReader->p_datareader->id == 25)
			{
			}else{*/
				p_datafull = rtps_message_process(p_rtpsReader, (char*)p_data->p_data, p_data->i_size, state, &p_dataFragfull);

				p_rtpsReader->checked_time = currenTime();
			//}

			/*if( p_rtpsReader->p_reader_cache->i_linked_size >5000 && p_rtpsReader->p_datareader->id == 25)
			{
				if (p_datafull)
				{
					destory_datafull_all(p_datafull);
				}

				p_datafull = NULL;
			}*/
		}
		else
		{
			qos_liveliness_check_wakeup_time_for_reader(p_data);//by kki (liveliness)

			continue;
		}
		//trace_msg(NULL,TRACE_LOG,"Reader_Thread2 : rtps_message_process end");
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/*if(p_dataFragfull)
		{
		if(p_dataFragfull) destory_data_frag_full_all(p_dataFragfull);
		p_dataFragfull = NULL;
		}*/




		/*if (!p_rtpsReader->is_builtin)
		{
			p_rtpsReader->is_builtin = p_rtpsReader->is_builtin;
		}*/

		if(is_quit())
		{
			p_domainparticipant->b_end = true;
			continue;
		}

		//[120822,전형국] User_xx는 모두 Stateful로 만들고, EDP_xx는 Stateless로 만든다.
		if (p_rtpsReader->behavior_type == STATELESS_TYPE && p_rtpsReader->reliability_level == BEST_EFFORT)
		{
			//trace_msg(NULL,TRACE_LOG,"best_effort_statelessreader_behavior start");
			best_effort_statelessreader_behavior((rtps_statelessreader_t *)p_rtpsReader, state, &p_datafull);
			//trace_msg(NULL,TRACE_LOG,"best_effort_statelessreader_behavior end");
		}
		else
		{
			if (p_rtpsReader->reliability_level == BEST_EFFORT)
			{
				best_effort_statefulreader_behavior((rtps_statefulreader_t *)p_rtpsReader, &p_datafull, p_data);
			}
			else
			{
#if 0
				reliable_statefulreader_behavior((rtps_statefulreader_t *)p_rtpsReader, p_datafull);
#endif			
			}
		}


		if (p_rtpsReader->behavior_type == STATEFUL_TYPE && p_rtpsReader->reliability_level == BEST_EFFORT)
		{
			if (p_datafull) destory_datafull_except_serialized_data(p_datafull);
		}
		else if(p_rtpsReader->behavior_type == STATEFUL_TYPE && p_rtpsReader->reliability_level == RELIABLE)
		{
			if (p_datafull)
			{
				destory_datafull_all(p_datafull);
			}
		} else if(p_rtpsReader->behavior_type == STATELESS_TYPE && p_rtpsReader->reliability_level == BEST_EFFORT)
		{

		}
		else
		{
			if (p_datafull) destory_datafull_all(p_datafull);
			//if(p_dataFragfull) destory_data_frag_full_all(p_dataFragfull);
		}
		p_datafull = NULL;
		//p_dataFragfull = NULL;

		if (p_data)
		{
			data_release(p_data);
		}

	}

	return MODULE_SUCCESS;
}


bool is_matched_remote_publisher(char* p_topic_name, char* p_type_name, void* p_cache)
{
	rtps_cachechange_t* p_rtps_cachechange = (rtps_cachechange_t*)p_cache;

///	UserDataQosPolicy user_data_qos;

	if (p_cache == NULL) return false;	

	if (p_rtps_cachechange->p_data_value != NULL)
	{
		SerializedPayloadForReader *p_serialized = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
		char *p_value = NULL;
		int	i_found = 0;
		int	i_size = 0;

		char *p_TopicName = NULL;
		char *p_TypeName = NULL;

		if(p_serialized) i_size = p_serialized->i_size;


		find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
		if(i_found && p_value)
		{
			i_found = 0;
			p_TopicName = p_value+4;

			if(strcmp(p_topic_name, p_TopicName) != 0) return false;

			find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
			if(i_found && p_value)
			{
				p_TypeName = p_value+4;

				if(strcmp(p_type_name, p_TypeName) != 0) return false;
			}
			
			return true;
		}

	}


	return false;
}


void get_publication_builtin_topic_data( PublicationBuiltinTopicData* p_publication_data, void* p_cache )
{
	rtps_cachechange_t	*p_rtps_cachechange = (rtps_cachechange_t	*)p_cache;

	if(p_publication_data == NULL || p_cache == NULL) return;


	if(p_rtps_cachechange->p_data_value != NULL)
	{
		SerializedPayloadForReader *p_serialized = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
		char *p_value = NULL;
		int	i_found = 0;
		int	i_size = 0;

		char *p_TopicName = NULL;
		char *p_TypeName = NULL;
		GUID_t	a_remoteEndpointGuid;

		if(p_serialized) i_size = p_serialized->i_size;


		find_parameter_list((char*)p_serialized->p_value,i_size, PID_ENDPOINT_GUID, &p_value, &i_found);

		if(i_found && p_value)
		{
			a_remoteEndpointGuid = *(GUID_t*)p_value;

			memcpy(&p_publication_data->key, &a_remoteEndpointGuid, sizeof(GUID_t));

		}

		i_found = 0;

		find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
		if(i_found && p_value)
		{
			i_found = 0;
			p_TopicName = p_value+4;
			p_publication_data->topic_name = strdup(p_TopicName);


			find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
			if(i_found && p_value)
			{
				p_TypeName = p_value+4;

				p_publication_data->type_name = strdup(p_TypeName);

				////////////////////////////////////////////////////////////////////
				//qos 추가 (added by kyy)
				i_found = 0;
				qos_find_publication_qos_from_parameterlist(p_publication_data, (char*)p_serialized->p_value, i_size);
				//////////////////////////////////////////////////////////////////////////


			}

		}

	}
}

void ignore_writerproxy( void* v_reader, void* v_key )
{
	rtps_writerproxy_t *p_writerproxy = NULL;
	rtps_statefulreader_t *p_statefulreader = (rtps_statefulreader_t *)v_reader;

	GUID_t *key = v_key;

	if(p_statefulreader==NULL || p_statefulreader->behavior_type == STATELESS_TYPE) return;

	p_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, *key);


	if(p_writerproxy)
	{
		p_writerproxy->is_ignore = true;
	}
}


void ignore_writerproxy_guid_prefix( void* v_reader, void* v_key )
{
	rtps_writerproxy_t *p_writerproxy = NULL;
	rtps_statefulreader_t *p_statefulreader = (rtps_statefulreader_t *)v_reader;

	GUID_t *key = v_key;
	int i;
	int i_size;

	if(p_statefulreader==NULL || p_statefulreader->behavior_type == STATELESS_TYPE) return;

	{
		READER_LOCK((rtps_reader_t*)p_statefulreader);

		i_size = p_statefulreader->i_matched_writers;

		for(i=i_size-1; i >= 0; i--)
		{
			if(memcmp(&p_statefulreader->pp_matched_writers[i]->remote_writer_guid, key, sizeof(GuidPrefix_t)) == 0)
			{
				READER_UNLOCK((rtps_reader_t*)p_statefulreader);
				p_writerproxy = p_statefulreader->pp_matched_writers[i];
				break;
			}
		}

		READER_UNLOCK((rtps_reader_t*)p_statefulreader);

	}


	if(p_writerproxy)
	{
		p_writerproxy->is_ignore = true;
	}
}