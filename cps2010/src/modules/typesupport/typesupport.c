/*
	typesupport 관련 구현 부분
	
	작성자 : 
	이력
	2010-08-5
*/

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


static typesupport_t *p_static_typesupport = NULL;

int launch_type_support_module( module_object_t* p_this )
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	typesupport_t *p_typesupport = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "typesupport", p_parser->psz_capability )) )
        {
            continue;
        }

		p_typesupport = object_create( p_this ,OBJECT_TYPESUPPORT);

		if(p_typesupport && (p_parser->pf_activate(OBJECT(p_typesupport)) == MODULE_SUCCESS))
		{
			p_typesupport->p_moduleload = p_parser;
			object_attach( OBJECT(p_typesupport), p_this );
			p_static_typesupport = p_typesupport;
		}else{
			object_destroy(OBJECT(p_typesupport));
			p_static_typesupport = NULL;
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}



void register_type(module_object_t* p_this, FooTypeSupport* p_foo_type_support, in_dds DomainParticipant* p_participant, in_dds string type_name)
{
	module_list_t *p_list = NULL;
	typesupport_t *p_typesupport = NULL;
	int i_index = -1;

	if(p_static_typesupport)
	{
		p_static_typesupport->register_type(p_static_typesupport, p_foo_type_support, p_participant, type_name);
	}else{

		p_list = moduleListFind(OBJECT(p_this), OBJECT_TYPESUPPORT, FIND_ANYWHERE);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_typesupport = (typesupport_t *)p_list->p_values[i_index].p_object;
			p_typesupport->register_type(p_typesupport, p_foo_type_support, p_participant, type_name);

		}

		if(p_list)moduleListRelease( p_list );
	}
}

string get_type_name(module_object_t* p_this, FooTypeSupport* p_foo_type_support)
{
	module_list_t *p_list = NULL;
	typesupport_t *p_typesupport = NULL;
	int i_index = -1;

	if(p_static_typesupport)
	{
		return p_static_typesupport->get_type_name(p_static_typesupport, p_foo_type_support);
	}else{

		p_list = moduleListFind(OBJECT(p_this), OBJECT_TYPESUPPORT, FIND_ANYWHERE);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_typesupport = (typesupport_t *)p_list->p_values[i_index].p_object;
			if(p_list)moduleListRelease( p_list );
			return p_typesupport->get_type_name(p_typesupport, p_foo_type_support);

		}

		if(p_list)moduleListRelease( p_list );
	}

	return NULL;
}

FooTypeSupport* domain_participant_find_support_type(module_object_t* p_this, DomainParticipant* p_participant, string type_name)
{
	module_list_t *p_list = NULL;
	typesupport_t *p_typesupport = NULL;
	int i_index = -1;

	if(p_static_typesupport)
	{
		return p_static_typesupport->domain_participant_find_support_type(p_static_typesupport, p_participant, type_name);
	}else{

		p_list = moduleListFind(OBJECT(p_this), OBJECT_TYPESUPPORT, FIND_ANYWHERE);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_typesupport = (typesupport_t *)p_list->p_values[i_index].p_object;
			if(p_list)moduleListRelease( p_list );
			return p_typesupport->domain_participant_find_support_type(p_typesupport, p_participant, type_name);

		}

		if(p_list)moduleListRelease( p_list );
	}

	return NULL;
}


FooTypeSupport* get_type_support(string type_name)
{
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	module_list_t *p_list = NULL;
	typesupport_t *p_typesupport = NULL;
	int i_index = -1;

	if(p_static_typesupport)
	{
		return p_static_typesupport->get_type_support (p_static_typesupport, type_name);
	}else{


		p_list = moduleListFind(OBJECT(p_module), OBJECT_TYPESUPPORT, FIND_ANYWHERE);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_typesupport = (typesupport_t *)p_list->p_values[i_index].p_object;
			if(p_list)moduleListRelease( p_list );
			return p_typesupport->get_type_support (p_typesupport, type_name);

		}

		if(p_list)moduleListRelease( p_list );
	}

	return NULL;

}