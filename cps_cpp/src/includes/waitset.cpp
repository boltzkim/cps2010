#include <core.hh>


namespace cps
{

//////////////////////////////////////////




ReturnCode_t WaitSet2::wait(inout_dds ConditionSeq2 &active_conditions,in_dds Duration_t timeout)
{
	return p_waitset->wait(p_waitset, active_conditions.getConditionSeqPtr(), timeout);
}


ReturnCode_t WaitSet2::attach_condition(in_dds Condition2 cond)
{
	return p_waitset->attach_condition(p_waitset, cond.getCondition());
}

ReturnCode_t WaitSet2::detach_condition(in_dds Condition2 cond)
{
	return p_waitset->detach_condition(p_waitset, cond.getCondition());
}

ReturnCode_t WaitSet2::get_conditions(inout_dds ConditionSeq &attached_conditions)
{
	return p_waitset->get_conditions(p_waitset, &attached_conditions);
}




//////////////////////////////////////////
}