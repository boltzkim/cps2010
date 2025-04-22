/*
	모듈 오브젝트를 위해 구현...

	작성자 : 
	이력
	2010-07-22 : 시작
*/

#include <core.h>
#include <stdlib.h>
#include <string.h>

static libmodule_t		libmodule;
static libmodule_t		*p_libmodule;
static module_t			*p_static_module=NULL;
static mutex_t			structure_lock;



static int findIndex( module_object_t *p_this,
                      module_object_t **pp_objects, int i_count )
{
    int i_middle = i_count / 2;

    if( i_count == 0 )
    {
        return 0;
    }

    if( pp_objects[i_middle] == p_this )
    {
        return i_middle;
    }

    if( i_count == 1 )
    {
        return 0;
    }

    if( pp_objects[i_middle]->i_object_id < p_this->i_object_id )
    {
        return i_middle + findIndex( p_this, pp_objects + i_middle,
                                             i_count - i_middle );
    }
    else
    {
        return findIndex( p_this, pp_objects, i_middle );
    }
}


static void setAttachment( module_object_t *p_this, bool b_attached )
{
    int i_index;

    for( i_index = p_this->i_children ; i_index-- ; )
    {
        setAttachment( p_this->pp_children[i_index], b_attached );
    }

    p_this->b_attached = b_attached;
}

/*
	모듈 오브젝트를 생성하는 함수이다.
*/
void *object_create(module_object_t *p_this, int i_type)
{
	int	i_size;
	char		*psz_type = NULL;
	module_object_t	*p_new;

	switch( i_type )
    {
		case OBJECT_ROOT:
			i_size = sizeof(libmodule_t);
			psz_type = "root";
			break;
		case OBJECT_MODULE:
			i_size = sizeof(module_t);
			psz_type = "module";
			break;
		case OBJECT_MODULE_LOAD:
			i_size = sizeof(moduleload_t);
			psz_type = "moduleload";
			break;
		case OBJECT_TRACE:
			i_size = sizeof(trace_t);
			psz_type = "trace";
			break;
		case OBJECT_SERVICE:
			i_size = sizeof(service_t);
			psz_type = "service";
			break;
		case OBJECT_TYPESUPPORT:
			i_size = sizeof(typesupport_t);
			psz_type = "typesupport";
			break;
		case OBJECT_MANAGEMENT:
			i_size = sizeof(management_t);
			psz_type = "management";
			break;
		case OBJECT_ACCESS:
			i_size = sizeof(access_t);
			psz_type = "access";
			break;
		case OBJECT_DISCOVERY:
			i_size = sizeof(discovery_t);
			psz_type = "discovery";
			break;
		case OBJECT_ACCESS_OUT:
			i_size = sizeof(access_out_t);
			psz_type = "accessout";
			break;
		case OBJECT_MONITORING:
			i_size = sizeof(monitoring_t);
			psz_type = "monitoring";
			break;
		case OBJECT_QOS_POLICY: //by kki (qos module)
			i_size = sizeof(qos_policy_t);
			psz_type = "qos_policy";
			break;
		default:
            i_size = i_type > (int)sizeof(module_object_t)
                         ? i_type : (int)sizeof(module_object_t);
            i_type = OBJECT_GENERIC;
            psz_type = "generic";
            break;
	}

	if( i_type == OBJECT_ROOT )
    {
        p_new = p_this;
    }
    else
    {
        p_new = malloc( i_size );
		memset(p_new, '\0', i_size);

        if( !p_new ) return NULL;
        memset( p_new, 0, i_size );
    }

	p_new->p_module = NULL;

	p_new->i_object_type = i_type;
	p_new->psz_object_type = psz_type;
    p_new->psz_object_name = NULL;

	p_new->b_attached = false;
    p_new->b_force = false;

	p_new->v_private = NULL;
	p_new->b_end = false;

	if( i_type == OBJECT_ROOT )
    {
		p_new->p_libmodule = (libmodule_t*)p_new;

		p_new->p_libmodule->i_counter = 0;
		p_new->i_object_id = 0;
		p_new->p_libmodule->i_objects = 1;
        p_new->p_libmodule->pp_objects = malloc( sizeof(module_object_t *) );
		memset(p_new->p_libmodule->pp_objects, '\0', sizeof(module_object_t *));
        p_new->p_libmodule->pp_objects[0] = p_new;

		//p_new->p_libmodule->i_objects = 0;
		//INSERT_ELEM( p_new->p_libmodule->pp_objects,
  //                   p_new->p_libmodule->i_objects,
  //                   p_new->p_libmodule->i_objects,
  //                   p_new );

		p_new->b_attached = true;

	}else{
		p_new->p_libmodule = p_this->p_libmodule;
        p_new->p_module = ( i_type == OBJECT_MODULE ) ? (module_t*)p_new : p_this->p_module;

		mutex_lock( &structure_lock );

        p_new->p_libmodule->i_counter++;
        p_new->i_object_id = p_new->p_libmodule->i_counter;

        INSERT_ELEM( p_new->p_libmodule->pp_objects,
                     p_new->p_libmodule->i_objects,
                     p_new->p_libmodule->i_objects,
                     p_new );

		mutex_unlock( &structure_lock );

	}

	p_new->b_thread = false;

	p_new->i_refcount = 0;
    p_new->p_parent = NULL;
    p_new->pp_children = NULL;
    p_new->i_children = 0;

	mutex_init(&p_new->object_lock );
    cond_init(&p_new->object_wait );
	mutex_init(&p_new->var_lock );


	p_new->i_vars = 0;
    p_new->p_vars = (variable_t *) malloc( 16 * sizeof( variable_t ) );
	memset(p_new->p_vars, '\0', 16 * sizeof(variable_t));

	if( !p_new->p_vars )
    {
		if( i_type != OBJECT_ROOT ){
            FREE( p_new );
		}
        return NULL;
    }


	if( i_type == OBJECT_ROOT )
    {
        mutex_init( &structure_lock );
	}

	return p_new;
}



void object_destroy( module_object_t *p_this )
{
	int i_delay = 0;

    if( p_this->i_children )
    {
        return;
    }

    if( p_this->p_parent )
    {
        return;
    }

	/*while( p_this->i_refcount )
    {
        i_delay++;
		Sleep(100);
	}*/

	if( p_this->i_object_type == OBJECT_ROOT )
    {
        FREE( p_this->p_libmodule->pp_objects );
        p_this->p_libmodule->pp_objects = NULL;
        p_this->p_libmodule->i_objects--;

		mutex_destroy( &structure_lock );
    }
    else
    {
        int i_index;

		mutex_lock( &structure_lock );

        i_index = findIndex( p_this, p_this->p_libmodule->pp_objects,
                             p_this->p_libmodule->i_objects );
        REMOVE_ELEM( p_this->p_libmodule->pp_objects,
                     p_this->p_libmodule->i_objects, i_index );

		mutex_unlock( &structure_lock );
    }

	while( p_this->i_vars )
    {
        var_destroy( p_this, p_this->p_vars[p_this->i_vars - 1].psz_name );
    }

	FREE( p_this->p_vars );


	mutex_destroy( &p_this->object_lock );
	cond_destroy( &p_this->object_wait );
	mutex_destroy(&p_this->var_lock );


	if( p_this->i_object_type != OBJECT_ROOT ){
        FREE( p_this );
	}

}

void object_attach( module_object_t *p_this, module_object_t *p_parent )
{
    mutex_lock( &structure_lock );

    p_this->p_parent = p_parent;

    INSERT_ELEM( p_parent->pp_children, p_parent->i_children,
                 p_parent->i_children, p_this );

    if( p_parent->b_attached )
    {
        setAttachment( p_this, true );
    }

    mutex_unlock( &structure_lock );
}

static void detachObject( module_object_t *p_this )
{
    module_object_t *p_parent = p_this->p_parent;
    int i_index, i;

    p_this->p_parent = NULL;

    for( i_index = p_parent->i_children ; i_index-- ; )
    {
        if( p_parent->pp_children[i_index] == p_this )
        {
            p_parent->i_children--;
            for( i = i_index ; i < p_parent->i_children ; i++ )
            {
                p_parent->pp_children[i] = p_parent->pp_children[i+1];
            }
        }
    }

    if( p_parent->i_children )
    {
        p_parent->pp_children = (module_object_t **)realloc( p_parent->pp_children,
                               p_parent->i_children * sizeof(module_object_t *) );
    }
    else
    {
        FREE( p_parent->pp_children );
        p_parent->pp_children = NULL;
    }
}

void object_detach(module_object_t *p_this)
{
    mutex_lock( &structure_lock );
    if( !p_this->p_parent )
    {
        mutex_unlock( &structure_lock );
        return;
    }

    if( p_this->p_parent->b_attached )
    {
        setAttachment( p_this, false );
    }

    detachObject( p_this );
    mutex_unlock( &structure_lock );
}

void *object_get(module_object_t *p_this, int i_id)
{
    int i_max, i_middle;
    module_object_t **pp_objects;


	if(p_this == NULL) return NULL;

    mutex_lock( &structure_lock );

    pp_objects = p_this->p_libmodule->pp_objects;

    for( i_max = p_this->p_libmodule->i_objects - 1 ; ; )
    {
        i_middle = i_max / 2;

        if( pp_objects[i_middle]->i_object_id > i_id )
        {
            i_max = i_middle;
        }
        else if( pp_objects[i_middle]->i_object_id < i_id )
        {
            if( i_middle )
            {
                pp_objects += i_middle;
                i_max -= i_middle;
            }
            else
            {
                if( pp_objects[i_middle+1]->i_object_id == i_id )
                {
                    mutex_unlock( &structure_lock );
                    pp_objects[i_middle+1]->i_refcount++;
                    return pp_objects[i_middle+1];
                }
                break;
            }
        }
        else
        {
            mutex_unlock( &structure_lock );
            pp_objects[i_middle]->i_refcount++;
            return pp_objects[i_middle];
        }

        if( i_max == 0 )
        {
            break;
        }
    }

    mutex_unlock( &structure_lock );
    return NULL;
}

static module_object_t *FindObject( module_object_t *p_this, int i_type, int i_mode )
{
    int i;
    module_object_t *p_tmp;

    switch( i_mode & 0x000f )
    {
    case FIND_PARENT:
        p_tmp = p_this->p_parent;
        if( p_tmp )
        {
            if( p_tmp->i_object_type == i_type )
            {
                p_tmp->i_refcount++;
                return p_tmp;
            }
            else
            {
                return FindObject( p_tmp, i_type, i_mode );
            }
        }
        break;

    case FIND_CHILD:
        for( i = p_this->i_children; i--; )
        {
            p_tmp = p_this->pp_children[i];
            if( p_tmp->i_object_type == i_type )
            {
                p_tmp->i_refcount++;
                return p_tmp;
            }
            else if( p_tmp->i_children )
            {
                p_tmp = FindObject( p_tmp, i_type, i_mode );
                if( p_tmp )
                {
                    return p_tmp;
                }
            }
        }
        break;

    case FIND_ANYWHERE:
        break;
    }

    return NULL;
}

void *object_find( module_object_t *p_this, int i_type, int i_mode )
{
    module_object_t *p_found;

    mutex_lock( &structure_lock );

    if( (i_mode & 0x000f) == FIND_ANYWHERE )
    {
        module_object_t *p_root = p_this;

        while( p_root->p_parent != NULL && p_root != OBJECT( p_this->p_module ) )
        {
            p_root = p_root->p_parent;
        }

        p_found = FindObject( OBJECT(p_root), i_type, (i_mode & ~0x000f)|FIND_CHILD );
        if( p_found == NULL && p_root != OBJECT( p_this->p_module ) )
        {
            p_found = FindObject( OBJECT( p_this->p_module ),i_type, (i_mode & ~0x000f)|FIND_CHILD );
        }
    }
    else
    {
        p_found = FindObject( p_this, i_type, i_mode );
    }

    mutex_unlock( &structure_lock );

    return p_found;
}

module_t *current_object( int i_object )
{
    if( i_object )
    {
         return object_get( OBJECT(p_libmodule), i_object );
    }

    return p_static_module;
}

void object_yield( module_object_t *p_this )
{
    mutex_lock( &structure_lock );
    p_this->i_refcount++;
    mutex_unlock( &structure_lock );
}


void object_release( module_object_t *p_this )
{
    mutex_lock( &structure_lock );
    p_this->i_refcount--;
    mutex_unlock( &structure_lock );
}


///////////////////////////////////// 모듈 등록 ////////////////////////////////////////////////////////////

#define CallFunction(name) name##__module_entry
#define CallFunctionDef(name) name##__module_entry(moduleload_t * p_module);

#define LoadMediaModule(name) \
		p_moduleload = loadModule(OBJECT(p_module),CallFunction(name));\
		if(p_moduleload){ \
			object_attach( OBJECT(p_moduleload), OBJECT(p_module->p_libmodule->p_module_bank) );\
		}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

CallFunctionDef(main);

CallFunctionDef(simpletrace);
CallFunctionDef(rtpsService);
CallFunctionDef(simpletypesupport);
CallFunctionDef(simple_management_for_messgae);
CallFunctionDef(IPv4);
CallFunctionDef(access_udp);
CallFunctionDef(rtpsDiscovery);
CallFunctionDef(access_udp_out);
CallFunctionDef(monitoring_for_cps);
CallFunctionDef(qos);//by kki (qos module)



///////////////////////////////////// 모듈 초기화 및 등록 //////////////////////////////////////////////////


moduleload_t *loadModule(module_object_t * p_this,int (* pf_symbol) ( moduleload_t * p_module ))
{
	moduleload_t * p_module;
	int size = sizeof(bool);

	p_module = object_create( p_this, OBJECT_MODULE_LOAD );
    if( p_module == NULL )
    {
        return NULL;
    }


	if( pf_symbol( p_module ) != 0 )
    {
        object_destroy( OBJECT(p_module));
        return NULL;
    }


	return p_module;
}

void moduleInitBank(module_object_t *p_this)
{
	module_bank_t *p_bank;
	if( p_this->p_libmodule->p_module_bank )
    {
		 p_this->p_libmodule->p_module_bank->i_usage++;
		 return;
	}

	p_bank = object_create( p_this, sizeof(module_bank_t) );
    p_bank->psz_object_name = "module bank";
    p_bank->i_usage = 1;

	p_this->p_libmodule->p_module_bank = p_bank;
    object_attach( OBJECT(p_bank), OBJECT(p_this->p_libmodule ));

    return;
}

int moduleInit(int i_object)
{
	module_t		*p_module = current_object( i_object );
	moduleload_t	*p_moduleload = NULL;

	if( !p_module )
    {
		return MODULE_ERROR_CREATE;
    }

	moduleInitBank(OBJECT(p_module));

	LoadMediaModule(main);
	LoadMediaModule(simpletrace);
	LoadMediaModule(rtpsService);
	LoadMediaModule(simpletypesupport);
	LoadMediaModule(simple_management_for_messgae);
	LoadMediaModule(IPv4);
	LoadMediaModule(access_udp);
	LoadMediaModule(rtpsDiscovery);
	LoadMediaModule(qos);//by kki (qos module)
	LoadMediaModule(access_udp_out);
	LoadMediaModule(monitoring_for_cps);

	//환경설정 초기화
	configuration(p_module);

	return MODULE_SUCCESS;
}


static void NetworkStart();

////////////////////////////////////////////////////////////////////////////////////////////////////////////

int moduleCreate()
{
	int i_ret = -1;
	module_t *p_module = NULL;

	NetworkStart();
	
	p_libmodule = &libmodule;
	i_ret = threads_init( OBJECT(p_libmodule) );
	

	p_module = object_create( OBJECT(p_libmodule), OBJECT_MODULE );


    if( p_module == NULL )
    {
		return MODULE_ERROR_CREATE;
    }

	p_module->thread_id = 0;
	p_module->psz_object_name = "module root";
	object_attach( OBJECT(p_module), OBJECT(p_libmodule) );
    p_static_module = p_module;

	//dcps 관련 변수 초기화
	p_module->i_domain_participants = 0;
	p_module->pp_domain_participants = NULL;

    return p_module->i_object_id;
}


///////////////////////// 모듈 검색 및 언로드 /////////////////////////////////

static module_list_t * NewList( int i_count )
{
    module_list_t * p_list = (module_list_t *)malloc( sizeof( module_list_t ) );
	memset(p_list, '\0', sizeof(module_list_t));

    if( p_list == NULL )
    {
        return NULL;
    }

    p_list->i_count = i_count;

    if( i_count == 0 )
    {
        p_list->p_values = NULL;
        return p_list;
    }

    p_list->p_values = malloc( i_count * sizeof( value_t ) );
	memset(p_list->p_values, '\0', sizeof(value_t));

    if( p_list->p_values == NULL )
    {
        p_list->i_count = 0;
        return p_list;
    }

    return p_list;
}

static void ListReplace(module_list_t *p_list, module_object_t *p_object, int i_index )
{
    if( p_list == NULL || i_index >= p_list->i_count )
    {
        return;
    }

    p_object->i_refcount++;

    p_list->p_values[i_index].p_object = p_object;

    return;
}

static int CountChildren(module_object_t *p_this, int i_type )
{
    module_object_t *p_tmp=NULL;
    int i, i_count = 0;

    for( i = 0; i < p_this->i_children; i++ )
    {
        p_tmp = p_this->pp_children[i];

        if( p_tmp->i_object_type == i_type )
        {
            i_count++;
        }

        if( p_tmp->i_children )
        {
            i_count += CountChildren( p_tmp, i_type );
        }
    }

    return i_count;
}

static void ListChildren(module_list_t *p_list, module_object_t *p_this, int i_type )
{
    module_object_t *p_tmp=NULL;
    int i;

    for( i = 0; i < p_this->i_children; i++ )
    {
        p_tmp = p_this->pp_children[i];

        if( p_tmp->i_object_type == i_type )
        {
            ListReplace( p_list, p_tmp, p_list->i_count++ );
        }

        if( p_tmp->i_children )
        {
            ListChildren( p_list, p_tmp, i_type );
        }
    }
}



moduleload_t * moduleUse(module_object_t *p_this, const char *psz_capability, const char *psz_name, bool b_strict )
{
	typedef struct moduleload_list_t moduleload_list_t;

    struct moduleload_list_t
    {
        moduleload_t *p_moduleload;
        int i_score;
        bool b_force;
        moduleload_list_t *p_next;
    };

    moduleload_list_t *p_list = NULL, *p_first = NULL, *p_tmp = NULL;
    module_list_t *p_all = NULL;

    int i_which_module, i_index = 0;
    bool b_intf = false;

    moduleload_t *p_moduleload = NULL;

    int   i_shortcuts = 0;
	char *psz_shortcuts = NULL, *psz_var = NULL;
	bool b_force_backup = p_this->b_force;

	if( psz_name && *psz_name )
    {
		char *psz_parser, *psz_last_shortcut;

		if( !strcmp( psz_name, "none" ) )
        {
            FREE( psz_var );
            return NULL;
        }

		i_shortcuts++;
        psz_shortcuts = psz_last_shortcut = strdup( psz_name );

		for( psz_parser = psz_shortcuts; *psz_parser; psz_parser++ )
        {
            if( *psz_parser == ',' )
            {
                 *psz_parser = '\0';
                 i_shortcuts++;
                 psz_last_shortcut = psz_parser + 1;
            }
        }

		if( psz_last_shortcut )
        {
            if( !strcmp(psz_last_shortcut, "none") )
            {
                b_strict = true;
                i_shortcuts--;
            }
            else if( !strcmp(psz_last_shortcut, "any") )
            {
                b_strict = false;
                i_shortcuts--;
            }
        }
	}

	p_all = moduleListFind( p_this, OBJECT_MODULE_LOAD, FIND_ANYWHERE );
    p_list = malloc( p_all->i_count * sizeof( moduleload_list_t ) );
	memset(p_list, '\0', p_all->i_count * sizeof(moduleload_list_t));

    p_first = NULL;

	for( i_which_module = 0; i_which_module < p_all->i_count; i_which_module++ )
    {
        int i_shortcut_bonus = 0;

        p_moduleload = (moduleload_t *)p_all->p_values[i_which_module].p_object;

        if( strcmp( p_moduleload->psz_capability, psz_capability ) )
        {
            continue;
        }

		if(i_shortcuts > 0)
        {
			bool b_trash;
            int i_dummy, i_short = i_shortcuts;
            char *psz_name = psz_shortcuts;
            b_trash = p_moduleload->i_score <= 0;

			while( i_short > 0 )
            {
                for( i_dummy = 0; p_moduleload->pp_shortcuts[i_dummy]; i_dummy++ )
                {
                    if( !strcasecmp( psz_name, p_moduleload->pp_shortcuts[i_dummy] ) )
                    {
                        b_trash = false;
                        i_shortcut_bonus = i_short * 10000;
                        break;
                    }
                }

                if( i_shortcut_bonus )
                {
                    break;
                }

                while( *psz_name )
                {
                    psz_name++;
                }
                psz_name++;
                i_short--;
            }
			
			if( i_short == 0 && b_strict )
            {
                b_trash = true;
            }

            if( b_trash )
            {
                continue;
            }
		}else if( p_moduleload->i_score <= 0 )
        {
            continue;
        }

		p_list[ i_index ].p_moduleload = p_moduleload;
        p_list[ i_index ].i_score = p_moduleload->i_score + i_shortcut_bonus;
        p_list[ i_index ].b_force = i_shortcut_bonus && b_strict;

		if( i_index == 0 )
        {
            p_list[ 0 ].p_next = NULL;
            p_first = p_list;
        }
        else
        {
			moduleload_list_t *p_newlist = p_first;
			if( p_first->i_score < p_list[ i_index ].i_score )
            {
                p_list[ i_index ].p_next = p_first;
                p_first = &p_list[ i_index ];
            }
            else
            {
                while( p_newlist->p_next != NULL &&
                    p_newlist->p_next->i_score >= p_list[ i_index ].i_score )
                {
                    p_newlist = p_newlist->p_next;
                }

                p_list[ i_index ].p_next = p_newlist->p_next;
                p_newlist->p_next = &p_list[ i_index ];
            }
		}
		i_index++;
	}

	p_tmp = p_first;
    while( p_tmp != NULL )
    {
        object_yield( OBJECT(p_tmp->p_moduleload) );
        p_tmp = p_tmp->p_next;
    }

	moduleListRelease( p_all );

	p_tmp = p_first;
    while( p_tmp != NULL )
    {
        p_this->b_force = p_tmp->b_force;
        if( p_tmp->p_moduleload->pf_activate
             && p_tmp->p_moduleload->pf_activate( p_this ) == MODULE_SUCCESS )
        {
            break;
        }

        object_release( OBJECT(p_tmp->p_moduleload) );
        p_tmp = p_tmp->p_next;
    }

    if( p_tmp != NULL )
    {
        p_moduleload = p_tmp->p_moduleload;
        p_tmp = p_tmp->p_next;
    }
    else
    {
        p_moduleload = NULL;
    }


    while( p_tmp != NULL )
    {
        object_release( OBJECT(p_tmp->p_moduleload) );
        p_tmp = p_tmp->p_next;
    }

    FREE( p_list );

    p_this->b_force = b_force_backup;

    if( psz_shortcuts )
    {
        FREE( psz_shortcuts );
    }

    if( psz_var )
    {
        FREE( psz_var );
    }

    return p_moduleload;
}


void moduleUnuse(module_object_t * p_this, moduleload_t * p_moduleload)
{
    if( p_moduleload->pf_deactivate )
    {
        p_moduleload->pf_deactivate( p_this );
    }

    object_release( OBJECT(p_moduleload) );
}

static int DeleteModule( moduleload_t * p_moduleload )
{
    object_detach( OBJECT(p_moduleload) );

    while( p_moduleload->i_children )
    {
        module_object_t *p_this = p_moduleload->pp_children[0];
        object_detach( p_this );
        object_destroy( p_this );
    }

    object_destroy( OBJECT(p_moduleload) );

    return 0;
}

void moduleEndBank( module_object_t *p_this )
{
    moduleload_t * p_next;
    if( !p_this->p_libmodule->p_module_bank )
    {
        return;
    }

    if( --p_this->p_libmodule->p_module_bank->i_usage )
    {
        return;
    }

    object_detach( OBJECT(p_this->p_libmodule->p_module_bank) );

    while( p_this->p_libmodule->p_module_bank->i_children )
    {
        p_next = (moduleload_t *)p_this->p_libmodule->p_module_bank->pp_children[0];
		config_free(p_next);

        if( DeleteModule( p_next ) )
        {
            object_detach( OBJECT(p_next) );
            object_destroy( OBJECT(p_next) );
        }
    }

    object_destroy( OBJECT(p_this->p_libmodule->p_module_bank) );
    p_this->p_libmodule->p_module_bank = NULL;

    return;
}

static void NetworkStart()
{
#ifdef _MSC_VER
	WSADATA Data;

	_fmode = _O_BINARY;
//    _setmode( _fileno( stdin_dds ), _O_BINARY );

	if( !WSAStartup( MAKEWORD( 2, 2 ), &Data ) )
    {
        if( LOBYTE( Data.wVersion ) != 2 || HIBYTE( Data.wVersion ) != 2 )
            WSACleanup( );
        else
            return;
    }

    if( !WSAStartup( MAKEWORD( 1, 1 ), &Data ) )
    {
        if( LOBYTE( Data.wVersion ) != 1 || HIBYTE( Data.wVersion ) != 1 )
            WSACleanup( );
        else
            return;
    }
#endif
}

////////////////////////////////////////// 모듈 레지스터 //////////////////////////////////////


int modules_register()
{
	int i_ret;
	module_t *p_module=NULL;
	int module_id = -1;

	

	module_id = moduleCreate();
	
	i_ret = moduleInit(module_id);

	p_module = current_object( module_id );


	//TRACE 모듈 론치
	launchTraceModule(OBJECT(p_module));

	//TYPESUPPORT 모듈 론치
	launch_type_support_module(OBJECT(p_module));

	//Managemetn 모듈 론치
	launch_management_module(OBJECT(p_module));

	//Service 모듈 론치
	launch_service_module(OBJECT(p_module));

	//Discovery 모듈 론치
	launch_discovery_module(OBJECT(p_module));

	//QoS 모듈 론치
	launchQosPolicyModule(OBJECT(p_module));//by kki (qos module)

	//Monitoring 모듈 론치
	//launchMonitoringModule(OBJECT(p_module));


	return module_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////// 모듈 레지스터 해제     //////////////////////////////////////

int modules_unregister(int module_id)
{
	module_t *p_module = current_object(module_id);
	libmodule_t *p_libmodule = p_module->p_libmodule;

	int i_children = p_module->i_children;
	module_list_t *p_list = NULL;

	p_module->b_end = true;

	while(p_module->i_children)
	{
		module_object_t *p_child = p_module->pp_children[0];

		if(p_child->i_object_type == OBJECT_TRACE)
		{
			trace_t *p_trace = (trace_t *)p_child;
			if(p_trace->p_moduleload)
			{
				moduleUnuse(p_child,p_trace->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_SERVICE)
		{
			service_t *p_service = (service_t *)p_child;
			if(p_service->p_moduleload)
			{
				moduleUnuse(p_child,p_service->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_TYPESUPPORT)
		{
			typesupport_t *p_typesupport = (typesupport_t *)p_child;
			if(p_typesupport->p_moduleload)
			{
				moduleUnuse(p_child,p_typesupport->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_DISCOVERY)
		{
			discovery_t *p_discovery = (discovery_t *)p_child;
			if(p_discovery->p_moduleload)
			{
				moduleUnuse(p_child,p_discovery->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_MANAGEMENT)
		{
			management_t *p_management = (management_t *)p_child;
			if(p_management->p_moduleload)
			{
				moduleUnuse(p_child,p_management->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_ACCESS)
		{
			access_t *p_access = (access_t *)p_child;
			if(p_access->p_moduleload)
			{
				moduleUnuse(p_child,p_access->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_ACCESS_OUT)
		{
			access_out_t *p_accessout = (access_out_t *)p_child;
			if(p_accessout->p_moduleload)
			{
				moduleUnuse(p_child,p_accessout->p_moduleload);
			}
			object_detach(p_child);
			object_destroy(p_child);
		}else if(p_child->i_object_type == OBJECT_MODULE_LOAD)
		{
			moduleload_t *p_moduleload = (moduleload_t *)p_child;
			object_detach(p_child);
			object_destroy(p_child);
		}else{
			object_detach(p_child);
			object_destroy(p_child);
		}
	}

	moduleEndBank(OBJECT(p_module));

	object_detach(OBJECT(p_module));
	object_destroy(OBJECT(p_module));

	threads_end(OBJECT(p_libmodule));
#ifdef _MSC_VER
	WSACleanup();
#endif

	return module_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef tracemalloc
#undef tracefree
#undef tracestrdup

module_list_t * moduleListFind(module_object_t *p_this, int i_type, int i_mode)
{
    module_list_t *p_list = NULL;
    module_object_t **pp_current=NULL, **pp_end=NULL;
    int i_count = 0, i_index = 0;

    mutex_lock( &structure_lock );

    /* Look for the objects */
    switch( i_mode & 0x000f )
    {
    case FIND_ANYWHERE:
        pp_current = p_this->p_libmodule->pp_objects;
        pp_end = pp_current + p_this->p_libmodule->i_objects;

        for( ; pp_current < pp_end ; pp_current++ )
        {
            if( (*pp_current)->b_attached
                 && (*pp_current)->i_object_type == i_type )
            {
                i_count++;
            }
        }

        p_list = NewList( i_count );
        pp_current = p_this->p_libmodule->pp_objects;

        for( ; pp_current < pp_end ; pp_current++ )
        {
            if( (*pp_current)->b_attached
                 && (*pp_current)->i_object_type == i_type )
            {
                ListReplace( p_list, *pp_current, i_index );
                if( i_index < i_count ) i_index++;
            }
        }
    break;

    case FIND_CHILD:
        i_count = CountChildren( p_this, i_type );
        p_list = NewList( i_count );

		if(p_list == NULL) break;

        /* Check allocation was successful */
        if( p_list->i_count != i_count )
        {
            p_list->i_count = 0;
            break;
        }

        p_list->i_count = 0;
        ListChildren( p_list, p_this, i_type );
        break;

    default:
        p_list = NewList( 0 );
        break;
    }

    mutex_unlock( &structure_lock );

    return p_list;
}

void moduleListRelease(module_list_t *p_list )
{
    int i_index;

    for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
        mutex_lock( &structure_lock );

        p_list->p_values[i_index].p_object->i_refcount--;

        mutex_unlock( &structure_lock );
    }

    FREE( p_list->p_values );
    FREE( p_list );
}

