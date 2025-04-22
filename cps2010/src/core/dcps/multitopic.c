/* 
	MultiTopic 관련 부분 구현
	작성자 : 
	이력
	2010-08-6 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static string get_subscription_expression(MultiTopic* p_multiTopic)
{
	return p_multiTopic->subscription_expression;
}

static ReturnCode_t get_expression_parameters(MultiTopic* p_multiTopic, inout_dds StringSeq *p_expression_parameters)
{
	if(p_expression_parameters)
	{
		int i;
		REMOVE_STRING_SEQ((*p_expression_parameters));
		
		for(i=0 ;i < p_multiTopic->expression_parameters.i_string;i++)
		{
			string str = strdup(p_multiTopic->expression_parameters.pp_string[i]);
			INSERT_ELEM( p_expression_parameters->pp_string, p_expression_parameters->i_string,
						 p_expression_parameters->i_string, str );
		}
	}

	return RETCODE_ERROR;
}

static ReturnCode_t set_expression_parameters(MultiTopic* p_multiTopic, in_dds StringSeq *p_expression_parameters)
{
	if(p_expression_parameters)
	{
		int i;
		REMOVE_STRING_SEQ(p_multiTopic->expression_parameters);

		for(i=0 ;i < p_expression_parameters->i_string;i++)
		{
			string str = strdup(p_expression_parameters->pp_string[i]);
			INSERT_ELEM( p_multiTopic->expression_parameters.pp_string, p_multiTopic->expression_parameters.i_string,
						 p_multiTopic->expression_parameters.i_string, str );
		}
	}

	return RETCODE_ERROR;
}


void init_multi_topic(MultiTopic* p_multi_topic)
{
	init_topic_description((TopicDescription *)p_multi_topic);

	p_multi_topic->get_subscription_expression = get_subscription_expression;
	p_multi_topic->get_expression_parameters = get_expression_parameters;
	p_multi_topic->set_expression_parameters = set_expression_parameters;

	/////

	p_multi_topic->subscription_expression = NULL;
	p_multi_topic->expression_parameters.i_string = 0;
	p_multi_topic->expression_parameters.pp_string = NULL;
}


void destroy_multi_topic(MultiTopic* p_multi_topic)
{

	destroy_topic_description((TopicDescription *)p_multi_topic);
	
	REMOVE_STRING_SEQ(p_multi_topic->expression_parameters);

	FREE(p_multi_topic->subscription_expression);
	FREE(p_multi_topic);
}