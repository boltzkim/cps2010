/*
	모듈 오브젝트를 위해 정의되는 부분

	작성자 : 
	이력
	2010-07-22DomainParticipant
*/

#if !defined(__MODULE_COMMON_H)
#define __MODULE_COMMON_H

enum MODULE_RETURN_TYPE
{
	MODULE_FAIL=-1,
	MODULE_SUCCESS,
	MODULE_ERROR_NORMAL,
	MODULE_ERROR_CREATE,
	MODULE_ERROR_THREAD,
	MODULE_ERROR_BADVAR,
	MODULE_ERROR_TIMEOUT,
	MODULE_ERROR_MEMORY,
	MODULE_ERROR_NETWORK,
	MODULE_ERROR_UNKNOWN_ENTITY,
	MODULE_ERROR_RTPS
};

enum OBJECT_TYPE
{
	OBJECT_GENERIC,
	OBJECT_ROOT,
	OBJECT_MODULE,
	OBJECT_MODULE_LOAD,
	OBJECT_TRACE,
	OBJECT_SERVICE,
	OBJECT_TYPESUPPORT,
	OBJECT_MANAGEMENT,
	OBJECT_ACCESS,
	OBJECT_DISCOVERY,
	OBJECT_ACCESS_OUT,
	OBJECT_MONITORING,
	OBJECT_QOS_POLICY//by kki (qos module)
};

enum CATEGORY_TYPE
{
	CAT_TRACE,
	CAT_SERVICE,
	CAT_TYPESUPPORT,
	CAT_MANAGEMENT,
	CAT_INPUT,
	CAT_DISCOVERY,
	CAT_OUT,
	CAT_MONITORING,
	CAT_QOS_POLICY//by kki (qos module)
};

enum MODULE_FIND_TYPE
{
	FIND_PARENT=1,
	FIND_CHILD,
	FIND_ANYWHERE
};


#define MODULE_COMMON_FIELD							\
	int							i_object_id;		\
	int							i_object_type;		\
	char*						psz_object_type;	\
	const char*					psz_object_name;    \
	libmodule_t*				p_libmodule;		\
	module_t*					p_module;			\
	module_object_t*			p_parent;           \
    module_object_t**			pp_children;        \
	volatile int				i_children;         \
	volatile int				i_refcount;			\
	volatile bool				b_attached;			\
	volatile bool				b_force;			\
	module_thread_t				thread_id;          \
	mutex_t						object_lock;        \
    cond_t						object_wait;		\
	mutex_t						var_lock;           \
    int							i_vars;             \
    variable_t*					p_vars;				\
	bool						b_thread;			\
	void*						v_private;          \
	bool						b_end;				\




#define OBJECT( x ) ((module_object_t *)(x))

struct module_object_t
{
    MODULE_COMMON_FIELD
};


struct libmodule_t
{
    MODULE_COMMON_FIELD

	bool				b_ready;
	int					i_counter;
	int					i_objects;
    module_object_t**	pp_objects;

	module_bank_t*		p_module_bank;
};

//////////////////////////////////////////////////////////////////////////////////////////

#if defined( _MSC_VER ) && _MSC_VER < 1300 && !defined( UNDER_CE )
#   define MEDIACVP (void**)
#else
#   define MEDIACVP 
#endif


#if defined( _MSC_VER ) && _MSC_VER < 1300 && !defined( UNDER_CE )
#   define PARAMCVP 
#else
#   define PARAMCVP (dds_parameter_t**)
#endif

#define INSERT_PARAM( p_ar, i_oldsize, i_pos, elem )                           \
    do                                                                        \
    {                                                                         \
        if( !i_oldsize ) (p_ar) = NULL;                                       \
        (p_ar) = PARAMCVP realloc( p_ar, ((i_oldsize) + 1) * sizeof(*(p_ar)) ); \
        if( (i_oldsize) - (i_pos) )                                           \
        {                                                                     \
            memmove( (p_ar) + (i_pos) + 1, (p_ar) + (i_pos),                  \
                     ((i_oldsize) - (i_pos)) * sizeof( *(p_ar) ) );           \
        }                                                                     \
        (p_ar)[i_pos] = elem;                                                 \
        (i_oldsize)++;                                                        \
    }                                                                         \
    while( 0 )


#define INSERT_ELEM( p_ar, i_oldsize, i_pos, elem )                           \
    do                                                                        \
    {                                                                         \
        if( !i_oldsize ) (p_ar) = NULL;                                       \
        (p_ar) = MEDIACVP realloc( p_ar, ((i_oldsize) + 1) * sizeof(*(p_ar)) ); \
        if( (i_oldsize) - (i_pos) )                                           \
        {                                                                     \
            memmove( (p_ar) + (i_pos) + 1, (p_ar) + (i_pos),                  \
                     ((i_oldsize) - (i_pos)) * sizeof( *(p_ar) ) );           \
        }                                                                     \
        (p_ar)[i_pos] = elem;                                                 \
        (i_oldsize)++;                                                        \
    }                                                                         \
    while( 0 )

#define REMOVE_ELEM( p_ar, i_oldsize, i_pos )                                 \
    do                                                                        \
    {                                                                         \
        if( (i_oldsize) - (i_pos) - 1 )                                       \
        {                                                                     \
            memmove( (p_ar) + (i_pos),                                        \
                     (p_ar) + (i_pos) + 1,                                    \
                     ((i_oldsize) - (i_pos) - 1) * sizeof( *(p_ar) ) );       \
        }                                                                     \
        if( i_oldsize > 1 )                                                   \
        {                                                                     \
            (p_ar) = MEDIACVP realloc( p_ar, ((i_oldsize) - 1) * sizeof( *(p_ar) ) );  \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            FREE( p_ar );                                                     \
            (p_ar) = NULL;                                                    \
        }                                                                     \
        (i_oldsize)--;                                                        \
    }                                                                         \
    while( 0 )

////////////////////////////////////////////////////////////////////////////////////////

#define _LINKED_LIST_HEAD \
	linked_list_atom_t *p_head_first; \
	linked_list_atom_t *p_head_last; \
	int		i_linked_size;

#define _LINKED_LIST_ATOM \
	linked_list_atom_t	*p_next; \
	linked_list_atom_t	*p_prev; \
	linked_list_atom_t	*p_extra;

typedef struct linked_list_atom_t linked_list_atom_t;

struct linked_list_atom_t
{
	_LINKED_LIST_ATOM
};

typedef struct linked_list_head_t
{
	_LINKED_LIST_HEAD
}linked_list_head_t;




static inline void insert_linked_list(linked_list_head_t *p_list_head, linked_list_atom_t *p_atom)
{

	if(p_list_head->p_head_first == NULL)
	{
		p_list_head->p_head_first = p_atom;
	}

	if(p_list_head->p_head_last == NULL)
	{
		p_list_head->p_head_last = p_atom;
	}else
	{
		p_list_head->p_head_last->p_next = p_atom;
		p_atom->p_prev = p_list_head->p_head_last;
		p_list_head->p_head_last = p_atom;
	}

	p_list_head->i_linked_size++;

}

static inline void remove_linked_list(linked_list_head_t *p_list_head, linked_list_atom_t *p_atom)
{

	linked_list_atom_t *p_atom_at=NULL;

	if(p_atom == NULL) return;

	if(p_list_head->p_head_first == p_atom)
	{
		p_list_head->p_head_first = p_list_head->p_head_first->p_next;
		if(p_list_head->p_head_first) p_list_head->p_head_first->p_prev = NULL;
	}

	if(p_list_head->p_head_last == p_atom)
	{
		p_list_head->p_head_last = p_list_head->p_head_last->p_prev;
		if(p_list_head->p_head_last)  p_list_head->p_head_last->p_next = NULL;
	}


	if(p_atom->p_prev)
	{
		p_atom->p_prev->p_next = p_atom->p_next;
	}

	if(p_atom->p_next)
	{
		p_atom->p_next->p_prev = p_atom->p_prev;
	}

	p_list_head->i_linked_size--;

	

}


static inline void insert_linked_list_for_extra_change(linked_list_head_t *p_list_head, linked_list_atom_t *p_atom)
{

	if(p_list_head->p_head_first == NULL)
	{
		p_list_head->p_head_first = p_atom;
	}

	if(p_list_head->p_head_last == NULL)
	{
		p_list_head->p_head_last = p_atom;
	}else
	{
		p_list_head->p_head_last->p_extra = p_atom;
		p_list_head->p_head_last = p_atom;
	}

	p_list_head->i_linked_size++;

}


////////////////////////////////////////////////////////////////////////////////////////

#define MODULE_SHORTCUT_MAX 50

#define STRINGIFY( z )   UGLY_KLUDGE( z )
#define UGLY_KLUDGE( z ) #z
#define MODULE_STRING(name) #name

#if defined( __cplusplus )
#   define EXTERN_SYMBOL           extern "C"
#else
#   define EXTERN_SYMBOL
#endif

struct module_bank_t
{
    MODULE_COMMON_FIELD

    int        i_usage;

    bool       b_main;
    bool       b_builtins;
    bool       b_plugins;
};

struct moduleload_t
{
	MODULE_COMMON_FIELD

	const char*	psz_shortname;
	const char*	psz_longname;
	const char*	psz_capability;
	int		i_score;

	const char*	pp_shortcuts[ MODULE_SHORTCUT_MAX ];
	

	int  (*pf_activate)   ( module_object_t * );
	void (*pf_deactivate) ( module_object_t * );

	module_config_t* p_config;
	unsigned int     i_config_items;
	unsigned int     i_bool_items;
};

#ifndef TOPIC_DEFINE
        #define TOPIC_DEFINE 1
        typedef struct Topic Topic;
#endif

struct module_t
{
    MODULE_COMMON_FIELD

	//// DCPS 관련 변수 등록..
	int					i_domain_participants;
	DomainParticipant**	pp_domain_participants;
};


#define module_define_start(name)                                             \
	EXTERN_SYMBOL int	name##__module_entry ( moduleload_t *p_module )		  \
	{                                                                         \
		int i_shortcut = 1;													  \
		int i_config = -1;													  \
		module_config_t *p_config = NULL;                                     \
		static module_config_t config_init = {								  \
			CONFIG_HINT_END,NULL,0,0,NULL,0,NULL,NULL,NULL,					  \
			NULL,NULL,NULL,false											  \
		};																	  \
		p_module->psz_object_name = MODULE_STRING(name);                      \
		p_module->psz_shortname = NULL;                                       \
		p_module->psz_longname = MODULE_STRING(name);                         \
		p_module->psz_capability = "";                                        \
        p_module->i_score = 1;                                                \
		p_module->pp_shortcuts[ 0 ] = MODULE_STRING(name);                    \
        p_module->pf_activate = NULL;                                         \
        p_module->pf_deactivate = NULL;                                       \
		{                                                                     \
            moduleload_t *p_submodule = p_module;							  \
			p_module->pp_shortcuts[ i_shortcut ] = NULL;					  \

#define module_define_end( )                                                  \
		}                                                                     \
		if( p_config )                                                        \
        {																	  \
			int i;                                                            \
            p_config[ ++i_config ] = config_init;                             \
            config_dup( p_module, p_config );								  \
            for( i = 0; i < i_config; i++ )                                   \
            {                                                                 \
                if( p_config[ i ].i_action )                                  \
                {                                                             \
                    FREE( p_config[ i ].ppf_action );                         \
                    FREE( p_config[ i ].ppsz_action_text );                   \
                }                                                             \
            }                                                                 \
            FREE( p_config );                                                 \
		}else{																  \
			config_dup( p_module, &config_init );							  \
		}																	  \
        return MODULE_SUCCESS && i_shortcut;                                  \
    }


#ifdef _MSC_VER
	typedef unsigned char       uint8_t;
	typedef signed char         int8_t;
	typedef unsigned short      uint16_t;
	typedef signed short        int16_t;
	typedef unsigned int        uint32_t;
	typedef signed int          int32_t;
#endif

#ifdef HAVE_STDINT_H
#	include <stdint.h>
#else

#	ifdef _MSC_VER
		typedef unsigned __int64    uint64_t;
		typedef signed __int64      int64_t;
#	else
#	ifndef Android
		typedef unsigned long long  uint64_t;
		typedef signed long long    int64_t;
#	endif
#	endif

#	ifndef __APPLE__
#		ifndef _WIN64
		typedef uint32_t            uintptr_t;
		typedef int32_t             intptr_t;
#	endif
#	endif
#endif

typedef uint8_t             byte_t;

typedef int64_t mtime_t;

#define VAR_HASCHOICE 0x0100
#define VAR_HASMIN    0x0200
#define VAR_HASMAX    0x0400
#define VAR_HASSTEP   0x0800

#define VAR_ISCOMMAND 0x2000
#define VAR_DOINHERIT 0x8000

#define VAR_TYPE      0x00ff

enum VAR_LOOK_TYPE
{
	VAR_VOID,
	VAR_BOOL,
	VAR_INTEGER,
	VAR_STRING,
	VAR_FLOAT,
	VAR_MODULE,
	VAR_VARIABLE,
	VAR_TIME,
	VAR_ADDRESS,
	VAR_MUTEX
};


typedef union
{
    int					i_int;
    bool				b_bool;
    float				f_float;
    char *				psz_string;
    void *				p_address;
    module_object_t *	p_object;
    module_list_t *		p_list;
    mtime_t				i_time;
	//struct { int32_t x; int32_t y; } coords;

   // struct { char *psz_name; int i_object_id; } var;

   /* Make sure the structure is at least 64bits */
   // struct { char a, b, c, d, e, f, g, h; } padding;

} value_t;

struct module_list_t
{
    int         i_count;
    value_t*	p_values;
    int*		pi_types;
};


#ifndef MAXUINT32
	#define MAXUINT32   ((uint32_t)~((uint32_t)0))
	#define MAXINT32    ((int32_t)(MAXUINT32 >> 1))
	#define MININT32    ((int32_t)~MAXINT32)
#endif

////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

	void *object_create(module_object_t *p_this, int i_type);
	void object_destroy(module_object_t *p_this);
	void object_attach(module_object_t *p_this, module_object_t *p_parent);
	void object_detach(module_object_t *p_this);
	void *object_get(module_object_t *p_this, int i_id);
	void *object_find( module_object_t *p_this, int i_type, int i_mode );
	module_t *current_object( int i_object );
	void object_yield( module_object_t *p_this );
	void object_release( module_object_t *p_this );

	void moduleInitBank(module_object_t *p_this);
	int moduleInit(int i_object);
	int moduleCreate();
	moduleload_t * moduleUse( module_object_t *p_this, const char *psz_capability, const char *psz_name, bool b_strict );
	void moduleUnuse(module_object_t * p_this, moduleload_t * p_moduleload);
	module_list_t * moduleListFind(module_object_t *p_this, int i_type, int i_mode);
	void moduleListRelease(module_list_t *p_list );
	void moduleEndBank( module_object_t *p_this );
	moduleload_t *loadModule(module_object_t * p_this,int (* pf_symbol) ( moduleload_t * p_module ));

	int modules_register();
	int modules_unregister(int module_id);
	

#ifdef __cplusplus
};
#endif
////////////////////////////////////////////////////////////////////////////////////////

#endif
