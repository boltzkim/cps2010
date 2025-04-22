/* 
	StatusCondition 관련 부분 구현
	작성자 : 
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static StatusMask get_enabled_statuses(StatusCondition* p_statusCondition)
{
	if(p_statusCondition){
		return p_statusCondition->enabled_statuses;
	}

	return RETCODE_ERROR;
}

static ReturnCode_t set_enabled_statuses(StatusCondition* p_statusCondition, in_dds StatusMask mask)
{

	if(p_statusCondition){
		p_statusCondition->enabled_statuses = mask;
		return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static Entity *get_entity(StatusCondition* p_statusCondition)
{
	return p_statusCondition->p_entity;
}

void init_status_condition(StatusCondition* p_status_condition)
{
	init_condition((Condition *) p_status_condition);
	p_status_condition->get_enabled_statuses = get_enabled_statuses;
	p_status_condition->set_enabled_statuses = set_enabled_statuses;
	p_status_condition->get_entity = get_entity;
	////////

//waitset by jun at initial time, enabled_statuses is 0
//	p_statusCondition->enabled_statuses = ANY_STATUS_STATE;
	p_status_condition->enabled_statuses = 0;

	p_status_condition->p_entity = NULL;
	p_status_condition->condition_type = STATUS_CONDITION;
}

void destroy_status_condition(StatusCondition* p_status_condition)
{
	destroy_condition((Condition *)p_status_condition);
	FREE(p_status_condition);
}
