/*
	디스커버리 관련 구현 부분
	
	작성자 : 
	이력
	2010-09-27 : 시작
*/


#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



int launch_discovery_module( module_object_t* p_this )
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	discovery_t *p_discovery = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "discovery", p_parser->psz_capability )) )
        {
            continue;
        }

		p_discovery = object_create( p_this ,OBJECT_DISCOVERY);

		if(p_discovery && (p_parser->pf_activate(OBJECT(p_discovery)) == MODULE_SUCCESS))
		{
			p_discovery->p_moduleload = p_parser;
			object_attach( OBJECT(p_discovery), p_this );	
		}else{
			object_destroy(OBJECT(p_discovery));
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}