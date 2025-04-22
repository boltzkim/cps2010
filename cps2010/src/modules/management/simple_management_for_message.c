/*
	Management를 하기 위한 모듈로서 일단 단순하게 관리하도록 한다.

	이력
	2010-08-8
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static int  Open  ( module_object_t * );
static void Close ( module_object_t * );

static void  simple_messageWrite(management_t *p_management, message_t *p_message);
static message_t **simple_readMessage(management_t *p_management, DataReader *p_datareader, int *p_size);

struct management_sys_t
{
	message_fifo_t *p_fifo;
	int				i_messages;
	message_t		**pp_message;
	int				i_datareader;
	DataReader		**pp_datareader;
};


module_define_start(simple_management_for_messgae)
	set_category( CAT_MANAGEMENT );
	set_description( "Simple Management For Message" );
	set_capability( "management", 200 );
	set_callbacks( Open, Close );
	add_shortcut( "simple_management_for_messgae" );
module_define_end()

static void processMessage(management_t *p_managent, message_t *p_message);

static void ManagementThread(module_object_t *p_this)
{
	management_t *p_managent = (management_t *)p_this;
	message_fifo_t *p_fifo = p_managent->p_sys->p_fifo;
	message_t *p_message;

	thread_ready((module_object_t *)p_managent);


	while(!p_managent->b_end)
	{
		p_message = message_fifo_get(p_fifo);
		if(p_message) processMessage(p_managent, p_message);
	}

	message_fifo_release(p_managent->p_sys->p_fifo);

}

static int  Open(module_object_t *p_this)
{
	management_t *p_managent = (management_t *)p_this;
	module_t *p_module = p_this->p_module;

	if(!p_module)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Use Module \"Simple Management For Message\" module. p_module is NULL.");
		return MODULE_ERROR_CREATE;
	}
	
	p_managent->p_sys = malloc(sizeof(management_sys_t));
	memset(p_managent->p_sys, '\0', sizeof(management_sys_t));

	if(!p_managent->p_sys)
	{
		trace_msg(OBJECT(p_this),TRACE_ERROR,"Can't Allocate management_sys_t in \"Simple Management For Message\" module.");
		return MODULE_ERROR_MEMORY;
	}

	memset(p_managent->p_sys,'\0',sizeof(management_sys_t));
	p_managent->message_write = simple_messageWrite;
	p_managent->message_read = simple_readMessage;

	p_managent->p_sys->p_fifo = message_fifo_new();

	p_managent->p_sys->i_messages = 0;
	p_managent->p_sys->pp_message = NULL;
	p_managent->p_sys->i_datareader = 0;
	p_managent->p_sys->pp_datareader = NULL;

	if( thread_create( OBJECT(p_managent), "management", (void*)ManagementThread, 0, false ) )
    {
		message_fifo_release(p_managent->p_sys->p_fifo);
		FREE(p_managent->p_sys);
		return MODULE_ERROR_CREATE;
    }

	return MODULE_SUCCESS;
}

static void Close(module_object_t *p_this)
{
	management_t *p_managent = (management_t *)p_this;
	message_fifo_t *p_fifo = p_managent->p_sys->p_fifo;

	message_t *p_message = malloc(sizeof(message_t));
	memset(p_message,'\0',sizeof(message_t));
	p_managent->b_end = true;

	message_fifo_put(p_fifo, p_message);
	thread_join( OBJECT(p_this) );
	
	message_release(p_message);
	
	//할당된 messages들 삭제..
	while(p_managent->p_sys->i_messages)
	{
		message_release(p_managent->p_sys->pp_message[0]);
		REMOVE_ELEM( p_managent->p_sys->pp_message, p_managent->p_sys->i_messages, 0);
	}

	FREE(p_managent->p_sys->pp_message);


	//할당된 DataReader들 삭제..
	/*while(p_managent->p_sys->i_datareader)
	{
		REMOVE_ELEM( p_managent->p_sys->pp_datareader, p_managent->p_sys->i_datareader, 0);
	}*/

	FREE(p_managent->p_sys->pp_message);


	FREE(p_managent->p_sys);
}


static void  simple_messageWrite(management_t *p_management, message_t *p_message)
{
	message_fifo_t *p_fifo = p_management->p_sys->p_fifo;

	message_fifo_put(p_fifo, p_message);
}


static SampleInfo *MakeSampleInfo(message_t *p_message)
{
	SampleInfo *p_sampleInfo = malloc(sizeof(SampleInfo));
	memset(p_sampleInfo, '\0', sizeof(SampleInfo));
	p_sampleInfo->sample_state = NOT_READ_SAMPLE_STATE;
	p_sampleInfo->view_state = NEW_VIEW_STATE;
	p_sampleInfo->instance_state = ALIVE_INSTANCE_STATE;
	p_sampleInfo->source_timestamp = p_message->current_time;
	p_sampleInfo->instance_handle = p_message->handle;
	p_sampleInfo->publication_handle = p_message->publication_handle;
	p_sampleInfo->disposed_generation_count = 0;
	p_sampleInfo->no_writers_generation_count = 0;
	p_sampleInfo->sample_rank = 0;
	p_sampleInfo->generation_rank = 0;
	p_sampleInfo->absolute_generation_rank = 0;
	p_sampleInfo->valid_data = true;

	return p_sampleInfo;
}

/*
7.1.4.2.2 Changes in Read Communication Statuses

For the read communication status, the StatusChangedFlag flag is initially set to FALSE.

The StatusChangedFlag becomes TRUE when either a data-sample arrives or else the ViewState, SampleState, or
InstanceState of any existing sample changes for any reason other than a call to DataReader::read, DataReader::take or
their variants. Specifically any of the following events will cause the StatusChangedFlag to become TRUE:

	- The arrival of new data.
	- A change in the InstanceState of a contained instance. This can be caused by either:
		- The arrival of the notification that an instance has been disposed by:
			- the DataWriter that owns it if OWNERSHIP QoS kind=EXLUSIVE
		- or by any DataWriter if OWNERSHIP QoS kind=SHARED.
		- The loss of liveliness of the DataWriter of an instance for which there is no other DataWriter.
		- The arrival of the notification that an instance has been unregistered by the only DataWriter that is known to be
		   writing the instance.

Depending on the kind of StatusChangedFlag, the flag transitions to FALSE again as follows:
	- The DATA_AVAILABLE StatusChangedFlag becomes FALSE when either the corresponding listener operation
	  (on_data_available) is called or the read or take operation (or their variants) is called on the associated DataReader.
	- The DATA_ON_READERS StatusChangedFlag becomes FALSE when any of the following events occurs:
		- The corresponding listener operation (on_data_on_readers) is called.
		- The on_data_available listener operation is called on any DataReader belonging to the Subscriber.
		- The read or take operation (or their variants) is called on any DataReader belonging to the Subscriber.
*/

static void processMessage(management_t *p_management, message_t *p_message)
{
	int i , j, k;
	DomainParticipant *p_domainParticipant = NULL;
	Subscriber *p_subscriber = NULL;
	DataReader *p_datareader = NULL;
	module_t *p_module = p_management->p_module;
	SubscriberListener *p_subscriberListener = NULL;
	message_t *p_tmp_message;
	char *p_topic_name = NULL;

	//printf("ProcessMessage\r\n");

	if(!p_message->p_topic_name || !p_message->p_type_name)
	{
//		if(p_message)message_release(p_message);
		return;
	}

	mutex_lock(&p_module->object_lock);
	for( i = 0; i < p_module->i_domain_participants; i++)
	{
		p_domainParticipant = p_module->pp_domain_participants[i];
		for( j = 0 ; j < p_domainParticipant->i_subscriber; j++)
		{
			p_subscriber = p_domainParticipant->pp_subscriber[j];
			p_subscriberListener = p_subscriber->get_listener(p_subscriber);

			for( k = 0 ; k < p_subscriber->i_datareaders; k++)
			{
				p_datareader = p_subscriber->pp_datareaders[k];

				if(p_datareader->p_topic->topic_type == CONTENTFILTER_TOPIC_TYPE)
				{
					ContentFilteredTopic *p_contentFilteredTopic = (ContentFilteredTopic *)p_datareader->p_topic;
					p_topic_name = p_contentFilteredTopic->p_related_topic->topic_name;

				}else if(p_datareader->p_topic->topic_type == MULTITOPIC_TYPE)
				{
					assert(false);
				}else {
					p_topic_name = p_datareader->p_topic->get_name(p_datareader->p_topic);
				}	

				if(!strcmp(p_topic_name, p_message->p_topic_name))
				{
					p_tmp_message = message_duplicate(p_message);
					//if(p_datareader->related_rtps_reader == p_message->related_rtps_reader || p_message->related_rtps_reader == NULL/*이 메세지는 로컬에서 오는 것임.*/)
					//{
						INSERT_ELEM( p_management->p_sys->pp_datareader, p_management->p_sys->i_datareader,
								p_management->p_sys->i_datareader, p_datareader);

						INSERT_ELEM( p_management->p_sys->pp_message, p_management->p_sys->i_messages, 
							p_management->p_sys->i_messages, p_tmp_message);


						cond_signal(&p_datareader->entity_wait);
						
						p_datareader->status_changed_flag.b_data_available_status_changed_flag = true;

						if(p_subscriberListener && p_subscriberListener->on_data_available)
						{
							p_subscriberListener->on_data_available(p_datareader);
							p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
						}

						
						p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = true;
					//}

				}

				
			}

			if(p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag)
			{
				if(p_subscriberListener && p_subscriberListener->on_data_on_readers)
				{
					p_subscriberListener->on_data_on_readers(p_subscriber);
					p_subscriber->notify_datareaders(p_subscriber);
				}

				p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
			}
		}
	}

	if(p_message)message_release(p_message);

	mutex_unlock(&p_module->object_lock);
}


static message_t **simple_readMessage(management_t *p_management, DataReader *p_datareader, int *p_size)
{
	int i;
	message_t *p_message = NULL;
	module_t *p_module = p_management->p_module;
	message_t **pp_message = NULL;
	int i_count = 0;
	bool is_found_datareader = false;

	mutex_lock(&p_module->object_lock);

	for(i = 0; i < p_management->p_sys->i_datareader; i++)
	{
		if(p_management->p_sys->pp_datareader[i] == p_datareader)
		{
			p_message = p_management->p_sys->pp_message[i];
			is_found_datareader = true;

			//REMOVE_ELEM( p_management->p_sys->pp_message, p_management->p_sys->i_messages, 0);
			//REMOVE_ELEM( p_management->p_sys->pp_datareader, p_management->p_sys->i_datareader, 0);
			
//			if(p_message->p_sampleInfo) p_message->p_sampleInfo->sample_state = READ_SAMPLE_STATE;

			INSERT_ELEM(pp_message, i_count, i_count, p_message);
			/*mutex_unlock(&p_module->object_lock);
			return p_message;*/
		}
	}


	if(is_found_datareader)
	{
		int i_size = p_management->p_sys->i_datareader;

		for(i = i_size-1; i >= 0; i--){

			if(p_management->p_sys->pp_datareader[i] == p_datareader)
			{
				REMOVE_ELEM( p_management->p_sys->pp_message, p_management->p_sys->i_messages, i);
				REMOVE_ELEM( p_management->p_sys->pp_datareader, p_management->p_sys->i_datareader, i);
			}
		}

		p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
		p_datareader->get_subscriber(p_datareader)->status_changed_flag.b_data_on_readers_status_changed_flag = false;
	}

	mutex_unlock(&p_module->object_lock);

	*p_size = i_count;

	return pp_message;
}
