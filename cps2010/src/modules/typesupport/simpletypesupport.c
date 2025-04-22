/*
	TypeSupport를 하기 위한 모듈로서 일단 단순하게 관리하도록 한다.

	작성자 : 
	이력
	2010-08-7
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

static int  Open  ( module_object_t * );
static void Close ( module_object_t * );

static int simple_registerType(typesupport_t *p_typesupport, FooTypeSupport *p_fooTypeSupport, DomainParticipant *p_participant, char *type_name);
static char*  simple_getTypeName(typesupport_t *p_typesupport, FooTypeSupport *p_fooTypeSupport);
static FooTypeSupport *simple_domainParticipantFindSupportType(typesupport_t *p_typesupport, DomainParticipant *p_participant, string type_name);
static FooTypeSupport *simple_FindSupportType(typesupport_t *p_typesupport, string type_name);

module_define_start(simpletypesupport)
	set_category( CAT_TYPESUPPORT );
	set_description( "Simple TypeSupport" );
	set_capability( "typesupport", 200 );
	set_callbacks( Open, Close );
	add_shortcut( "simpletypesupport" );
module_define_end()

static int  Open(module_object_t *p_this)
{
	typesupport_t *p_typesupport = (typesupport_t*)p_this;

	p_typesupport->register_type = simple_registerType;
	p_typesupport->get_type_name = simple_getTypeName;
	p_typesupport->domain_participant_find_support_type = simple_domainParticipantFindSupportType;
	p_typesupport->get_type_support = simple_FindSupportType;

	p_typesupport->i_type_count = 0;
	p_typesupport->pp_foo_type_supports = NULL;

	p_typesupport->i_typename_count = 0;
	p_typesupport->pp_typename = NULL;

	p_typesupport->i_domain_participants = 0;
	p_typesupport->pp_domain_participants = NULL;

	return MODULE_SUCCESS;
}

static void Close(module_object_t *p_this)
{
	typesupport_t *p_typesupport = (typesupport_t*)p_this;

	while(p_typesupport->i_typename_count)
	{
//		FREE(p_typesupport->pp_typename[0]);
		REMOVE_ELEM( p_typesupport->pp_typename, p_typesupport->i_typename_count, 0);
	}

	FREE(p_typesupport->pp_typename);

	while(p_typesupport->i_type_count)
	{
//		FREE(p_typesupport->pp_foo_type_supports[0]);
		REMOVE_ELEM( p_typesupport->pp_foo_type_supports, p_typesupport->i_type_count, 0);
	}

	FREE(p_typesupport->pp_foo_type_supports);

	while(p_typesupport->i_domain_participants)
	{
//		FREE(p_typesupport->pp_domain_participants[0]);
		REMOVE_ELEM( p_typesupport->pp_domain_participants, p_typesupport->i_domain_participants, 0);
	}

	FREE(p_typesupport->pp_domain_participants);
}

static int simple_registerType(typesupport_t *p_typesupport, FooTypeSupport *p_fooTypeSupport, DomainParticipant *p_participant, char *type_name)
{
	FooTypeSupport* p_finded_foo_typesupport;

	mutex_lock(&p_typesupport->object_lock);

	p_finded_foo_typesupport = simple_domainParticipantFindSupportType(p_typesupport, p_participant, type_name);

	if (!p_finded_foo_typesupport)
	{
		INSERT_ELEM(p_typesupport->pp_typename, p_typesupport->i_typename_count, p_typesupport->i_typename_count, strdup(type_name));
		INSERT_ELEM(p_typesupport->pp_foo_type_supports, p_typesupport->i_type_count, p_typesupport->i_type_count, p_fooTypeSupport);
		INSERT_ELEM(p_typesupport->pp_domain_participants, p_typesupport->i_domain_participants, p_typesupport->i_domain_participants, p_participant);
	}

	mutex_unlock(&p_typesupport->object_lock);

	return (!p_finded_foo_typesupport) ? RETCODE_OK : RETCODE_ERROR;
}

static char*  simple_getTypeName(typesupport_t *p_typesupport, FooTypeSupport *p_fooTypeSupport)
{
	bool b_finded = false;
	int i;

	mutex_lock(&p_typesupport->object_lock);

	for(i=0 ;i < p_typesupport->i_type_count ;i++)
	{
		if(p_typesupport->pp_foo_type_supports[i], p_fooTypeSupport)
		{
			b_finded = true;
			break;
		}
	}

	if(!b_finded)
	{
		return p_typesupport->pp_typename[i];
	}

	mutex_unlock(&p_typesupport->object_lock);

	return NULL;
}

static FooTypeSupport *simple_domainParticipantFindSupportType(typesupport_t *p_typesupport, DomainParticipant *p_participant, string type_name)
{
	int i;

	//printf("simple_domainParticipantFindSupportType(%d)\r\n", p_typesupport->i_typename_count);

	for(i=0 ;i < p_typesupport->i_typename_count ;i++)
	{
		//printf("simple_domainParticipantFindSupportType(%p,%p)\r\n", p_typesupport->pp_domain_participants[i], p_participant);
		//printf("simple_domainParticipantFindSupportType(%s,%s)\r\n", p_typesupport->pp_typename[i], type_name);

		if(!strcmp(p_typesupport->pp_typename[i], type_name) && p_typesupport->pp_domain_participants[i] == p_participant)
		{
			return p_typesupport->pp_foo_type_supports[i];
		}
	}

	return NULL;
}



static FooTypeSupport *simple_FindSupportType(typesupport_t *p_typesupport, string type_name)
{
	int i;

	for(i=0 ;i < p_typesupport->i_typename_count ;i++)
	{
		if(!strcmp(p_typesupport->pp_typename[i], type_name))
		{
			return p_typesupport->pp_foo_type_supports[i];
		}
	}

	return NULL;
}