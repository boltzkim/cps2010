/*
	module에서 사용되는 configure에 관련되어 선언.
	작성자 : 
	이력
	2010-07-26 : 시작
*/

#if !defined(__MODULE_CONFIG_H)
#define __MODULE_CONFIG_H


enum CONFIG_TYPE
{
	CONFIG_HINT,
	CONFIG_HINT_END,
	CONFIG_ITEM_STRING,
	CONFIG_ITEM_MODULE,
	CONFIG_ITEM_INTEGER,
	CONFIG_ITEM_BOOL,
	CONFIG_ITEM_FLOAT,
	CONFIG_ITEM,
	CONFIG_CATEGORY
};

typedef int ( * module_callback_t ) ( module_object_t *,char const *,value_t, value_t,void * );

struct module_config_t
{
	int					i_type;
	char*				psz_name;
	int					i_value;
	float				f_value;
	char*				psz_value;
	int					i_action;
	module_callback_t*	ppf_action;
    char**				ppsz_action_text;
	mutex_t*			p_lock;

	module_callback_t	pf_callback;
	char*				psz_text;
    char*				psz_longtext;
	bool				b_advanced;
};


#define add_config_inner( ) \
    i_config++; \
    if( (i_config % 10) == 0 ) \
        p_config = (module_config_t *) \
             realloc(p_config, (i_config+11) * sizeof(module_config_t)); \
    memset( p_config + i_config, 0, sizeof( *p_config ) )

#define add_type_inner( type ) \
    add_config_inner( ); \
    p_config[i_config].i_type = type

#define add_typedesc_inner( type, text, longtext ) \
    add_type_inner( type ); \
    p_config[i_config].psz_text = text; \
    p_config[i_config].psz_longtext = longtext

#define add_typeadv_inner( type, text, longtext, advc ) \
    add_typedesc_inner( type, text, longtext ); \
    p_config[i_config].b_advanced = advc

#define add_typename_inner( type, name, text, longtext, advc, cb ) \
    add_typeadv_inner( type, text, longtext, advc ); \
    p_config[i_config].psz_name = name; \
    p_config[i_config].pf_callback = cb

#define add_int_inner( type, name, text, longtext, advc, cb, value ) \
    add_typename_inner( type, name, text, longtext, advc, cb ); \
    p_config[i_config].i_value = value

#define add_string_inner( type, name, text, longtext, advc, cb, value ) \
    add_typename_inner( type, name, text, longtext, advc, cb ); \
    p_config[i_config].psz_value = value


#define add_bool( name, value, p_callback, text, longtext, advc ) \
    add_typename_inner( CONFIG_ITEM_BOOL, name, text, longtext, advc, p_callback ); \
    p_config[i_config].i_value = value


#define add_integer( name, value, p_callback, text, longtext, advc ) \
    add_int_inner( CONFIG_ITEM_INTEGER, name, text, longtext, advc, p_callback, value )

#define add_string( name, value, p_callback, text, longtext, advc ) \
    add_string_inner( CONFIG_ITEM_STRING, name, text, longtext, advc, p_callback, value )



#define set_category( i_id ) \
    add_type_inner( CONFIG_CATEGORY ); \
    p_config[i_config].i_value = i_id

#define set_shortname( desc )                                                 \
    p_submodule->psz_shortname = desc

#define set_description( desc )                                               \
    p_submodule->psz_longname = desc

#define set_capability( cap, score )                                          \
    p_submodule->psz_capability = cap;                                        \
    p_submodule->i_score = score

#define set_callbacks( activate, deactivate )                                 \
    p_submodule->pf_activate = activate;                                      \
    p_submodule->pf_deactivate = deactivate

#define add_shortcut( shortcut )                                              \
    p_submodule->pp_shortcuts[ i_shortcut ] = shortcut;                       \
    i_shortcut++



#endif