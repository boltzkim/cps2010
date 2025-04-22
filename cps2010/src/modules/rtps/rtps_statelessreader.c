/*
	RTSP StatelessReader class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"

rtps_statelessreader_t *rtps_statelessreader_new()
{

	rtps_statelessreader_t *p_rtps_statelessreader = malloc(sizeof(rtps_statelessreader_t));
	memset(p_rtps_statelessreader, 0, sizeof(rtps_statelessreader_t));

	rtps_reader_init((rtps_reader_t *)p_rtps_statelessreader);

	p_rtps_statelessreader->behavior_type = STATELESS_TYPE;
	return p_rtps_statelessreader;
}

void rtps_statelessreader_destory(rtps_statelessreader_t *p_rtps_statelessreader){
	if(p_rtps_statelessreader)
	{
		//FREE(p_rtps_statelessreader);
	}
}


void remove_Participant_and_etc(GUID_t remote_pariticpant_guid)
{
	rtps_statefulwriter_t *p_statefulwriter = NULL;
	rtps_statefulreader_t *p_statefulreader = NULL;
	rtps_readerproxy_t *p_readerproxy = NULL;
	rtps_writerproxy_t *p_writerproxy = NULL;
	GUID_t a_reader_guid;
	GUID_t a_writer_guid;



	a_reader_guid = remote_pariticpant_guid;
	a_writer_guid = remote_pariticpant_guid;

	rtps_participant_lock();
	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_publication_writer();

	a_reader_guid.entity_id = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
	p_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, a_reader_guid);
	if (p_readerproxy) {
		rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_readerproxy);
	}

	p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_Builtin_publication_reader();

	a_writer_guid.entity_id = ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
	p_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, a_writer_guid);
	if (p_writerproxy)
	{
		rtps_statefulreader_matched_writer_remove(p_statefulreader, p_writerproxy);

	}

	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_subscription_writer();

	a_reader_guid.entity_id = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
	p_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, a_reader_guid);
	if (p_readerproxy) rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_readerproxy);

	p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_builtin_subscription_reader();

	a_writer_guid.entity_id = ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER;
	p_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, a_writer_guid);
	if (p_writerproxy)
	{
		rtps_statefulreader_matched_writer_remove(p_statefulreader, p_writerproxy);
	}

	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_topic_writer();

	if (p_statefulwriter)
	{
		a_reader_guid.entity_id = ENTITYID_SEDP_BUILTIN_TOPIC_READER;
		p_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, a_reader_guid);
		if (p_readerproxy) rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_readerproxy);
	}

	p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_builtin_topic_reader();
	if (p_statefulwriter)
	{
		a_writer_guid.entity_id = ENTITYID_SEDP_BUILTIN_TOPIC_WRITER;
		p_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, a_writer_guid);
		if (p_writerproxy) rtps_statefulreader_matched_writer_remove(p_statefulreader, p_writerproxy);
	}


	p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_participant_message_writer();

	a_reader_guid.entity_id = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;
	p_readerproxy = rtps_statefulwriter_matched_reader_lookup(p_statefulwriter, a_reader_guid);
	if(p_readerproxy) rtps_statefulwriter_matched_reader_remove(p_statefulwriter, p_readerproxy);

	p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_builtin_participant_message_reader();

	a_writer_guid.entity_id = ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER;
	p_writerproxy = rtps_statefulreader_matched_writer_lookup(p_statefulreader, a_writer_guid);
	if (p_writerproxy)
	{
		rtps_statefulreader_matched_writer_remove(p_statefulreader, p_writerproxy);
	}

	rtps_participant_unlock();


	{
		remove_matched_endpoint_proxy(remote_pariticpant_guid);
	}


	
	remote_remote_participant(remote_pariticpant_guid);
	

	
}


extern int i_remoteParticipant;


void best_effort_statelessreader_behavior(rtps_statelessreader_t *p_rtps_statelessreader, int state, DataFull **pp_datafull)
{
	DataReader *p_datareader = p_rtps_statelessreader->p_datareader;
	rtps_statefulwriter_t *p_statefulwriter = NULL;
	rtps_statefulreader_t *p_statefulreader = NULL;
	Locator_t *p_unicatlocator = NULL, *p_multicatlocator = NULL;
	rtps_writerproxy_t *p_rtpswriterproxy = NULL;
	rtps_cachechange_t	*p_rtps_cachechange = NULL;
	 DataFull *p_datafull = *pp_datafull;

	if(p_datareader == NULL || p_datafull == NULL) return;


	//trace_msg(NULL, TRACE_LOG, "Call best_effort_statelessreader_behavior : \r\n");
	

	if(strcmp(p_datareader->get_topicdescription(p_datareader)->topic_name, PARTICIPANTINFO_NAME) == 0)
	{

		SerializedPayloadForReader *p_serialized = (SerializedPayloadForReader *)p_datafull->p_serialized_data;
		int	i_size = 0;
		char *p_value = NULL;
		int	i_found = 0;
		BuiltinEndpointSet_t availableEndpoints;
		GUID_t a_remoteguid;
		Locator_t a_remoteLocator;
		Locator_t a_remoteDefaultLocator;
		Locator_t a_remoteMulticastLocator;
		Locator_t a_remoteDefaultMulticastLocator;
		Duration_t a_remoteDuration;
		bool is_found_metatraffic_multicastlocator = false;

		memset(&a_remoteLocator, 0, sizeof(Locator_t));
		memset(&a_remoteDefaultLocator, 0, sizeof(Locator_t));
		memset(&a_remoteMulticastLocator, 0, sizeof(Locator_t));
		memset(&a_remoteDefaultMulticastLocator, 0, sizeof(Locator_t));


		if(p_serialized) i_size = p_serialized->i_size;

		if(i_size == 0){
			if(p_datafull->inline_qos.i_linked_size)
			{
				int i=0;
				GUID_t remote_pariticpant_guid;
				ParameterWithValue *p_atom_parameter;

				memset(&remote_pariticpant_guid, 0, sizeof(GUID_t));

				p_atom_parameter = get_real_parameter(p_datafull->inline_qos.p_head_first);

				while(p_atom_parameter)
				{
					if(p_atom_parameter->parameter_id == PID_STATUS_INFO && p_atom_parameter->length == 4)
					{
						const uint8_t *p = p_atom_parameter->p_value;
						int info_value = ((int)p[0] << 24) |
											((int)p[1] << 16) |
											((int)p[2] << 8) |
											((int)p[3]);

						if(info_value == 1)
						{
 							p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_DISPOSED, remote_pariticpant_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
						}
						else if (info_value == 2)
						{
							p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, remote_pariticpant_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
								
						}
						else if (info_value == 3)
						{
							p_rtps_cachechange = rtps_cachechange_new(NOT_ALIVE_UNREGISTERED, remote_pariticpant_guid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
							remove_Participant_and_etc(remote_pariticpant_guid);
							
						}

						if (p_rtps_cachechange)
						{
							rtps_statelessreader_t *p_statelessreader = NULL;
							p_statelessreader = (rtps_statelessreader_t *)rtps_get_spdp_builtin_participant_reader();
							p_rtps_cachechange = rtps_historycache_add_change(p_statelessreader->p_reader_cache, p_rtps_cachechange);
						}

					}
					else if (p_atom_parameter->parameter_id == PID_KEY_HASH && p_atom_parameter->length == 16)
					{
						remote_pariticpant_guid = *((GUID_t*)&p_atom_parameter->p_value[0]);
					}

					p_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
				}
			}

			return;
		}

		find_parameter_list(p_serialized->p_value,i_size, PID_BUILTIN_ENDPOINT_SET, &p_value, &i_found);
		if(i_found && p_value)
		{
			availableEndpoints = *p_value;
			i_found = 0;
		}else
		{
			availableEndpoints = 0;
			i_found = 0;
		}

		///////////////////
		/*
			Remote Participant를 추가
		*/

		find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

		if(i_found){
			a_remoteguid = *(GUID_t*)p_value;
		}else
		{
			if(p_datafull) {
				destory_datafull_all(p_datafull);
				*pp_datafull = NULL;
			}
			return;
		}


		if(is_exist_participant(a_remoteguid))
		{


			if(p_datafull) {
				destory_datafull_all(p_datafull);
				*pp_datafull = NULL;
			}
			return;
		}

		
		i_found = 0;
		find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

		

		if(i_found){
			a_remoteLocator = *(Locator_t*)p_value;
		}

		i_found = 0;
		find_parameter_list(p_serialized->p_value,i_size, PID_DEFAULT_UNICAST_LOCATOR, &p_value, &i_found);

		

		if(i_found){
			a_remoteDefaultLocator = *(Locator_t*)p_value;
		}


		i_found = 0;
		find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

		

		if(i_found){
			a_remoteMulticastLocator = *(Locator_t*)p_value;
			is_found_metatraffic_multicastlocator = true;
		}


		i_found = 0;
		find_parameter_list(p_serialized->p_value,i_size, PID_DEFAULT_MULTICAST_LOCATOR, &p_value, &i_found);

		

		if(i_found){
			a_remoteDefaultMulticastLocator = *(Locator_t*)p_value;
		}else{
			if(is_found_metatraffic_multicastlocator)
			{
				a_remoteDefaultMulticastLocator = a_remoteMulticastLocator;
			}
		}

		//added by kyy(UserData QoS)///////////////////////////////////////////////////////////////////
		//PID_USER_DATA(DOMAINPARTICIPANT)
		{
			UserDataQosPolicy p_user_data_qos;
			i_found = 0;
			find_parameter_list(p_serialized->p_value,i_size, PID_USER_DATA, &p_value, &i_found);
			if (i_found)
			{
				qos_find_user_data_qos_from_parameterlist(&p_user_data_qos, (char*)p_serialized->p_value, i_size);
				printf("DomainParticipant: ");
				qos_print_user_data_qos(p_user_data_qos);
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////
		

		i_found = 0;
		find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_LEASE_DURATION, &p_value, &i_found);

		if (i_found)
		{			
			rtps_readerproxy_t	*p_rtps_readerproxy = NULL;
			rtps_statelesswriter_t *p_statelesswriter = NULL;
			rtps_statelessreader_t *p_statelessreader = NULL;
			a_remoteDuration = *(Duration_t*)p_value;



			add_remote_participant(a_remoteguid, a_remoteLocator, a_remoteDefaultLocator, a_remoteMulticastLocator, a_remoteDefaultMulticastLocator, a_remoteDuration);

			// 2011.11.10 by pws
			p_statelessreader = (rtps_statelessreader_t *)rtps_get_spdp_builtin_participant_reader();
			//p_rtpswriterproxy = rtps_writerproxy_new((rtps_reader_t *)p_statefulreader, a_remoteguid, &a_remoteLocator, p_multicatlocator);
			//rtps_statefulreader_matched_writer_add(p_statefulreader, p_rtpswriterproxy);
			//
			//새로운 Participant를 받았기 때문에, 현재 자신의 ParticipantData를 바로 보내자.
			p_statelesswriter = (rtps_statelesswriter_t *)rtps_get_spdp_builtin_participant_writer();
			//p_rtps_readerproxy = rtps_readerproxy_new((rtps_writer_t *)p_statelesswriter, guid, p_unicatlocator, p_multicatlocator, false);
			//rtps_statefulwriter_matched_reader_add(p_statelesswriter, p_rtps_readerproxy);

			
			p_rtps_cachechange = rtps_cachechange_new(ALIVE, a_remoteguid, p_datafull->writer_sn.value, p_datafull->p_serialized_data, 0);
			
			

			{
				message_t *p_message = malloc(sizeof(message_t));
				

				ParticipantBuiltinTopicData *p_participant_data = malloc(sizeof(ParticipantBuiltinTopicData));
				memset(p_message, 0, sizeof(message_t));
				memset(p_participant_data, 0, sizeof(ParticipantBuiltinTopicData));
				memcpy(&p_participant_data->key, &a_remoteguid, sizeof(GUID_t));

				p_message->i_datasize = sizeof(ParticipantBuiltinTopicData);
				p_message->v_data = p_participant_data;

				p_rtps_cachechange->p_org_message = p_message;
			}
			
			p_rtps_cachechange = rtps_historycache_add_change(p_statelessreader->p_reader_cache, p_rtps_cachechange);


			//writer by jun
			//best effort 수행을 위해서 호출 
			//job은 한번만 수행되며, 우선 순위는 2로 설정
			//job이 추가가 되면 Writer_Thread에 시그널을 보냄
			if (p_statelesswriter)
			{
				data_t* p_jobdata=NULL;
				DataWriter				*p_dataWriter;

				p_jobdata = data_new(0);

				p_jobdata->p_rtps_writer = (rtps_writer_t  *)p_statelesswriter;
				p_dataWriter = p_statelesswriter->p_datawriter;
		//		p_jobdata->b_thread_first = true;

				p_jobdata->initial_time = currenTime();

				p_jobdata->next_wakeup_time.sec = 0;
				p_jobdata->next_wakeup_time.nanosec = 0;
				p_jobdata->priority = 2;

				timed_job_queue_time_compare_data_fifo_put_add_job(p_dataWriter->p_publisher->p_domain_participant->p_writer_thread_fifo,p_jobdata);
				trace_msg(NULL, TRACE_LOG, "cond_signal best_effort_statelessreader_behavior");
				//cond_signal( &p_dataWriter->p_publisher->p_domain_participant->writer_thread_wait );
			}


			//if(p_statelesswriter)cond_signal(&p_statelesswriter->object_wait);
		}

		

		


		///////////////////

		/*
			8.5.5.1 Discovery of a new remote Participant

			Using the SPDPbuiltinParticipantReader, a local Participant ‘local_participant’ discovers the existence of another
			Participant described by the DiscoveredParticipantData participant_data. The discovered Participant uses the SEDP.
		*/

		/*
			IF ( PUBLICATIONS_READER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER>;
				writer = local_participant.SEDPbuiltinPublicationsWriter;
				proxy = new ReaderProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				writer.matched_reader_add(proxy);
			ENDIF
		*/

		if (availableEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER)
		{
			p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_publication_writer();
			if (p_statefulwriter)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if (i_found)
				{
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER, sizeof(EntityId_t));
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_unicatlocator = malloc(sizeof(Locator_t));
					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				p_rtps_readerproxy = rtps_readerproxy_new((rtps_writer_t *)p_statefulwriter, guid, p_unicatlocator, p_multicatlocator, false, TRANSIENT_DURABILITY_QOS);
				p_rtps_readerproxy->reliablility_kind = RELIABLE_RELIABILITY_QOS;
				rtps_statefulwriter_matched_reader_add(p_statefulwriter, p_rtps_readerproxy);

				p_statefulwriter->heartbeat_period.sec = 1;
				cond_signal(&p_statefulwriter->object_wait);
			}
		}

		

		/*
			IF ( PUBLICATIONS_WRITER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER>;
				reader = local_participant.SEDPbuiltinPublicationsReader;
				proxy = new WriterProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				reader.matched_writer_add(proxy);
			ENDIF
		*/

		//Opensplice 때문에 일단 무조건 넣자.
		//if(availableEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR)
		{
			p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_Builtin_publication_reader();
			if (p_statefulreader)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if (i_found)
				{
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER, sizeof(EntityId_t));
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}



				


				if (p_unicatlocator->address[12] == 127)
				{
					trace_msg(NULL, TRACE_LOG, "\r\n#############################################################\r\n");
					trace_msg(NULL, TRACE_LOG, "(P)Please check the WriterProxy[Maybe... WriterProxy IP set local address[127.0.0.1].. see data[p]..]\r\n");
					trace_msg(NULL, TRACE_LOG, "\r\n#############################################################\r\n");
					pause_system();
				}
				
				p_rtpswriterproxy = rtps_writerproxy_new((rtps_reader_t *)p_statefulreader, guid, p_unicatlocator, p_multicatlocator);
				//p_rtpswriterproxy->reliablility_kind = RELIABLE_RELIABILITY_QOS;
				rtps_statefulreader_matched_writer_add(p_statefulreader, p_rtpswriterproxy);


				

				if (i_remoteParticipant != p_statefulreader->i_matched_writers)
				{
					trace_msg(NULL, TRACE_LOG2, "\r\n(P:%d:%d)Remote Participant added writerproxy UNICAST_LOCATOR[%d.%d.%d.%d:%d], MULTICAST_LOCATOR[[%d.%d.%d.%d:%d]\r\n", i_remoteParticipant, p_statefulreader->i_matched_writers
						, p_unicatlocator->address[12], p_unicatlocator->address[13], p_unicatlocator->address[14], p_unicatlocator->address[15], p_unicatlocator->port
						, p_multicatlocator->address[12], p_multicatlocator->address[13], p_multicatlocator->address[14], p_multicatlocator->address[15], p_multicatlocator->port);
				}
			}
			else {

				trace_msg(NULL, TRACE_LOG2,"NU>...................................");
			}
		}

		/*

			IF ( SUBSCRIPTIONS_READER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER>;
				writer = local_participant.SEDPbuiltinSubscriptionsWriter;
				proxy = new ReaderProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				writer.matched_reader_add(proxy);
			ENDIF
		*/

		if (availableEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER)
		{
			p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_subscription_writer();
			if (p_statefulwriter)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if (i_found)
				{
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER, sizeof(EntityId_t));
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_unicatlocator = malloc(sizeof(Locator_t));
					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}
				
				p_rtps_readerproxy = rtps_readerproxy_new((rtps_writer_t *)p_statefulwriter, guid, p_unicatlocator, p_multicatlocator, true, TRANSIENT_DURABILITY_QOS);
				p_rtps_readerproxy->reliablility_kind = RELIABLE_RELIABILITY_QOS;
				rtps_statefulwriter_matched_reader_add(p_statefulwriter, p_rtps_readerproxy);

				p_statefulwriter->heartbeat_period.sec = 1;
				cond_signal(&p_statefulwriter->object_wait);
			}
		}

		/*

			IF ( SUBSCRIPTIONS_WRITER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER>;
				reader = local_participant.SEDPbuiltinSubscriptionsReader;
				proxy = new WriterProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				reader.matched_writer_add(proxy);
			ENDIF
		*/

		//Opensplice 때문에 일단 무조건 넣자.
		//if(availableEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR)
		{
			p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_builtin_subscription_reader();
			if (p_statefulreader)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if (i_found)
				{
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER, sizeof(EntityId_t));
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}


				


				if (p_unicatlocator->address[12] == 127)
				{
					trace_msg(NULL, TRACE_LOG, "\r\n#############################################################\r\n");
					trace_msg(NULL, TRACE_LOG, "(S)Please check the WriterProxy[Maybe... WriterProxy IP set local address[127.0.0.1].. see data[p]..]\r\n");
					trace_msg(NULL, TRACE_LOG, "\r\n#############################################################\r\n");

					pause_system();
				}

				p_rtpswriterproxy = rtps_writerproxy_new((rtps_reader_t *)p_statefulreader, guid, p_unicatlocator, p_multicatlocator);
				
				rtps_statefulreader_matched_writer_add(p_statefulreader, p_rtpswriterproxy);


				if (i_remoteParticipant != p_statefulreader->i_matched_writers)
				{
					trace_msg(NULL, TRACE_LOG2, "\r\n(S:%d:%d)Remote Participant added writerproxy UNICAST_LOCATOR[%d.%d.%d.%d:%d], MULTICAST_LOCATOR[[%d.%d.%d.%d:%d]\r\n", i_remoteParticipant, p_statefulreader->i_matched_writers
						, p_unicatlocator->address[12], p_unicatlocator->address[13], p_unicatlocator->address[14], p_unicatlocator->address[15], p_unicatlocator->port
						, p_multicatlocator->address[12], p_multicatlocator->address[13], p_multicatlocator->address[14], p_multicatlocator->address[15], p_multicatlocator->port);
				}
			}
		}

		/*

			IF ( TOPICS_READER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_TOPICS_READER>;
				writer = local_participant.SEDPbuiltinTopicsWriter;
				proxy = new ReaderProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				writer.matched_reader_add(proxy);
			ENDIF

		*/

		if (availableEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER)
		{
			p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_spdp_builtin_topic_writer();
			if (p_statefulwriter)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if (i_found)
				{
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_READER, sizeof(EntityId_t));
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if (i_found)
				{
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}
				else
				{
					return;
				}

				p_rtps_readerproxy = rtps_readerproxy_new((rtps_writer_t *)p_statefulwriter, guid, p_unicatlocator, p_multicatlocator, true, TRANSIENT_DURABILITY_QOS);
				p_rtps_readerproxy->reliablility_kind = RELIABLE_RELIABILITY_QOS;
				rtps_statefulwriter_matched_reader_add(p_statefulwriter, p_rtps_readerproxy);
			}
		}

		/*

			IF ( TOPICS_WRITER IS_IN participant_data.availableEndpoints ) THEN
				guid = <participant_data.guidPrefix, ENTITYID_SEDP_BUILTIN_TOPICS_WRITER>;
				reader = local_participant.SEDPbuiltinTopicsReader;
				proxy = new WriterProxy( guid,
				participant_data.metatrafficUnicastLocatorList,
				participant_data.metatrafficMulticastLocatorList);
				reader.matched_writer_add(proxy);
			ENDIF
		/*/

		if(availableEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR)
		{
			p_statefulreader = (rtps_statefulreader_t *)rtps_get_spdp_builtin_topic_reader();
			if(p_statefulreader)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if(i_found){
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_SEDP_BUILTIN_TOPIC_WRITER, sizeof(EntityId_t));
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}else{
					return;
				}

				p_rtpswriterproxy = rtps_writerproxy_new((rtps_reader_t *)p_statefulreader, guid, p_unicatlocator, p_multicatlocator);
				rtps_statefulreader_matched_writer_add(p_statefulreader, p_rtpswriterproxy);


				if (i_remoteParticipant != p_statefulreader->i_matched_writers)
				{
					trace_msg(NULL, TRACE_LOG2, "\r\n(T:%d:%d)Remote Participant added writerproxy UNICAST_LOCATOR[%d.%d.%d.%d:%d], MULTICAST_LOCATOR[[%d.%d.%d.%d:%d]\r\n", i_remoteParticipant, p_statefulreader->i_matched_writers
						, p_unicatlocator->address[12], p_unicatlocator->address[13], p_unicatlocator->address[14], p_unicatlocator->address[15], p_unicatlocator->port
						, p_multicatlocator->address[12], p_multicatlocator->address[13], p_multicatlocator->address[14], p_multicatlocator->address[15], p_multicatlocator->port);
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		////         Liveliness 를 위한 것...
		////////////////////////////////////////////////////////////////////////////////////////////////


		//if(availableEndpoints & BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER)
		{
			p_statefulwriter = (rtps_statefulwriter_t *)rtps_get_liveliness_writer();
			if(p_statefulwriter)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if(i_found){
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER, sizeof(EntityId_t));
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}else{
					return;
				}

				p_rtps_readerproxy = rtps_readerproxy_new((rtps_writer_t *)p_statefulwriter, guid, p_unicatlocator, p_multicatlocator, true, TRANSIENT_DURABILITY_QOS);
				p_rtps_readerproxy->reliablility_kind = RELIABLE_RELIABILITY_QOS;
				rtps_statefulwriter_matched_reader_add(p_statefulwriter, p_rtps_readerproxy);

				p_statefulwriter->heartbeat_period.sec = 1;
				cond_signal(&p_statefulwriter->object_wait);
			}
		}


		///////////////////////////////////
		//if(availableEndpoints & BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER)
		{
			p_statefulreader = (rtps_statefulreader_t *)rtps_get_liveliness_reader();
			if(p_statefulreader)
			{
				GUID_t guid;
				rtps_readerproxy_t	*p_rtps_readerproxy = NULL;

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

				if(i_found){
					memcpy(&guid, p_value, i_found);
					memcpy(&guid.entity_id, &ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER, sizeof(EntityId_t));
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_UNICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_unicatlocator = malloc(sizeof(Locator_t));

					memset(p_unicatlocator, 0, sizeof(Locator_t));
					memcpy(p_unicatlocator, p_value, i_found);
				}else{
					return;
				}

				i_found = 0;
				find_parameter_list(p_serialized->p_value,i_size, PID_METATRAFFIC_MULTICAST_LOCATOR, &p_value, &i_found);

				if(i_found){
					p_multicatlocator = malloc(sizeof(Locator_t));

					memset(p_multicatlocator, 0, sizeof(Locator_t));
					memcpy(p_multicatlocator, p_value, i_found);
				}else{
					return;
				}

				p_rtpswriterproxy = rtps_writerproxy_new((rtps_reader_t *)p_statefulreader, guid, p_unicatlocator, p_multicatlocator);
				rtps_statefulreader_matched_writer_add(p_statefulreader, p_rtpswriterproxy);

				if (i_remoteParticipant != p_statefulreader->i_matched_writers)
				{
					trace_msg(NULL, TRACE_LOG2, "\r\n(L:%d:%d)Remote Participant added writerproxy UNICAST_LOCATOR[%d.%d.%d.%d:%d], MULTICAST_LOCATOR[[%d.%d.%d.%d:%d]\r\n", i_remoteParticipant, p_statefulreader->i_matched_writers
						, p_unicatlocator->address[12], p_unicatlocator->address[13], p_unicatlocator->address[14], p_unicatlocator->address[15], p_unicatlocator->port
						, p_multicatlocator->address[12], p_multicatlocator->address[13], p_multicatlocator->address[14], p_multicatlocator->address[15], p_multicatlocator->port);
				}
			}
		}
	}

}

/////////////////////
/////////////////////
/////////////////////

void get_builtin_participant_data( ParticipantBuiltinTopicData* p_participant_data, void* p_cache )
{
	rtps_cachechange_t	*p_rtps_cachechange = (rtps_cachechange_t	*)p_cache;

	if(p_participant_data == NULL || p_cache == NULL) return;


	if(p_rtps_cachechange->p_data_value != NULL)
	{
		SerializedPayloadForReader *p_serialized = (SerializedPayloadForReader *)p_rtps_cachechange->p_data_value;
		char *p_value = NULL;
		int	i_found = 0;
		int	i_size = 0;
		
		if(p_serialized) i_size = p_serialized->i_size;

		find_parameter_list(p_serialized->p_value,i_size, PID_PARTICIPANT_GUID, &p_value, &i_found);

		if(i_found){
			GUID_t a_remoteguid;
			a_remoteguid = *(GUID_t*)p_value;

			memcpy(&p_participant_data->key, &a_remoteguid, sizeof(GUID_t));
		}

		//added by kyy(UserData QoS)
//		p_participant_data->user_data.p_value = NULL;
	}
}