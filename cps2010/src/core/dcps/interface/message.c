/*
	메시지 관련...

	이력
	2010-08-8 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


int set_string_rtps_to_type_support(dds_string2* p_string, char* p_data, int i_parameter_string_size, int packet_size, long offset)
{
	int i_string = 0;
	int32_t i_string_len = *(int32_t*)p_data;

	memset(p_string, 0, i_parameter_string_size);
	//memcpy(p_string, p_data, packet_size);

	p_string->size = i_string_len;
	
	if(p_string->size <= i_parameter_string_size)
	{
		i_string = p_string->size / 4;
		if((p_string->size % 4)!=0)
		{
			i_string++;
		}
		i_string *= 4;
	}

	i_string += sizeof(int32_t); // i_size

	memcpy(p_string, p_data, i_string);

	return i_string;
}


// Check windows
//#if _WIN32 || _WIN64
//#if _WIN64
//#define PADDDING_NUM 8
//#else
//#define PADDDING_NUM 4
//#endif
//#endif
//
//// Check GCC
//#if __GNUC__
//#if __x86_64__ || __ppc64__
//#define PADDDING_NUM 8
//#else
//#define PADDDING_NUM 4
//#endif
//#endif


#define PADDDING_NUM 4

/************************************************************************/
/* struct member alignment 로 발생되는 padding bit 를 계산하는 함수     */
/************************************************************************/
static int32_t get_padding_bit(const int32_t offset, int i_size)
{
	int i_padding_bit = 0;

#ifdef __i386__
	i_size = (i_size > sizeof(int)) ? sizeof(int) : i_size;
#endif
	if (offset % PADDDING_NUM)
	{
		i_padding_bit = PADDDING_NUM - (offset % PADDDING_NUM);
	}

	return i_padding_bit;
}

void caculate_message_data_rtps_to_type_support(message_t* p_message, FooTypeSupport* p_type_support, char* support_value, int i_typesupport_size, char* p_value, int i_size)
{
	int i = 0;
	int32_t offset = 0;
	int32_t len = 0;
	int32_t i_len = 0;
	dds_parameter_t* p_parameter = NULL;

	for (i = 0; i < p_type_support->i_parameters; i++)
	{
		p_parameter = p_type_support->pp_parameters[i];

		switch (p_parameter->type)
		{
			case DDS_STRING_TYPE:
				i_len  = set_string_rtps_to_type_support((dds_string2*)(support_value+offset), &p_value[len], p_parameter->i_size, i_size, offset);
				break;
			default:
				//offset += get_padding_bit(offset, p_parameter->i_size);
				memcpy(support_value+offset, &p_value[len], p_parameter->i_size);
				i_len  = p_parameter->offset;
				break;
		}

		if (p_parameter->is_key)
		{
			p_message->keylist[p_message->i_key].offset = offset;
			p_message->keylist[p_message->i_key].len = i_len;
			p_message->keylist[p_message->i_key].p_value = &p_value[len];
			p_message->i_key++;
		}

		len += i_len;

		offset += p_parameter->i_size;
	}
	//printf("len : %d, %d", len, p_type_support->offsetcount);
	assert(len <= p_type_support->offsetcount);
}


long caculate_message_data_type_support_to_rtps( message_t* p_message, FooTypeSupport* p_type_support, char* p_send_value, int i_type_support_size, char* p_value)
{
	int i = 0;
	int32_t offset = 0;
	int32_t len = 0;
	int i_temp = 0;
	dds_parameter_t* p_parameter = NULL;
	dds_string2* p_string = NULL;
	char* p_temp = NULL;
	int32_t i_len = 0;

	for (i = 0; i < p_type_support->i_parameters; i++)
	{
		p_parameter = p_type_support->pp_parameters[i];

		switch (p_parameter->type)
		{
			case DDS_STRING_TYPE:
				p_temp = get_string(&p_value[offset+sizeof(int32_t)], &i_temp);
				memcpy(&p_send_value[len], p_temp, i_temp);
				i_len = i_temp;
				FREE(p_temp);
				break;
			default:
				//offset += get_padding_bit(offset, p_parameter->i_size);
				memcpy(&p_send_value[len], &p_value[offset], p_parameter->i_size);
				i_len =  p_parameter->offset;
				break;
		}

		if (p_parameter->is_key)
		{
			uint8_t guid_key[16];
			int copy_len = 0;

			memset(guid_key, 0, 16);

			p_message->keylist[p_message->i_key].offset = offset;
			p_message->keylist[p_message->i_key].len = i_len;
			p_message->keylist[p_message->i_key].p_value = &p_value[len];
			
			p_message->keylist[p_message->i_key].id = p_parameter->id;

			if (i_len < 12)
			{
				copy_len = i_len;
			}
			else
			{
				copy_len = 12;
			}

			memcpy(guid_key, &p_parameter->id, 4);
			memcpy(guid_key+4, p_message->keylist[p_message->i_key].p_value, copy_len);

			get_key_guid(p_message, guid_key, 16);

			p_message->i_key++;
		}

		len += i_len;
		offset += p_parameter->i_size;
	}

	return len;
}


message_t* message_new(DataWriter* p_datawriter, void* v_data)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );
	message_t *p_message = NULL;
	Topic *p_topic = NULL;
	string type_name=NULL;
	FooTypeSupport *p_typeSupport = NULL;


	if(p_module->b_end) return NULL;

	
	p_topic = p_datawriter->get_topic(p_datawriter);
	type_name = p_topic->get_type_name(p_topic);
	

	if(!p_datawriter || !type_name || !v_data)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"don't make new a massage. p_DataWriter, type_name or v_data is NULL.");
	}
		
	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), type_name);

	p_message = (message_t *)malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));

	p_message->p_next = NULL;
	p_message->p_prev = NULL;

	assert(p_typeSupport);

	/// TypeSupport 정해지면 여기서 체크해서 정확히 값을 가져옴.
	if(p_typeSupport)
	{
		p_message->i_datasize = p_typeSupport->offsetcount;
	}else
	{
		p_message->i_datasize = 0;
	}


	if(p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}
	//added by kyy(Presentation QoS Writer Side)
	else if(/*p_DataWriter->p_publisher->begin_coherent == false ||*/ v_data == NULL)
	{
		p_message->i_datasize = 0;
		p_message->v_data = NULL;
	}
	///////////////////////////////////////////////////////////////////////////
	else{
		int32_t tmp_size;
		p_message->v_data = malloc(p_message->i_datasize);
		memset(p_message->v_data, '\0', p_message->i_datasize);
		tmp_size = caculate_message_data_type_support_to_rtps(p_message, p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, (char*)v_data);

		if(p_message->i_datasize < tmp_size)
		{
			assert(0);
		}

		p_message->i_datasize = tmp_size;
	}

	p_message->sequence = 0;

	p_message->p_topic_name = NULL;
	p_message->p_type_name = NULL;

	if(p_topic)
	{
		p_message->p_topic_name = strdup(p_topic->get_name(p_topic));
		p_message->p_type_name = strdup(p_topic->get_type_name(p_topic));
	}

	p_message->current_time = currenTime();
	//p_message->sampleInfo = NULL;
	p_message->v_related_rtps_reader = NULL;
	p_message->v_related_cachechange =  NULL;
	p_message->coherent_set_number = 0;
	
	return p_message;
}


message_t* message_new_for_builtin_reader(DataReader* p_datareader, void* v_data, int i_reader_data_size)
{
	module_t *p_module = current_object( get_domain_participant_factory_module_id() );

	message_t *p_message = NULL;
	TopicDescription *p_TopicDescription = p_datareader->get_topicdescription(p_datareader);
	string type_name = p_TopicDescription->type_name;
	FooTypeSupport *p_typeSupport = NULL;

	if(!p_datareader || !type_name || !v_data)
	{
		trace_msg(OBJECT(p_module),TRACE_ERROR,"don't make new a massage. p_DataWriter, type_name or v_data is NULL.");
	}
		
	p_typeSupport = domain_participant_find_support_type(OBJECT(p_module), p_TopicDescription->p_domain_participant, type_name);

	p_message = malloc(sizeof(message_t));
	memset(p_message, '\0', sizeof(message_t));
	//
	assert(p_typeSupport);
	/*
		정의한것보다 받은 패킷의 크기가 작거나 같아야 한다.
	*/
	assert(p_typeSupport->offsetcount >= i_reader_data_size);

	//p_message->i_datasize = i_reader_data_size;
	//p_message->i_datasize = p_typeSupport->i_size;
	p_message->i_datasize = p_typeSupport->offsetcount;

	//added by kyy(Presentation QoS)
	if(v_data == NULL)
	{
		p_message->i_datasize = 0;
	}
	/////////////////////////////////////////////////////

	if(p_message->i_datasize == 0)
	{
		p_message->v_data = NULL;
	}else{
		/*p_message->v_data = malloc(p_message->i_datasize);
		memcpy(p_message->v_data,v_data,p_message->i_datasize);*/

		p_message->v_data = malloc(p_typeSupport->offsetcount+1);
		memset(p_message->v_data, '\0', p_typeSupport->offsetcount + 1);

		caculate_message_data_rtps_to_type_support(p_message, p_typeSupport,  (char*)p_message->v_data, p_message->i_datasize, (char*)v_data, i_reader_data_size);
		
		//memcpy(p_message->v_data,v_data,p_typeSupport->i_size);
	}

	p_message->sequence = 0;

	p_message->p_topic_name = NULL;
	p_message->p_type_name = NULL;

	if(p_TopicDescription)
	{
		if(p_TopicDescription->topic_type == CONTENTFILTER_TOPIC_TYPE)
		{
			ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_datareader->p_topic;
			expression_t result;

			p_message->p_topic_name = strdup(p_contentFilteredTopic->p_related_topic->topic_name);
			assert(p_contentFilteredTopic->p_expr != NULL);
			result = expression_caculate(p_contentFilteredTopic->p_expr, p_typeSupport, (char*)v_data, i_reader_data_size);
			if(result.kind == BOOLEAN_KIND)
			{
			//	printf("Result : %d\r\n", result.value.boolean);
			}else{
				printf("I can't defined result...");
				assert(false);
			}

			if( result.value.boolean == false)
			{
				message_release(p_message);
				return NULL;
			}
		
		}else if(p_TopicDescription->topic_type == MULTITOPIC_TYPE)
		{
			assert(false);
		}else{
			p_message->p_topic_name = strdup(p_TopicDescription->topic_name);
		}

		
		p_message->p_type_name = strdup(p_TopicDescription->type_name);
	}

	

	p_message->current_time = currenTime();
//	p_message->p_sampleInfo = NULL;
	
	p_message->v_related_rtps_reader = p_datareader->p_related_rtps_reader;

	//printf("message_new : %p\r\n", p_message);

	return p_message;
}

void message_release(message_t* p_message)
{
	//printf("message_release : %p\r\n", p_message);

	if(p_message->i_datasize)
	{
		FREE(p_message->v_data);
	}else{
		int i=0;
		i++;
	}

//	FREE(p_message->p_sampleInfo);
	FREE(p_message->p_topic_name);
	FREE(p_message->p_type_name);
    FREE(p_message);
}


message_fifo_t* message_fifo_new()
{
    message_fifo_t *p_fifo;

    p_fifo = malloc( sizeof( message_fifo_t));
	memset(p_fifo, '\0', sizeof(message_fifo_t));
    mutex_init(&p_fifo->lock );
    cond_init(&p_fifo->wait );
    p_fifo->i_depth = p_fifo->i_size = 0;
    p_fifo->p_first = NULL;
    p_fifo->pp_last = &p_fifo->p_first;

    return p_fifo;
}



message_t* message_fifo_get(message_fifo_t* p_fifo)
{
    message_t *m;

    mutex_lock( &p_fifo->lock );

    while( p_fifo->p_first == NULL )
    {
        cond_wait( &p_fifo->wait, &p_fifo->lock );
    }

    m = p_fifo->p_first;

    p_fifo->p_first = m->p_next;
    p_fifo->i_depth--;
	p_fifo->i_size -= m->i_datasize;

    if( p_fifo->p_first == NULL )
    {
        p_fifo->pp_last = &p_fifo->p_first;
    }

    mutex_unlock( &p_fifo->lock );

    m->p_next = NULL;
    return m;
}


void message_fifo_empty(message_fifo_t* p_fifo)
{
    message_t* p_message;

    mutex_lock(&p_fifo->lock);
    for (p_message = p_fifo->p_first; p_message != NULL;)
    {
        message_t *p_next;

        p_next = p_message->p_next;
        message_release(p_message);
        p_message = p_next;
    }

    p_fifo->i_depth = p_fifo->i_size = 0;
    p_fifo->p_first = NULL;
    p_fifo->pp_last = &p_fifo->p_first;
    mutex_unlock( &p_fifo->lock );
}

void message_fifo_release(message_fifo_t* p_fifo)
{
    message_fifo_empty( p_fifo );
    cond_destroy( &p_fifo->wait );
    mutex_destroy( &p_fifo->lock );
    FREE( p_fifo );
}


int message_fifo_put(message_fifo_t* p_fifo, message_t* p_message)
{
    int i_size = 0;
    mutex_lock( &p_fifo->lock );

    do
    {
        i_size += p_message->i_datasize;

        *p_fifo->pp_last = p_message;
        p_fifo->pp_last = &p_message->p_next;
        p_fifo->i_depth++;
        p_fifo->i_size += p_message->i_datasize;

        p_message = p_message->p_next;

    } while( p_message );


    cond_signal( &p_fifo->wait );
    mutex_unlock( &p_fifo->lock );

    return i_size;
}


char *get_string(char* p_chars, int* p_size)
{
	int i_len;
	int i_length = 0;
	char* p_string;
	if (p_chars == NULL)
	{
		*p_size = 0;
		return NULL;
	}
	i_len = strlen(p_chars)+1;
	
	i_length = i_len / sizeof(int32_t);
	if (i_len % sizeof(int32_t))
	{
		i_length++;
	}

	p_string = malloc(sizeof(int32_t)+(i_length*4));
	memset(p_string, 0, sizeof(int32_t)+(i_length*4));

	memcpy(p_string, &i_len, sizeof(int32_t));
	memcpy(p_string+sizeof(int32_t), p_chars, i_len);

	

	*p_size =  sizeof(int32_t)+(i_length*sizeof(int32_t));
	return p_string;
}

