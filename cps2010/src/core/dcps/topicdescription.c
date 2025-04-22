/* 
	TopicDescription 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static string _get_name(Topic *p_topic)
{
	return p_topic->topic_name;
}

static string _get_type_name(Topic *p_topic)
{
	return p_topic->type_name;
}


static DomainParticipant *_get_participant(Topic *p_topic)
{
	return p_topic->p_domain_participant;
}

void init_topic_description(TopicDescription* p_topic_description)
{
	p_topic_description->get_type_name = _get_type_name;
	p_topic_description->get_name = _get_name;
	p_topic_description->get_participant = _get_participant;

	p_topic_description->topic_name = NULL;
	p_topic_description->type_name = NULL;
	p_topic_description->p_domain_participant = NULL;
}


void destroy_topic_description(TopicDescription* p_topic_description)
{
	FREE(p_topic_description->topic_name);
	FREE(p_topic_description->type_name);
}
