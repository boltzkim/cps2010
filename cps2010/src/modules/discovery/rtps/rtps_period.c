/*
	RTPS PERIOD CLASS
	작성자 : 
	이력
	2010-09-28 : 처음 시작
*/

#include "../rtps_discovery.h"

typedef struct period_object_t
{
	MODULE_COMMON_FIELD
}period_object_t;

static void PeriodThread(module_object_t *p_this);

int rtps_period_thread_start(discovery_t *p_discovery)
{
	period_object_t *p_period = (period_object_t *)object_create(OBJECT(p_discovery),sizeof(period_object_t));
	object_attach( OBJECT(p_period), OBJECT(p_discovery) );
	
	if( thread_create( OBJECT(p_period), "discovery_period", (void*)PeriodThread, 0, false ) )
    {
		FREE(p_discovery->p_sys);
		return MODULE_ERROR_CREATE;
    }

	return MODULE_SUCCESS;
}

int rtps_period_thread_end(discovery_t *p_discovery)
{
	period_object_t *p_period = (period_object_t *)object_find(OBJECT(p_discovery),OBJECT_GENERIC, FIND_CHILD);

	thread_join(OBJECT(p_period));

    object_detach( OBJECT(p_period) );
    object_destroy( OBJECT(p_period) );

	return MODULE_SUCCESS;
}


static void PeriodThread(module_object_t *p_this)
{
	period_object_t *p_period = (period_object_t *)p_this;

	thread_ready((module_object_t *)p_period);


	while(!p_period->p_parent->b_end)
	{
		mutex_lock( &p_period->object_lock );
		//일단 심플하게 1초마다 한번씩 깨우자.....
		cond_waittimed(&p_period->object_wait,&p_period->object_lock, 1000);
		mutex_unlock( &p_period->object_lock );

		//
	}
}

static void generate_spdp_discovered_participant_data(rtps_participant_t* p_rtps_participant)
{
	if (p_rtps_participant == NULL) return;
}


static void send_participants_data(discovery_t* p_discovery)
{
	rtps_participant_t** pp_rtps_participants = NULL;
	int i_prts_participant = 0;
	int i = 0;

	rtps_participant_lock();
	
	pp_rtps_participants = get_rtps_participants(&i_prts_participant);

	for (i = 0; i < i_prts_participant ; i++)
	{
		generate_spdp_discovered_participant_data(pp_rtps_participants[i]);
	}

	rtps_participant_unlock();
}
