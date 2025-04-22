#if !defined(__WAITSET_HH__)
#define __WAITSET_HH__

namespace cps
{

///////////////////////

class WaitSet2
{
public:
	WaitSet2()
	{
		p_waitset = waitset_new();
	}

	WaitSet2(WaitSet *waitset)
	{
		if(waitset)
		{
			p_waitset = waitset;
		}else
		{
			p_waitset = waitset_new();
		}
	}

	WaitSet *getWaitSet()
	{
		return p_waitset;
	}

	//////////////////////////////////////

	ReturnCode_t wait(inout_dds ConditionSeq2 &active_conditions,in_dds Duration_t timeout);
	ReturnCode_t attach_condition(in_dds Condition2 cond);
	ReturnCode_t detach_condition(in_dds Condition2 cond);
	ReturnCode_t get_conditions(inout_dds ConditionSeq &attached_conditions);

	//////////////////////////////////////

private:
	WaitSet *p_waitset;
};

///////////////////////

}

#endif