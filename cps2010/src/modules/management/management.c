/*
	Management 관련 구현 부분
	
	작성자 : 
	이력
	2010-08-8
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


int launch_management_module( module_object_t* p_this )
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	management_t *p_management = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "management", p_parser->psz_capability )) )
        {
            continue;
        }

		p_management = object_create( p_this ,OBJECT_MANAGEMENT);

		if(p_management && (p_parser->pf_activate(OBJECT(p_management)) == MODULE_SUCCESS))
		{
			p_management->p_moduleload = p_parser;
			object_attach( OBJECT(p_management), p_this );	
		}else{
			object_destroy(OBJECT(p_management));
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}


void message_write( message_t* p_message )
{
	module_list_t *p_list = NULL;
	management_t *p_management = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if(p_module && p_message){
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MANAGEMENT, FIND_CHILD);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_management = (management_t *)p_list->p_values[i_index].p_object;
			p_management->message_write(p_management, message_duplicate(p_message));
		}

		message_release(p_message);

		if(p_list)moduleListRelease( p_list );
	}
}



message_t** message_read( DataReader* p_datareader, int* p_count )
{

	module_list_t *p_list = NULL;
	management_t *p_management = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());
	message_t **pp_message = NULL;

	if(p_module && p_datareader){
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MANAGEMENT, FIND_CHILD);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_management = (management_t *)p_list->p_values[i_index].p_object;
			pp_message = p_management->message_read(p_management, p_datareader, p_count);
			break;
		}

		if(p_list)moduleListRelease( p_list );
	}

	return pp_message;
}
