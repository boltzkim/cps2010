/*
	서비스 관련 구현 부분
	
	작성자 : 
	이력
	2010-07-28 : 시작
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static service_t *p_static_service = NULL;


int launch_service_module( module_object_t* p_this )
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	service_t *p_service = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "service", p_parser->psz_capability )) )
        {
            continue;
        }

		p_service = object_create( p_this ,OBJECT_SERVICE);

		if(p_service && (p_parser->pf_activate(OBJECT(p_service)) == MODULE_SUCCESS))
		{
			p_service->p_moduleload = p_parser;
			object_attach( OBJECT(p_service), p_this );	
			p_static_service = p_service;
		}else{
			object_destroy(OBJECT(p_service));
			p_static_service = NULL;
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}



void add_entity_to_service( Entity* p_entity )
{

	if(p_static_service)
	{
		p_static_service->add_entity(OBJECT(p_static_service), p_entity);
	}else{


		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		//DomainParticipant의 module_id 같을 경우
		if(p_module && p_entity){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->add_entity(OBJECT(p_service), p_entity);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}



}

void remove_entity_to_service( Entity* p_entity )
{
	if(p_static_service)
	{
		p_static_service->remove_entity(OBJECT(p_static_service), p_entity);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_entity){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->remove_entity(OBJECT(p_service), p_entity);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}


void write_to_service( rtps_writer_t* p_rtps_writer, message_t* p_message )
{

	if(p_static_service)
	{
		p_static_service->write(OBJECT(p_static_service), p_rtps_writer, p_message);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_rtps_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->write(OBJECT(p_service), p_rtps_writer, p_message);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}


void dispose_to_service( rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle )
{

	if(p_static_service)
	{
		p_static_service->dispose(OBJECT(p_static_service), p_rtps_writer, v_data, handle);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_rtps_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->dispose(OBJECT(p_service), p_rtps_writer,v_data, handle);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}

void unregister_to_service( rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, bool b_dispose )
{

	if(p_static_service)
	{
		p_static_service->unregister(OBJECT(p_static_service), p_rtps_writer,v_data, handle, b_dispose);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_rtps_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->unregister(OBJECT(p_service), p_rtps_writer,v_data, handle, b_dispose);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}



message_t** message_read_from_service( DataReader* p_datareader, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states )
{

	message_t **pp_message = NULL;

	if(p_static_service)
	{
		pp_message =  p_static_service->read(OBJECT(p_static_service), p_datareader,max_samples ,p_count, sample_states, view_states, instance_states);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());
		

		if(p_module && p_datareader){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				pp_message = p_service->read(OBJECT(p_service), p_datareader,max_samples ,p_count, sample_states, view_states, instance_states);
				break;
			}

			if(p_list)moduleListRelease( p_list );
		}

	}
	return pp_message;
}


message_t** message_take_from_service( DataReader* p_datareader, int32_t max_samples, int* p_count, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states )
{

	message_t **pp_message = NULL;

	if(p_static_service)
	{
		pp_message = p_static_service->take(OBJECT(p_static_service), p_datareader,max_samples, p_count, sample_states, view_states, instance_states);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());
		

		if(p_module && p_datareader){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				pp_message = p_service->take(OBJECT(p_service), p_datareader,max_samples, p_count, sample_states, view_states, instance_states);
				break;
			}

			if(p_list)moduleListRelease( p_list );
		}

	}

	return pp_message;
}


message_t** message_read_instance_from_service( DataReader* p_datareader, InstanceHandle_t handle, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states )
{

	message_t **pp_message = NULL;

	if(p_static_service)
	{
		pp_message = p_static_service->read_instance(OBJECT(p_static_service), p_datareader,handle, max_samples ,p_count, sample_states, view_states, instance_states);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());
		

		if(p_module && p_datareader){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				pp_message = p_service->read_instance(OBJECT(p_service), p_datareader,handle, max_samples ,p_count, sample_states, view_states, instance_states);
				break;
			}

			if(p_list)moduleListRelease( p_list );
		}
	}

	return pp_message;
}


message_t** message_take_instance_from_service( DataReader* p_datareader, InstanceHandle_t handle, int32_t max_samples, int* p_count , SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states)
{

	message_t **pp_message = NULL;

	if(p_static_service)
	{
		pp_message =  p_static_service->take_instance(OBJECT(p_static_service), p_datareader,handle, max_samples, p_count, sample_states, view_states, instance_states);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_datareader){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				pp_message = p_service->take_instance(OBJECT(p_service), p_datareader,handle, max_samples, p_count, sample_states, view_states, instance_states);
				break;
			}

			if(p_list)moduleListRelease( p_list );
		}
	}

	return pp_message;
}

void add_thread_to_service( Entity* p_entity )
{

	if(p_static_service)
	{
		p_static_service->add_thread(OBJECT(p_static_service), p_entity);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_entity){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->add_thread(OBJECT(p_service), p_entity);
			}

			if(p_list)moduleListRelease( p_list );
		}

	}
}



//timestamp by jun
void writeToServiceTimestamp(rtps_writer_t *p_writer, message_t *p_message, Time_t source_timestamp)
{

	if(p_static_service)
	{
		p_static_service->write_timestamp(OBJECT(p_static_service), p_writer, p_message, source_timestamp);
	}else{
		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->write_timestamp(OBJECT(p_service), p_writer, p_message, source_timestamp);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}

//timestamp by jun
void unregister_to_service_timestamp( rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, Time_t source_timestamp )
{

	if(p_static_service)
	{
		p_static_service->unregister_timestamp(OBJECT(p_static_service), p_rtps_writer,v_data, handle, source_timestamp);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_rtps_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->unregister_timestamp(OBJECT(p_service), p_rtps_writer,v_data, handle, source_timestamp);
			}

			if(p_list)moduleListRelease( p_list );
		}
	}
}



//timestamp by jun
void dispose_to_service_timestamp( rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, Time_t source_timestamp )
{

	if(p_static_service)
	{
		p_static_service->dispose_timestamp(OBJECT(p_static_service), p_rtps_writer,v_data, handle, source_timestamp);
	}else{

		module_list_t *p_list = NULL;
		service_t *p_service = NULL;
		int i_index = -1;
		module_t *p_module = current_object(get_domain_participant_factory_module_id());

		if(p_module && p_rtps_writer){
			p_list = moduleListFind(OBJECT(p_module), OBJECT_SERVICE, FIND_CHILD);

			for( i_index = 0; i_index < p_list->i_count; i_index++ )
			{
				p_service = (service_t *)p_list->p_values[i_index].p_object;
				p_service->dispose_timestamp(OBJECT(p_service), p_rtps_writer,v_data, handle, source_timestamp);
			}

			if(p_list)moduleListRelease( p_list );
		}

	}
}

