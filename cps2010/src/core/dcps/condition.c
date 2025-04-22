/* 
	Condition 관련 부분 구현
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static bool get_trigger_value(Condition* p_condition)
{
	return p_condition->b_trigger_value;
}

void init_condition(Condition* p_condition)
{
	p_condition->get_trigger_value = get_trigger_value;
	/////
	p_condition->b_trigger_value = false;
	p_condition->condition_type = NORMAL_CONDITION;
}

void destroy_condition(Condition* p_condition)
{

}
