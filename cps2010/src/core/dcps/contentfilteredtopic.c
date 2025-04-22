/* 
	ContentFilteredTopic 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static string get_filter_expression(ContentFilteredTopic* p_topicinitTopicDescription)
{
	return p_topicinitTopicDescription->filter_expression;
}

static ReturnCode_t get_expression_parameters(ContentFilteredTopic* p_topicinitTopicDescription, inout_dds StringSeq *p_expression_parameters)
{
	if(p_expression_parameters)
	{
		int i;
		REMOVE_STRING_SEQ((*p_expression_parameters));
		p_expression_parameters->i_string = 0;

		for(i=0 ;i < p_topicinitTopicDescription->expression_parameters.i_string;i++)
		{
			string str = strdup(p_topicinitTopicDescription->expression_parameters.pp_string[i]);
			INSERT_ELEM( p_expression_parameters->pp_string, p_expression_parameters->i_string,
						 p_expression_parameters->i_string, str );
		}
	}

	return RETCODE_OK;
}

static ReturnCode_t set_expression_parameters(ContentFilteredTopic* p_topicinitTopicDescription, in_dds StringSeq *p_expression_parameters)
{
	if(p_expression_parameters)
	{
		int i;
		REMOVE_STRING_SEQ(p_topicinitTopicDescription->expression_parameters);

		for(i=0 ;i < p_expression_parameters->i_string;i++)
		{
			string str = strdup(p_expression_parameters->pp_string[i]);
			INSERT_ELEM( p_topicinitTopicDescription->expression_parameters.pp_string, p_topicinitTopicDescription->expression_parameters.i_string,
						 p_topicinitTopicDescription->expression_parameters.i_string, str );
		}
	}

	return RETCODE_OK;
}

static Topic *get_related_topic(ContentFilteredTopic* p_topicinitTopicDescription)
{
	return p_topicinitTopicDescription->p_related_topic;
}

void init_content_filtered_topic(ContentFilteredTopic* p_topic_init_topic_description)
{
	init_topic_description((TopicDescription *)p_topic_init_topic_description);

	p_topic_init_topic_description->get_filter_expression = get_filter_expression;
	p_topic_init_topic_description->get_expression_parameters = get_expression_parameters;
	p_topic_init_topic_description->set_expression_parameters = set_expression_parameters;
	p_topic_init_topic_description->get_related_topic = get_related_topic;

	/////
	p_topic_init_topic_description->p_related_topic = NULL;
	p_topic_init_topic_description->filter_expression = NULL;
	p_topic_init_topic_description->expression_parameters.i_string = 0;
	p_topic_init_topic_description->expression_parameters.pp_string = NULL;
	p_topic_init_topic_description->p_expr = NULL;
}


void destroy_content_filtered_topic( ContentFilteredTopic* p_topic_init_topic_description )
{

	destroy_topic_description((TopicDescription *)p_topic_init_topic_description);
	
	REMOVE_STRING_SEQ(p_topic_init_topic_description->expression_parameters);

	FREE(p_topic_init_topic_description->filter_expression);
	FREE(p_topic_init_topic_description);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
