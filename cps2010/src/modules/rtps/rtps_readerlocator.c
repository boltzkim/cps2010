/*
	RTSP StatelessWriter class
	작성자 : 
	이력
	2010-08-11 : 처음 시작
*/

#include "rtps.h"




rtps_readerlocator_t *rtps_readerlocator_new(Locator_t a_locator, bool expectsInlineQos)
{
	rtps_readerlocator_t *p_rtps_readerlocator = malloc(sizeof(rtps_readerlocator_t));

	memset(p_rtps_readerlocator, 0, sizeof(rtps_readerlocator_t));

	p_rtps_readerlocator->locator = a_locator;
	p_rtps_readerlocator->expects_inline_qos = expectsInlineQos;
	p_rtps_readerlocator->requested_changes.p_head_first = NULL;
	p_rtps_readerlocator->requested_changes.p_head_last = NULL;
	p_rtps_readerlocator->requested_changes.i_linked_size = 0;
	p_rtps_readerlocator->unsent_changes.p_head_first = NULL;
	p_rtps_readerlocator->unsent_changes.p_head_last = NULL;
	p_rtps_readerlocator->unsent_changes.i_linked_size = 0;
	//
//	p_rtps_readerlocator->i_unsent_changes = 0;
	p_rtps_readerlocator->i_sent_changes = 0;
	p_rtps_readerlocator->i_for_writer_state = INITIAL_STATE;

	return p_rtps_readerlocator;
}

void rtps_readerlocator_destory(rtps_readerlocator_t *p_rtps_readerlocator)
{
	FREE(p_rtps_readerlocator);
}

rtps_cachechange_t *rtps_readerlocator_next_requested_change()
{
	return NULL;
}

rtps_cachechange_t *rtps_readerlocator_next_unsent_change(rtps_readerlocator_t *p_rtps_readerlocator)
{
	rtps_cachechange_t *p_change_atom = NULL;

	if(p_rtps_readerlocator->unsent_changes.i_linked_size > p_rtps_readerlocator->i_sent_changes)
	{
		p_change_atom = (rtps_cachechange_t *)p_rtps_readerlocator->unsent_changes.p_head_last;

		while(p_change_atom)
		{
			p_rtps_readerlocator->i_sent_changes++;
			//다시 살펴보기
			return p_change_atom;
			p_change_atom = (rtps_cachechange_t *)p_change_atom->p_next;
		}

		//return p_rtps_readerlocator->pp_unsent_changes[p_rtps_readerlocator->i_sent_changes++];
	}
	return NULL;
}

rtps_cachechange_t **rtps_readerlocator_requested_changes()
{
	return NULL;
}

void rtps_readerlocator_requested_changes_set(SequenceNumber_t **req_seq_num_set)
{

}

/*
	A list of changes in the writer’s
	HistoryCache that have not been sent yet to
	this ReaderLocator.
*/
linked_list_head_t rtps_readerlocator_unsent_changes(rtps_readerlocator_t *p_rtps_readerlocator, int *p_size)
{

	if(p_rtps_readerlocator->unsent_changes.i_linked_size > p_rtps_readerlocator->i_sent_changes)
	{
		*p_size = p_rtps_readerlocator->unsent_changes.i_linked_size;
		return p_rtps_readerlocator->unsent_changes;
	}

	*p_size = 0;
	return p_rtps_readerlocator->unsent_changes;
}

