/*
	RTSP Writer class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Specialization of RTPS Endpoint representing the objects that can be the sources of
messages communicating CacheChanges.
*/

#include "rtps.h"

static void write_direct_reliable(rtps_statefulwriter_t* p_rtps_statefulwriter, rtps_cachechange_t *p_change);


/*
this.guid := <as specified in the constructor>;
this.unicast_locator_list := <as specified in the constructor>;
this.multicastLocatorList := <as specified in the constructor>;
this.reliabilityLevel := <as specified in the constructor>;
this.topicKind := <as specified in the constructor>;
this.pushMode := <as specified in the constructor>;
this.heartbeatPeriod := <as specified in the constructor>;
this.nackResponseDelay := <as specified in the constructor>;
this.nackSuppressionDuration := <as specified in the constructor>;
this.lastChangeSequenceNumber := 0;
this.writer_cache := new HistoryCache;
*/
static int Writer_Thread();
static void StartLiveliness_Thread(rtps_writer_t *p_rtps_writer);
static module_thread_t liveliness_thread_id = 0;
bool remote_initialize = false;


static Duration_t DEFAULT_HEARTBEATPERIOD = {1,0};


static uint8_t entity_id_check = 0;
static mutex_t				structure_lock;
static bool structure_lock_initialize = false;

static uint8_t Default_Writer_Entity_id_get()
{
	int ret;
	if (!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}

	mutex_lock(&structure_lock);
	ret = ++entity_id_check;
	mutex_unlock(&structure_lock);
	
	return ret;
}


void rtps_writer_init( rtps_writer_t* p_rtps_writer )
{
	init_rtps_endpoint((rtps_endpoint_t *)p_rtps_writer);

	p_rtps_writer->push_mode = true;		//임시로...
	p_rtps_writer->heartbeat_period = DEFAULT_HEARTBEATPERIOD;
	p_rtps_writer->nack_suppression_duration = TIME_ZERO;
	p_rtps_writer->last_change_sequence_number.high = 0;
	p_rtps_writer->last_change_sequence_number.low = 0;
	p_rtps_writer->nack_response_delay.sec = 0;
	p_rtps_writer->nack_response_delay.nanosec  = 200 * 1000 * 1000; //200 milliseconds
	p_rtps_writer->p_writer_cache = rtps_historycache_new();
	p_rtps_writer->p_writer_cache->p_rtps_writer = p_rtps_writer;
	//
	mutex_init(&p_rtps_writer->object_lock );
	cond_init(&p_rtps_writer->object_wait );
	p_rtps_writer->b_end = false;
	p_rtps_writer->behavior_type = DEFAULT_TYPE;

	p_rtps_writer->p_datawriter = NULL;
	p_rtps_writer->p_accessout = NULL;
	p_rtps_writer->b_has_liveliness = false;
	p_rtps_writer->i_entity_type = WRITER_ENTITY;

	p_rtps_writer->i_heartbeat_frag_count = 0;


	p_rtps_writer->send_count = 0;
	p_rtps_writer->send_bytes = 0;
	p_rtps_writer->send_sample_count = 0;
	p_rtps_writer->send_sample_bytes = 0;

	p_rtps_writer->send_count_throughput = 0;
	p_rtps_writer->send_bytes_throughput = 0;
	p_rtps_writer->send_sample_count_throughput = 0;
	p_rtps_writer->send_sample_bytes_throughput = 0;


	p_rtps_writer->b_thread_first = true;
}


rtps_writer_t *rtps_writer_new(module_object_t * p_this)
{
	rtps_writer_t *p_rtps_writer = (rtps_writer_t *)malloc(sizeof(rtps_writer_t));
	
	memset(p_rtps_writer, 0, sizeof(rtps_writer_t));

	rtps_writer_init(p_rtps_writer);
	
	return p_rtps_writer;
}


void rtps_writer_destroy( rtps_writer_t* p_rtps_writer )
{
	if (p_rtps_writer)
	{
		if (p_rtps_writer->b_has_liveliness && remote_initialize)
		{
			destory_liveliness();
		}

		if (p_rtps_writer->behavior_type == STATELESS_TYPE)
		{
			rtps_statelesswriter_destory((rtps_statelesswriter_t *)p_rtps_writer);
		}
		else if (p_rtps_writer->behavior_type == STATEFUL_TYPE)
		{
			rtps_statefulwriter_destory((rtps_statefulwriter_t *)p_rtps_writer);
		}

		qos_durability_service_save(p_rtps_writer);//by kki (durability service)

		destroy_endpoint((rtps_endpoint_t *)p_rtps_writer);
		rtps_historycache_destory(p_rtps_writer->p_writer_cache);

		mutex_destroy(&p_rtps_writer->object_lock );
		cond_destroy(&p_rtps_writer->object_wait );
		//FREE(p_rtps_writer);
		
	}
}





rtps_cachechange_t *rtps_writer_new_change(rtps_writer_t *p_rtps_writer, ChangeKind_t kind, SerializedPayloadForWriter *p_serializedData, InstanceHandle_t handle)
{
	rtps_cachechange_t*	p_rtps_cachechange = NULL;
	if (p_rtps_writer == NULL) return NULL;


	p_rtps_writer->last_change_sequence_number = sequnce_number_inc(p_rtps_writer->last_change_sequence_number);

	p_rtps_cachechange = rtps_cachechange_new(kind,p_rtps_writer->guid, p_rtps_writer->last_change_sequence_number, (SerializedPayload *)p_serializedData, handle);
	
	return p_rtps_cachechange;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

static SerializedPayloadForWriter *rtps_generate_publicationInfo(DataWriter *p_dataWriter, rtps_writer_t *p_rtps_writer, rtps_writer_t *p_writer)
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;

	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	//VendorId_t vendor_id = {'0','9'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data;
	int32_t i_temp = 0;
	char *p_temp;
	Publisher *p_publisher;

	p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	
	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);
	memcpy(&p_serialized_data->entity_key_guid, &p_rtps_writer->guid, sizeof(GUID_t));

	if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), PUBLICATIONINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_writer->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_writer->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), TOPICINFO_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_writer->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}else if(strcmp(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), LIVELINESSP2P_NAME) == 0)
	{
		memcpy(&p_serialized_data->entity_key_guid.entity_id, &p_rtps_writer->p_rtps_participant->guid.entity_id, sizeof(EntityId_t));
	}

	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;
	i_size += 2;
	//option skip 2byte
	i_size +=2;
	//make ParameterList

	p_publisher = p_dataWriter->get_publisher(p_dataWriter);

	//PID_ENDPOINT_GUID
	gererate_parameter(p_serialized+i_size, &i_size, PID_ENDPOINT_GUID, 16, &p_rtps_writer->guid);

	//PID_TOPIC_NAME
	p_temp = get_string(p_dataWriter->get_topic(p_dataWriter)->get_name(p_dataWriter->get_topic(p_dataWriter)), &i_temp);
	
	gererate_parameter(p_serialized+i_size, &i_size, PID_TOPIC_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_TYPE_NAME
	p_temp = get_string(p_dataWriter->get_topic(p_dataWriter)->get_type_name(p_dataWriter->get_topic(p_dataWriter)), &i_temp);
	
	gererate_parameter(p_serialized+i_size, &i_size, PID_TYPE_NAME, i_temp, p_temp);
	FREE(p_temp);

	//PID_RELIABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_RELIABILITY, sizeof(ReliabilityQosPolicy), &p_dataWriter->datawriter_qos.reliability);

	//PID_OWNERSHIP_STRENGTH
	if(&p_writer->pp_unicast_locator_list) gererate_parameter(p_serialized+i_size, &i_size, PID_OWNERSHIP_STRENGTH, sizeof(OwnershipStrengthQosPolicy), &p_dataWriter->datawriter_qos.ownership_strength);

	//PID_LIVELINESS
	gererate_parameter(p_serialized+i_size, &i_size, PID_LIVELINESS, sizeof(LivelinessQosPolicy), &p_dataWriter->datawriter_qos.liveliness);

	//PID_DURABILITY
	gererate_parameter(p_serialized+i_size, &i_size, PID_DURABILITY, sizeof(DurabilityQosPolicy), &p_dataWriter->datawriter_qos.durability);

	//PID_OWNERSHIP
	gererate_parameter(p_serialized+i_size, &i_size, PID_OWNERSHIP, sizeof(OwnershipQosPolicy), &p_dataWriter->datawriter_qos.ownership);

	//PID_PRESENTATION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PRESENTATION, sizeof(PresentationQosPolicy), &p_publisher->publisher_qos.presentation);

	//PID_DESTINATION_ORDER
	gererate_parameter(p_serialized+i_size, &i_size, PID_DESTINATION_ORDER, sizeof(DestinationOrderQosPolicy), &p_dataWriter->datawriter_qos.destination_order);

	//PID_DEADINE
	gererate_parameter(p_serialized+i_size, &i_size, PID_DEADINE, sizeof(DeadlineQosPolicy), &p_dataWriter->datawriter_qos.deadline);

	//PID_LATENCY_BUDGET
	gererate_parameter(p_serialized+i_size, &i_size, PID_LATENCY_BUDGET, sizeof(LatencyBudgetQosPolicy), &p_dataWriter->datawriter_qos.latency_budget);

	//PID_LIFESPAN
	gererate_parameter(p_serialized+i_size, &i_size, PID_LIFESPAN, sizeof(LifespanQosPolicy), &p_dataWriter->datawriter_qos.lifespan);

	//PID_PARTICIPANT_LEASE_DURATION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PARTICIPANT_LEASE_DURATION, sizeof(Duration_t), &DefaultleaseDuration);

	//PID_PRODUCT_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PRODUCT_VERSION, 4, &product_version);

	//PID_PROTOCOL_VERSION
	gererate_parameter(p_serialized+i_size, &i_size, PID_PROTOCOL_VERSION, 4, &protocol);

	//PID_VENDORID
	gererate_parameter(p_serialized+i_size, &i_size, PID_VENDORID, 4, &p_writer->p_rtps_participant->vendor_id);

	//added by kyy(Partition QoS)
	//PID_PARTITION(DataWriter)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_partition(p_serialized,p_publisher->publisher_qos.partition,i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//added by kyy(UserData QoS)
	//PID_USER_DATA(DataWriter)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_user_data(p_serialized, p_dataWriter->datawriter_qos.user_data, i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//PID_TOPIC_DATA(DataWriter)///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_topic_data(p_serialized, p_dataWriter->p_topic->topic_qos.topic_data, i_size);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//PID_GROUP_DATA(DataWriter)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i_size = qos_generate_group_data(p_serialized, p_publisher->publisher_qos.group_data,i_size);
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


static SerializedPayloadForWriter *rtps_generate_publicationInfo_remove(DataWriter *p_dataWriter, rtps_writer_t *p_rtps_writer, rtps_writer_t *p_writer)
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;

	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data;
	int32_t i_temp = 0;

	char status_info[] = {0x00,0x00,0x00,0x03};

	p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));
	
	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));

	if(p_writer->p_accessout)
	{
		if(p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);
	memcpy(&p_serialized_data->entity_key_guid, &p_rtps_writer->guid, sizeof(GUID_t));


	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;
	//i_size += 2;
	////option skip 2byte
	//i_size +=2;
	////make ParameterList


	//PID_KEY_HASH
	gererate_parameter(p_serialized+i_size, &i_size, PID_KEY_HASH, 16, &p_rtps_writer->guid);

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


void rtps_added_user_defined_writer(DataWriter* p_datawriter, rtps_writer_t* p_rtps_writer)
{
///	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//builtinPublication을 찾자.
	rtps_statefulwriter_t* p_statefulwriter = (rtps_statefulwriter_t*)rtps_get_spdp_builtin_publication_writer();
	if (p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
///	p_builtin_datawriter = p_statefulwriter->p_datawriter;
	p_serialized_data = rtps_generate_publicationInfo(p_datawriter, p_rtps_writer, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin((rtps_writer_t *)p_statefulwriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


//[KKI] EDP로 DATA(w[UD])메시지가 날아감.
void rtps_remove_user_defined_Writer(rtps_writer_t *p_rtps_writer)
{
///	DataWriter* p_builtin_datawriter = NULL;
	SerializedPayloadForWriter* p_serialized_data = NULL;
	//builtinPublication을 찾자.
	rtps_statefulwriter_t* p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_publication_writer();
	if (p_statefulwriter == NULL || p_statefulwriter->p_datawriter == NULL) return;
///	p_builtin_datawriter = p_statefulwriter->p_datawriter;
	p_serialized_data = rtps_generate_publicationInfo_remove(p_rtps_writer->p_datawriter, p_rtps_writer, (rtps_writer_t *)p_statefulwriter);
	rtps_write_from_builtin_rapidly((rtps_writer_t *)p_statefulwriter, p_serialized_data, (InstanceHandle_t)p_serialized_data);
}


static void add_readerproxy_to_writer(rtps_writer_t* p_datawriter, SerializedPayload* p_data_value)
{
	SerializedPayloadForReader* p_serialized = NULL;
	int32_t	i_size = 0;
	char* p_value = NULL;
	int32_t	i_found = 0;
	GUID_t	a_remote_endpoint_guid;
	char* p_topic_name = NULL;
	char* p_type_name = NULL;
	bool is_builtin = false;

	if (p_datawriter->behavior_type == STATEFUL_TYPE)
	{
		if (p_data_value)
		{
			p_serialized = (SerializedPayloadForReader *)p_data_value;
			i_size = p_serialized->i_size;
		}
		else
		{
			return;
		}	

		find_parameter_list((char*)p_serialized->p_value, i_size, PID_ENDPOINT_GUID, &p_value, &i_found);
		if(i_found && p_value)
		{
			a_remote_endpoint_guid = *(GUID_t*)p_value;
			i_found = 0;

			find_parameter_list((char*)p_serialized->p_value, i_size, PID_TOPIC_NAME, &p_value, &i_found);
			if(i_found && p_value)
			{
				i_found = 0;
				p_topic_name = p_value+4;

				find_parameter_list((char*)p_serialized->p_value, i_size, PID_TYPE_NAME, &p_value, &i_found);
				if(i_found && p_value)
				{
					p_type_name = p_value+4;

					if(have_topic_name_topic_type_writer(p_datawriter, p_topic_name, p_type_name))
					{
						rtps_endpoint_t		**pp_rtpsEndpoint =  NULL;
						int ii_size=0;
						int i;
						rtps_writer_t *p_rtps_writer;
						GUID_t guid;
						bool compare_value;

						RxOQos a_pub_rxo_qos;
						RxOQos a_sub_rxo_qos;
						RxOQos a_requested_qos;
						RxOQos a_offered_qos;


						SubscriberQos sqos;

						static_get_default_subscriber_qos(&sqos);

						find_subscriber_qos_from_parameterlist(&sqos, (char*)p_serialized->p_value, i_size);

						memset(&a_pub_rxo_qos, 0, sizeof(RxOQos));
						memset(&a_sub_rxo_qos, 0, sizeof(RxOQos));
						memset(&a_requested_qos, 0, sizeof(RxOQos));
						memset(&a_offered_qos, 0, sizeof(RxOQos));


						memset(&guid,0,sizeof(GUID_t));

						pp_rtpsEndpoint = rtps_compare_writer(guid, p_topic_name, p_type_name, &ii_size);

						find_rxo_qos_from_parameterlist(&a_sub_rxo_qos, p_serialized->p_value, i_size);
						
						for(i=0; i < ii_size; i++)
						{

							p_rtps_writer = (rtps_writer_t *)pp_rtpsEndpoint[i];
							if(p_rtps_writer && p_rtps_writer->behavior_type == STATEFUL_TYPE)
							{
								Locator_t *remoteLocator = get_default_remote_participant_locator(a_remote_endpoint_guid);
								Locator_t *remoteMulticastLocator = get_default_remote_participant_multicast_locator(a_remote_endpoint_guid);
								rtps_readerproxy_t *p_rtps_readerproxy = NULL;
								bool equal_parition = true;
							


								

								//////////////////////////////////////////////////////////////////////////////////////////////////
								//added by kyy (RxO Compare : requested vs offered)
								//RxO Compare
								/*p_sub_rxo_qos = malloc(sizeof(RxOQos));
								p_offered_qos	= malloc(sizeof(RxOQos));*/
								
								get_offered_qos(&a_offered_qos, p_rtps_writer);
								compare_value = compare_rxo(&a_offered_qos, &a_sub_rxo_qos);

								/*FREE(p_sub_rxo_qos);
								FREE(p_offered_qos);*/


								if (p_rtps_writer->p_datawriter && p_rtps_writer->p_datawriter->p_publisher)
								{
									equal_parition = qos_compare_partition(sqos.partition,  p_rtps_writer->p_datawriter->p_publisher->publisher_qos.partition);
								}

								REMOVE_STRING_SEQ(sqos.partition.name);


								if(equal_parition && remoteLocator && compare_value)
								{
									rtps_statefulwriter_t *p_statefulwriter = NULL;
									Locator_t *p_unicatlocator = (Locator_t *)malloc(sizeof(Locator_t));
									Locator_t *p_multicastlocator = (Locator_t *)malloc(sizeof(Locator_t));
									bool b_ret;

									i_found=0;
									memset(p_unicatlocator, 0, sizeof(Locator_t));
									memset(p_multicastlocator, 0, sizeof(Locator_t));

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

									p_rtps_readerproxy = rtps_readerproxy_new(p_rtps_writer, a_remote_endpoint_guid, p_unicatlocator, p_multicastlocator, false, a_sub_rxo_qos.durability.kind);
									p_rtps_readerproxy->remote_endpoint_guid = a_remote_endpoint_guid;
									p_rtps_readerproxy->reliablility_kind = a_sub_rxo_qos.reliability.kind;
									b_ret = rtps_statefulwriter_matched_reader_add((rtps_statefulwriter_t *)p_rtps_writer, p_rtps_readerproxy);
									/// 연관된 Builtin writer 깨우기
									/*p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_SPDPBultinSubscriptionWriter();

									if(p_statefulwriter){
										p_statefulwriter->heartbeatPeriod.sec = 1;
										cond_signal(&p_statefulwriter->object_wait);
									}*/

									//WakeUpBuiltinPub_and_Sub();

									
									if(b_ret) trace_msg(NULL, TRACE_LOG, "Add the ReaderProxy[%s:%d] to a Writer.[%d.%d.%d.%d]", p_topic_name, ((rtps_statefulwriter_t *)p_rtps_writer)->i_matched_readers, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);

								}else{
									if(remoteLocator)
										trace_msg(NULL, TRACE_LOG, "Couldn't add the ReaderProxy[%s] to a Writer.[%d.%d.%d.%d]", p_topic_name, remoteLocator->address[12], remoteLocator->address[13], remoteLocator->address[14], remoteLocator->address[15]);
									else
										trace_msg(NULL, TRACE_LOG, "Couldn't add the ReaderProxy[%s] to a Writer.remoteLocator is null.", p_topic_name);
								}
							
							}else
							{

							}
						}

						if(ii_size == 0) trace_msg(NULL, TRACE_LOG, "Couldn't add the ReaderProxy[%s] to a Writer. : No Matched proxy does not exist!!", p_topic_name);

						FREE(pp_rtpsEndpoint);
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


////BuiltinSubscriptionReader에 있는 history캐쉬에서 연관되는 Entity를 찾아 rtps_readerproxy_new를 이용해서 추가 시키는 함수
void rtps_add_readerproxies_to_writer(rtps_writer_t* p_rtps_writer)
{
	rtps_reader_t* p_rtps_builtin_reader = (rtps_reader_t*)rtps_get_spdp_builtin_subscription_reader();
	
	rtps_cachechange_t *p_change_atom = NULL;

	//msleep(100000);

	while (get_define_user_entity_lock())
	{
		trace_msg(NULL, TRACE_LOG, "Define_UserEntity_Lock sleep writer");
		msleep(10000);
	}

//	HISTORYCACHE_LOCK(p_rtps_builtin_reader->p_reader_cache);
	p_change_atom = (rtps_cachechange_t *)p_rtps_builtin_reader->p_reader_cache->p_head_first;

	while(p_change_atom)
	{
		rtps_cachechange_t* cachechange = p_change_atom;
		if (cachechange && cachechange->p_data_value)
		{
			if (cachechange->b_isdisposed == false)
			{
				add_readerproxy_to_writer(p_rtps_writer, cachechange->p_data_value);
			}
		}
		p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
	}
//	HISTORYCACHE_UNLOCK(p_rtps_builtin_reader->p_reader_cache);
}


int32_t rtps_add_datawriter(module_object_t* p_this, DataWriter* p_datawriter)
{
	rtps_writer_t *p_rtps_writer = NULL;
	service_t* p_service = (service_t *)p_this;
	bool is_user_defined = false;
	
	//일단 여기서 BuiltinWriter인지 판별 그러면 statelessWriter로....

	//printf("rtps_add_datawriter 1(%s) \r\n", p_datawriter->p_topic->topic_name);

	if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)),PARTICIPANTINFO_NAME) == 0)
	{
		p_rtps_writer = (rtps_writer_t *)rtps_statelesswriter_new(p_this, true);
		p_rtps_writer->topic_kind = WITH_KEY;
		//The default rate by which SPDP periodic announcements are sent equals 30 seconds.
		//((rtps_statelesswriter_t*)p_rtps_writer)->resend_data_period.sec = 30;

		//Participant 빨리 찾기위해서 임시로 시간 단축.
		((rtps_statelesswriter_t*)p_rtps_writer)->resend_data_period.sec = 5;
		((rtps_statelesswriter_t*)p_rtps_writer)->resend_data_period.nanosec = 0;
		
		set_unicast_and_multicast_spdp(p_this,  (rtps_endpoint_t *)p_rtps_writer);

		memcpy(&p_rtps_writer->guid.entity_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, 4);

		//
		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), SPDP_OUT_TYPE);


		set_participant_writer(p_rtps_writer);

	}
	else if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)), PUBLICATIONINFO_NAME) == 0)
	{
		rtps_statefulwriter_t *p_statefulwriter = rtps_statefulwriter_new(p_this);
		p_rtps_writer = (rtps_writer_t *)p_statefulwriter;
		p_rtps_writer->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_writer);
		memcpy(&p_rtps_writer->guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, 4);
		p_statefulwriter->push_mode = true;
		//set_enable_multicast((service_t*)p_this, p_statefulwriter);

		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), SEDP_OUT_TYPE);
		set_pub_writer((rtps_writer_t *)p_statefulwriter);
	}
	else if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)), SUBSCRIPTIONINFO_NAME) == 0)
	{
		rtps_statefulwriter_t *p_statefulwriter = rtps_statefulwriter_new(p_this);
		p_rtps_writer = (rtps_writer_t *)p_statefulwriter;
		p_rtps_writer->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_writer);
		memcpy(&p_rtps_writer->guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, 4);
		p_statefulwriter->push_mode = true;
		//set_enable_multicast((service_t*)p_this, p_statefulwriter);

		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), SEDP_OUT_TYPE);
		set_sub_writer((rtps_writer_t *)p_statefulwriter);
	}
	else if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)), TOPICINFO_NAME) == 0)
	{
		rtps_statefulwriter_t *p_statefulwriter = rtps_statefulwriter_new(p_this);
		p_rtps_writer = (rtps_writer_t *)p_statefulwriter;
		p_rtps_writer->topic_kind = WITH_KEY;
		set_unicast_and_multicast_sedp(p_this,  (rtps_endpoint_t *)p_rtps_writer);
		memcpy(&p_rtps_writer->guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, 4);
		p_statefulwriter->push_mode = true;
		//set_enable_multicast((service_t*)p_this, p_statefulwriter);

		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), SEDP_OUT_TYPE);
		set_topic_writer((rtps_writer_t *)p_statefulwriter);
	}
	else if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)), LIVELINESSP2P_NAME) == 0)
	{
		rtps_statefulwriter_t *p_statefulwriter = rtps_statefulwriter_new(p_this);
		p_rtps_writer = (rtps_writer_t *)p_statefulwriter;
		p_rtps_writer->topic_kind = WITH_KEY;
		set_unicast_and_multicast_spdp(p_this,  (rtps_endpoint_t *)p_rtps_writer);
		memcpy(&p_rtps_writer->guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, 4);
		p_statefulwriter->push_mode = true;
		//set_enable_multicast((service_t*)p_this, p_statefulwriter);

		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), SPDP_OUT_TYPE);
		set_liveness_writer((rtps_writer_t *)p_statefulwriter);
	}
	else
	{
		EntityId_t a_userEntityId;
		int i;
		FooTypeSupport *p_typeSupport = NULL;
		Topic *p_topic = p_datawriter->p_topic;
		bool is_foundkey = false;
		module_t *p_module = current_object( get_domain_participant_factory_module_id() );
		rtps_statefulwriter_t *p_statefulwriter = rtps_statefulwriter_new(p_this);

		//printf("rtps_add_datawriter 2(%s)\r\n", p_topic->type_name);


		//trace_msg(NULL,TRACE_LOG,"rtps_addDataWriter 1");


		p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), p_topic->type_name);


		//trace_msg(NULL,TRACE_LOG,"rtps_addDataWriter 2");

		//printf("rtps_add_datawriter 3(%p)\r\n", p_typeSupport);

		p_rtps_writer = (rtps_writer_t *)p_statefulwriter;

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
			p_rtps_writer->topic_kind = WITH_KEY;
		}
		else
		{
			p_rtps_writer->topic_kind = NO_KEY;
		}
		

		
		set_unicast_and_multicast_default(p_this,  (rtps_endpoint_t *)p_rtps_writer);
		a_userEntityId.entity_key[0] = Default_Writer_Entity_id_get();
		a_userEntityId.entity_key[1] = 0;
		a_userEntityId.entity_key[2] = 0;
		a_userEntityId.entity_kind = KIND_OF_ENTITY_WRITER_WITH_KEY_USER_DEFINED;
		memcpy(&p_rtps_writer->guid.entity_id, &a_userEntityId, sizeof(EntityId_t));
		if (p_service->getaccessout) p_rtps_writer->p_accessout = p_service->getaccessout(OBJECT(p_service), DEFAULT_OUT_TYPE);

		//User-defined Writer가 생성되었기 때문에 BultinPublication에게 알려주자..
		rtps_added_user_defined_writer(p_datawriter, p_rtps_writer);

		if (p_datawriter->datawriter_qos.reliability.kind == BEST_EFFORT_RELIABILITY_QOS)
			p_statefulwriter->push_mode = true;
		else
			p_statefulwriter->push_mode = true;//false;

		is_user_defined = true;
	}

	p_rtps_writer->p_writer_cache->history_kind = p_datawriter->datawriter_qos.history.kind;

	if (p_datawriter->datawriter_qos.history.depth)
	{
		p_rtps_writer->p_writer_cache->history_depth = p_datawriter->datawriter_qos.history.depth;
	}

	//by kki...(history and reliability)
	p_rtps_writer->p_writer_cache->history_max_length = qos_get_history_max_length(&p_datawriter->datawriter_qos.history, &p_datawriter->datawriter_qos.resource_limits);
	p_rtps_writer->p_writer_cache->reliability_kind = p_datawriter->datawriter_qos.reliability.kind;

	if (is_user_defined)
	{
		p_rtps_writer->is_builtin = false;
	}
	else
	{
		p_rtps_writer->is_builtin = true;
	}

	p_rtps_writer->fragment_size = rtps_get_fragmentsize(p_this);

	if (p_rtps_writer)
	{
		if (p_datawriter->datawriter_qos.reliability.kind == BEST_EFFORT_RELIABILITY_QOS)
		{
			p_rtps_writer->reliability_level = BEST_EFFORT;
		}
		else if (p_datawriter->datawriter_qos.reliability.kind == RELIABLE_RELIABILITY_QOS)
		{
			p_rtps_writer->reliability_level = RELIABLE;
		}
		else
		{
			p_rtps_writer->reliability_level = RELIABLE;
		}
	}


	p_rtps_writer->p_datawriter = p_datawriter;

	/*
	Transition T1

	the_rtps_writer = new RTPS::Writer;
	the_dds_writer.related_rtps_writer := the_rtps_writer;
	*/
	p_datawriter->p_related_rtps_writer = p_rtps_writer;
	if (p_datawriter && p_datawriter->p_publisher && p_datawriter->p_publisher->p_domain_participant)
	{
		if (rtps_add_endpoint_to_participant(p_this, (rtps_endpoint_t*)p_rtps_writer, p_datawriter->p_publisher->p_domain_participant) != MODULE_SUCCESS)
		{
			trace_msg(p_this, TRACE_ERROR,"RTPS cannot add EndPoint(Writer) To Participant..");
			FREE(p_rtps_writer);
			return MODULE_ERROR_RTPS;
		}
	}


	

	if (is_user_defined)
	{
		//trace_msg(NULL,TRACE_LOG,"rtps_addDataWriter 3");

		////BuiltinSubscriptionReader에 있는 history캐쉬에서 연관되는 Entity를 찾아 rtps_readerproxy_new를 이용해서 추가 시키자.
		rtps_add_readerproxies_to_writer(p_rtps_writer);
		////{
		////	rtps_reader_t* p_rtps_builtin_reader = (rtps_reader_t*)rtps_get_spdp_builtin_subscription_reader();
		////	int i;

		////	//trace_msg(NULL,TRACE_LOG,"rtps_addDataWriter 4");
		////	msleep(100000);
		////	//trace_msg(NULL,TRACE_LOG,"rtps_addDataWriter 5");

		////	while (get_define_user_entity_lock())
		////	{
		////		trace_msg(NULL, TRACE_LOG, "Define_UserEntity_Lock sleep writer");
		////		msleep(10000);
		////	}

		////	HISTORYCACHE_LOCK(p_rtps_builtin_reader->p_reader_cache);
		////	for (i = 0; i < p_rtps_builtin_reader->p_reader_cache->i_changes; i++)
		////	{
		////		rtps_cachechange_t* cachechange = p_rtps_builtin_reader->p_reader_cache->pp_changes[i];
		////		if (cachechange && cachechange->p_data_value)
		////		{
		////			if (cachechange->b_isdisposed == false) add_readerproxy_to_writer(p_rtps_writer, cachechange->p_data_value);
		////		}
		////	}
		////	HISTORYCACHE_UNLOCK(p_rtps_builtin_reader->p_reader_cache);
		////}
		qos_add_liveliness_writer(p_datawriter);//by kki (liveliness)
		qos_durability_service_load(p_rtps_writer);//by kki (durability service)
	}

	//by jun
	/*
	if( (p_rtps_writer->thread_id =thread_create2(p_rtps_writer, &p_rtps_writer->object_wait, &p_rtps_writer->object_lock, "RTPS WRITER", (void*)Writer_Thread, 0, false )) == 0)
    {
		return MODULE_ERROR_CREATE;
    }
	*/

	//writer by jun
	//새로운 writer가 생기는 경우 writer를 job 형태로 만들어서 job queue에 삽입
	//여기서 우선 순위는 0이다 job 중에서 가장 먼저 수행되어야 한다. 
	//job 이 추가되면 signal Wrtier_Thread에 보낸다.
	{
		data_t* p_jobdata=NULL;
		p_jobdata = data_new(0);


		p_jobdata->p_rtps_writer = p_rtps_writer;
		p_jobdata->b_thread_first = true;

		p_jobdata->initial_time = currenTime();

		p_jobdata->next_wakeup_time.sec = 0;
		p_jobdata->next_wakeup_time.nanosec = 0;
		p_jobdata->priority = 0;

		timed_job_queue_time_compare_data_fifo_put_add_job(p_datawriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
		//trace_msg(NULL, TRACE_LOG, "cond_signal rtps_addDataWriter");
		cond_signal( &p_datawriter->p_publisher->p_domain_participant->writer_thread_wait );
	}


	/////////////////////////////////////////////////////



	if (strcmp(p_datawriter->get_topic(p_datawriter)->get_name(p_datawriter->get_topic(p_datawriter)), PARTICIPANTINFO_NAME) == 0 && !remote_initialize)
	{
		/////////////////////////// Liveliness Protocol에 관련된 작업을 시작 하자....
		if ((liveliness_thread_id = thread_create2(p_rtps_writer, &p_rtps_writer->object_wait, &p_rtps_writer->object_lock, "RTPS WRITER", (void*)qos_liveliness_thread, 0, false )) == 0)//by kki (liveliness)
		{
			return MODULE_ERROR_CREATE;
		}
		p_rtps_writer->b_has_liveliness = true;
	}

	////////////////////////////////////////////////////

	qos_writer_set_liveliness_kind_and_lease_duration(p_rtps_writer);//by kki (liveliness)
	qos_add_liveliness_job_for_datawriter(p_datawriter);//by kki (liveliness)

	return MODULE_SUCCESS;
}

int32_t rtps_remove_datawriter(module_object_t* p_this, DataWriter* p_datawriter)
{
	qos_remove_liveliness_job_for_datawriter(p_datawriter);//by kki (liveliness)

	if (p_datawriter->p_related_rtps_writer)
	{
		rtps_writer_t *p_rtps_writer = p_datawriter->p_related_rtps_writer;

		///////////////////////////////
		if (p_rtps_writer->is_builtin == false)
		{
			//[KKI] EDP로 DATA(w[UD])메시지가 날아감.
			rtps_remove_user_defined_Writer(p_rtps_writer);
		}
		///////////////////////////////
		p_rtps_writer->b_end = true;
		cond_signal(&p_rtps_writer->object_wait);

		thread_join2(p_rtps_writer->thread_id);
	}

	if (p_datawriter && p_datawriter->p_publisher && p_datawriter->p_publisher->p_domain_participant)
	{
		if (rtps_remove_endpoint_from_participant(p_this,(Entity *)p_datawriter) != MODULE_SUCCESS)
		{
			return MODULE_ERROR_RTPS;
		}

		rtps_writer_destroy(p_datawriter->p_related_rtps_writer);
	}

	return MODULE_SUCCESS;
}


////////////// rtps writer에 guard condition이라는 부분이 있는데, 일단 thread를 signal wait방식으로 만들어 보자..





void* rtps_write_from_builtin(rtps_writer_t* p_writer, SerializedPayloadForWriter* p_serialized_data, InstanceHandle_t handle)
{
	void* ret;
	rtps_cachechange_t* p_rtps_cachechange;

	message_t* p_message;
	p_message = malloc(sizeof(message_t));
	memset(p_message,0, sizeof(message_t));

//	p_message->source_time = currenTime();

	ret = p_rtps_cachechange = rtps_writer_new_change(p_writer, ALIVE, p_serialized_data, handle);

	p_message->v_related_cachechange = p_rtps_cachechange;
	p_rtps_cachechange->p_org_message = p_message;
//time by jun
	p_rtps_cachechange->source_timestamp = currenTime();
	//
	
	//8.2.9.1.2 Transition T2
	rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
	if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
		|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
	{
		//trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
	}

	p_writer->heartbeat_period.sec = 1;

	//writer by jun
	//domainparticpant와 liveliness, data_reader가 생성될 경우 수행 
	//job은 한번만 수행되며, 우선 순위는 2로 설정
	//job이 추가가 되면 Writer_Thread에 시그널을 보냄
	{
		data_t* p_jobdata = NULL;
		DataWriter* p_dataWriter;

		p_jobdata = data_new(0);

		p_jobdata->p_rtps_writer = (rtps_writer_t *)p_writer;
		p_dataWriter = p_writer->p_datawriter;
//		p_jobdata->b_thread_first = true;

		p_jobdata->initial_time = currenTime();

		p_jobdata->next_wakeup_time.sec = 0;
		p_jobdata->next_wakeup_time.nanosec = 0;
		p_jobdata->priority = 2;

		timed_job_queue_time_compare_data_fifo_put_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
		//trace_msg(NULL, TRACE_LOG, "cond_signal rtps_write_from_builtin");
		cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
	}

	//cond_signal(&p_writer->object_wait);
	return ret;
}

//종료시 데이터를 빨리 보내기 위해서
void* rtps_write_from_builtin_rapidly(rtps_writer_t* p_writer, SerializedPayloadForWriter* p_serialized_data, InstanceHandle_t handle)
{
	void* ret;
	rtps_cachechange_t* p_rtps_cachechange;

	Time_t message_time = currenTime();

	message_t* p_message;
	p_message = malloc(sizeof(message_t));
	memset(p_message,0, sizeof(message_t));

	ret = p_rtps_cachechange = rtps_writer_new_change(p_writer, ALIVE, p_serialized_data, handle);
	p_message->v_related_cachechange = p_rtps_cachechange;
	p_rtps_cachechange->p_org_message = p_message;
	//
	//time by jun
	p_rtps_cachechange->source_timestamp = currenTime();
	
	//8.2.9.1.2 Transition T2
	rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
	if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
		|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
	{
		//trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
	}

	

	if (p_writer->behavior_type == STATEFUL_TYPE && p_writer->reliability_level == RELIABLE)
	{
		uint32_t i , size;
		DataFull *p_data = NULL;
		SerializedPayloadForWriter *p_serializedPlayloadwriter = NULL;
		rtps_readerproxy_t *p_rtps_readerproxy = NULL;
		rtps_statefulwriter_t *p_rtps_statefulwriter = (rtps_statefulwriter_t *)p_writer;
		bool is_builtin = false;

		mutex_lock(&p_rtps_statefulwriter->object_lock);
		size = p_rtps_statefulwriter->i_matched_readers;

		for (i = 0; i < size ; i++)
		{
			p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];

			p_data = rtps_data_new(p_rtps_cachechange);
			p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
			HISTORYCACHE_LOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);

			if (p_rtps_readerproxy->expects_inline_qos)
			{
				ParameterWithValue* p_parameter;
								
				p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_serializedPlayloadwriter->entity_key_guid);

				insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);

				p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);

				insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
			}

			p_data->octets_to_inline_qos = 16;

			p_data->extra_flags = 0;
			p_data->reader_id.value = ENTITYID_UNKNOWN;

			rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, message_time);

			HISTORYCACHE_UNLOCK(p_rtps_readerproxy->p_rtps_writer->p_writer_cache);
		}

		mutex_unlock(&p_rtps_statefulwriter->object_lock);
	}
	else if (p_writer->behavior_type == STATELESS_TYPE)
	{
		int32_t i, size;
		rtps_readerlocator_t* p_rtps_readerlocator = NULL;
		rtps_statelesswriter_t* p_rtps_statelesswriter = (rtps_statelesswriter_t*)p_writer;
		SerializedPayloadForWriter* p_serializedPlayloadwriter = NULL;
		DataFull* p_data = NULL;

		mutex_lock(&p_rtps_statelesswriter->object_lock);
		size = p_rtps_statelesswriter->i_reader_locators;

		for (i = 0; i < size ; i++)
		{
			p_rtps_readerlocator = p_rtps_statelesswriter->pp_reader_locators[i];

			p_data = rtps_data_new(p_rtps_cachechange);
			p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;

			if (p_rtps_readerlocator->expects_inline_qos)
			{
				ParameterWithValue* p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_rtps_statelesswriter->p_rtps_participant->guid);

				insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);

				p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);
					
				insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
			}

			p_data->octets_to_inline_qos = 16;

			p_data->extra_flags = 0;
			p_data->reader_id.value = ENTITYID_UNKNOWN;

			rtps_send_data_to((rtps_writer_t *)p_rtps_statelesswriter, p_rtps_readerlocator->locator, p_data, message_time);
		}

		mutex_unlock(&p_rtps_statelesswriter->object_lock);
	}

	p_writer->heartbeat_period.sec = 1;

	cond_signal(&p_writer->object_wait);

	return ret;
}


//////////////////////////////////////////////////////////

/*
	8.4.13 Writer Liveliness Protocol

	The DDS specification requires the presence of a liveliness mechanism. RTPS realizes this requirement with the Writer
	Liveliness Protocol. The Writer Liveliness Protocol defines the required information exchange between two Participants
	in order to assert the liveliness of Writers contained by the Participants.
	All implementations must support the Wirter Liveliness Protocol in order to be interoperable.
*/

/////////////////////////////////////////////////////////


typedef struct RemoteParticipant_t
{
	GUID_t		a_remoteParticipantGuid;
	Locator_t	a_remotelocator;
	Locator_t	a_defaultLocator;
	Locator_t	a_remoteMulticastlocator;
	Locator_t	a_defaultMulticastLocator;
	bool		is_remoteAlive;
	Duration_t  a_remoteDuration;
	Time_t		a_lastsendTime;
	uint32_t count;
	bool		is_ignore;
} RemoteParticipant_t;

RemoteParticipant_t**	pp_remoteParticipant = NULL;
int						i_remoteParticipant = 0;


mutex_t				remote_lock;
cond_t				remote_cond;


//static bool b_end_liveliness = false;
//
//
//#ifndef QOS_L
//static void StartLiveliness_Thread(rtps_writer_t *p_rtps_writer)
//{
//	int i;
//	Time_t current;
//
//	if(!remote_initialize){
//		mutex_init(&remote_lock);
//		cond_init(&remote_cond);
//		remote_initialize = true;
//	}else
//	{
//		return;
//	}
//
//
//	while(!b_end_liveliness)
//	{
//		int32_t sec = DefaultleaseDuration.sec;
//		uint32_t nanosec = DefaultleaseDuration.nanosec;
//		int32_t tmpsec;
//		uint32_t tmpnanosec;
//
//		uint32_t waketime;
//
//		mutex_lock(&remote_lock);
//
//		for(i=0 ; i < i_remoteParticipant; i++)
//		{
//			if(pp_remoteParticipant[i]->is_remoteAlive == false) continue;
//
//			tmpsec = pp_remoteParticipant[i]->a_remoteDuration.sec;
//			tmpnanosec = pp_remoteParticipant[i]->a_remoteDuration.nanosec;
//
//			if(tmpsec < sec)
//			{
//				sec = tmpsec;
//				nanosec = tmpnanosec ;
//			}else if(sec == tmpnanosec)
//			{
//				nanosec = tmpnanosec < nanosec ? tmpnanosec : nanosec;
//			}
//		}
//
//		mutex_unlock(&remote_lock);
//
//
//		if(sec > 1) sec -= 1;
//
//		waketime = sec * 1000 
//				+ nanosec /1000000;
//		mutex_lock(&remote_lock);
//		cond_waittimed(&remote_cond, &remote_lock, waketime);
//		mutex_unlock(&remote_lock);
//
//
//		if(b_end_liveliness) break;
//
//		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		mutex_lock(&remote_lock);
//		for(i=0 ; i < i_remoteParticipant; i++)
//		{
//			current = currenTime();
//
//			if( current.sec > pp_remoteParticipant[i]->a_lastsendTime.sec + pp_remoteParticipant[i]->a_remoteDuration.sec)
//			{
//				pp_remoteParticipant[i]->is_remoteAlive = false;
//			}else if( current.sec == pp_remoteParticipant[i]->a_lastsendTime.sec + pp_remoteParticipant[i]->a_remoteDuration.sec)
//			{
//				if( current.nanosec > pp_remoteParticipant[i]->a_lastsendTime.nanosec + pp_remoteParticipant[i]->a_remoteDuration.nanosec)
//				{
//					pp_remoteParticipant[i]->is_remoteAlive = false;
//				}
//			}
//
//			if(pp_remoteParticipant[i]->is_remoteAlive)
//			{
//				DataFull *p_data = malloc(sizeof(DataFull));
//				Heartbeat a_heartBeatData;
//				ParticipantMessageData *p_participantMessageData=NULL;
//				SerializedPayloadForWriter *p_serializedPlayloadwriter = malloc(sizeof(SerializedPayloadForWriter));
//				memcpy(&p_data->writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, 4);
//				memcpy(&p_data->reader_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, 4);
//				p_data->writer_sn.value.high = 0;
//				p_data->writer_sn.value.low = 1;
//				p_data->p_serialized_data = (SerializedPayload *)p_serializedPlayloadwriter;
//				p_data->inline_qos.i_parameters = 0;
//				p_data->inline_qos.pp_parameters = NULL;
//				p_data->octets_to_inline_qos = 0;
//
//				{
//					ParameterWithValue		*p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_rtps_writer->guid);
//
//					INSERT_ELEM( p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters,
//						p_data->inline_qos.i_parameters, p_parameter);
//
//					p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);
//
//					INSERT_ELEM( p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters,
//						p_data->inline_qos.i_parameters, p_parameter);
//
//					p_data->octets_to_inline_qos = 16;
//				}
//
//				p_participantMessageData = (ParticipantMessageData *)malloc(sizeof(ParticipantMessageData));
//				p_serializedPlayloadwriter->p_value = (uint8_t *)p_participantMessageData;
//				
//				p_participantMessageData->participant_guid_prefix = p_rtps_writer->p_rtps_participant->guid.guid_prefix;
//				memcpy(p_participantMessageData->kind , PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE, 4);
//				p_participantMessageData->sequence_length[0] = 0;
//				p_participantMessageData->sequence_length[1] = 0;
//				p_participantMessageData->sequence_length[2] = 0;
//				p_participantMessageData->sequence_length[3] = 1;
//				memset(p_participantMessageData->data, 0, sizeof(p_participantMessageData->data));
//				p_participantMessageData->data[0] = 1;
//				p_participantMessageData->data[1] = 0;
//				p_serializedPlayloadwriter->i_size = sizeof(ParticipantMessageData);
//				p_data->extra_flags = 0;
//
//				//send Data......................
//				rtps_send_data_with_dst_to(p_rtps_writer, pp_remoteParticipant[i]->a_remoteParticipantGuid, pp_remoteParticipant[i]->a_remotelocator, p_data);
//				FREE(p_participantMessageData);
//				FREE(p_serializedPlayloadwriter);
//
//				//send heartBeat......................
//
//				memcpy(&a_heartBeatData.writer_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, 4);
//				memcpy(&a_heartBeatData.reader_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, 4);
//				a_heartBeatData.first_sn.value.high = 0;
//				a_heartBeatData.first_sn.value.low = 1;
//				a_heartBeatData.last_sn.value.high = 0;
//				a_heartBeatData.last_sn.value.low = 0;
//				a_heartBeatData.count.value = ++pp_remoteParticipant[i]->count;
//				rtps_send_heartbeat_with_dst_to(p_rtps_writer, pp_remoteParticipant[i]->a_remoteParticipantGuid, pp_remoteParticipant[i]->a_remotelocator, a_heartBeatData);
//
//			}
//		}
//
//		mutex_unlock(&remote_lock);
//	}
//}
//#endif

bool is_exist_participant(GUID_t a_remote_participant_guid)
{
	int32_t		i;
	bool	is_found = false;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid, &a_remote_participant_guid, sizeof(GUID_t)) == 0)
		{
			pp_remoteParticipant[i]->a_lastsendTime = currenTime();
			is_found = true;
			break;
		}
	}
	mutex_unlock(&remote_lock);

	return is_found;
}



void ignoreRemoteParticipant(GUID_t a_remoteParticipantGuid)
{
	int32_t		i;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid, &a_remoteParticipantGuid, sizeof(GUID_t)) == 0)
		{
			pp_remoteParticipant[i]->is_ignore = true;
			break;
		}
	}
	mutex_unlock(&remote_lock);
}


bool is_ignore_participant( GuidPrefix_t guid_prefix )
{
	int32_t		i;
	bool	is_ignore = false;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid, &guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			if (pp_remoteParticipant[i]->is_ignore)
			{
				is_ignore = true;
			}
			break;
		}
	}
	mutex_unlock(&remote_lock);

	return is_ignore;
}

Locator_t* get_remote_participant_locator( GUID_t a_remote_guid )
{
	int32_t		i;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid.guid_prefix, &a_remote_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			mutex_unlock(&remote_lock);
			return &pp_remoteParticipant[i]->a_remotelocator;
		}
	}
	mutex_unlock(&remote_lock);

	return NULL;
}


Locator_t* get_default_remote_participant_locator( GUID_t a_remote_guid )
{
	int32_t		i;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for(i=0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid.guid_prefix, &a_remote_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			mutex_unlock(&remote_lock);
			return &pp_remoteParticipant[i]->a_defaultLocator;
		}
	}
	mutex_unlock(&remote_lock);

	return NULL;
}


Locator_t* get_remote_participant_multicast_locator( GUID_t a_remote_guid )
{
	int32_t		i;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid.guid_prefix, &a_remote_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			mutex_unlock(&remote_lock);
			return &pp_remoteParticipant[i]->a_remoteMulticastlocator;
		}
	}
	mutex_unlock(&remote_lock);

	return NULL;
}


Locator_t* get_default_remote_participant_multicast_locator( GUID_t a_remote_guid )
{
	int32_t		i;

	if (!remote_initialize)
	{
		mutex_init(&remote_lock);
		cond_init(&remote_cond);
		remote_initialize = true;
	}

	mutex_lock(&remote_lock);
	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid.guid_prefix, &a_remote_guid.guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			mutex_unlock(&remote_lock);
			return &pp_remoteParticipant[i]->a_defaultMulticastLocator;
		}
	}
	mutex_unlock(&remote_lock);

	return NULL;
}

extern Time_t time_addition(const Time_t augend_time, const Time_t addend_time);
extern bool time_left_bigger(const Time_t time_1, const Time_t time_2);


void check_remote_participant_leaseDuration()
{

	int i;

	Time_t c_time = currenTime();
	GUID_t a_remote_participant_guid;
	bool found_remove_remote_participant = false;

	if(!remote_initialize) return;

	mutex_lock(&remote_lock);


	c_time = currenTime();

	for (i = 0; i < i_remoteParticipant; i++)
	{
		Time_t d_time;
		Time_t n_time;

		d_time.sec = pp_remoteParticipant[i]->a_remoteDuration.sec;
		//d_time.sec = 5;
		d_time.nanosec = pp_remoteParticipant[i]->a_remoteDuration.nanosec; 



		n_time = time_addition(pp_remoteParticipant[i]->a_lastsendTime, d_time);

		if(time_left_bigger(c_time, n_time))
		{
			//remote Participant 제거..
			pp_remoteParticipant[i]->is_remoteAlive = false;
			a_remote_participant_guid = pp_remoteParticipant[i]->a_remoteParticipantGuid;
			found_remove_remote_participant = true;
			break;
		}
	}

	mutex_unlock(&remote_lock);

	if(found_remove_remote_participant)
	{
		remove_Participant_and_etc(a_remote_participant_guid);
	}

}


//static is_test = true;


void add_remote_participant( GUID_t a_remote_participant_guid, Locator_t a_locator, Locator_t a_default_locator, Locator_t a_mlocator, Locator_t a_default_mlocator, Duration_t a_duration )
{
	int32_t		i;
	bool	is_found = false;

	/*if(is_test == false)
		return;*/

	if(!remote_initialize) return;

	mutex_lock(&remote_lock);
	for (i = 0; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid, &a_remote_participant_guid, sizeof(GUID_t)) == 0)
		{
			pp_remoteParticipant[i]->is_remoteAlive = true;
			pp_remoteParticipant[i]->a_lastsendTime = currenTime();
			is_found = true;
			break;
		}
	}

	if (!is_found)
	{
		RemoteParticipant_t	*p_remoteParticipant = malloc(sizeof(RemoteParticipant_t));
		memset(p_remoteParticipant, 0, sizeof(RemoteParticipant_t));

		p_remoteParticipant->a_remoteParticipantGuid = a_remote_participant_guid;
		p_remoteParticipant->a_remotelocator = a_locator;
		p_remoteParticipant->a_defaultLocator = a_default_locator;
		p_remoteParticipant->a_remoteMulticastlocator = a_mlocator;
		p_remoteParticipant->a_defaultMulticastLocator = a_default_mlocator;
		p_remoteParticipant->is_remoteAlive = true;
		p_remoteParticipant->a_remoteDuration = a_duration;
		p_remoteParticipant->a_lastsendTime = currenTime();
		p_remoteParticipant->count = 0;
		p_remoteParticipant->is_ignore = false;
		INSERT_ELEM(pp_remoteParticipant,i_remoteParticipant, i_remoteParticipant, p_remoteParticipant);
		cond_signal(&remote_cond);
		trace_msg(NULL, TRACE_LOG2, "[%d]Add Remote Participant : %d.%d.%d.%d:%d:%d\r\n", i_remoteParticipant
			, a_locator.address[12], a_locator.address[13], a_locator.address[14], a_locator.address[15]
			, a_locator.port, a_default_locator.port);

		//is_test = false;
	}

	mutex_unlock(&remote_lock);
}



void remote_remote_participant( GUID_t a_remote_participant_guid )
{
	int32_t i;

	if (!remote_initialize) return;

	mutex_lock(&remote_lock);

	for (i = 0 ; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid, &a_remote_participant_guid, sizeof(GUID_t)) == 0)
		{
			FREE(pp_remoteParticipant[i]);
			REMOVE_ELEM( pp_remoteParticipant, i_remoteParticipant, i);
			break;
		}
	}

	mutex_unlock(&remote_lock);
}

//void setStopliveliness(bool state);

void destory_liveliness()
{
	if (remote_initialize == false) return;

//	b_end_liveliness = true;
	qos_set_stop_liveliness(true); //by kki (liveliness)

	if (liveliness_thread_id)
	{
		cond_signal(&remote_cond);
		thread_join2(liveliness_thread_id);
	}

	liveliness_thread_id = 0;


//	mutex_lock(&remote_lock);
	while(i_remoteParticipant)
	{
		FREE(pp_remoteParticipant[0]);
		REMOVE_ELEM( pp_remoteParticipant, i_remoteParticipant, 0);
	}
//	mutex_unlock(&remote_lock);

	mutex_destroy(&remote_lock);
	cond_destroy(&remote_cond);
	remote_initialize = false;
}

void heartbeat_wakeup_participant( GuidPrefix_t a_guid_prefix )
{
	int i;

	if (!remote_initialize) return;
	mutex_lock(&remote_lock);

	for (i = 0; i < i_remoteParticipant; i++)
	{
		if (memcmp(&pp_remoteParticipant[i]->a_remoteParticipantGuid.guid_prefix, &a_guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			pp_remoteParticipant[i]->is_remoteAlive = true;;
			pp_remoteParticipant[i]->a_lastsendTime = currenTime();
			break;
		}
	}

	mutex_unlock(&remote_lock);
}



void	*rtps_write(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message)
{
	void *ret;
	rtps_cachechange_t *p_rtps_cachechange;
	SerializedPayloadForWriter *p_serializedData = NULL;

	// 데이터 제너레이트 해야함...
	// 그러나, 내 생각에는 이부분은 SerializedData가 되어야 할 것 같다.
	// 그 이유는 데이터를 보내기 전에 다시한번 Data를 만드는데, 여기서 만드는것 하고 틀리며, 
	// cachecange에 들어갈 데이터는 아마 SerializedData일 것이다.
	// 왜냐하면 message는 다른 로직에 의해서 Delete되기 때문.
	////

		p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);

	
	////
	
	ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,ALIVE,p_serializedData,p_message->handle);
	p_message->v_related_cachechange = p_rtps_cachechange;
	
	//added by kyy(Lifespan QoS)
	//input_lifespan(p_rtps_cachechange, p_writer, p_writer->p_dataWriter->dataWriterQos.lifespan.duration.sec);
//time by jun
//	p_rtps_cachechange->source_timestamp = currenTime();
	p_rtps_cachechange->source_timestamp = p_message->source_time;
	//p_rtps_cachechange->source_timestamp.nanosec = p_statefulreader->source_timestamp.nanosec;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(p_writer->p_writer_cache->p_rtps_writer->behavior_type == STATEFUL_TYPE)
	{
		int i;
		rtps_readerproxy_t *p_rtps_readerproxy = NULL;
		rtps_statefulwriter_t *p_rtps_statefulwriter = (rtps_statefulwriter_t *)p_writer->p_writer_cache->p_rtps_writer;
		
		int size;
		bool _found = false;


		mutex_lock(&p_rtps_statefulwriter->object_lock);
		size = p_rtps_statefulwriter->i_matched_readers;

		for (i = 0; i < size; i++)
		{
			p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];
			Is_exist_in_cachechange_for_writer(p_rtps_readerproxy->remote_reader_guid, p_rtps_cachechange, p_rtps_readerproxy->p_rtps_writer->push_mode, &_found);
		}
		mutex_unlock(&p_rtps_statefulwriter->object_lock);
	}
	

	//8.2.9.1.2 Transition T2

	if (qos_check_historycache_for_writer(p_writer, p_writer->p_writer_cache, p_rtps_cachechange))
	{
		p_rtps_cachechange->p_org_message = p_message;

		/*if(p_writer->p_datawriter->id == 23)
		{
			rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		}else{*/
			rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
	//	}

		qos_liveliness_writer_set_alive(p_writer);//by kki (liveliness);

		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}


		if(p_writer->reliability_level == RELIABLE)
		{
			rtps_statefulwriter_t *p_rtps_statefulwriter = (rtps_statefulwriter_t *)p_writer;

			mutex_lock(&p_rtps_statefulwriter->object_lock);
			write_direct_reliable((rtps_statefulwriter_t*) p_writer, p_rtps_cachechange);
			mutex_unlock(&p_rtps_statefulwriter->object_lock);
		}

		//writer by jun
		//DCPS로 부터 수신된 데이터를 전송하기 위해서 job을 만들어 Writer_Thread에 전달한다. 
		//job은 한번만 수행되며, 우선 순위는 2로 설정
		//job이 추가가 되면 Writer_Thread에 시그널을 보냄
		if(p_writer->reliability_level != RELIABLE)
		{
			data_t* p_jobdata=NULL;
			DataWriter				*p_dataWriter;

			p_jobdata = data_new(0);

			p_jobdata->p_rtps_writer = (rtps_writer_t *)p_writer;
			p_dataWriter = p_writer->p_datawriter;
			//		p_jobdata->b_thread_first = true;

			p_jobdata->initial_time = currenTime();

			p_jobdata->next_wakeup_time.sec = 0;
			p_jobdata->next_wakeup_time.nanosec = 0;
			p_jobdata->priority = 2;

			timed_job_queue_time_compare_data_fifo_put_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
			//trace_msg(NULL, TRACE_LOG, "cond_signal rtps_write");
			cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
		}
	}
	else {
		// 추가하지 못한 cachechange 를 삭제
		rtps_cachechange_destory(p_rtps_cachechange);
		// SequenceNumber 를 원래대로
		p_writer->last_change_sequence_number = sequnce_number_dec(p_writer->last_change_sequence_number);
		//((FooDataWriter*)p_writer->p_datawriter)->b_write_blocked = true;
	}


//	if(p_writer->reliability_level == RELIABLE)
//	{
//
//		//int i;
//		//rtps_readerproxy_t *p_rtps_readerproxy = NULL;
//		//rtps_statefulwriter_t *p_rtps_statefulwriter = (rtps_statefulwriter_t *)p_writer;
//		//int size = p_rtps_statefulwriter->i_matched_readers;
//		//bool _found = false;
//
//		//for (i = 0; i < size; i++)
//		//{
//		//	p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];
//		//	Is_exist_in_cachechange_for_writer(p_rtps_readerproxy->remote_reader_guid, p_rtps_cachechange, p_rtps_readerproxy->p_rtps_writer->push_mode, &_found);
//		//}
//
//		write_direct_reliable((rtps_statefulwriter_t*) p_writer, p_rtps_cachechange);
//	}
//
//	
//
//	//else
//	//{
//	//	// 추가하지 못한 cachechange 를 삭제
//	//	//rtps_cachechange_destory(p_rtps_cachechange);
//	//	// SequenceNumber 를 원래대로
//	//	p_writer->last_change_sequence_number = sequnce_number_dec(p_writer->last_change_sequence_number);
//
//	//	((FooDataWriter*)p_writer->p_datawriter)->b_write_blocked = true;
//	//}
//
//
//	
//
//	//messageRelease(p_message);
//
//	//writer by jun
//	//DCPS로 부터 수신된 데이터를 전송하기 위해서 job을 만들어 Writer_Thread에 전달한다. 
//	//job은 한번만 수행되며, 우선 순위는 2로 설정
//	//job이 추가가 되면 Writer_Thread에 시그널을 보냄
//	if(p_writer->reliability_level != RELIABLE)
//	{
//		data_t* p_jobdata=NULL;
//		DataWriter				*p_dataWriter;
//
//		p_jobdata = data_new(0);
//
//		p_jobdata->p_rtps_writer = (rtps_writer_t *)p_writer;
//		p_dataWriter = p_writer->p_datawriter;
////		p_jobdata->b_thread_first = true;
//
//		p_jobdata->initial_time = currenTime();
//
//		p_jobdata->next_wakeup_time.sec = 0;
//		p_jobdata->next_wakeup_time.nanosec = 0;
//		p_jobdata->priority = 2;
//
//		timed_job_queue_time_compare_data_fifo_put_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
//		//trace_msg(NULL, TRACE_LOG, "cond_signal rtps_write");
//		cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
//	}

	
	
//	cond_signal(&p_writer->object_wait);
	return ret;
}


void	*rtps_dispose(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle)
{
	void *ret = NULL;
	rtps_cachechange_t *p_rtps_cachechange = NULL;
	message_t *p_message = message_new(p_writer->p_datawriter, v_data);
	SerializedPayloadForWriter *p_serializedData;

	if (p_message == NULL) return ret;

	p_message->handle = handle;
	p_message->sequence = ++p_writer->p_datawriter->last_sequencenum;


	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);

	//[120821,김경일] This operation has no effect if the topicKind==NO_KEY. (RTPS p.26)
	if (p_writer->topic_kind == WITH_KEY)
	{
		ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,NOT_ALIVE_DISPOSED,p_serializedData,handle);
		p_rtps_cachechange->source_timestamp = currenTime();

		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}

		cond_signal(&p_writer->object_wait);
	}

	return ret;
}



/*
void	*rtps_unregister(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, bool b_dispose)
{

	void *ret = NULL;

	
	
	rtps_cachechange_t *p_rtps_cachechange = NULL;

	message_t *p_message = message_new(p_writer->p_dataWriter, v_data);
	SerializedPayloadForWriter *p_serializedData;

	ChangeKind_t change_kind;
	change_kind = (b_dispose) ? (NOT_ALIVE_UNREGISTERED | NOT_ALIVE_DISPOSED) : NOT_ALIVE_UNREGISTERED;

	p_message->handle = handle;
	p_message->sequence = ++p_writer->p_dataWriter->last_sequencenum;




	rtps_remove_user_defined_Writer(p_writer);


	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);



	if(p_writer->topicKind == WITH_KEY){
		ret = p_rtps_cachechange = rtps_writer_new_change(p_writer, change_kind, p_serializedData,handle);
		p_rtps_cachechange->source_timestamp = currenTime();

		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->writer_cache, p_rtps_cachechange);
		if(p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}

		cond_signal(&p_writer->object_wait);
	}

	return ret;
}
*/

void	*rtps_unregister(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, bool b_dispose)
{

	void *ret = NULL;

	
	
	rtps_cachechange_t *p_rtps_cachechange = NULL;

	message_t *p_message = message_new(p_writer->p_datawriter, v_data);
	SerializedPayloadForWriter *p_serializedData;

	p_message->handle = handle;
	p_message->sequence = ++p_writer->p_datawriter->last_sequencenum;



	//[KKI] EDP로 DATA(w[UD])메시지가 날아감.
	rtps_remove_user_defined_Writer(p_writer);


	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);


	//[120821,김경일] This operation has no effect if the topicKind==NO_KEY. (RTPS p.26)
	if (p_writer->topic_kind == WITH_KEY)
	{
		ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,NOT_ALIVE_UNREGISTERED,p_serializedData,handle);
		p_rtps_cachechange->source_timestamp = currenTime();

		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}

		cond_signal(&p_writer->object_wait);
	}

	/*
	if(p_writer->topicKind == WITH_KEY){
		rtps_remove_user_defined_Writer(p_writer);
	}
	*/

	return ret;
}


SerializedPayloadForWriter* rtps_generate_dispose_unregister( DataWriter* p_datawriter, rtps_writer_t* p_rtps_writer, rtps_writer_t* p_writer, bool is_dispose, bool is_unregister )
{
	int32_t i_size = 0;
	ProtocolVersion_t	protocol = PROTOCOLVERSION;

	uint32_t endpoint_set = 0x00000000; 
	char *p_serialized;
	//VendorId_t vendor_id = {'0','0'};
	char product_version[] = "1111";
	EntityName_t entity_name = {9, "[ENTITY]"};
	int32_t	mtu = DEFAULT_MTU;
	SerializedPayloadForWriter	*p_serialized_data;
	int32_t i_temp = 0;
	char flag = 0;

	char status_info[] = {0x00,0x00,0x00,0x03};

	p_serialized_data = malloc(sizeof(SerializedPayloadForWriter));


	memset(p_serialized_data, 0, sizeof(SerializedPayloadForWriter));
	
	if (p_writer->p_accessout)
	{
		if (p_writer->p_accessout->mtu > 0)
		{
			mtu = p_writer->p_accessout->mtu;
		}
	}
	p_serialized = p_serialized_data->p_value = malloc(mtu);
	memcpy(&p_serialized_data->entity_key_guid, &p_rtps_writer->guid, sizeof(GUID_t));




	memset(p_serialized, 0, mtu);
	p_serialized[0] = 0x00;
	p_serialized[1] = 0x03;
	//i_size += 2;
	////option skip 2byte
	//i_size +=2;
	////make ParameterList

	if (is_dispose && is_unregister)
	{
		p_serialized[1] = 0x03;
		status_info[3] = 0x03;
	}
	else if (is_dispose)
	{
		p_serialized[1] = 0x01;
		status_info[3] = 0x01;
	}
	else if(is_unregister)
	{
		p_serialized[1] = 0x02;
		status_info[3] = 0x02;
	}
	else
	{
		p_serialized[1] = 0x00;
		status_info[3] = 0x00;
	}


	//PID_KEY_HASH
	gererate_parameter(p_serialized+i_size, &i_size, PID_KEY_HASH, 16, &p_rtps_writer->guid);

	//PID_STATUS_INFO
	gererate_parameter(p_serialized+i_size, &i_size, PID_STATUS_INFO, 4, status_info);



	//PID_SENTINEL
	gererate_parameter(p_serialized+i_size, &i_size, PID_SENTINEL, 0, NULL);

	i_temp = i_size % 4;

	if(i_temp)
	{
		p_serialized_data->i_size = i_size + (4-i_temp);
	}
	else
	{
		p_serialized_data->i_size = i_size;
	}

	return p_serialized_data;
}


int32_t Writer_Thread(rtps_writer_t* p_rtps_writer)
{
	int test;
	bool b_thread_first = true;
	test = 0;

	while (!p_rtps_writer->b_end)
	{
		//by kki...(entity factory)
		if (!qos_is_enabled((Entity*) p_rtps_writer->p_datawriter))
		{
			msleep(1);
			continue;
		}

		if(p_rtps_writer->b_end) break;

		mutex_lock(&p_rtps_writer->object_lock);

		if(p_rtps_writer->behavior_type == STATELESS_TYPE)
		{
			uint32_t waketime = ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.sec * 1000 
				+ ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.nanosec /1000000;
			if (!b_thread_first)
			{
				cond_waittimed(&p_rtps_writer->object_wait,&p_rtps_writer->object_lock, waketime);
			}
			else
			{
				b_thread_first = false;
			}
		}
		else
		{
			uint32_t waketime = ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.sec * 1000 
				+ ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.nanosec /1000000;
			if (!b_thread_first)
			{
				cond_waittimed(&p_rtps_writer->object_wait,&p_rtps_writer->object_lock, waketime);
			}else
			{
				b_thread_first = false;
			}
		}

		mutex_unlock(&p_rtps_writer->object_lock);

		if(p_rtps_writer->b_end) break;
		
		if(p_rtps_writer->behavior_type == STATELESS_TYPE)
		{
			/*
				The SPDPbuiltinParticipantWriter periodically sends this data-object to a pre-configured list of locators to announce the
				Participant’s presence on the network. This is achieved by periodically calling StatelessWriter::unsent_changes_reset,
				which causes the StatelessWriter to resend all changes present in its HistoryCache to all locators. The periodic rate at
				which the SPDPbuiltinParticipantWriter sends out the SPDPdiscoveredParticipantData defaults to a PSM specified
				value. This period should be smaller than the leaseDuration specified in the SPDPdiscoveredParticipantData (see also
				Section 8.5.3.3.2).
			*/

			rtps_statelesswriter_unsent_changes_reset((rtps_statelesswriter_t *)p_rtps_writer);
		}

		if (p_rtps_writer->behavior_type == STATELESS_TYPE && p_rtps_writer->reliability_level == BEST_EFFORT)
		{
			best_effort_statelesswriter_behavior((rtps_statelesswriter_t *)p_rtps_writer);
		}
		else if (p_rtps_writer->behavior_type == STATEFUL_TYPE && p_rtps_writer->reliability_level == RELIABLE)
		{
			reliable_statefulwriter_behavior((rtps_statefulwriter_t *)p_rtps_writer);
		}
		else if (p_rtps_writer->behavior_type == STATEFUL_TYPE && p_rtps_writer->reliability_level == BEST_EFFORT)
		{
			best_effort_statefulwriter_behavior((rtps_statefulwriter_t *)p_rtps_writer);
		}
	}
	
	return MODULE_SUCCESS;
}

void calculate_nextWakeupTime(data_t *p_data, rtps_writer_t *p_rtps_writer);

int writer_behavior(rtps_writer_t *p_rtps_writer)
{
	p_rtps_writer->checked_time = currenTime();


	if (p_rtps_writer->behavior_type == STATELESS_TYPE)
	{

		/*
			The SPDPbuiltinParticipantWriter periodically sends this data-object to a pre-configured list of locators to announce the
			Participant’s presence on the network. This is achieved by periodically calling StatelessWriter::unsent_changes_reset,
			which causes the StatelessWriter to resend all changes present in its HistoryCache to all locators. The periodic rate at
			which the SPDPbuiltinParticipantWriter sends out the SPDPdiscoveredParticipantData defaults to a PSM specified
			value. This period should be smaller than the leaseDuration specified in the SPDPdiscoveredParticipantData (see also
			Section 8.5.3.3.2).
		*/

		rtps_statelesswriter_unsent_changes_reset((rtps_statelesswriter_t *)p_rtps_writer);
	}

	if (p_rtps_writer->behavior_type == STATELESS_TYPE && p_rtps_writer->reliability_level == BEST_EFFORT)
	{
		//trace_msg(NULL, TRACE_LOG, "best_effort_statelesswriter_behavior start 1");
		best_effort_statelesswriter_behavior((rtps_statelesswriter_t *)p_rtps_writer);
	}
	else if (p_rtps_writer->behavior_type == STATEFUL_TYPE && p_rtps_writer->reliability_level == RELIABLE)
	{
		reliable_statefulwriter_behavior((rtps_statefulwriter_t *)p_rtps_writer);
	}
	else if (p_rtps_writer->behavior_type == STATEFUL_TYPE && p_rtps_writer->reliability_level == BEST_EFFORT)
	{
				
		best_effort_statefulwriter_behavior((rtps_statefulwriter_t *)p_rtps_writer);
	}

	return 0;
}

int writer_thread2(DomainParticipant* p_domainparticipant)
{
	data_t *r_job;
	int i=0;
	time_t waketime =1000;
	rtps_writer_t * p_rtps_writer;
	Time_t current_time;
	int temp_sec;
	int temp_nanosec;
	int i_new_job;


	while (!is_quit() && !p_domainparticipant->b_end2)
	{

suspend :

		if (p_domainparticipant->b_end2) break;

		//새로운 writer가 추가되거나 외부로 부터 event(데이터 전달,ack/nack )되는 경우과 반복되는 job 중 다음 job이 수행되어야 할 시간이 
		//경과되는 경우 wakeup 된다.
		//waketime = 10000000;

		//trace_msg(NULL, TRACE_LOG,"s->p_domainparticipant->thread_id2 : %p", p_domainparticipant->thread_id2);

		mutex_lock( &p_domainparticipant->writer_thread_lock );
		//trace_msg(NULL, TRACE_LOG,"e->p_domainparticipant->thread_id2 : %p", p_domainparticipant->thread_id2);
		//trace_msg(NULL, TRACE_LOG, "Writer_Thread2 %ld", waketime);

		cond_waittimed2(&p_domainparticipant->writer_thread_wait,&p_domainparticipant->writer_thread_lock, waketime);
		mutex_unlock( &p_domainparticipant->writer_thread_lock );

		if (p_domainparticipant->b_end2) break;

		
		//trace_msg(NULL, TRACE_LOG, "p_domainparticipant waked");
		//1. 3. job이 queue에 잇는 경우
		//새로운 job이 추가되면 job queue의 i_new_job 카운트를 증가하게 하엿다. //2.의 반복적인 job은 이값이 증가되지 않음
		if ((i_new_job = get_number_new_job(p_domainparticipant->p_writer_thread_fifo)) > 0 )
		{
			//trace_msg(NULL, TRACE_LOG, "Writer_Thread2 job 1 = (%d):%ld", p_domainparticipant->p_writer_thread_fifo->i_depth, waketime);
			//1.3.에 해당하는 모든 작업이 처리 될 때까지 수행
			while ((i_new_job = get_number_new_job(p_domainparticipant->p_writer_thread_fifo)) > 0 )
			{
				//job을 하나 뽑아옴 i_new_job 의 개수를 한개 감소
				if ((r_job = timed_job_queue_data_fifo_get_remove_job(p_domainparticipant->p_writer_thread_fifo)) !=NULL)
				{
					p_rtps_writer = r_job->p_rtps_writer;

					if(p_rtps_writer->b_end)
					{
						FREE(p_rtps_writer);
						goto suspend;
					}


					if (p_rtps_writer->p_datawriter->p_publisher->is_suspend)
					{
						goto suspend;
					}

					//added by kyy(Presentation QoS Writer Side)
					//다음 begin_coherent 가 true가 될 때까지 loop을 돈다.
					if (p_rtps_writer->p_datawriter->p_publisher->begin_coherent == false && p_rtps_writer->p_datawriter->begin_coherent == true)
					{
//						goto suspend;
					}

					current_time = currenTime();

					//2.의 job, job이 외부로 부터 추가된 경우
					if (r_job->priority == 2)
					{
					// behavior 실행

						//if(p_rtps_writer->p_datawriter->id == 23)
						/*{
							p_rtps_writer->p_datawriter->id = 23;
						}else{*/
					writer_behavior(p_rtps_writer);
						//}

//일회성이므로 job release
						if (r_job)
						data_release(r_job);
//새로운 job을 하나 추출, 추출하는 이유는 
//condwaittimed에서 기다릴 시간을 계산하기 위해서 사용 job queue에는 반복적으로 동작해야 하는 job이 같이 존재하므로 현재 job 3.가 수행되고 나서 반복적으로 동작해야 할 job 2.가 있는지 확인하고 waketime을 계산하기 위해서 사용
						if ((r_job = timed_job_queue_data_fifo_get(p_domainparticipant->p_writer_thread_fifo)) != NULL)
						{
//만약 다음 것도 job1.과 job3.이면 계속 수행하기 위해서 queue에 집어 넣음
							if(r_job->priority == 0 || r_job->priority == 2) {

								timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);

							}
							else {
//job이 반복적으로 동작하는 것이면 다음 waketime을 계산
								//시간이 지난 놈 체크 waketime =0;
								
								temp_sec = r_job->next_wakeup_time.sec - current_time.sec;
								temp_nanosec = r_job->next_wakeup_time.nanosec - current_time.nanosec;

								waketime = (temp_sec)* 1000 + (temp_nanosec)/1000000;
//waketime을 계산했는데, job3.을 처리하느라고 waketime이 지나버린 경우 바로 job2.(반복 작엄)을 수행하기 위해 waketime을 0으로 설정
								//여기서 한번 더 실행 시간이 지난 것
								if(waketime < 0)	//시간이 지난 것 체크
									waketime = 0;

								timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
							}
						} 
						else {
							
							waketime = 1000;

						}
					
					}
					else if(r_job->priority == 0) {
//job1.인 경우 새로운 writer가 생성된 경우이다. 
						p_rtps_writer->b_thread_first = false;
						
						// behavior 실행
	
						writer_behavior(p_rtps_writer);

//job1.은 한번 수행된 후 반복 작업인 job2.로 수행되어야 하기 때문에 현재 시간을 기준으로 다음에 수행되어야 할 시간(wakeuptime)dmf rPtks 
						calculate_nextWakeupTime(r_job, p_rtps_writer);
//job2.로 동작되기 위해서 추가						
						timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
//혹시 처리되지 않고 남은 job1. 3.가 있는지 확인
						if((r_job = timed_job_queue_data_fifo_get(p_domainparticipant->p_writer_thread_fifo)) != NULL){

							if(r_job->priority == 0 || r_job->priority == 2) {
		
								timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);

							}
							else {
//다음 job이 수행되어야 할 시간 계산	
								//????여기서 한번 더 실행 
								temp_sec = r_job->next_wakeup_time.sec - current_time.sec;
								temp_nanosec = r_job->next_wakeup_time.nanosec - current_time.nanosec;
								waketime = (temp_sec)* 1000 + (temp_nanosec)/1000000;
//job1.을 수행하느라 처리 되지 못한 job 처리를 위해서 							
								//여기서 한번 더 실행 시간이 지난 것 
								if(waketime < 0)	//시간이 지난 것 체크
									waketime = 0;
								

								timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
							}
						} 
						else {

							waketime = 1000;

						}
					}
				}
				else{

					waketime = 1000;
				}
			} 
		}
		else{
			//trace_msg(NULL, TRACE_LOG, "Writer_Thread2 job 2 = (%d)", p_domainparticipant->p_writer_thread_fifo->i_depth);
//job2.를 수행하기 위해서 사용
			if((r_job = timed_job_queue_data_fifo_get(p_domainparticipant->p_writer_thread_fifo)) != NULL){
				//trace_msg(NULL, TRACE_LOG, "Writer_Thread2 r_job 2 = (%p:%d)", r_job, r_job->priority);
//혹시나 job1.3.가 있는지 확인
				if(r_job->priority == 0 || r_job->priority == 2) {

					timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
					waketime = 0;

				}
				else
				{
					p_rtps_writer = r_job->p_rtps_writer;
					current_time = currenTime();

					if(p_rtps_writer->b_end)
					{
						FREE(p_rtps_writer);
						goto suspend;
					}

					if (p_rtps_writer->p_datawriter->p_publisher->is_suspend)
					{
						goto suspend;
					}

					if (p_rtps_writer->p_datawriter->p_publisher->begin_coherent)
					{
//						goto suspend;
					}

//없는경우 현재시간과 job이 수행되어야 할 시간을 비교
					if ((current_time.sec > r_job->next_wakeup_time.sec)||
					(current_time.sec == r_job->next_wakeup_time.sec && current_time.nanosec >= r_job->next_wakeup_time.nanosec ))
					{
//만약 수행될 시간을 지난 경우 
						// behavior 실행

						if (!qos_is_liveliness_job(r_job))//by kki (liveliness)
						{
							writer_behavior(p_rtps_writer);
	
		// 현재 job이 다음에 실행될 시간을 계산					
							calculate_nextWakeupTime(r_job, p_rtps_writer);
						}
						else
						{
							qos_liveliness_check_wakeup_time_for_writer(r_job); //by kki (liveliness)
						}
						timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
						waketime = 0;
					}
					else
					{
//아직 job2.가 처리되어야 할 시간이 되지 않아서 waketime을 다시 계산
						temp_sec = r_job->next_wakeup_time.sec - current_time.sec;
						temp_nanosec = r_job->next_wakeup_time.nanosec - current_time.nanosec;
						waketime = (temp_sec)* 1000 + (temp_nanosec)/1000000;

						timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);

						if ((r_job = timed_job_queue_data_fifo_get(p_domainparticipant->p_writer_thread_fifo)) != NULL)
						{
							if (r_job->priority == 0 || r_job->priority == 2)
							{
								waketime = 0;
							}
							timed_job_queue_time_compare_data_fifo_put(p_domainparticipant->p_writer_thread_fifo,r_job);
						}
					}
				}

			} 
			else{
				

				waketime = 1000;

			}
			
		}

	}
	return MODULE_SUCCESS;
}

void calculate_nextWakeupTime(data_t *p_data, rtps_writer_t *p_rtps_writer)
{

	if(p_data->next_wakeup_time.sec == 0){
		if (p_rtps_writer->behavior_type == STATELESS_TYPE)
		{
			p_data->priority = 4;
			//SPDP Period
			//((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.sec = 30;
			((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.sec = 3;
			p_data->next_wakeup_time.sec = p_data->initial_time.sec + ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.sec;
			p_data->next_wakeup_time.nanosec = p_data->initial_time.nanosec + ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.nanosec;
		}
		else
		{
			p_data->priority = 4;
			//((rtps_statefulwriter_t *)p_rtps_writer)->heartbeatPeriod.sec = 10;
			((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.sec = 1;
			p_data->next_wakeup_time.sec = p_data->initial_time.sec + ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.sec;
			p_data->next_wakeup_time.nanosec = p_data->initial_time.nanosec + ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.nanosec;
		}
	}
	else
	{
		if (p_rtps_writer->behavior_type == STATELESS_TYPE)
		{
			p_data->priority = 4;
			p_data->next_wakeup_time.sec = p_data->next_wakeup_time.sec + ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.sec;
			p_data->next_wakeup_time.nanosec = p_data->next_wakeup_time.nanosec + ((rtps_statelesswriter_t *)p_rtps_writer)->resend_data_period.nanosec;
		}
		else
		{
			p_data->priority = 4;
			//((rtps_statefulwriter_t *)p_rtps_writer)->heartbeatPeriod.sec = 10;
			((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.sec = 1;
			p_data->next_wakeup_time.sec = p_data->next_wakeup_time.sec + ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.sec;
			p_data->next_wakeup_time.nanosec = p_data->next_wakeup_time.nanosec + ((rtps_statefulwriter_t *)p_rtps_writer)->heartbeat_period.nanosec;
		}
	}
}





bool is_matched_remote_subscriber( char* p_topic_name, char* p_type_name, void* p_cache )
{


	rtps_cachechange_t	*p_rtps_cachechange = (rtps_cachechange_t	*)p_cache;

	if(p_cache == NULL) return false;


	if(p_rtps_cachechange->p_data_value != NULL)
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


void get_subscription_builtin_topic_data( SubscriptionBuiltinTopicData* p_subscription_data, void* p_cache )
{
	rtps_cachechange_t	*p_rtps_cachechange = (rtps_cachechange_t	*)p_cache;

	if(p_subscription_data == NULL || p_cache == NULL) return;


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

			memcpy(&p_subscription_data->key, &a_remoteEndpointGuid, sizeof(GUID_t));

		}

		i_found = 0;

		find_parameter_list((char*)p_serialized->p_value,i_size, PID_TOPIC_NAME, &p_value, &i_found);
		if(i_found && p_value)
		{
			i_found = 0;
			p_TopicName = p_value+4;
			p_subscription_data->topic_name = strdup(p_TopicName);


			find_parameter_list((char*)p_serialized->p_value,i_size, PID_TYPE_NAME, &p_value, &i_found);
			if(i_found && p_value)
			{
				p_TypeName = p_value+4;

				p_subscription_data->type_name = strdup(p_TypeName);

				////////////////////////////////////////////////////////////////////
				//qos 추가 (added by kyy)
				i_found = 0;
				qos_find_subscription_qos_from_parameterlist(p_subscription_data, (char*)p_serialized->p_value, i_size);
				//////////////////////////////////////////////////////////////////////////




			}

		}

	}
}


void ignore_readerproxy( void* v_writer, void* v_key )
{
	rtps_readerproxy_t *p_readerproxy = NULL;
	rtps_statefulwriter_t *p_statefulwriter = (rtps_statefulwriter_t *)v_writer;

	GUID_t *key = v_key;

	if(p_statefulwriter==NULL || p_statefulwriter->behavior_type == STATELESS_TYPE) return;

	p_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, *key);


	if(p_readerproxy)
	{
		p_readerproxy->is_ignore = true;
	}
}


void ignore_readerproxy_guid_prefix( void* v_writer, void* v_key )
{
	rtps_readerproxy_t *p_readerproxy = NULL;
	rtps_statefulwriter_t *p_statefulwriter = (rtps_statefulwriter_t *)v_writer;
	int i;
	int i_size;

	GUID_t *key = v_key;

	if(p_statefulwriter==NULL || p_statefulwriter->behavior_type == STATELESS_TYPE) return;

	mutex_lock(&p_statefulwriter->object_lock);

	i_size = p_statefulwriter->i_matched_readers;

	for(i=0; i < i_size; i++)
	{
		if(memcmp(&p_statefulwriter->pp_matched_readers[i]->remote_reader_guid, key, sizeof(GuidPrefix_t)) == 0)
		{
			mutex_unlock(&p_statefulwriter->object_lock);
			p_readerproxy = p_statefulwriter->pp_matched_readers[i];
			break;
		}
	}


	mutex_unlock(&p_statefulwriter->object_lock);


	if(p_readerproxy)
	{
		p_readerproxy->is_ignore = true;
	}
}




//timestamp by jun

void	*rtps_write_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message, Time_t source_timestamp)
{
	void *ret;
	rtps_cachechange_t *p_rtps_cachechange;
	SerializedPayloadForWriter *p_serializedData = NULL;

	// 데이터 제너레이트 해야함...
	// 그러나, 내 생각에는 이부분은 SerializedData가 되어야 할 것 같다.
	// 그 이유는 데이터를 보내기 전에 다시한번 Data를 만드는데, 여기서 만드는것 하고 틀리며, 
	// cachecange에 들어갈 데이터는 아마 SerializedData일 것이다.
	// 왜냐하면 message는 다른 로직에 의해서 Delete되기 때문.
	////


	

	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);

	////
	
	ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,ALIVE,p_serializedData,p_message->handle);
	p_message->v_related_cachechange = p_rtps_cachechange;
	
	//added by kyy(Lifespan QoS)
	//input_lifespan(p_rtps_cachechange, p_writer, p_writer->p_dataWriter->dataWriterQos.lifespan.duration.sec);
	//timestamp by jun
	p_rtps_cachechange->source_timestamp = source_timestamp;
	//p_rtps_cachechange->source_timestamp.nanosec = p_statefulreader->source_timestamp.nanosec;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//8.2.9.1.2 Transition T2

	//by kki...(history)
	if (qos_check_historycache_for_writer(p_writer, p_writer->p_writer_cache, p_rtps_cachechange))
	{
		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}
	}
	//else
	//{
	//	// 추가하지 못한 cachechange 를 삭제
	//	//rtps_cachechange_destory(p_rtps_cachechange);
	//	// SequenceNumber 를 원래대로
	//	p_writer->last_change_sequence_number = sequnce_number_dec(p_writer->last_change_sequence_number);

	//	((FooDataWriter*)p_writer->p_datawriter)->b_write_blocked = true;
	//}


	//messageRelease(p_message);

	//writer by jun
	//DCPS로 부터 수신된 데이터를 전송하기 위해서 job을 만들어 Writer_Thread에 전달한다. 
	//job은 한번만 수행되며, 우선 순위는 2로 설정
	//job이 추가가 되면 Writer_Thread에 시그널을 보냄
	{
		data_t* p_jobdata=NULL;
		DataWriter				*p_dataWriter;

		p_jobdata = data_new(0);

		p_jobdata->p_rtps_writer = (rtps_writer_t *)p_writer;
		p_dataWriter = p_writer->p_datawriter;
//		p_jobdata->b_thread_first = true;

		p_jobdata->initial_time = currenTime();

		p_jobdata->next_wakeup_time.sec = 0;
		p_jobdata->next_wakeup_time.nanosec = 0;
		p_jobdata->priority = 2;

		timed_job_queue_time_compare_data_fifo_put_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
		//trace_msg(NULL, TRACE_LOG, "cond_signal rtps_write");
		cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
	}

	
//	cond_signal(&p_writer->object_wait);
	return ret;
}


void	*rtps_dispose_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t source_timestamp)
{
	void *ret = NULL;
	rtps_cachechange_t *p_rtps_cachechange = NULL;
	message_t *p_message = message_new(p_writer->p_datawriter, v_data);
	SerializedPayloadForWriter *p_serializedData;

	if(p_message == NULL) return ret;

	p_message->handle = handle;
	p_message->sequence = ++p_writer->p_datawriter->last_sequencenum;


	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);

	//[120821,김경일] This operation has no effect if the topicKind==NO_KEY. (RTPS p.26)
	if (p_writer->topic_kind == WITH_KEY)
	{
		ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,NOT_ALIVE_DISPOSED,p_serializedData,handle);
//timestamp by jun
		p_rtps_cachechange->source_timestamp = source_timestamp;

		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}

		cond_signal(&p_writer->object_wait);
	}

	return ret;
}


void	*rtps_unregister_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t source_timestamp)
{

	void *ret = NULL;

	
	
	rtps_cachechange_t *p_rtps_cachechange = NULL;

	message_t *p_message = message_new(p_writer->p_datawriter, v_data);
	SerializedPayloadForWriter *p_serializedData;

	p_message->handle = handle;
	p_message->sequence = ++p_writer->p_datawriter->last_sequencenum;



	//[KKI] EDP로 DATA(w[UD])메시지가 날아감.
	rtps_remove_user_defined_Writer(p_writer);


	p_serializedData = rtps_generate_serialized_payload(p_writer, p_message);


	//[120821,김경일] This operation has no effect if the topicKind==NO_KEY. (RTPS p.26)
	if (p_writer->topic_kind == WITH_KEY)
	{
		ret = p_rtps_cachechange = rtps_writer_new_change(p_writer,NOT_ALIVE_UNREGISTERED,p_serializedData,handle);
//timestamp by jun
		p_rtps_cachechange->source_timestamp = source_timestamp;

		p_rtps_cachechange->p_org_message = p_message;
		rtps_historycache_add_change(p_writer->p_writer_cache, p_rtps_cachechange);
		if (p_rtps_cachechange->sequence_number.high != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).high
			|| p_rtps_cachechange->sequence_number.low != rtps_historycache_get_seq_num_max(p_writer->p_writer_cache).low)
		{
			trace_msg(OBJECT(p_this),TRACE_ERROR,"When Write.. Sequence wrong in RTPS(rtps_write)");
		}

		cond_signal(&p_writer->object_wait);
	}

	/*
	if(p_writer->topicKind == WITH_KEY){
		rtps_remove_user_defined_Writer(p_writer);
	}
	*/

	return ret;
}


void register_instance_timestamp_set_to_cache_for_writer(void *related_cache, Time_t value)
{
	rtps_cachechange_t *p_rtps_cachechange = (rtps_cachechange_t *)related_cache;

	if(p_rtps_cachechange)
	{
		p_rtps_cachechange->source_timestamp = value;
	}
}



static void write_direct_reliable(rtps_statefulwriter_t* p_rtps_statefulwriter, rtps_cachechange_t *p_change)
{
	int i , size;
	int	i_unsentchanges = 0;
	rtps_readerproxy_t *p_rtps_readerproxy = NULL;
	rtps_cachechange_t **p_rtps_cachechanges = NULL;
	Time_t current;
	bool is_sentHeartbeat = false;
	int i_unacked = 0;
	int i_request_chages = 0;
	rtps_cachechange_t **request_chaanges = NULL;
	DataWriter *p_dataWriter = p_rtps_statefulwriter->p_datawriter;
//time by jun
	Time_t message_time;

	//added by kyy
	bool correct_deadline = true;
	bool send_data = false;
	cache_for_guid		*cache_for_guids;
	

	size = p_rtps_statefulwriter->i_matched_readers;
	
	for (i = 0; i < size; i++)
	{
		p_rtps_readerproxy = p_rtps_statefulwriter->pp_matched_readers[i];

		if (p_rtps_readerproxy->is_ignore == true) continue;
						
		{

			message_time = p_change->source_timestamp;
			/*if(p_rtps_statefulwriter->pushMode){
				p_change->is_relevant = true;
			}else{
				*p_change->status = UNDERWAY;
			}*/

			

			cache_for_guids = rtps_cachechange_get_associated_cache_for_guid(p_rtps_readerproxy->remote_reader_guid, p_change);

			if(cache_for_guids == NULL) continue;

			

			////////cache_for_guids->is_relevant = true;//by kki(130917)
			cache_for_guids->status = UNDERWAY;

			if(cache_for_guids->is_relevant)
			{
				bool is_send_data_and_heartbeat = false;
				DataFull *p_data = rtps_data_new(p_change);

				//added by kyy(Presentation QoS Writer Side)
				//InlineQoS 입력조건 설정
				//if(1){//p_rtps_readerproxy->expectsInlineQos){
				if (p_rtps_readerproxy->p_rtps_writer->is_builtin == false)
				{
					p_rtps_readerproxy->expects_inline_qos = true;
				}
				////////////////////////////////////////////////////////////////////////

				if (p_rtps_readerproxy->expects_inline_qos)
				{
					SerializedPayloadForWriter *p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
					ParameterWithValue		*p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_serializedPlayloadwriter->entity_key_guid);

					insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);

					//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//Make COHERENT SET PARAMETER
					if (p_rtps_readerproxy->p_rtps_writer->is_builtin != true)
					{
						qosim_process_coherent_set(p_rtps_readerproxy, p_change);										
						qosim_make_coherent_set_parameter(p_data,&p_rtps_readerproxy->p_rtps_writer->p_writer_cache->coherent_set_number);										
					}
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);

					insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
				}

				p_data->octets_to_inline_qos = 16;
				p_data->extra_flags = 0;
				p_data->reader_id.value = ENTITYID_UNKNOWN;


				if(p_rtps_statefulwriter->last_change_sequence_number.low % p_rtps_statefulwriter->p_writer_cache->history_max_length == 0)
				{
					is_send_data_and_heartbeat = true;
					
				}

				//added by kyy(Presentation QoS Writer Side)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//HeartBeat 전송
				if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->p_publisher->begin_coherent == false && p_change->p_data_value->p_value == NULL)
				{
					is_send_data_and_heartbeat = true;
					//SendHeartBeat(p_rtps_statefulwriter,p_rtps_readerproxy,false);
					//printf("Send HEART BEAT Message");
				}
				//////////////////////////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////




				if(is_send_data_and_heartbeat)
				{
					rtps_cachechange_t *p_change_atom = NULL;
					p_change_atom = (rtps_cachechange_t *)p_rtps_statefulwriter->p_writer_cache->p_head_last;

					
					if(p_rtps_statefulwriter->b_enable_multicast)
					{
						rtps_send_data_and_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy, *p_rtps_readerproxy->pp_multicast_locator_list[0], p_data, true, false, message_time);
					}else{
					
						rtps_send_data_and_heartbeat_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, false, message_time);
					}

					block_write_for_waiting_ack(p_rtps_statefulwriter);

				}else{
					if(p_rtps_statefulwriter->b_enable_multicast)
					{
						rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_multicast_locator_list[0], p_data, true, message_time);
					}else{
					
						rtps_send_data_with_dst_to((rtps_writer_t *)p_rtps_statefulwriter, p_rtps_readerproxy->remote_reader_guid, *p_rtps_readerproxy->pp_unicast_locator_list[0], p_data, false, message_time);
					}
				}

				

			
				p_rtps_readerproxy->i_for_writer_state = PUSHING_STATE;

				send_data = true;

				
				p_rtps_readerproxy->flags = 0x00;


				if(p_rtps_statefulwriter->b_enable_multicast)
				{
					ChangeStatefulwriterMulticast(p_rtps_statefulwriter, p_change, p_rtps_readerproxy->pp_multicast_locator_list[0]);
				
				}

				

			}
		}
		//added_by_kyy(DataWriter Deadline QoS)
		//Deadline 검사 후 base_time 수정
		if(p_rtps_readerproxy->start == 0)
		{
			current = currenTime();
			p_rtps_readerproxy->deadline_base_time.sec = current.sec;
			p_rtps_readerproxy->deadline_base_time.nanosec = current.nanosec;
			p_rtps_readerproxy->start++;
		}
		correct_deadline = qos_compare_deadline(current_duration(), &(p_rtps_readerproxy->deadline_base_time), p_rtps_statefulwriter->p_datawriter->datawriter_qos.deadline.period);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		if (correct_deadline == false)
		{
			change_OfferedDeadlineMissed_status(p_rtps_statefulwriter->p_datawriter, 0);
		}
	}
		
}


void block_write_for_waiting_ack(rtps_statefulwriter_t *p_rtps_statefulwriter)
{
	uint32_t waittime = get_millisecond(p_rtps_statefulwriter->p_datawriter->datawriter_qos.reliability.max_blocking_time);
	
	mutex_lock(&p_rtps_statefulwriter->p_datawriter->entity_lock);
	cond_waittimed(&p_rtps_statefulwriter->p_datawriter->entity_wait, &p_rtps_statefulwriter->p_datawriter->entity_lock, waittime);
	mutex_unlock(&p_rtps_statefulwriter->p_datawriter->entity_lock);
}