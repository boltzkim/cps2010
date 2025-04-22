/* 
	WaitSet 관련 부분 구현
	작성자 : 
	이력
	2010-08-8 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



#ifdef __APPLE__
static ReturnCode_t wait_APPLE(WaitSet* p_waitSet, inout_dds ConditionSeq *p_active_conditions, in_dds Duration_t timeout)
#else
static ReturnCode_t wait(WaitSet* p_waitSet, inout_dds ConditionSeq *p_active_conditions,in_dds Duration_t timeout)
#endif
{
	int i;
	int j = 0;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	unsigned long waketime =0;
	Condition *p_condition=NULL;

	waketime = timeout.sec * 1000 + timeout.nanosec /1000000;

	mutex_lock(&p_waitSet->waitset_lock);

	// cond_waittimed 사용하기 전에 일단 dcpsinstance에 데이터의 상태를 먼저 체크하고,
	// 없을 경우에 wait 해야함 안그러면은 데이터 하나 읽어가고 wait됨..

	cond_waittimed(&p_waitSet->waitset_wait, &p_waitSet->waitset_lock, waketime);

	for(i=0 ; i < p_waitSet->i_conditions; i++)
	{
		p_condition = p_waitSet->pp_conditions[i];

		if(p_condition->b_trigger_value == true)
		{
			INSERT_ELEM( p_active_conditions->pp_conditions, p_active_conditions->i_seq,
								p_active_conditions->i_seq, p_condition);
		//	j++;
			p_condition->b_trigger_value = false;
		}
	}
	//p_active_conditions->i_seq = j;
	mutex_unlock(&p_waitSet->waitset_lock);
	return RETCODE_OK;
}

ReturnCode_t data_on_readers_statuscondition(Entity * subscriber_entity){

	Subscriber * p_subscriber = (Subscriber *)subscriber_entity;
	if(subscriber_entity->p_status_condition)		
	{
		if(subscriber_entity->p_status_condition->enabled_statuses & DATA_ON_READERS_STATUS)
		{	
			if(subscriber_entity->p_status_condition->p_waitset) {
				
				subscriber_entity->p_status_condition->b_trigger_value =true;

				p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
				p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;

				cond_signal(&subscriber_entity->p_status_condition->p_waitset->waitset_wait);
			}
		}
	}
	return RETCODE_OK;
}

ReturnCode_t data_available_statuscondition(Entity * reader_entity) 
{	
	DataReader * p_datareader = (DataReader *)reader_entity;
	Subscriber * p_subscriber = p_datareader->p_subscriber;
	if(reader_entity->p_status_condition)
	{		
		if(reader_entity->p_status_condition->enabled_statuses & DATA_AVAILABLE_STATUS)
		{	
			if(reader_entity->p_status_condition->p_waitset) {
					
				reader_entity->p_status_condition->b_trigger_value =true;

				p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
				p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;
				
				p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
				p_datareader->l_status_changes -= DATA_AVAILABLE_STATUS;
				
				cond_signal(&reader_entity->p_status_condition->p_waitset->waitset_wait);
			}
		}
	}
	return RETCODE_OK;
}

//waitset by jun
//waitset에 condition을 붙이고 컨디션에 이벤트가 발생한 경우 waitset_wait에 이벤트
static ReturnCode_t attach_condition(WaitSet* p_waitSet, in_dds Condition *p_cond)
{
	mutex_lock(&p_waitSet->waitset_lock);
	p_cond->p_waitset = p_waitSet;
	INSERT_ELEM( p_waitSet->pp_conditions, p_waitSet->i_conditions, p_waitSet->i_conditions, p_cond);
	mutex_unlock(&p_waitSet->waitset_lock);

	return RETCODE_OK;
}

//waiset by jun
static ReturnCode_t detach_condition(WaitSet* p_waitSet, in_dds Condition *p_cond)
{
	if(p_waitSet && p_cond)
	{
		int i;
		mutex_lock(&p_waitSet->waitset_lock);
		for(i=0 ; i < p_waitSet->i_conditions; i++)
		{
			if(p_waitSet->pp_conditions[i] == p_cond)
			{
				p_cond->p_waitset =NULL;
				REMOVE_ELEM( p_waitSet->pp_conditions, p_waitSet->i_conditions, i);
				return RETCODE_OK;
			}
		}
		mutex_unlock(&p_waitSet->waitset_lock);
	}

	return RETCODE_ERROR;
}

static ReturnCode_t get_conditions(WaitSet* p_waitSet, inout_dds ConditionSeq *p_attached_conditions)
{
	if(p_waitSet && p_attached_conditions)
	{
		int i;
		mutex_lock(&p_waitSet->waitset_lock);
		for(i=0 ; i < p_waitSet->i_conditions; i++)
		{
			INSERT_ELEM( p_attached_conditions->pp_conditions, p_waitSet->i_conditions, p_waitSet->i_conditions, p_waitSet->pp_conditions[i]);
		}
		mutex_unlock(&p_waitSet->waitset_lock);
	}

	return RETCODE_OK;
}

void init_waitset(WaitSet* p_waitset)
{
#ifdef __APPLE__
	p_waitset->wait = wait_APPLE;
#else
	p_waitset->wait = wait;
#endif
	p_waitset->attach_condition = attach_condition;
	p_waitset->detach_condition = detach_condition;
	p_waitset->get_conditions = get_conditions;
	///
	mutex_init(&p_waitset->waitset_lock);
	p_waitset->i_conditions = 0;
	p_waitset->pp_conditions = NULL;

//	waitset by jun
	cond_init(&p_waitset->waitset_wait);
}

void destroy_waitset(WaitSet* p_waitset)
{
	mutex_destroy(&p_waitset->waitset_lock);

//	waitset by jun
	cond_destroy(&p_waitset->waitset_wait);

	FREE(p_waitset->pp_conditions);
	FREE(p_waitset);
}

WaitSet* waitset_new()
{
	WaitSet *p_waitSet = malloc(sizeof(WaitSet));
	memset(p_waitSet, '\0', sizeof(WaitSet));

	init_waitset(p_waitSet);
	return p_waitSet;
}

ReturnCode_t data_on_readers_and_data_available_set(Entity * publisher_entity , Entity * reader_entity)
{
	if(publisher_entity->p_status_condition)			//초기에 설정 
	{
		if(publisher_entity->p_status_condition->enabled_statuses&DATA_ON_READERS_STATUS)
		{
			if(publisher_entity->p_status_condition->p_waitset) {
				publisher_entity->p_status_condition->b_trigger_value =true;
				cond_signal(&publisher_entity->p_status_condition->p_waitset->waitset_wait);
			}
		}
	}

	if(reader_entity->p_status_condition)
	{	
		if(reader_entity->p_status_condition->enabled_statuses&DATA_AVAILABLE_STATUS)
		{
			if(reader_entity->p_status_condition->p_waitset) {
				reader_entity->p_status_condition->b_trigger_value =true;
				cond_signal(&reader_entity->p_status_condition->p_waitset->waitset_wait);
			}
			
		}
	}
	return RETCODE_OK;
}

ReturnCode_t status_condition_plain_status_set(Entity* p_entity, StatusKind kind)
{

	if(p_entity->p_status_condition)
	{
		if(p_entity->p_status_condition->enabled_statuses&kind)
		{
			if(p_entity->p_status_condition->p_waitset) {
				p_entity->p_status_condition->b_trigger_value =true;
				cond_signal(&p_entity->p_status_condition->p_waitset->waitset_wait);
//reader
				if(kind == SAMPLE_LOST_STATUS){
					DataReader * p_datareader = (DataReader *)p_entity;
					p_datareader->status.sample_lost.total_count_change = 0;
					p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
					p_datareader->l_status_changes -= SAMPLE_LOST_STATUS;

				} else if(kind == REQUESTED_INCOMPATIBLE_QOS_STATUS){
					DataReader * p_datareader = (DataReader *)p_entity;
					p_datareader->status.requested_incompatible_qos.total_count_change = 0;
					p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
					p_datareader->l_status_changes -= REQUESTED_INCOMPATIBLE_QOS_STATUS;

				} else if(kind == SUBSCRIPTION_MATCHED_STATUS){
					DataReader * p_datareader = (DataReader *)p_entity;
					p_datareader->status.subscriber_matched.total_count_change = 0;
					p_datareader->status.subscriber_matched.current_count_change = 0;
					p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
					p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
					
				} else if(kind == REQUESTED_DEADLINE_MISSED_STATUS){
					DataReader * p_datareader = (DataReader *)p_entity;
					p_datareader->status.requested_deadline_missed.total_count_change = 0;
					p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
					p_datareader->l_status_changes -= REQUESTED_DEADLINE_MISSED_STATUS;
				}
//writer
				if(kind == PUBLICATION_MATCHED_STATUS){
					DataWriter * p_datawriter = (DataWriter *)p_entity;
					p_datawriter->status.publication_matched.total_count_change = 0;
					p_datawriter->status.publication_matched.current_count_change = 0;
					p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
					p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;

				} else if(kind == OFFERED_INCOMPATIBLE_QOS_STATUS){
					DataWriter * p_datawriter = (DataWriter *)p_entity;
					p_datawriter->status.offered_incompatible_qos.total_count_change = 0;
					p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
					p_datawriter->l_status_changes -= OFFERED_INCOMPATIBLE_QOS_STATUS;

				} else if(kind == OFFERED_DEADLINE_MISSED_STATUS){
					DataWriter * p_datawriter = (DataWriter *)p_entity;
					p_datawriter->status.offered_deadline_missed.total_count_change = 0;
					p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
					p_datawriter->l_status_changes -= OFFERED_DEADLINE_MISSED_STATUS;
				}
//topic
				if(kind == INCONSISTENT_TOPIC_STATUS){
					Topic * p_topic = (Topic *)p_entity;
					p_topic->status.inconsistent_topic.total_count_change = 0;
					p_topic->status_changed_flag.b_inconsistent_topic_status_changed_flag = false;
					p_topic->l_status_changes -= INCONSISTENT_TOPIC_STATUS;

				}
				
			}
		}
	}
	return RETCODE_OK;
}
		


//readcondition by jun

ReturnCode_t check_all_samples_sample_status( instanceset_t* p_instanceset )
{
	int i;
	for(i=0; i<p_instanceset->i_messages; i++){
		p_instanceset->sample_state |= p_instanceset->pp_messages[i]->sampleInfo.sample_state;
	}
	
	return RETCODE_OK;
}
ReturnCode_t read_condition_set( DataReader* p_datareader, instanceset_t* p_instanceset )
{
	int i;
	ReadCondition  *p_readCondtion;
	QueryCondition *p_queryCondition;
	

	if (p_datareader->i_read_conditions)
	{
		for (i = 0; i < p_datareader->i_read_conditions ; i++) {

			p_readCondtion = p_datareader->pp_read_conditions[i];

			if((p_readCondtion->l_sampleStateMask & p_instanceset->sample_state)&&(p_readCondtion->l_viewStateMask & p_instanceset->view_state)&&(p_readCondtion->l_instanceStateMask & p_instanceset->instance_state) ){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}


	if (p_datareader->i_query_conditions)
	{
		for (i = 0; i < p_datareader->i_query_conditions ; i++) {

			p_queryCondition = p_datareader->pp_query_conditions[i];

			if((p_queryCondition->l_sampleStateMask & p_instanceset->sample_state)&&(p_queryCondition->l_viewStateMask & p_instanceset->view_state)&&(p_queryCondition->l_instanceStateMask & p_instanceset->instance_state) ){

				if(p_queryCondition->p_waitset){

					

					p_queryCondition->b_trigger_value = true;
					cond_signal(&p_queryCondition->p_waitset->waitset_wait);


				}
			}
		}
	}


	return RETCODE_OK;
}


ReturnCode_t read_condition_set_from_add_instanceset( DataReader* p_datareader, instanceset_t* p_instanceset, message_t *p_message )
{
	int i;
	ReadCondition  *p_readCondtion;
	QueryCondition *p_queryCondition;
	

	if (p_datareader->i_read_conditions)
	{
		for (i = 0; i < p_datareader->i_read_conditions ; i++) {

			p_readCondtion = p_datareader->pp_read_conditions[i];

			if((p_readCondtion->l_sampleStateMask & p_instanceset->sample_state)&&(p_readCondtion->l_viewStateMask & p_instanceset->view_state)&&(p_readCondtion->l_instanceStateMask & p_instanceset->instance_state) ){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}


	if (p_datareader->i_query_conditions)
	{
		for (i = 0; i < p_datareader->i_query_conditions ; i++) {

			p_queryCondition = p_datareader->pp_query_conditions[i];

			if((p_queryCondition->l_sampleStateMask & p_instanceset->sample_state)&&(p_queryCondition->l_viewStateMask & p_instanceset->view_state)&&(p_queryCondition->l_instanceStateMask & p_instanceset->instance_state) ){

				if(p_queryCondition->p_waitset){

					expression_t result = expression_caculate_for_message(p_queryCondition->p_expr, p_queryCondition->p_typeSupport, (char*)p_message->v_data, p_message->i_datasize, p_queryCondition->query_parameters);
					if(result.kind == BOOLEAN_KIND)
					{
					//	printf("Result : %d\r\n", result.value.boolean);
					}else{
						printf("I can't defined result...");
						assert(false);
					}

					if( result.value.boolean == false)
					{
						
						continue;
					}

					p_queryCondition->b_trigger_value = true;
					cond_signal(&p_queryCondition->p_waitset->waitset_wait);


				}
			}
		}
	}


	return RETCODE_OK;
}


//readcondition by jun

ReturnCode_t Readcondition_SampleState_Set(DataReader* p_datareader , SampleStateMask kind )
{
	int i;
	ReadCondition * p_readCondtion;
	

	if (p_datareader->i_read_conditions)
	{
		for (i = 0; i < p_datareader->i_read_conditions ; i++) {

			p_readCondtion = p_datareader->pp_read_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	if (p_datareader->i_query_conditions)
	{
		for (i = 0; i < p_datareader->i_query_conditions ; i++) {

			p_readCondtion = (ReadCondition *)p_datareader->pp_query_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	return RETCODE_OK;
}

//readcondition by jun
ReturnCode_t Readcondition_ViewState_Set(DataReader* p_datareader , ViewStateMask kind )
{
	int i;
	ReadCondition * p_readCondtion;

	if (p_datareader->i_read_conditions)
	{
		for (i = 0; i < p_datareader->i_read_conditions ; i++) {

			p_readCondtion = p_datareader->pp_read_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	if (p_datareader->i_query_conditions)
	{
		for (i = 0; i < p_datareader->i_query_conditions ; i++) {

			p_readCondtion = (ReadCondition *)p_datareader->pp_query_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	return RETCODE_OK;
}

//readcondition by jun
ReturnCode_t Readcondition_InstanceState_Set(DataReader* p_datareader , InstanceStateMask kind )
{
	int i;
	ReadCondition * p_readCondtion;

	if (p_datareader->i_read_conditions)
	{
		for (i = 0; i < p_datareader->i_read_conditions ; i++) {

			p_readCondtion = p_datareader->pp_read_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	if (p_datareader->i_query_conditions)
	{
		for (i = 0; i < p_datareader->i_query_conditions ; i++) {

			p_readCondtion = (ReadCondition *)p_datareader->pp_query_conditions[i];

			if(p_readCondtion->l_sampleStateMask & kind){

				if(p_readCondtion->p_waitset){
					p_readCondtion->b_trigger_value = true;
					cond_signal(&p_readCondtion->p_waitset->waitset_wait);
				}
			}
		}
	}

	return RETCODE_OK;
}




