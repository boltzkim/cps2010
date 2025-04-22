#if !defined(__CONDITION_HH__)
#define __CONDITION_HH__

namespace cps
{

class DataReader2;
class Entity2;

class Condition2
{
public:
	Condition2(Condition *condition)
	{
		p_condition = condition;
	}

	Condition *getCondition()
	{
		return p_condition;
	}

	////////////////////////////

	bool get_trigger_value();

	////////////////////////////

	
private:
	Condition *p_condition;
};



class ReadCondition2 : public Condition2
{
public:
	ReadCondition2(ReadCondition *condition) : Condition2((Condition *)condition)
	{
		p_readerCondition = condition;
	}

	ReadCondition *getReadCondition()
	{
		return p_readerCondition;
	}

	////////////////////////////

	SampleStateMask get_sample_state_mask();
	ViewStateMask get_view_state_mask();
	InstanceStateMask get_instance_state_mask();
	DataReader2 get_datareader();

	////////////////////////////

	
private:
	ReadCondition *p_readerCondition;
};

class QueryCondition2 : public ReadCondition2
{
public:
	QueryCondition2(QueryCondition *condition) : ReadCondition2((ReadCondition *)condition)
	{
		p_queryCondition = condition;
	}

	QueryCondition *getQueryCondition()
	{
		return p_queryCondition;
	}

	////////////////////////////

	string get_query_expression();
	ReturnCode_t get_query_parameters(inout_dds StringSeq &query_parameters);
	ReturnCode_t set_query_parameters(in_dds StringSeq query_parameters);

	////////////////////////////

	
private:
	QueryCondition *p_queryCondition;
};


class StatusCondition2 : public Condition2
{
public:
	StatusCondition2(StatusCondition *condition) : Condition2((Condition *)condition)
	{
		p_statusCondition = condition;
	}

	StatusCondition *getStatusCondition()
	{
		return p_statusCondition;
	}

	////////////////////////////

	StatusMask get_enabled_statuses();
	ReturnCode_t set_enabled_statuses(in_dds StatusMask mask);
	Entity2 get_entity();

	////////////////////////////

	
private:
	StatusCondition *p_statusCondition;
};

}

#endif