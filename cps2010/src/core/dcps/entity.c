/* 
	Entity 관련 부분 구현
	작성자 : 
	이력
	2010-07-18 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



void pause_system()
{
	printf("++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n++++++++++++\r\n");
	printf("pause......\r\n Enter Anything..............");

	MessageBox(NULL, "시스템이 일시 정지 상태입니다. 로컬 아이피를 확인하세요.", "정지", MB_ICONWARNING | MB_OK);
	

}

static int32_t id = 0;

static ReturnCode_t enable(Entity *p_entity)
{
	if (p_entity)
	{
		p_entity->b_enable = true;
	}
	return RETCODE_OK;
}


static StatusCondition *get_statuscondition(Entity *p_entity)
{
	return p_entity->p_status_condition;
}


static StatusMask get_status_changes(Entity *p_entity)
{
	return p_entity->l_status_changes;
}


static InstanceHandle_t get_instance_handle(Entity *p_entity)
{
	return (InstanceHandle_t)p_entity;
}

void init_entity(Entity* p_entity)
{
	p_entity->id = ++id;
	p_entity->enable = enable;
	p_entity->get_statuscondition = get_statuscondition;
	p_entity->get_status_changes = get_status_changes;
	p_entity->get_instance_handle = get_instance_handle;
	/////

	p_entity->b_enable = false;
	p_entity->p_status_condition = malloc(sizeof(StatusCondition));
	memset(p_entity->p_status_condition, '\0', sizeof(StatusCondition));

	p_entity->l_status_mask = 0;
	init_status_condition(p_entity->p_status_condition);
	p_entity->p_status_condition->p_entity = p_entity;
	mutex_init(&p_entity->entity_lock);
	cond_init(&p_entity->entity_wait);
//ack by jun
	mutex_init(&p_entity->ack_lock);
	cond_init(&p_entity->ack_wait);
	p_entity->i_entity_type = ENTITY_ENTITY;
	p_entity->last_sequencenum = 0;
	p_entity->test_for_reliablity_division = 1;
	p_entity->test_for_reliablity_bulk_start = -1;
	p_entity->test_for_reliablity_bulk_last = -1;
}


void destroy_entity( Entity* p_entity )
{
	destroy_status_condition(p_entity->p_status_condition);

	cond_destroy(&p_entity->entity_wait);
	mutex_destroy(&p_entity->entity_lock);
}