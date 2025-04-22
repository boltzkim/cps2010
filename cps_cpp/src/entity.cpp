#include <core.hh>


namespace cps
{

ReturnCode_t Entity2::enable()
{
	return p_entity->enable(p_entity);
}

StatusCondition2 Entity2::get_statuscondition()
{
	StatusCondition2 statusCondition(p_entity->get_statuscondition(p_entity));
	return statusCondition;
}

StatusMask Entity2::get_status_changes()
{
	return p_entity->get_status_changes(p_entity);
}

InstanceHandle_t Entity2::get_instance_handle()
{
	return p_entity->get_instance_handle(p_entity);
}



}