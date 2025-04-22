/* 
	ReadCondition 관련 부분 구현
	작성자 : 
	이력
	2010-08-7 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static SampleStateMask get_sample_state_mask(ReadCondition* p_readcondition)
{
	return p_readcondition->l_sampleStateMask;
}

static ViewStateMask get_view_state_mask(ReadCondition* p_readcondition)
{
	return p_readcondition->l_viewStateMask;
}

static InstanceStateMask get_instance_state_mask(ReadCondition* p_readcondition)
{
	return p_readcondition->l_instanceStateMask;
}

static DataReader *get_datareader(ReadCondition* p_readcondition)
{
	return p_readcondition->p_datareader;
}


void init_read_condition(ReadCondition* p_read_condition)
{
	init_condition((Condition*)p_read_condition);

	p_read_condition->get_sample_state_mask = get_sample_state_mask;
	p_read_condition->get_view_state_mask = get_view_state_mask;
	p_read_condition->get_instance_state_mask = get_instance_state_mask;
	p_read_condition->get_datareader = get_datareader;
	//////////////

	p_read_condition->l_sampleStateMask = ANY_SAMPLE_STATE;
	p_read_condition->l_viewStateMask = ANY_VIEW_STATE;
	p_read_condition->l_instanceStateMask = ANY_INSTANCE_STATE;
	p_read_condition->p_datareader = NULL;

	p_read_condition->condition_type = READ_CONDITION;

}

void destroy_read_condition_except_read_condition(ReadCondition* p_read_condition)
{
	destroy_condition((Condition*)p_read_condition);
}

void destroy_read_condition(ReadCondition* p_read_condition)
{
	destroy_read_condition_except_read_condition(p_read_condition);

	FREE(p_read_condition);
}



void check_new_data_sample_status( Entity* p_entity, instanceset_t* p_instanceset, message_t* p_message )
{
	p_instanceset->sample_state = NOT_READ_SAMPLE_STATE;
	read_condition_set_from_add_instanceset((DataReader *)p_entity, p_instanceset, p_message);
}


void check_new_data_instance_stautus( Entity* p_entity, instanceset_t* p_instanceset, message_t* p_message )
{

	int i;
//instance
	if(p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE || p_instanceset->instance_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) {

		p_instanceset->instance_state = ALIVE_INSTANCE_STATE;
		
		for(i=0; i<p_instanceset->i_messages; i++){
		
			p_instanceset->pp_messages[i]->sampleInfo.instance_state = ALIVE_INSTANCE_STATE;
		}
		check_all_samples_sample_status(p_instanceset);
		read_condition_set_from_add_instanceset((DataReader *)p_entity, p_instanceset, p_message);
		
	} else if(p_instanceset->instance_state == ALIVE_INSTANCE_STATE){

		p_message->sampleInfo.instance_state = ALIVE_INSTANCE_STATE;

	}
}

void check_newData_viewStautus(Entity * p_entity, instanceset_t * p_instanceset, message_t *p_message)
{
//view
	int i;

	if(p_instanceset->view_state == NEW_VIEW_STATE){
		//NEW
		p_message->sampleInfo.view_state = p_instanceset->view_state;
	}
	if(p_instanceset->view_state == NOT_NEW_VIEW_STATE){
	
		if(p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE || p_instanceset->instance_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE){
			//NOT ALIVE -> NEW DATA RECEIVED
			p_instanceset->view_state = NEW_VIEW_STATE;
	
			for(i=0; i<p_instanceset->i_messages; i++){
				p_instanceset->pp_messages[i]->sampleInfo.view_state = NEW_VIEW_STATE;
			}
			check_all_samples_sample_status(p_instanceset);
			read_condition_set_from_add_instanceset((DataReader *)p_entity, p_instanceset, p_message);
			
		}else {//NOT_NEW
			p_message->sampleInfo.view_state = p_instanceset->view_state;
		
		}
	}
}

void check_instanceStatus_disposed(Entity *p_entity, instanceset_t *p_instanceset)
{
	int i;
//TERMINATION OF INSTANCE
	if(p_instanceset->instance_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE){  //

		if( p_instanceset->i_messages == 0){
			instanceset_remove(p_entity, p_instanceset);

		}
	}
	else if(p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE){
		if( p_instanceset->i_messages == 0 && p_instanceset->i_key_guid == 0){
			instanceset_remove(p_entity, p_instanceset);
		}
	
	} else if(p_instanceset->instance_state == ALIVE_INSTANCE_STATE){
		p_instanceset->instance_state = NOT_ALIVE_DISPOSED_INSTANCE_STATE;
		//DELETE INSTANCE
		if(p_instanceset->i_messages == 0 && p_instanceset->i_key_guid == 0){
			instanceset_remove(p_entity ,p_instanceset);
		} else {
			for(i=0; i<p_instanceset->i_messages; i++){
	
				p_instanceset->pp_messages[i]->sampleInfo.instance_state = p_instanceset->instance_state;
			}
			check_all_samples_sample_status(p_instanceset);
			read_condition_set((DataReader *)p_entity, p_instanceset);
		}
	}
}

void check_instanceStatus_unregister(Entity * p_entity, instanceset_t * p_instanceset)
{
	int i;

	printf("check_instanceStatus_unregister..\r\n");
	if(p_instanceset->i_key_guid == 0){
		
		if(p_instanceset->view_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE){
			if(p_instanceset->i_messages == 0){
				instanceset_remove(p_entity, p_instanceset);
			}
		} else	if(p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE){
			if(p_instanceset->i_messages == 0 && p_instanceset->i_key_guid == 0){
				instanceset_remove(p_entity, p_instanceset);
			}
			
		}else if(p_instanceset->instance_state == ALIVE_INSTANCE_STATE){
			p_instanceset->instance_state = NOT_ALIVE_NO_WRITERS_INSTANCE_STATE;

			if(p_instanceset->i_messages == 0) {
				instanceset_remove(p_entity, p_instanceset);

			}else {
				for(i=0; i<p_instanceset->i_messages; i++){
		
					p_instanceset->pp_messages[i]->sampleInfo.instance_state = NOT_ALIVE_NO_WRITERS_INSTANCE_STATE;
				}
				check_all_samples_sample_status(p_instanceset);
				read_condition_set((DataReader *)p_entity, p_instanceset);
				
			}
		}
	}
}

void read_instance_change_sample_status( DataReader* p_datareader, instanceset_t* p_instanceset )
{	

	p_instanceset->sample_state = READ_SAMPLE_STATE;

	read_condition_set(p_datareader, p_instanceset);
}



void read_instance_change_instance_status( DataReader* p_datareader, instanceset_t* p_instanceset )
{	
	int j;
	if(p_instanceset->view_state == NEW_VIEW_STATE){

		p_instanceset->view_state = NOT_NEW_VIEW_STATE;
		//ALL DATA NOT NEW
		for(j=0; j<p_instanceset->i_messages; j++){
			p_instanceset->pp_messages[j]->sampleInfo.view_state = NOT_NEW_VIEW_STATE;
		}
		check_all_samples_sample_status(p_instanceset);
		read_condition_set(p_datareader, p_instanceset);
	}
}




void read_change_sample_status( DataReader* p_datareader, int i )
{	

	instanceset_t *p_instanceset;
	p_instanceset = (instanceset_t*)p_datareader->pp_message_order[i]->handle;
	p_instanceset->sample_state = READ_SAMPLE_STATE;
	read_condition_set(p_datareader, p_instanceset);
}
 


void read_change_instance_status( DataReader* p_datareader, int i )
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
}

void take_remove_instance(DataReader * p_datareader, instanceset_t * p_instanceset)
{
	if(p_instanceset->instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE && p_instanceset->i_messages == 0 && p_instanceset->i_key_guid == 0)
		instanceset_remove((Entity *)p_datareader,p_instanceset);
	else if(p_instanceset->instance_state == NOT_ALIVE_NO_WRITERS_INSTANCE_STATE && p_instanceset->i_messages == 0 )
		instanceset_remove((Entity *)p_datareader,p_instanceset);		
}

void take_instance_change_instance_status( DataReader* p_datareader, instanceset_t* p_instanceset )
{
	int j;
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


void take_change_instance_status( DataReader* p_datareader, int i )
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
