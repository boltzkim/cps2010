/* 
	QueryCondition 관련 부분 구현
	작성자 : 
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static string get_query_expression(QueryCondition* p_queryCondition)
{
	return p_queryCondition->query_expression;
}

static ReturnCode_t get_query_parameters(QueryCondition* p_queryCondition, inout_dds StringSeq *p_query_parameters)
{
	if(p_query_parameters)
	{
		int i;
		REMOVE_STRING_SEQ((*p_query_parameters));
		
		for(i=0 ;i < p_queryCondition->query_parameters.i_string;i++)
		{
			string str = strdup(p_queryCondition->query_parameters.pp_string[i]);
			INSERT_ELEM( p_query_parameters->pp_string, p_query_parameters->i_string,
						 p_query_parameters->i_string, str );
		}
	}

	return RETCODE_ERROR;
}
	
static ReturnCode_t set_query_parameters(QueryCondition* p_queryCondition, in_dds StringSeq *p_query_parameters)
{
	if(p_query_parameters)
	{
		int i;
		REMOVE_STRING_SEQ(p_queryCondition->query_parameters);

		for(i=0 ;i < p_query_parameters->i_string;i++)
		{
			string str = strdup(p_query_parameters->pp_string[i]);
			INSERT_ELEM( p_queryCondition->query_parameters.pp_string, p_queryCondition->query_parameters.i_string,
						 p_queryCondition->query_parameters.i_string, str );
		}
	}

	return RETCODE_ERROR;
}

void init_query_condition(QueryCondition* p_query_condition)
{
	init_read_condition((ReadCondition*)p_query_condition);
	p_query_condition->get_query_expression = get_query_expression;
	p_query_condition->get_query_parameters = get_query_parameters;
	p_query_condition->set_query_parameters = set_query_parameters;
	//////

	p_query_condition->query_expression = NULL;
	p_query_condition->query_parameters.i_string = 0;
	p_query_condition->query_parameters.pp_string = NULL;
	p_query_condition->condition_type = QUERY_CONDITION;


	p_query_condition->l_sampleStateMask = ANY_SAMPLE_STATE;
	p_query_condition->l_viewStateMask = ANY_VIEW_STATE;
	p_query_condition->l_instanceStateMask = ANY_INSTANCE_STATE;
	p_query_condition->p_datareader = NULL;
	p_query_condition->condition_type = QUERY_CONDITION;

	p_query_condition->p_expr = NULL;
	p_query_condition->p_typeSupport = NULL;
}

void destroy_query_condition(QueryCondition* p_query_condition)
{
	destroy_read_condition_except_read_condition((ReadCondition*)p_query_condition);

	FREE(p_query_condition->query_expression);
	REMOVE_STRING_SEQ(p_query_condition->query_parameters);
	FREE(p_query_condition->p_expr);

	FREE(p_query_condition);
}



void take_change_instance_status_querycondition( DataReader* p_datareader, message_t *p_message, int i )
{

	int j;
	instanceset_t *p_instanceset;
	p_instanceset = (instanceset_t*)p_datareader->pp_message_order[i]->handle;

	if(p_instanceset->view_state == NEW_VIEW_STATE){

		p_instanceset->view_state = NOT_NEW_VIEW_STATE;
		//ALL DATA NOT NEW
		for(j=0; j<p_instanceset->i_messages; j++){
			p_instanceset->pp_messages[j]->sampleInfo.view_state = NOT_NEW_VIEW_STATE;
		}
		check_all_samples_sample_status(p_instanceset);
		read_condition_set(p_datareader, p_instanceset);
	}
	take_remove_instance(p_datareader, p_instanceset);
}
