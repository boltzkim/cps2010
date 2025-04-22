#include <core.hh>


namespace cps
{

bool Condition2::get_trigger_value()
{
	return p_condition->get_trigger_value();
}


SampleStateMask ReadCondition2::get_sample_state_mask()
{
	return p_readerCondition->get_sample_state_mask(p_readerCondition);
}

ViewStateMask ReadCondition2::get_view_state_mask()
{
	return p_readerCondition->get_view_state_mask(p_readerCondition);
}

InstanceStateMask ReadCondition2::get_instance_state_mask()
{
	return p_readerCondition->get_instance_state_mask(p_readerCondition);
}

DataReader2 ReadCondition2::get_datareader()
{
	DataReader2 datareader(p_readerCondition->get_datareader(p_readerCondition));
	return datareader;
}

string QueryCondition2::get_query_expression()
{
	return p_queryCondition->get_query_expression(p_queryCondition);
}

ReturnCode_t QueryCondition2::get_query_parameters(inout_dds StringSeq &query_parameters)
{
	return p_queryCondition->get_query_parameters(p_queryCondition, &query_parameters);
}

ReturnCode_t QueryCondition2::set_query_parameters(in_dds StringSeq query_parameters)
{
	return p_queryCondition->set_query_parameters(p_queryCondition, &query_parameters);
}

StatusMask StatusCondition2::get_enabled_statuses()
{
	return p_statusCondition->get_enabled_statuses(p_statusCondition);
}

ReturnCode_t StatusCondition2::set_enabled_statuses(in_dds StatusMask mask)
{
	return p_statusCondition->set_enabled_statuses(p_statusCondition, mask);
}

Entity2 StatusCondition2::get_entity()
{
	return Entity2(p_statusCondition->get_entity(p_statusCondition));
}


}