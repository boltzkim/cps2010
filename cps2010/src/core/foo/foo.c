/* 
	foo 관련 부분 구현
	작성자 : 
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
 

static ReturnCode_t _register_type(FooTypeSupport *p_foo_type_support, in_dds DomainParticipant *p_participant, in_dds string type_name)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_foo_type_support && p_participant && type_name)
	{
		assert(p_foo_type_support->i_parameters);
		register_type(OBJECT(p_module), p_foo_type_support, p_participant, type_name);
		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static string _get_type_name(FooTypeSupport* p_foo_type_support)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	if (p_foo_type_support)
	{
		return get_type_name(OBJECT(p_module), p_foo_type_support);
	}

	return NULL;
}


static bool _insert_parameter_struct(FooTypeSupport *p_fooTypeSupport, string parameter_name, int size, bool is_key)
{
	dds_parameter_t *p_para;
	int offset_size = size;

	if(is_key)
	{
		p_para = get_new_parameter_key(parameter_name, DDS_STRUCT_TYPE, size);
	}else{
		p_para = get_new_parameter(parameter_name, DDS_STRUCT_TYPE, size);
	}

	if(size % 4)
	{
		offset_size += (4 - (size % 4));
	}

	p_para->offset = offset_size;


	INSERT_PARAM(p_fooTypeSupport->pp_parameters,p_fooTypeSupport->i_parameters, p_fooTypeSupport->i_parameters, p_para);

	p_fooTypeSupport->offsetcount += p_para->offset;
	p_fooTypeSupport->i_size += p_para->i_size;

	return true;
}

static bool _insert_parameter(FooTypeSupport *p_fooTypeSupport, string parameter_name, SUPPORT_TYPE type, bool is_key)
{
	int size = 0;
	int offset_size = 0;
	dds_parameter_t *p_para;

	switch(type)
	{
		case DDS_STRING_TYPE:
			size = sizeof(dds_string);
			offset_size = sizeof(dds_string);
			break;
		case DDS_BYTE_TYPE:
		case DDS_INTEGER8_TYPE:
		case DDS_UINTEGER8_TYPE:
			size = sizeof(int8_t);
			offset_size = 4;
			break;
		case DDS_INTEGER16_TYPE:
		case DDS_UINTEGER16_TYPE:
			size = sizeof(int16_t);
			offset_size = 4;
			break;
		case DDS_INTEGER32_TYPE:
		case DDS_UINTEGER32_TYPE:
			size = sizeof(int32_t);
			offset_size = 4;
			break;
		case DDS_INTEGER64_TYPE:
		case DDS_UINTEGER64_TYPE:
			size = sizeof(int64_t);
			offset_size = 8;
			break;
		case DDS_FLOAT_TYPE:
			size = sizeof(int32_t);
			offset_size = 4;
			break;
		case DDS_DOUBLE_TYPE:
			size = sizeof(int64_t);
			offset_size = 8;
			break;
		case DDS_BOOLEAN_TYPE:
			size = 1;
			offset_size = 4;
			break;
		case DDS_CHARACTER_TYPE:
			size = 1;
			offset_size = 4;
			break;
		case DDS_STRUCT_TYPE:
			trace_msg(NULL,TRACE_ERROR,"if you want struct type, use fuction insert_parameter_struct.");
			return false;
			break;
	}

	

	if(is_key)
	{
		p_para = get_new_parameter_key(parameter_name, type, size);
	}else{
		p_para = get_new_parameter(parameter_name, type, size);
	}

	p_para->offset = offset_size;
	
	INSERT_PARAM(p_fooTypeSupport->pp_parameters,p_fooTypeSupport->i_parameters, p_fooTypeSupport->i_parameters, p_para);
	p_fooTypeSupport->offsetcount += p_para->offset;
	p_fooTypeSupport->i_size += p_para->i_size;
	return true;
}

void init_type_support(TypeSupport* p_type_support)
{
	p_type_support->i_size = 0;
	p_type_support->i_parameters = 0;
	p_type_support->pp_parameters = NULL;
	p_type_support->offsetcount = 0;
}

FooTypeSupport* get_new_foo_type_support()
{
	FooTypeSupport* p_foo_type_support = (FooTypeSupport*)malloc(sizeof(FooTypeSupport));
	memset(p_foo_type_support, '\0', sizeof(FooTypeSupport));

	init_foo_type_support(p_foo_type_support);

	return p_foo_type_support;
}



void init_foo_type_support(FooTypeSupport* p_foo_type_support)
{
	init_type_support((TypeSupport*)p_foo_type_support);
	p_foo_type_support->register_type = _register_type;
	p_foo_type_support->get_type_name = _get_type_name;
	p_foo_type_support->insert_parameter = _insert_parameter;
	p_foo_type_support->insert_parameter_struct = _insert_parameter_struct;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static InstanceHandle_t register_instance(FooDataWriter *p_foo_datawriter, in_dds Foo *p_instance_data)
{
	uint32_t i,j;
	bool found_key_instanceset = false;
	FooTypeSupport *p_typeSupport = NULL;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	Topic *p_topic = NULL;
	string type_name=NULL;
	message_t *p_message = NULL;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t	*p_instanceset = NULL;


	if (p_module->b_end) return 0;

	p_topic = p_foo_datawriter->p_topic;

	type_name = p_topic->get_type_name(p_topic);


	if (!type_name || !p_instance_data)
	{
		return 0;
	}

	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), type_name);

	p_message = (message_t *)malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));
	p_message->p_next = NULL;
	p_message->p_prev = NULL;

	assert(p_typeSupport);

	/// TypeSupport 정해지면 여기서 체크해서 정확히 값을 가져옴.
	if (p_typeSupport)
	{
		p_message->i_datasize = p_typeSupport->i_size;
	}
	else
	{
		p_message->i_datasize = 0;
	}


	if (p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}
	else
	{
		int32_t tmp_size;
		p_message->v_data = malloc(p_message->i_datasize);
		memset(p_message->v_data, '\0', sizeof(p_message->i_datasize));

		tmp_size = caculate_message_data_type_support_to_rtps(p_message, p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, (char*)p_instance_data);

		if (p_message->i_datasize < tmp_size)
		{
			assert(0);
		}

		p_message->i_datasize = tmp_size;
	}

	if (p_message->i_key)
	{
		for (i = 0; i < i_size; i++)
		{
			if (p_foo_datawriter->pp_instanceset[i]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j], p_foo_datawriter->pp_instanceset[i]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{

				p_instanceset = p_foo_datawriter->pp_instanceset[i];
				found_key_instanceset = true;
				break;
			}
		}
	}

	if (found_key_instanceset == false)
	{
		p_instanceset = instaceset_new(p_message, p_foo_datawriter->datawriter_qos.history.depth, p_foo_datawriter->datawriter_qos.resource_limits.max_samples_per_instance);
		INSERT_ELEM(p_foo_datawriter->pp_instanceset, p_foo_datawriter->i_instanceset, p_foo_datawriter->i_instanceset, p_instanceset);
	}


	message_release(p_message);

	return (InstanceHandle_t)p_instanceset;
}


//timestamp by jun
static InstanceHandle_t register_instance_timestamp(FooDataWriter *p_foo_datawriter, in_dds Foo* p_instance_data, Time_t source_timestamp)
{
	uint32_t i, j;
	bool found_key_instanceset = false;
	FooTypeSupport* p_typeSupport = NULL;
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	Topic* p_topic = NULL;
	string type_name=NULL;
	message_t* p_message = NULL;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;
	bool haskey = false;

	if (p_module->b_end) return 0;

	p_topic = p_foo_datawriter->p_topic;

	type_name = p_topic->get_type_name(p_topic);

	if (!type_name || !p_instance_data)
	{
		return 0;
	}

	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), type_name);

	p_message = (message_t *)malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));

	p_message->p_next = NULL;
	p_message->p_prev = NULL;

	assert(p_typeSupport);

	/// TypeSupport 정해지면 여기서 체크해서 정확히 값을 가져옴.
	if (p_typeSupport)
	{
		p_message->i_datasize = p_typeSupport->i_size;
	}
	else
	{
		p_message->i_datasize = 0;
	}


	if (p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}
	else
	{
		int32_t tmp_size;
		p_message->v_data = malloc(p_message->i_datasize);
		memset(p_message->v_data, '\0', sizeof(p_message->i_datasize));

		tmp_size = caculate_message_data_type_support_to_rtps(p_message, p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, (char*)p_instance_data);

		if (p_message->i_datasize < tmp_size)
		{
			assert(0);
		}

		p_message->i_datasize = tmp_size;
	}

	if (p_message->i_key)
	{
		haskey = true;
	}

	if (p_message->i_key)
	{
		for (i = 0; i < i_size; i++)
		{
			if (p_foo_datawriter->pp_instanceset[i]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j], p_foo_datawriter->pp_instanceset[i]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{
				p_instanceset = p_foo_datawriter->pp_instanceset[i];
				found_key_instanceset = true;
				break;
			}
		}
	}

	if (found_key_instanceset == false)
	{
		p_instanceset = instaceset_new(p_message, p_foo_datawriter->datawriter_qos.history.depth, p_foo_datawriter->datawriter_qos.resource_limits.max_samples_per_instance);
		INSERT_ELEM(p_foo_datawriter->pp_instanceset, p_foo_datawriter->i_instanceset, p_foo_datawriter->i_instanceset, p_instanceset);
	}

	if (haskey)
	{
		p_instanceset->register_instance_timestamp_set = true;
		p_instanceset->register_instance_timestamp_value = source_timestamp;
	}

	message_release(p_message);

	return (InstanceHandle_t)p_instanceset;
}

static ReturnCode_t dispose(FooDataWriter *p_foo_datawriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t instance_handle)
{
	uint32_t i1;
	int32_t i2;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t	*p_instanceset = NULL;

	rtps_writer_t *p_rtps_writer = p_foo_datawriter->p_related_rtps_writer;
//time by jun

	Time_t message_time;

	if (p_foo_datawriter->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i1 = 0; i1 < i_size; i1++)
	{
		if (p_foo_datawriter->pp_instanceset[i1] == (instanceset_t *)instance_handle)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i1];
			found_key_instanceset = true;
//time by jun
			if (p_instanceset->register_instance_timestamp_set)
			{
				message_time = p_instanceset->register_instance_timestamp_value;
			}
			else
			{
				message_time = currenTime();
			}
			
			break;
		}
	}

	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}


	for (i2 = 0; i2 < p_instanceset->i_messages; i2++)
	{
		if (p_instanceset->pp_messages[i2]->v_related_cachechange != NULL)
		{
			p_instanceset->pp_messages[i2]->sampleInfo.instance_state = NOT_ALIVE_DISPOSED_INSTANCE_STATE;
		}
	}


	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);


//	datawriter_dispose((DataWriter *)p_foo_datawriter, p_instance_data, instance_handle);
	datawriter_dispose_timestamp((DataWriter *)p_foo_datawriter, p_instance_data, instance_handle, message_time);


	return RETCODE_OK;
}


//timestamp by jun
static ReturnCode_t dispose_timestamp(FooDataWriter *p_foo_datawriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t instance_handle, Time_t source_timestamp)
{
	uint32_t i1;
	int32_t i2;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;

	rtps_writer_t* p_rtps_writer = p_foo_datawriter->p_related_rtps_writer;


	if (p_foo_datawriter->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i1 = 0; i1 < i_size; i1++)
	{
		if (p_foo_datawriter->pp_instanceset[i1] == (instanceset_t *)instance_handle)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i1];
			found_key_instanceset = true;
			break;
		}
	}

	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i2 = 0; i2 < p_instanceset->i_messages; i2++)
	{
		if (p_instanceset->pp_messages[i2]->v_related_cachechange != NULL)
		{
			p_instanceset->pp_messages[i2]->sampleInfo.instance_state = NOT_ALIVE_DISPOSED_INSTANCE_STATE;
		}
	}

	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);

	datawriter_dispose_timestamp((DataWriter *)p_foo_datawriter, p_instance_data, instance_handle, source_timestamp);

	return RETCODE_OK;
}


static ReturnCode_t unregister_instance(FooDataWriter* p_foo_datawriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t handle)
{
	bool b_is_autodispose_unregistered_instances = false;
	uint32_t i1;
	int32_t i2;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t*	p_instanceset = NULL;

	rtps_writer_t* p_rtps_writer = p_foo_datawriter->p_related_rtps_writer;
//time by jun
	Time_t message_time;

	if (p_foo_datawriter->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i1 = 0; i1 < i_size; i1++)
	{
		if (p_foo_datawriter->pp_instanceset[i1] == (instanceset_t *)handle)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i1];
			found_key_instanceset = true;
			//time by jun
			if(p_instanceset->register_instance_timestamp_set)
			{
				message_time = p_instanceset->register_instance_timestamp_value;
			}
			else
			{
				message_time = currenTime();
			}
			
			break;
		}
	}

	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i2 = 0; i2 < p_instanceset->i_messages; i2++)
	{
		if (p_instanceset->pp_messages[i2]->v_related_cachechange != NULL)
		{
			p_instanceset->pp_messages[i2]->sampleInfo.instance_state = NOT_ALIVE_NO_WRITERS_INSTANCE_STATE;
		}
	}

	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);

	b_is_autodispose_unregistered_instances = qos_is_autodispose_unregistered_instances(&p_foo_datawriter->datawriter_qos.writer_data_lifecycle);
//time by jun
//	datawriter_unregister((DataWriter *)p_foo_datawriter, p_instance_data, handle, b_is_autodispose_unregistered_instances);
	datawriter_unregister_timestamp((DataWriter *)p_foo_datawriter, p_instance_data, handle, message_time);

	//datawriter_unregister((DataWriter *)p_fooDataWriter, instance_data, handle);

	return RETCODE_OK;
}


//timestamp by jun
static ReturnCode_t unregister_instance_timestamp(FooDataWriter *p_foo_datawriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t handle, Time_t source_timestamp)
{
	uint32_t i1;
	int32_t i2;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;

	rtps_writer_t* p_rtps_writer = p_foo_datawriter->p_related_rtps_writer;

	if (p_foo_datawriter->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i1 = 0; i1 < i_size; i1++)
	{
		if (p_foo_datawriter->pp_instanceset[i1] == (instanceset_t *)handle)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i1];
			found_key_instanceset = true;
			break;
		}
	}

	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i2 = 0; i2 < p_instanceset->i_messages; i2++)
	{
		if (p_instanceset->pp_messages[i2]->v_related_cachechange != NULL)
		{
			p_instanceset->pp_messages[i2]->sampleInfo.instance_state = NOT_ALIVE_NO_WRITERS_INSTANCE_STATE;
		}
	}

	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);


#ifdef QOS_WDL
	// unregister_instance 기능을 수행하고...
	// 마지막에 dispose() 를 호출하고 끝낸다.
	if (is_autodispose_unregistered_instances(&p_foo_datawriter->datawriter_qos.writer_data_lifecycle))
	{
		dispose(p_foo_datawriter, p_instance_data, handle);
	}
#endif

	datawriter_unregister_timestamp((DataWriter *)p_foo_datawriter, p_instance_data, handle, source_timestamp);

	return RETCODE_OK;
}

extern uint32_t get_millisecond(const Duration_t duration);
static InstanceHandle_t lookup_instanceWriter(FooDataWriter *p_foo_datawriter, inout_dds Foo *p_key_holder);
static ReturnCode_t Write(FooDataWriter* p_foo_datawriter, in_dds Foo* p_instance_data, in_dds InstanceHandle_t handle)
{
//time by jun
	InstanceHandle_t instance_set = handle;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;
	bool found_key_instanceset = false;
	uint32_t i;
	Time_t message_time;


	if (!qos_is_enabled((Entity*) p_foo_datawriter)) { return RETCODE_NOT_ENABLED; }//by kki


	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);



	if (p_foo_datawriter->datawriter_qos.reliability.kind == RELIABLE_RELIABILITY_QOS)
	{
		/*if (p_foo_datawriter->b_write_blocked)
		{
			mutex_lock(&p_foo_datawriter->entity_lock);
			{
				uint32_t waittime = get_millisecond(p_foo_datawriter->datawriter_qos.reliability.max_blocking_time);
				cond_waittimed(&p_foo_datawriter->entity_wait, &p_foo_datawriter->entity_lock, waittime);
				p_foo_datawriter->b_write_blocked = false;
			}
			mutex_unlock(&p_foo_datawriter->entity_lock);
		}*/

		////자고 깨어났는데도 HistoryCache가 FULL이면, TIMEOUT 리턴
		/*if (historycache_is_full(((rtps_writer_t*)p_foo_datawriter->p_related_rtps_writer)->p_writer_cache))
		{
			p_foo_datawriter->b_write_blocked = true;
			return RETCODE_TIMEOUT;
		}*/
	}


	//handle이 NULL이면, 일단 핸들은 p_instance_data의 핸들값으로 하자.

//time by jun
	if (instance_set == 0)
	{
		uint8_t *pp = p_instance_data;
		/*int *a = pp+260;
		int *b = pp+264;
		int *c = pp+268;

		printf("%d, %d, %d, \r\n", *a, *b, *c);
		fflush(stdout);*/
	
		instance_set = lookup_instanceWriter(p_foo_datawriter, p_instance_data);
	}

	for (i = 0; i < i_size; i++)
	{
		if (p_foo_datawriter->pp_instanceset[i] == (instanceset_t *)instance_set)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i];
			found_key_instanceset = true;
			break;
		}
	}
	if (instance_set != 0)
	{
		if (p_instanceset->register_instance_timestamp_set)
		{
			message_time = p_instanceset->register_instance_timestamp_value;
		}
		else
		{
			message_time = currenTime();
		}
	}
	else
	{
		message_time = currenTime();
	}
	
	/*if(p_foo_datawriter->id != 23)
	{*/
		if (handle == 0)
		{
			datawriter_write((DataWriter*)p_foo_datawriter, (void*)p_instance_data, handle, message_time);
		}
		else
		{
			datawriter_write((DataWriter*)p_foo_datawriter, (void*)p_instance_data, handle, message_time);
		}
	//}

	return RETCODE_OK;
}


//timestamp by jun
static ReturnCode_t Write_Timestamp(FooDataWriter *p_foo_datawriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t handle, Time_t source_timestamp)
{

	qos_operation_called_for_liveliness_qos((DataWriter*)p_foo_datawriter);



	if (p_foo_datawriter->datawriter_qos.reliability.kind == RELIABLE_RELIABILITY_QOS)
	{

		/*if (p_foo_datawriter->b_write_blocked)
		{
			mutex_lock(&p_foo_datawriter->entity_lock);
			{
				uint32_t waittime = get_millisecond(p_foo_datawriter->datawriter_qos.reliability.max_blocking_time);
				cond_waittimed(&p_foo_datawriter->entity_wait, &p_foo_datawriter->entity_lock, waittime);
				p_foo_datawriter->b_write_blocked = false;
			}
			mutex_unlock(&p_foo_datawriter->entity_lock);
		}*/


	//자고 깨어났는데도 HistoryCache가 FULL이면, TIMEOUT 리턴
	/*if (is_full(((rtps_writer_t*)p_foo_datawriter->related_rtps_writer)->writer_cache))
	{
		p_foo_datawriter->b_write_blocked = true;
		return RETCODE_TIMEOUT;
	}*/
	}


	//handle이 NULL이면, 일단 핸들은 p_instance_data의 핸들값으로 하자.
	if (handle == 0)
	{
		datawriter_write_timestamp((DataWriter*)p_foo_datawriter,(void*)p_instance_data, handle, source_timestamp);
	}
	else
	{
		datawriter_write_timestamp((DataWriter*)p_foo_datawriter,(void*)p_instance_data, handle, source_timestamp);
	}

	return RETCODE_OK;
}


static ReturnCode_t get_key_valueWriter(FooDataWriter *p_foo_datawriter, inout_dds Foo *p_key_holder, in_dds InstanceHandle_t handle)
{
	uint32_t i;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;
	cpskey_t* key = NULL;
	uint8_t* start_p = (uint8_t *)p_key_holder;

	if (p_key_holder == NULL) return RETCODE_BAD_PARAMETER;

	if (p_foo_datawriter->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i = 0; i < i_size; i++)
	{
		if (p_foo_datawriter->pp_instanceset[i] == (instanceset_t *)handle)
		{
			p_instanceset = p_foo_datawriter->pp_instanceset[i];
			found_key_instanceset = true;
			break;

		}
	}

	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i = 0; i < p_instanceset->i_key; i++)
	{
		key = &p_instanceset->keylist[i];

		memcpy(start_p+key->offset, key->p_value, key->len);
	}

	return RETCODE_OK;
}

static InstanceHandle_t lookup_instanceWriter(FooDataWriter* p_foo_datawriter, inout_dds Foo* p_key_holder)
{
	uint32_t i,j;
	bool found_key_instanceset = false;
	FooTypeSupport* p_typeSupport = NULL;
	module_t* p_module = current_object(get_domain_participant_factory_module_id());

	Topic* p_topic = NULL;
	string type_name=NULL;
	message_t* p_message = NULL;
	uint32_t i_size = p_foo_datawriter->i_instanceset;
	instanceset_t* p_instanceset = NULL;

	if (p_module->b_end) return 0;

	p_topic = p_foo_datawriter->p_topic;

	type_name = p_topic->get_type_name(p_topic);

	if (!type_name || !p_key_holder)
	{
		return 0;
	}

	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), type_name);

	p_message = (message_t *)malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));

	p_message->p_next = NULL;
	p_message->p_prev = NULL;

	assert(p_typeSupport);

	/// TypeSupport 정해지면 여기서 체크해서 정확히 값을 가져옴.
	if (p_typeSupport)
	{
		p_message->i_datasize = p_typeSupport->offsetcount;
	}
	else
	{
		p_message->i_datasize = 0;
	}

	/*if(p_typeSupport->offsetcount == 0)
	{
		p_typeSupport->offsetcount = p_message->i_datasize;
	}*/

	if (p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}
	else
	{
		int32_t tmp_size;
		p_message->v_data = malloc(p_typeSupport->offsetcount);
		memset(p_message->v_data, '\0', sizeof(p_typeSupport->offsetcount));

		tmp_size = caculate_message_data_type_support_to_rtps(p_message, p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, (char*)p_key_holder);

		//if (p_message->i_datasize < tmp_size)
		if (p_message->i_datasize < tmp_size)
		{
			assert(0);
		}

		p_message->i_datasize = tmp_size;
	}

	if (p_message->i_key)
	{
		for (i = 0; i < i_size; i++)
		{
			if (p_foo_datawriter->pp_instanceset[i]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j], p_foo_datawriter->pp_instanceset[i]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{
				p_instanceset = p_foo_datawriter->pp_instanceset[i];
				found_key_instanceset = true;
				break;
			}
		}
	}

	if (found_key_instanceset == false)
	{
	//p_instanceset = instaceset_new(p_message, p_fooDataWriter->dataWriterQos.history.depth, p_fooDataWriter->dataWriterQos.resource_limits.max_samples_per_instance);
	//INSERT_ELEM(p_fooDataWriter->pp_instanceset, p_fooDataWriter->i_instanceset, p_fooDataWriter->i_instanceset, p_instanceset);
		//jeman 20140915
		message_release(p_message);
		return 0;
	}

	message_release(p_message);

	return (InstanceHandle_t)p_instanceset;
}


void init_foo_datawriter(FooDataWriter* p_foo_datawriter)
{
	init_datawriter((DataWriter*)p_foo_datawriter);

	p_foo_datawriter->b_write_blocked = false;
	p_foo_datawriter->register_instance = register_instance;
	p_foo_datawriter->register_instance_w_timestamp = register_instance_timestamp;
	p_foo_datawriter->unregister_instance = unregister_instance;
	p_foo_datawriter->unregister_instance_w_timestamp = unregister_instance_timestamp;
	p_foo_datawriter->write = Write;
	p_foo_datawriter->write_w_timestamp = Write_Timestamp;
	p_foo_datawriter->dispose = dispose;
	p_foo_datawriter->dispose_w_timestamp = dispose_timestamp;
	p_foo_datawriter->get_key_value = get_key_valueWriter;
	p_foo_datawriter->lookup_instance = lookup_instanceWriter;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static Check_coherent_set(FooDataReader *p_foo_datareader)
{
	//added by kyy(Presentation QoS)//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//coherent_set이 끝나야만 다음 단계로 갈 수 있다.(TOPIC_PRESENTATION_QOS)
	//coherent_set이 끝나는 기준 설정해야 함(Destination Order 관련)
	if (p_foo_datareader->p_subscriber->subscriber_qos.presentation.access_scope == TOPIC_PRESENTATION_QOS && p_foo_datareader->p_subscriber->subscriber_qos.presentation.coherent_access == true)
	{
		bool end_coherent;
		end_coherent = false;
		// 조건을 바꿔야 함(Coherent_set이 끝남을 알 수 있는 방법]
		while (p_foo_datareader->i_message_order > 0 && end_coherent == false)
		{
			int i;
			//msleep(100);
			for (i = 0; i< p_foo_datareader->i_message_order; i++)
			{
				if (p_foo_datareader->pp_message_order[i]->is_coherent_set == false)
				{
					end_coherent = true;
				}
			}
		}
	}
}

static ReturnCode_t Read(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
          {
	message_t** pp_message = NULL;
	Foo* p_foo = NULL;
	//SampleInfo *p_sampleInfo = NULL;
	int i_size=0;
	int i;
	TopicDescription *p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
	bool b_check_insert = false;

	assert(p_topicDescription != NULL);

	//[KKI] Reader Data Lifecycle 을 위한 함수
	//[KKI] desposed 상태이면서 autopurge_disposed_samples_delay 시간이 지났으면, 그 인스턴스를 삭제한다.
	qos_remove_disposed_instance((DataReader*) p_foo_datareader);//by kki (reader data lifecycle)

	/*printf("1 size : %d, %p\r\n", p_data_values->i_seq,  p_data_values->pp_foo);
	printf("2 size : %d, %p\r\n", p_sample_infos->i_seq,  p_sample_infos->pp_sample_infos);
	fflush(stdout);*/
	p_data_values->i_seq = 0;
	FREE(p_data_values->pp_foo);
	p_sample_infos->i_seq = 0;
	FREE(p_sample_infos->pp_sample_infos);

	if (p_foo_datareader && p_data_values){
		//pp_message = message_read((DataReader *)p_foo_datareader,&i_size);

//		Check_coherent_set(p_foo_datareader);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/*printf("1 pass\r\n");
		fflush(stdout);*/
		pp_message = message_read_from_service((DataReader *)p_foo_datareader,max_samples, &i_size, sample_states, view_states, instance_states);

	/*	printf("2 pass\r\n");
		fflush(stdout);*/

		if (pp_message)
		{


			/*printf("3 pass : %d\r\n", i_size);
			fflush(stdout);*/

			for (i = 0; i < i_size; i++)
			{
	//added by kyy(Presentation QoS)
	//coh_num == unknown and data == NULL 확인
				if (pp_message[i]->i_datasize == 0)
				{
					printf("Data Size is O \n");
				}
				else
				{
					p_foo = (Foo *)pp_message[i];
	//				p_sampleInfo = pp_message[i]->p_sampleInfo;

					if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
					{
						ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
						assert(p_contentFilteredTopic != NULL);
						assert(p_contentFilteredTopic->p_expr != NULL);



					}
					else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
					{
						assert(false);
					}
					else
					{
						b_check_insert = true;
					}

					INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq, p_data_values->i_seq, p_foo);

	//if(p_sampleInfo){
					INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq, p_sample_infos->i_seq, &pp_message[i]->sampleInfo);
	//}
				}
			}
			FREE(pp_message);

			msleep(1); //read는 넘 빨리 가져가서 ....
			/*printf("4 pass\r\n");
			fflush(stdout);*/
			return RETCODE_OK;
		}
		msleep(1);
		return RETCODE_NO_DATA;
	}
	msleep(1);
	return RETCODE_NO_DATA;
}


static ReturnCode_t Take(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	message_t **pp_message = NULL;
	Foo *p_foo = NULL;
	//SampleInfo *p_sampleInfo = NULL;
	int i_size=0;
	int i;
	TopicDescription *p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
	bool b_check_insert = false;

	assert(p_topicDescription != NULL);

	//[KKI] Reader Data Lifecycle 을 위한 함수
	//[KKI] despose 상태이면서 autopurge_disposed_samples_delay 시간이 지났으면, 그 인스턴스를 삭제한다.
	qos_remove_disposed_instance((DataReader*) p_foo_datareader);//by kki (reader data lifecycle)

	p_data_values->i_seq = 0;
	FREE(p_data_values->pp_foo);
	p_sample_infos->i_seq = 0;
	FREE(p_sample_infos->pp_sample_infos);

	if (p_foo_datareader && p_data_values)
	{
		//pp_message = message_read((DataReader *)p_foo_datareader,&i_size);

		Check_coherent_set(p_foo_datareader);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		pp_message = message_take_from_service((DataReader *)p_foo_datareader,max_samples, &i_size, sample_states, view_states, instance_states);


	//added by kyy(Destination Order)////////////////////////////////////////////////////////////////////////
	//pp_message를 ordering 하여 Application에 제공
	/*		if(p_foo_datareader->dataReaderQos.destination_order.kind == BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS || p_foo_datareader->p_subscriber->subscriber_qos.presentation.ordered_access == true)
		{
			qosim_dcps_message_ordering_source_timestamp(pp_message, i_size);
		}
	*/		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (pp_message)
		{
			for (i = 0; i < i_size; i++)
			{
				//added by kyy(Presentation QoS)
				//coh_num == unknown and data == NULL 확인
				if (pp_message[i]->i_datasize == 0)
				{
					printf("Data Size is O \n");
				}
				/////////////////////////////////////////
				else
				{
					p_foo = (Foo *)pp_message[i];
					//	p_sampleInfo = pp_message[i]->p_sampleInfo;

					if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
					{
						ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
						assert(p_contentFilteredTopic != NULL);
						assert(p_contentFilteredTopic->p_expr != NULL);



					}
					else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
					{
						assert(false);
					}
					else
					{
						b_check_insert = true;
					}

					INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq, p_data_values->i_seq, p_foo);

	
					INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq, p_sample_infos->i_seq, &pp_message[i]->sampleInfo);
				}

			}
			FREE(pp_message);

			return RETCODE_OK;
		}
		msleep(1);
		return RETCODE_NO_DATA;
	}
	msleep(1);
	return RETCODE_NO_DATA;
}


static ReturnCode_t read_instance(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values,inout_dds SampleInfoSeq *p_sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states)
{
	message_t** pp_message = NULL;
	Foo* p_foo = NULL;
	//SampleInfo *p_sampleInfo = NULL;
	int i_size = 0;
	int i;
	TopicDescription* p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
	bool b_check_insert = false;

	assert(p_topicDescription != NULL);

	p_data_values->i_seq = 0;
	FREE(p_data_values->pp_foo);
	p_sample_infos->i_seq = 0;
	FREE(p_sample_infos->pp_sample_infos);

	if (p_foo_datareader && p_data_values)
	{
	//pp_message = message_read((DataReader *)p_foo_datareader,&i_size);
		pp_message = message_read_instance_from_service((DataReader *)p_foo_datareader,a_handle, max_samples, &i_size, sample_states, view_states, instance_states);
		if (pp_message)
		{
			for (i = 0; i < i_size; i++)
			{
				p_foo = (Foo *)pp_message[i];
	//				p_sampleInfo = pp_message[i]->p_sampleInfo;

				if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
				{
					ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
					assert(p_contentFilteredTopic != NULL);
					assert(p_contentFilteredTopic->p_expr != NULL);
				}
				else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
				{
					assert(false);
				}
				else
				{
					b_check_insert = true;
				}


				INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq, p_data_values->i_seq, p_foo);

	//if(p_sampleInfo){
					INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq, p_sample_infos->i_seq, &pp_message[i]->sampleInfo);
	//}
			}
			FREE(pp_message);

			msleep(1); //read는 넘 빨리 가져가서 ....

			return RETCODE_OK;
		}
		msleep(1);
		return RETCODE_NO_DATA;
	}
	msleep(1);
	return RETCODE_NO_DATA;
}

static ReturnCode_t take_instance(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values,inout_dds SampleInfoSeq *p_sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{

	message_t **pp_message = NULL;
	Foo *p_foo = NULL;
	//SampleInfo *p_sampleInfo = NULL;
	int i_size=0;
	int i;
	TopicDescription *p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
	bool b_check_insert = false;

	assert(p_topicDescription != NULL);

	p_data_values->i_seq = 0;
	FREE(p_data_values->pp_foo);
	p_sample_infos->i_seq = 0;
	FREE(p_sample_infos->pp_sample_infos);

	if (p_foo_datareader && p_data_values)
	{
	//pp_message = message_read((DataReader *)p_foo_datareader,&i_size);
		pp_message = message_take_instance_from_service((DataReader *)p_foo_datareader,a_handle, max_samples, &i_size, sample_states, view_states, instance_states);
		if (pp_message)
		{
			for (i = 0; i < i_size; i++)
			{
				p_foo = (Foo *)pp_message[i];
	//	p_sampleInfo = pp_message[i]->p_sampleInfo;

				if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
				{
					ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
					assert(p_contentFilteredTopic != NULL);
					assert(p_contentFilteredTopic->p_expr != NULL);
				}
				else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
				{
					assert(false);
				}
				else
				{
					b_check_insert = true;
				}


				INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq, p_data_values->i_seq, p_foo);

	//		if(p_sampleInfo){
					INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq, p_sample_infos->i_seq, &pp_message[i]->sampleInfo);
	//		}
			}
			FREE(pp_message);

			return RETCODE_OK;
		}
		msleep(1);
		return RETCODE_NO_DATA;
	}
	msleep(1);
	return RETCODE_NO_DATA;
}


static ReturnCode_t get_key_valueReader(FooDataReader *p_foo_datareader, inout_dds Foo *p_key_holder, in_dds InstanceHandle_t handle)
{
	uint32_t i;
	bool found_key_instanceset = false;
	uint32_t i_size = p_foo_datareader->i_instanceset;
	instanceset_t* p_instanceset = NULL;
	cpskey_t* key = NULL;
	uint8_t* start_p = (uint8_t *)p_key_holder;


	if (p_key_holder == NULL) return RETCODE_BAD_PARAMETER;

	if (p_foo_datareader->i_instanceset == 0)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i = 0; i < i_size; i++)
	{
		if (p_foo_datareader->pp_instanceset[i] == (instanceset_t *)handle)
		{
			p_instanceset = p_foo_datareader->pp_instanceset[i];
			found_key_instanceset = true;
			break;

		}
	}


	if (found_key_instanceset == false)
	{
		return RETCODE_BAD_PARAMETER;
	}

	for (i = 0; i < p_instanceset->i_key; i++)
	{
		key = &p_instanceset->keylist[i];

		memcpy(start_p+key->offset, key->p_value, key->len);
	}

	return RETCODE_OK;
}


static InstanceHandle_t lookup_instanceReader(FooDataReader *p_foo_datareader, inout_dds Foo *p_key_holder)
{
	uint32_t i, j;
	bool found_key_instanceset = false;
	FooTypeSupport *p_typeSupport = NULL;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	Topic *p_topic = NULL;
	string type_name=NULL;
	message_t *p_message = NULL;
	uint32_t i_size = p_foo_datareader->i_instanceset;
	instanceset_t	*p_instanceset = NULL;

	if (p_module->b_end) return 0;

	p_topic = p_foo_datareader->p_topic;

	type_name = p_topic->get_type_name(p_topic);

	if (!type_name || !p_key_holder)
	{
		return 0;
	}

	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), type_name);

	p_message = (message_t *)malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));

	p_message->p_next = NULL;
	p_message->p_prev = NULL;

	assert(p_typeSupport);

	/// TypeSupport 정해지면 여기서 체크해서 정확히 값을 가져옴.
	if (p_typeSupport)
	{
		p_message->i_datasize = p_typeSupport->i_size;
	}else
	{
		p_message->i_datasize = 0;
	}


	if (p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}
	else
	{
		int32_t tmp_size;
		p_message->v_data = malloc(p_message->i_datasize);
		memset(p_message->v_data, '\0', sizeof(p_message->i_datasize));

		tmp_size = caculate_message_data_type_support_to_rtps(p_message, p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, (char*)p_key_holder);

		if (p_message->i_datasize < tmp_size)
		{
			assert(0);
		}

		p_message->i_datasize = tmp_size;
	}

	if (p_message->i_key)
	{
		for (i = 0; i < i_size; i++)
		{
			if (p_foo_datareader->pp_instanceset[i]->i_key != p_message->i_key) continue;

			found_key_instanceset = true;

			for (j = 0; j < p_message->i_key; j++)
			{
				if (!cpskey__is_same(p_message->keylist[j], p_foo_datareader->pp_instanceset[i]->keylist[j]))
				{
					found_key_instanceset = false;
					break;
				}
			}

			if (found_key_instanceset)
			{

				p_instanceset = p_foo_datareader->pp_instanceset[i];
				found_key_instanceset = true;
				break;
			}
		}
	}

	if (found_key_instanceset == false)
	{
		p_instanceset = instaceset_new(p_message, p_foo_datareader->datareader_qos.history.depth, p_foo_datareader->datareader_qos.resource_limits.max_samples_per_instance);
		INSERT_ELEM(p_foo_datareader->pp_instanceset, p_foo_datareader->i_instanceset, p_foo_datareader->i_instanceset, p_instanceset);
	}


	message_release(p_message);

	return (InstanceHandle_t)p_instanceset;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ReturnCode_t Read_w_condition(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, ReadCondition *p_condition)
{
	if(p_condition->condition_type == READ_CONDITION)
	{
		return p_foo_datareader->read(p_foo_datareader, p_data_values, p_sample_infos, max_samples, p_condition->l_sampleStateMask, p_condition->l_viewStateMask, p_condition->l_instanceStateMask);
	}else{
		message_t **pp_message = NULL;
		Foo *p_foo = NULL;
		//SampleInfo *p_sampleInfo = NULL;
		int i_size=0;
		int i;
		QueryCondition* p_queryCondition = (QueryCondition*)p_condition;
		TopicDescription *p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
		bool b_check_insert = false;

		assert(p_queryCondition->p_expr  != NULL);
		assert(p_topicDescription != NULL);


		qos_remove_disposed_instance((DataReader*) p_foo_datareader);//by kki (reader data lifecycle)


		p_data_values->i_seq = 0;
		FREE(p_data_values->pp_foo);
		p_sample_infos->i_seq = 0;
		FREE(p_sample_infos->pp_sample_infos);

		if (p_foo_datareader && p_data_values){
			Check_coherent_set(p_foo_datareader);
		

			pp_message = message_read_from_service((DataReader *)p_foo_datareader,max_samples, &i_size, p_queryCondition->l_sampleStateMask,  p_queryCondition->l_viewStateMask,  p_queryCondition->l_instanceStateMask);

			if (pp_message){
				expression_t result;
				for (i=0; i < i_size; i++)
				{
					if (pp_message[i]->i_datasize == 0)
					{
						printf("Data Size is O \n");
					}
					else{
						p_foo = (Foo *)pp_message[i];

						if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
						{
							ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
							assert(p_contentFilteredTopic != NULL);
							assert(p_contentFilteredTopic->p_expr != NULL);



						}else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
						{
							assert(false);
						}else {
							b_check_insert = true;
						}


						result = expression_caculate_for_message(p_queryCondition->p_expr, p_queryCondition->p_typeSupport, (char*)pp_message[i]->v_data, pp_message[i]->i_datasize, p_queryCondition->query_parameters);
						if(result.kind == BOOLEAN_KIND)
						{
						//	printf("Result : %d\r\n", result.value.boolean);
						}else{
							printf("I can't defined result...");
							assert(false);
						}

						if( result.value.boolean == false)
						{
							message_release(pp_message[i]);
							continue;
						}


						INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq,
							p_data_values->i_seq, p_foo);

						INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq,
							p_sample_infos->i_seq, &pp_message[i]->sampleInfo);
					}
				}
				FREE(pp_message);

				msleep(1); //read가 너무 빨리 가져가므로 지연

				return RETCODE_OK;
			}
			msleep(1);
			return RETCODE_NO_DATA;
		}
		msleep(1);
		return RETCODE_NO_DATA;
	}

	assert(false);
	return RETCODE_PRECONDITION_NOT_MET;
}


static ReturnCode_t Take_w_condition(FooDataReader *p_foo_datareader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, ReadCondition *p_condition)
{

	if(p_condition->condition_type == READ_CONDITION)
	{
		return p_foo_datareader->take(p_foo_datareader, p_data_values, p_sample_infos, max_samples, p_condition->l_sampleStateMask, p_condition->l_viewStateMask, p_condition->l_instanceStateMask);
	}else if(p_condition->condition_type == QUERY_CONDITION){
		message_t **pp_message = NULL;
		Foo *p_foo = NULL;
		int i_size=0;
		int i;
		TopicDescription *p_topicDescription = p_foo_datareader->get_topicdescription((DataReader *)p_foo_datareader);
		bool b_check_insert = false;
		QueryCondition* p_queryCondition = (QueryCondition*)p_condition;
		module_t *p_module = current_object( get_domain_participant_factory_module_id() );
		

		assert(p_queryCondition->p_expr  != NULL);
		assert(p_topicDescription != NULL);
		
		

		qos_remove_disposed_instance((DataReader*) p_foo_datareader);

		p_data_values->i_seq = 0;
		FREE(p_data_values->pp_foo);
		p_sample_infos->i_seq = 0;
		FREE(p_sample_infos->pp_sample_infos);



		if (p_foo_datareader && p_data_values){
			Check_coherent_set(p_foo_datareader);

			pp_message = message_take_from_service((DataReader *)p_foo_datareader,max_samples, &i_size, p_queryCondition->l_sampleStateMask, p_queryCondition->l_viewStateMask, p_queryCondition->l_instanceStateMask);


			if (pp_message){
				expression_t result;

				for (i=0; i < i_size; i++)
				{
					if (pp_message[i]->i_datasize == 0)
					{
						printf("Data Size is O \n");
					}
					else{
						

						p_foo = (Foo *)pp_message[i];

						if (p_topicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
						{
							ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_foo_datareader->p_topic;
							assert(p_contentFilteredTopic != NULL);
							assert(p_contentFilteredTopic->p_expr != NULL);

						}else if (p_topicDescription->topic_type == MULTITOPIC_TYPE)
						{
							assert(false);
						}else {
							b_check_insert = true;
						}

						result = expression_caculate_for_message(p_queryCondition->p_expr, p_queryCondition->p_typeSupport, (char*)pp_message[i]->v_data, pp_message[i]->i_datasize, p_queryCondition->query_parameters);
						if(result.kind == BOOLEAN_KIND)
						{
						//	printf("Result : %d\r\n", result.value.boolean);
						}else{
							printf("I can't defined result...");
							assert(false);
						}

						if( result.value.boolean == false)
						{
							message_release(pp_message[i]);
							continue;
						}

						INSERT_ELEM(p_data_values->pp_foo, p_data_values->i_seq,
							p_data_values->i_seq, p_foo);

						INSERT_ELEM(p_sample_infos->pp_sample_infos, p_sample_infos->i_seq,
							p_sample_infos->i_seq, &pp_message[i]->sampleInfo);


					}

				}
				FREE(pp_message);

				return RETCODE_OK;
			}
			msleep(1);
			return RETCODE_NO_DATA;
		}
		msleep(1);
		return RETCODE_NO_DATA;

	}


	assert(false);
	return RETCODE_PRECONDITION_NOT_MET;
}



ReturnCode_t Read_next_sample(FooDataReader *p_fooDataReader, inout_dds Foo **data_value,inout_dds SampleInfo **sample_info)
{
	int max_len = 1;
	SampleStateMask sample_states = NOT_READ_SAMPLE_STATE;
	ViewStateMask view_states = ANY_VIEW_STATE;
	InstanceStateMask instance_states = ANY_INSTANCE_STATE;

	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;

	Read(p_fooDataReader, &fseq, &sSeq, max_len, sample_states, view_states, instance_states);

	if(fseq.i_seq && sSeq.i_seq)
	{
		*data_value = fseq.pp_foo[0];
		*sample_info = sSeq.pp_sample_infos[0];

		REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
		REMOVE_ELEM(sSeq.pp_sample_infos, sSeq.i_seq, 0);

		return RETCODE_OK;
	}else{
		return RETCODE_NO_DATA;
	}

	
}


ReturnCode_t Take_next_sample(FooDataReader *p_fooDataReader, inout_dds Foo **data_value,inout_dds SampleInfo **sample_info)
{

	int max_len = 1;
	SampleStateMask sample_states = NOT_READ_SAMPLE_STATE;
	ViewStateMask view_states = ANY_VIEW_STATE;
	InstanceStateMask instance_states = ANY_INSTANCE_STATE;

	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;

	Take(p_fooDataReader, &fseq, &sSeq, max_len, sample_states, view_states, instance_states);

	if(fseq.i_seq && sSeq.i_seq)
	{
		*data_value = fseq.pp_foo[0];
		*sample_info = sSeq.pp_sample_infos[0];

		REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
		REMOVE_ELEM(sSeq.pp_sample_infos, sSeq.i_seq, 0);

		return RETCODE_OK;
	}else{
		return RETCODE_NO_DATA;
	}
}


ReturnCode_t Read_next_instance(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t *previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	InstanceHandle_t a_handle = 0;

	if(p_fooDataReader == NULL)
	{
		assert(false);
		return RETCODE_PRECONDITION_NOT_MET;
	}

	if(*previous_handle == 0)
	{
		if(p_fooDataReader->i_instanceset)
		{
			a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[0];
		}

		
	}else
	{
		int i;
		int i_size = p_fooDataReader->i_instanceset;
		bool is_found =  false;

		for(i=0; i < i_size; i++)
		{
			if(*previous_handle == (InstanceHandle_t)p_fooDataReader->pp_instanceset[i])
			{
				is_found = true;
				break;
			}
		}

		if(is_found == true && i_size)
		{
			if(i+1 == i_size)
			{
				//맨마지막에 있을 경우. 맨 처음것으로 변경.
				a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[0];
			}else 
			{
				a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[i+1];
			}
		}
	}

	*previous_handle = a_handle;

	return read_instance(p_fooDataReader, data_values, sample_infos, max_samples, a_handle, sample_states, view_states, instance_states);
}

ReturnCode_t Take_next_instance(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t *previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states)
{
	InstanceHandle_t a_handle = 0;

	if(p_fooDataReader == NULL)
	{
		assert(false);
		return RETCODE_PRECONDITION_NOT_MET;
	}

	if(*previous_handle == 0)
	{
		if(p_fooDataReader->i_instanceset)
		{
			a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[0];
		}

		
	}else
	{
		int i;
		int i_size = p_fooDataReader->i_instanceset;
		bool is_found =  false;

		for(i=0; i < i_size; i++)
		{
			if(*previous_handle == (InstanceHandle_t)p_fooDataReader->pp_instanceset[i])
			{
				is_found = true;
				break;
			}
		}

		if(is_found == true && i_size)
		{
			if(i+1 == i_size)
			{
				//맨마지막에 있을 경우. 맨 처음것으로 변경.
				a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[0];
			}else 
			{
				a_handle= (InstanceHandle_t)p_fooDataReader->pp_instanceset[i+1];
			}
		}
	}

	*previous_handle = a_handle;

	return take_instance(p_fooDataReader, data_values, sample_infos, max_samples, a_handle, sample_states, view_states, instance_states);
}
	

void init_foo_datareader(FooDataReader* p_foo_datareader)
{
	init_datareader((DataReader*)p_foo_datareader);

	p_foo_datareader->read = Read;
	p_foo_datareader->take = Take;
	p_foo_datareader->read_w_condition = Read_w_condition;
	p_foo_datareader->take_w_condition = Take_w_condition;
	p_foo_datareader->read_next_sample = Read_next_sample;
	p_foo_datareader->take_next_sample = Take_next_sample;
	p_foo_datareader->read_instance = read_instance;
	p_foo_datareader->take_instance = take_instance;
	p_foo_datareader->read_next_instance = Read_next_instance;
	p_foo_datareader->take_next_instance = Take_next_instance;
	p_foo_datareader->get_key_value = get_key_valueReader;
	p_foo_datareader->lookup_instance = lookup_instanceReader;
}


void remove_foo_seq(FooSeq* p_fseq, int index)
{
	if (p_fseq == NULL)
		return;

	REMOVE_ELEM(p_fseq->pp_foo, p_fseq->i_seq, index);
}


