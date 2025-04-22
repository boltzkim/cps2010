/*
	RTSP StatelessWriter class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"

extern Locator_t defaultUnicastLocator;
extern Locator_t defaultMulticastLocator;

rtps_statelesswriter_t* rtps_statelesswriter_new( module_object_t* p_this, bool expects_inline_qos )
{
	
	rtps_statelesswriter_t *p_statelesswriter = malloc(sizeof(rtps_statelesswriter_t));

	rtps_readerlocator_t *p_rtps_readerlocator = rtps_readerlocator_new(defaultMulticastLocator, expects_inline_qos);

	memset(p_statelesswriter, 0, sizeof(rtps_statelesswriter_t));

	rtps_writer_init((rtps_writer_t *)p_statelesswriter);

	
	p_statelesswriter->resend_data_period.sec = 0;
	p_statelesswriter->resend_data_period.nanosec = 0;

	p_statelesswriter->pp_reader_locators = NULL;
	p_statelesswriter->i_reader_locators = 0;
	p_statelesswriter->behavior_type = STATELESS_TYPE;

	rtps_statelesswriter_reader_locator_add(p_statelesswriter,p_rtps_readerlocator);
	
	return p_statelesswriter;
}

void rtps_statelesswriter_destory(rtps_statelesswriter_t *p_rtps_statelesswriter){
	if(p_rtps_statelesswriter)
	{
		//할당된 reader locators들 삭제..

		mutex_lock(&p_rtps_statelesswriter->object_lock);
		while(p_rtps_statelesswriter->i_reader_locators)
		{
			rtps_readerlocator_destory(p_rtps_statelesswriter->pp_reader_locators[0]);
			REMOVE_ELEM( p_rtps_statelesswriter->pp_reader_locators, p_rtps_statelesswriter->i_reader_locators, 0);
		}
		mutex_unlock(&p_rtps_statelesswriter->object_lock);

		FREE(p_rtps_statelesswriter->pp_reader_locators);
	}
}

void rtps_statelesswriter_reader_locator_add(rtps_statelesswriter_t *p_rtps_statelesswriter, rtps_readerlocator_t *p_rtps_readerlocator)
{
	mutex_lock(&p_rtps_statelesswriter->object_lock);
	INSERT_ELEM( p_rtps_statelesswriter->pp_reader_locators, p_rtps_statelesswriter->i_reader_locators,
					 p_rtps_statelesswriter->i_reader_locators, p_rtps_readerlocator );
	mutex_unlock(&p_rtps_statelesswriter->object_lock);

}

void rtps_statelesswriter_locator_remove(rtps_statelesswriter_t *p_rtps_statelesswriter, rtps_readerlocator_t *p_rtps_readerlocator)
{

}

/*
	This operation modifies the set of ‘unsent_changes’ for all the ReaderLocators in the StatelessWriter::reader_locators.
	The list of unsent changes is reset to match the complete list of changes available in the writer’s HistoryCache.

	FOREACH readerLocator in {this.reader_locators} DO
		readerLocator.unsent_changes := {this.writer_cache.pp_changes}
*/
void rtps_statelesswriter_unsent_changes_reset(rtps_statelesswriter_t *p_rtps_statelesswriter)
{
	int size = p_rtps_statelesswriter->i_reader_locators;
	int i;

	//mutex_lock(&p_rtps_statelesswriter->object_lock);
	HISTORYCACHE_LOCK(p_rtps_statelesswriter->p_writer_cache);
	for (i = 0; i < size; i++)
	{
		p_rtps_statelesswriter->pp_reader_locators[i]->unsent_changes.p_head_first = p_rtps_statelesswriter->p_writer_cache->p_head_first;
		p_rtps_statelesswriter->pp_reader_locators[i]->unsent_changes.p_head_last = p_rtps_statelesswriter->p_writer_cache->p_head_last;
		p_rtps_statelesswriter->pp_reader_locators[i]->unsent_changes.i_linked_size = p_rtps_statelesswriter->p_writer_cache->i_linked_size;
		p_rtps_statelesswriter->pp_reader_locators[i]->i_sent_changes = 0;
	}
	HISTORYCACHE_UNLOCK(p_rtps_statelesswriter->p_writer_cache);
	//mutex_unlock(&p_rtps_statelesswriter->object_lock);

}

///////////////////////////

extern Time_t time_subtraction(const Time_t minuend_time, const Time_t subtrahend_time);

void best_effort_statelesswriter_behavior( rtps_statelesswriter_t* p_rtps_statelesswriter )
{
	int i , size;
	int	i_cachechanges = 0;
	rtps_readerlocator_t *p_rtps_readerlocator = NULL;
	//rtps_cachechange_t **p_rtps_cachechanges = NULL;
	rtps_cachechange_t *p_rtps_cachechange = NULL;
	SerializedPayloadForWriter *p_serializedPlayloadwriter = NULL;
	Time_t message_time =  currenTime();
	linked_list_head_t liked_list;

	mutex_lock(&p_rtps_statelesswriter->object_lock);
	size = p_rtps_statelesswriter->i_reader_locators;
	for (i = 0; i < size; i++)
	{
		p_rtps_readerlocator = p_rtps_statelesswriter->pp_reader_locators[i];
		
		p_rtps_readerlocator->i_for_writer_state = IDLE_STATE;
		do
		{
			liked_list = rtps_readerlocator_unsent_changes(p_rtps_readerlocator, &i_cachechanges);
			if (p_rtps_readerlocator->i_for_writer_state == IDLE_STATE && liked_list.i_linked_size)
			{
				/*
				8.4.8.1.2 Transition T2
				This transition is triggered by the guard condition [RL::unsent_changes() != <empty>] indicating that there are some
				changes in the RTPS Writer HistoryCache that have not been sent to the RTPS ReaderLocator.
				The transition performs no logical actions in the virtual machine.
				*/
				p_rtps_readerlocator->i_for_writer_state = PUSHING_STATE;
			}
			else if (p_rtps_readerlocator->i_for_writer_state == PUSHING_STATE && (liked_list.p_head_first == NULL || i_cachechanges == 0))
			{
				/*
				8.4.8.1.3 Transition T3
				This transition is triggered by the guard condition [RL::unsent_changes() == <empty>] indicating that all changes in the
				RTPS Writer HistoryCache have been sent to the RTPS ReaderLocator. Note that this does not indicate that the changes
				have been received, only that an attempt was made to send them.
				The transition performs no logical actions in the virtual machine.
				*/

				p_rtps_readerlocator->i_for_writer_state = IDLE_STATE;
			}
			else if (p_rtps_readerlocator->i_for_writer_state == PUSHING_STATE && liked_list.p_head_first)
			{
				/*
					8.4.8.1.4 Transition T4
					This transition is triggered by the guard condition [RL::can_send() == true] indicating that the RTPS Writer ‘the_writer’
					has the resources needed to send a change to the RTPS ReaderLocator ‘the_reader_locator.’

					The transition performs the following logical actions in the virtual machine:
					
						a_change := the_reader_locator.next_unsent_change();
						DATA = new DATA(a_change);
						IF (the_reader_locator.expectsInlineQos) {
							DATA.inline_qos := the_writer.related_dds_writer.qos;
						}
						DATA.reader_id := ENTITYID_UNKNOWN;
						sendto the_reader_locator.locator, DATA;

					After the transition, the following post-conditions hold:
						( a_change BELONGS-TO the_reader_locator.unsent_changes() ) == FALSE
				*/
				p_rtps_cachechange = rtps_readerlocator_next_unsent_change(p_rtps_readerlocator);

				if (p_rtps_cachechange)
				{
					DataFull *p_data = rtps_data_new(p_rtps_cachechange);
					Time_t _subtime;

					if (p_rtps_readerlocator->expects_inline_qos)
					{
						ParameterWithValue		*p_parameter = rtps_make_parameter(PID_KEY_HASH, 16, &p_rtps_statelesswriter->p_rtps_participant->guid);

						insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);

						p_parameter = rtps_make_parameter(PID_SENTINEL, 0, NULL);
					
						insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
					}

					p_data->octets_to_inline_qos = 16;

					p_data->extra_flags = 0;
					p_data->reader_id.value = ENTITYID_UNKNOWN;
					
					p_serializedPlayloadwriter = (SerializedPayloadForWriter *)p_data->p_serialized_data;
					_subtime = time_subtraction(currenTime(), p_rtps_statelesswriter->last_send);

					if (_subtime.sec > 1 || p_rtps_statelesswriter->last_send.sec == 0)
					{
						if ((p_rtps_statelesswriter->fragment_size != 0) && (p_serializedPlayloadwriter->i_size > p_rtps_statelesswriter->fragment_size))
						{
							rtps_send_data_frag_to((rtps_writer_t *)p_rtps_statelesswriter, p_rtps_readerlocator->locator, p_data, message_time);
						}
						else
						{
							//trace_msg(NULL, TRACE_LOG, "best_Effort_StatelessWriter_Behavior rtps_send_data_to");
							rtps_send_data_to((rtps_writer_t *)p_rtps_statelesswriter, p_rtps_readerlocator->locator, p_data, message_time);
						}

						p_rtps_statelesswriter->last_send = currenTime();
					}
					

					p_rtps_readerlocator->i_for_writer_state = IDLE_STATE;

					
				}


			}else
			{
				/*
					8.4.8.1.5 Transition T5
					This transition is triggered by the configuration of an RTPS Writer ‘the_rtps_writer’ to no longer send to the RTPS
					ReaderLocator ‘the_reader_locator.’ This configuration is done by the Discovery protocol (Section 8.5) as a consequence
					of breaking a pre-existing match of a DDS DataReader with the DDS DataWriter related to ‘the_rtps_writer.’
					The transition performs the following logical actions in the virtual machine:

						the_rtps_writer.reader_locator_remove(the_reader_locator);
						delete the_reader_locator;
				*/

			}
		}while(p_rtps_readerlocator->i_for_writer_state != IDLE_STATE);
		
	}
	mutex_unlock(&p_rtps_statelesswriter->object_lock);
}

