/*
	TYPESUPPORT 관련 헤더
	작성자 : 
	이력
	2010-08-07 : 시작

*/

#if !defined(__TYPESUPPORT_H)
#define __TYPESUPPORT_H
#ifndef FOOTYPESUPPORT_DEFINE
        #define FOOTYPESUPPORT_DEFINE 1
        typedef struct FooTypeSupport FooTypeSupport;
#endif

#ifndef DOMAINPARTICIPANT_DEFINE
        #define DOMAINPARTICIPANT_DEFINE 1
        typedef struct DomainParticipant DomainParticipant;
#endif

struct typesupport_t
{
    MODULE_COMMON_FIELD
	moduleload_t* p_moduleload;

	int (*register_type)(typesupport_t *p_typesupport, FooTypeSupport* p_foo_type_support, DomainParticipant* p_participant, char* p_type_name);
	char* (*get_type_name)(typesupport_t* p_typesupport, FooTypeSupport* p_foo_type_support);
	FooTypeSupport* (*domain_participant_find_support_type)(typesupport_t* p_typesupport, DomainParticipant* p_participant, char* p_type_name);
	FooTypeSupport* (*get_type_support)(typesupport_t* p_typesupport, char* p_type_name);

	int i_typename_count;
	char** pp_typename;
	int i_type_count;
	FooTypeSupport** pp_foo_type_supports;
	int i_domain_participants;
	DomainParticipant** pp_domain_participants;
};

#ifdef __cplusplus
extern "C" {
#endif

	
	int launch_type_support_module(module_object_t* p_this);
	


#ifdef __cplusplus
}
#endif


#endif
