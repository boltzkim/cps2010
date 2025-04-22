/**
	@file configuration.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief 해당파일은 환경 설정 파일을 읽어서 처리하는 기능을 가짐
	@details 환경 설정 파일을 읽어서 처리하는 부분
*/
#include <core.h>

static void processConfiguration(FILE *p_file, module_t *p_module);
static void CheckValue(variable_t *p_var, value_t *p_val);

/**
	@brief Module Object에 대한 환경 설정하는 함수
	@details 'cps.config' 파일에서 정의한 것을 읽어와 Module Object 환경 설정
	@param[in] p_module Module Object를 가리키는 포인터
*/
void configuration(module_t *p_module)
{
	const char* conf_path = getenv("CPS_CONF");
	FILE *p_file = NULL;

	if(p_module == NULL) return;

	if(conf_path == NULL)
	{
		conf_path = "cps.config";
	}

	p_file = open_file(conf_path,"rt");

	if(p_file){
		processConfiguration(p_file, p_module);
	}


	if(p_file){
		close_file(p_file);
	}
}

/**
	@brief 파일에서 정의한 Configuration 정보를 Module Configuration 으로 복사하는 함수
	@details 파일에서 정의한 것을 읽어와 Configure Type에 따라 Module Object에서 찾은 Module Load 구조체에 속한 Module Configure 구조체에 값을 복사함
	@param[in] p_file File을 가리키는 포인터
	@param[in] p_module Module Object를 가리키는 포인터
*/
static void processConfiguration(FILE *p_file, module_t *p_module)
{
	module_list_t *p_list = NULL;
	int i_index;
	moduleload_t *p_parser = NULL;
	char line[1024];
	module_config_t *p_item = NULL;
	char *p_index = NULL, *p_name = NULL, *p_value = NULL;

	if(p_file == NULL) return;

	p_list = moduleListFind(OBJECT(p_module),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		/*if(strcmp( "main", p_parser->psz_object_name ) )
        {
            continue;
        }*/

		//파일의 끝으로 포인터를 옮김
		fseek( p_file, 0L, SEEK_SET );

		while( fgets( line, 1024, p_file ) )
        {
			if( (line[0] == '#') || (line[0] == '\r') || (line[0] == '\n') || (line[0] == (char)0) )
                continue;

			if( line[strlen(line)-1] == '\n' ){
                line[strlen(line)-1] = (char)0;
			}

			p_name = line;
			p_value = NULL;
			p_index = strchr( line, '=' );

			if( !p_index ) break;

			 *p_index = (char)0;
            p_value = p_index + 1;

			if( !p_parser->i_config_items )
            {
                continue;
            }

			for( p_item = p_parser->p_config; p_item->i_type != CONFIG_HINT_END; p_item++)
            {
				if( p_item->i_type & CONFIG_HINT )
                    continue;

				if(p_item->psz_name == NULL) continue;

				if( !strcmp( p_item->psz_name, p_name ) )
                {
					switch( p_item->i_type ){
						case CONFIG_ITEM_BOOL:
						case CONFIG_ITEM_INTEGER:
							if( !*p_value ) break;

							//문자열을 숫자로 바꿔줌
							p_item->i_value = strtol( p_value, 0, 0 );
							break;
						case CONFIG_ITEM_FLOAT:
							if( !*p_value ) break;

							//문자열 인수에 해당하는 double값 반환
							p_item->f_value = (float) atof( p_value);
							break;
						default:
							mutex_lock(p_item->p_lock);

							if( p_item->psz_value )
								FREE( p_item->psz_value );

							//새로운 메모리를 할당하고 문자열을 복사한 후 그 포인터를 리턴
							//복제된 string은 필요가 없을 경우 free함수로 strdup가 할당한 메모리를 반드시 해제해 주어야 함
							p_item->psz_value = *p_value ? strdup( p_value ) : NULL;

							mutex_unlock(p_item->p_lock);
							break;
					}
				}
			}

		}
	}

	if(p_list)moduleListRelease( p_list );
}

/**
	@brief Module Configure를 Module Load에 복사하는 함수
	@details 외부에서 입력 받은 Module Configure를 Module Load에 복사함
	@param[in] p_moduleload Module Load 구조체를 가리키는 포인터
	@param[in] p_orig Module Load에 복사해 넣을 Module Configure 구조체를 가리키는 포인터
*/
void config_dup( moduleload_t* p_moduleload, module_config_t* p_orig )
{
    int i, i_lines = 1;
    module_config_t *p_item = NULL;

    p_moduleload->i_config_items = 0;
    p_moduleload->i_bool_items = 0;

    for( p_item = p_orig; p_item->i_type != CONFIG_HINT_END; p_item++ )
    {
        i_lines++;

        if( p_item->i_type & CONFIG_ITEM )
        {
            p_moduleload->i_config_items++;
        }

        if( p_item->i_type == CONFIG_ITEM_BOOL )
        {
            p_moduleload->i_bool_items++;
        }
    }

    p_moduleload->p_config = (module_config_t *)malloc( sizeof(module_config_t) * i_lines );
    if( p_moduleload->p_config == NULL )
    {
        return;
    }

	for( i = 0; i < i_lines ; i++ )
	{
		p_moduleload->p_config[i] = p_orig[i];
		p_moduleload->p_config[i].p_lock = &p_moduleload->object_lock;

		p_moduleload->p_config[i].psz_name = p_orig[i].psz_name ? strdup( p_orig[i].psz_name ) : NULL;
		p_moduleload->p_config[i].psz_value = p_orig[i].psz_value ? strdup( p_orig[i].psz_value ) : NULL;
		
		p_moduleload->p_config[i].psz_text = p_orig[i].psz_text ? strdup( p_orig[i].psz_text ) : NULL;
        p_moduleload->p_config[i].psz_longtext = p_orig[i].psz_longtext ? strdup( p_orig[i].psz_longtext ) : NULL;

		if( p_orig[i].i_action )
        {
            int j;

            p_moduleload->p_config[i].ppf_action = malloc( p_orig[i].i_action * sizeof(void *) );
            p_moduleload->p_config[i].ppsz_action_text = malloc( p_orig[i].i_action * sizeof(char *) );

            for( j = 0; j < p_orig[i].i_action; j++ )
            {
                p_moduleload->p_config[i].ppf_action[j] = p_orig[i].ppf_action[j];
                p_moduleload->p_config[i].ppsz_action_text[j] = p_orig[i].ppsz_action_text[j] ? strdup( p_orig[i].ppsz_action_text[j] ) : NULL;
            }
        }

        p_moduleload->p_config[i].pf_callback = p_orig[i].pf_callback;
	}
}

/**
	@brief Module Load가 가지고 있는 Module Configure를 삭제하는 함수
	@details Module Load가 가지고 있는 Module Configure에 대한 메모리 삭제 수행
	@param[in] p_moduleload Module Configure를 삭제 할 Module Load 구조체를 가리키는 포인터
*/
void config_free( moduleload_t* p_moduleload )
{
    module_config_t *p_item = p_moduleload->p_config;

    if( p_item == NULL )
    {
        return;
    }

    for( ; p_item->i_type != CONFIG_HINT_END ; p_item++ )
    {

        if( p_item->psz_name )
            FREE( p_item->psz_name );

        if( p_item->psz_text )
            FREE( p_item->psz_text );

        if( p_item->psz_longtext )
            FREE( p_item->psz_longtext );

        if( p_item->psz_value )
            FREE( p_item->psz_value );
    }

    FREE( p_moduleload->p_config );
    p_moduleload->p_config = NULL;
}

/**
	@brief value_t 구조체 중 Bool 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 Boolean 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpBool(value_t v, value_t w)
{ 
	//v.b_bool과 w.b_bool이 true면 0을 리턴하고
	//v.b_bool이 true 이고 w.b_bool이 false이면 1을 리턴하고
	//v.b_bool이 false이고 w.b_bool이 true이면 -1을 리턴하고
	//그 이외의 경우에는 0을 리턴함
	return v.b_bool ? w.b_bool ? 0 : 1 : w.b_bool ? -1 : 0;
}

/**
	@brief value_t 구조체 중 Int 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 Integer 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpInt(value_t v, value_t w)
{
	//v.i_int가 w.i_int와 같으면 0을 리턴하고
	//v.i_int가 w.i_int보다 크면 1을 리턴하고
	//그 이외의 경우에는 -1을 리턴함
	return v.i_int == w.i_int ? 0 : v.i_int > w.i_int ? 1 : -1;
}

/**
	@brief value_t 구조체 중 Time 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 Time 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpTime(value_t v, value_t w)
{
	//v.i_int가 w.i_int와 같으면 0을 리턴하고
	//v.i_int가 w.i_int보다 크면 1을 리턴하고
	//그 이외의 경우에는 -1을 리턴함
    return v.i_time == w.i_time ? 0 : v.i_time > w.i_time ? 1 : -1;
}

/**
	@brief value_t 구조체 중 String 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 String 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpString(value_t v, value_t w)
{ 
	//v.psz_string과 w.psz_string 값이 같으면 0을 리턴하고
	//v.psz_string과 w.psz_string 보다 크면 1을 리턴하고
	//v.psz_string과 w.psz_string 보다 작으면 -1을 리턴함
	return strcmp( v.psz_string, w.psz_string );
}

/**
	@brief value_t 구조체 중 Float 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 Float 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpFloat(value_t v, value_t w)
{
	//v.f_float가 w.f_float와 같으면 0을 리턴하고
	//v.f_float가 w.f_float보다 크면 1을 리턴하고
	//그 이외의 경우에는 -1을 리턴함
	return v.f_float == w.f_float ? 0 : v.f_float > w.f_float ? 1 : -1;
}

/**
	@brief value_t 구조체 중 Address 자료형이 일치하는 지 비교하는 함수
	@details value_t 구조체가 가지고 있는 Address 값을 서로 비교
	@param[in] v 비교할 value_t 구조체
	@param[in] w 비교할 value_t 구조체
	@return 비교한 int 형 결과 값
*/
static int CmpAddress(value_t v, value_t w)
{
	//v.p_address가 w.p_address와 같으면 0을 리턴하고
	//v.p_address가 w.p_address보다 크면 1을 리턴하고
	//그 이외의 경우에는 -1을 리턴함
	return v.p_address == w.p_address ? 0 : v.p_address > w.p_address ? 1 : -1;
}

/**
	@brief value_t 구조체를 Void 형태로 반환하는 함수
	@details value_t 구조체를 Void 형태로 타입 캐스팅 함
	@param[in] p_val value_t 구조체를 가리키는 포인터
	@return value_t 구조체를 타입 캐스팅 한 void 형 결과 값
*/
static void DupDummy(value_t *p_val) 
{ 
	(void)p_val;
}

/**
	@brief 자신의 value_t 구조체가 가지고 있는 psz_string 값에 새로운 공간을 할당하는 함수
	@details 자신의 value_t 구조체 중 psz_string 값을 새로운 주소에 대한 문자열 포인터를 복사함
	@param[in] p_val value_t 구조체를 가리키는 포인터
*/
static void DupString(value_t *p_val)
{ 
	p_val->psz_string = strdup( p_val->psz_string );
}

/**
	@brief value_t 구조체를 Void 형태로 반환하는 함수
	@details value_t 구조체를 Void 형태로 타입 캐스팅 함
	@param[in] p_val value_t 구조체를 가리키는 포인터
	@return value_t 구조체를 타입 캐스팅 한 void 형 결과 값
*/
static void FreeDummy(value_t *p_val)
{ 
	(void)p_val;
}

/**
	@brief value_t 구조체 중 psz_string 값을 메모리 해제하는 함수
	@details value_t 구조체 중 psz_string 값을 메모리 해제 함
	@param[in] p_val value_t 구조체를 가리키는 포인터
*/
static void FreeString(value_t *p_val) 
{ 
	FREE( p_val->psz_string );
}

/**
	@brief value_t 구조체 중 address 값을 메모리 해제하는 함수
	@details value_t 구조체 중 address 값에 대한 mutex_t 구조체로 형변환 하여 Mutex를 삭제하고 메모리 해제 함
	@param[in] p_val value_t 구조체를 가리키는 포인터
*/
static void FreeMutex(value_t *p_val )
{ 
	mutex_destroy((mutex_t*)p_val->p_address );
	FREE(p_val->p_address);
}

/**
	@brief String 값을 Hash 값으로 변환하는 함수
	@details 4byte 이상 긴 문자열을 4byte의 hash 값으로 바꿈
	@param[in] psz_string Hash 값으로 변환 할 String
	@return String 값을 Hash 값으로 변환한 값
*/
static uint32_t HashString( const char *psz_string )
{
    uint32_t i_hash = 0;

    while( *psz_string )
    {
        i_hash += *psz_string++;
        i_hash += i_hash << 10;
        i_hash ^= i_hash >> 8;
    }

    return i_hash;
}

/**
	@brief Variable 구조체에서 비교할 Hash 값이 어느정도 위치에 있는지 Variable 구조체의 Index를 찾는 함수
	@details Variable 구조체의 Hash 값과 비교할 Hash 값을 비교하여 첫번째 Variable 구조체의 hash 값보다 비교할 Hash 값이 작으면 0을 리턴하고 
	Variable 구조체의 Hash 값이 입력받은 Hash 보다 크면 그 Variable 구조체의 Index를 리턴함
	@param[in] p_vars Hash 값을 가지고 있는 Variable 구조체
	@param[in] i_count Hash 값을 비교할 Variable 구조체의 Index
	@param[in] i_hash 비교할 Hash 값
	@return 검색한 Hash 값
*/
static int LookupInner( variable_t *p_vars, int i_count, uint32_t i_hash )
{
    int i_middle;

    if( i_hash <= p_vars[0].i_hash )
    {
        return 0;
    }

    if( i_hash >= p_vars[i_count-1].i_hash )
    {
        return i_count - 1;
    }

    i_middle = i_count / 2;


    if( i_hash < p_vars[i_middle].i_hash )
    {
        return LookupInner( p_vars, i_middle, i_hash );
    }

    if( i_hash > p_vars[i_middle].i_hash )
    {
        return i_middle + LookupInner( p_vars + i_middle, i_count - i_middle, i_hash);
    }

    return i_middle;
}

/**
	@brief Variable 구조체의 String 값과 비교할 String 값이 같은 Variable 구조체의 Index 값을 구하는 함수
	@details Variable 구조체의 String 값과 입력받은 String 값을 비교하여 값이 같은 Variable 구조체의 Index를 리턴함
	@param[in] p_vars String 값을 가지고 있는 Variable 구조체
	@param[in] i_count String 값을 비교할 Variable 구조체의 Index
	@param[in] psz_name Hash 값으로 변환 할 String
	@return String이 같은 Variable 구조체의 Index 값
*/
static int Lookup( variable_t *p_vars, int i_count, const char *psz_name )
{
    uint32_t i_hash;
    int i, i_pos;

    if( i_count == 0 )
    {
        return -1;
    }

	//String 값을 Hash 값으로 변환
    i_hash = HashString( psz_name );

	//Hash 값을 이용하여 Variable 구조체의 Index를 찾음
    i_pos = LookupInner( p_vars, i_count, i_hash );

    if( i_hash != p_vars[i_pos].i_hash )
    {
        return -1;
    }

	//String 값과 Variable의 Index 번째의 String 값이 같을 경우 Index값을 리턴
    if( !strcmp( psz_name, p_vars[i_pos].psz_name ) )
    {
        return i_pos;
    }

	//Index 값 부터 아래로 For 문을 돌면서 Hash 값이 같아질 때까지 비교하여 같은값을 찾았을 경우 그 Index를 리턴
    for( i = i_pos - 1 ; i > 0 && i_hash == p_vars[i].i_hash ; i-- )
    {
        if( !strcmp( psz_name, p_vars[i].psz_name ) )
        {
            return i;
        }
    }

	//Index 값 부터 위로 For 문을 돌면서 Hash 값이 같아질 때까지 비교하여 같은값을 찾았을 경우 그 Index를 리턴
    for( i = i_pos + 1 ; i < i_count && i_hash == p_vars[i].i_hash ; i++ )
    {
        if( !strcmp( psz_name, p_vars[i].psz_name ) )
        {
            return i;
        }
    }

    return -1;
}

/**
	@brief Variable 구조체에서 비교할 Hash 값이 어느정도 위치에 있는지 Variable 구조체의 다음 Index를 찾는 함수
	@details Variable 구조체의 Hash 값과 비교할 Hash 값을 비교하여 첫번째 Variable 구조체의 hash 값보다 비교할 Hash 값이 작으면 0을 리턴하고 
	Variable 구조체의 Hash 값이 입력받은 Hash 보다 크면 그 Variable 구조체의 다음 Index(Index+1)를 리턴함
	@param[in] p_vars Hash 값을 가지고 있는 Variable 구조체
	@param[in] i_count Hash 값을 비교할 Variable 구조체의 Index
	@param[in] i_hash 비교할 Hash 값
	@return 검색한 Hash 값
*/
static int InsertInner( variable_t *p_vars, int i_count, uint32_t i_hash )
{
    int i_middle;

    if( i_hash <= p_vars[0].i_hash )
    {
        return 0;
    }

    if( i_hash >= p_vars[i_count - 1].i_hash )
    {
        return i_count;
    }

    i_middle = i_count / 2;


    if( i_hash < p_vars[i_middle].i_hash )
    {
        return InsertInner( p_vars, i_middle, i_hash );
    }

    if( i_hash > p_vars[i_middle + 1].i_hash )
    {
        return i_middle + 1 + InsertInner(p_vars + i_middle + 1, i_count - i_middle - 1, i_hash );
    }

    return i_middle + 1;
}

/**
	@brief Variable 구조체의 String 값과 비교할 String 값이 같은 Variable 구조체의 다음 Index 값을 구하는 함수
	@details Variable 구조체의 String 값과 입력받은 String 값을 비교하여 값이 같은 Variable 구조체의 다음 Index(Index + 1)를 리턴함
	@param[in] p_vars String 값을 가지고 있는 Variable 구조체
	@param[in] i_count String 값을 비교할 Variable 구조체의 Index
	@param[in] psz_name Hash 값으로 변환 할 String
	@return String이 같은 Variable 구조체의 Index 값
*/
static int Insert( variable_t *p_vars, int i_count, const char *psz_name )
{
    if( i_count == 0 )
    {
        return 0;
    }

    return InsertInner( p_vars, i_count, HashString( psz_name ) );
}

/**
	@brief Module Object의 부모 Module의 Value 구조체의 속성값을 설정하는 함수
	@details Module Object의 부모 Module이 없을 경우에 Value 구조체의 속성값을 설정함
	@param[in] p_this Value 구조체의 속성값을 설정 할 Module Object
	@param[in] psz_name Module Object의 최종 부모 노드가 가지고 있는 Module Configuration 구조체를 String Type 값을 바탕으로 찾기 위한 정보
	@param[in] p_val 설정할 Value 구조체
	@param[in] i_type Module Object에서 사용하는 자료형의 Type
	@return String이 같은 Variable 구조체의 Index 값
*/
static int InheritValue(module_object_t *p_this, const char *psz_name, value_t *p_val, int i_type )
{
    int i_var;
    variable_t *p_var;

	//Module Object의 부모 Module이 없을 경우에만 수행
    if( !p_this->p_parent )
    {
        switch( i_type & VAR_TYPE )
        {
        case VAR_STRING:
        case VAR_MODULE:
            p_val->psz_string = config_get_psz( p_this, psz_name );
            if( !p_val->psz_string ) p_val->psz_string = strdup("");
            break;
        case VAR_FLOAT:
            p_val->f_float = config_get_float( p_this, psz_name );
            break;
        case VAR_INTEGER:
            p_val->i_int = config_get_int( p_this, psz_name );
            break;
        case VAR_BOOL:
            p_val->b_bool = config_get_int( p_this, psz_name );
            break;
        default:
			return MODULE_ERROR_BADVAR;
            break;
        }

        return MODULE_SUCCESS;
    }

    mutex_lock( &p_this->p_parent->var_lock );

	//p_vars의 String 값과 psz_name이 같은 Variable 구조체의 Index 리턴 = i_var
    i_var = Lookup( p_this->p_parent->p_vars, p_this->p_parent->i_vars, psz_name);

	//i_var 찾았을 경우 p_var 값을 p_var의 val값으로 재할당 함
    if( i_var >= 0 )
    {
        p_var = &p_this->p_parent->p_vars[i_var];

        *p_val = p_var->val;

        p_var->pf_dup( p_val );

        mutex_unlock( &p_this->p_parent->var_lock );
        return MODULE_SUCCESS;
    }
	
	mutex_unlock( &p_this->p_parent->var_lock );

	//Module Object의 부모 모듈에 대한 함수 call
    return InheritValue( p_this->p_parent, psz_name, p_val, i_type );
}

/**
	@brief Module Object에 Variable 구조체를 생성하는 함수
	@details 
	@param[in] p_this Module Load를 찾기 위한 Module Object 구조체
	@param[in] psz_name Module Load가 가지고 있는 Module Configuration 구조체를 String Type 값을 바탕으로 찾기 위한 정보
	@param[in] i_type Module Object에서 사용하는 자료형의 Type
	@return String이 같은 Module Load의 Module Configuration 구조체의 String 값
*/
int var_create( module_object_t* p_this, const char* psz_name, int i_type )
{
	int i_new;
    variable_t *p_var = NULL;
    static module_list_t dummy_null_list = {0, NULL, NULL};

    mutex_lock( &p_this->var_lock );

	//p_this의 p_vars의 String 값과 psz_name이 같은 Variable 구조체의 Index 리턴 = i_new
	i_new = Lookup( p_this->p_vars, p_this->i_vars, psz_name );

	//i_new 찾았을 경우 p_var의 i_usage 값을 1 증가시키고 type을 VAR_ISCOMMAND(0x2000)과 OR 연산함
	if( i_new >= 0 )
    {
        if( (i_type & ~(VAR_DOINHERIT | VAR_ISCOMMAND)) != p_this->p_vars[i_new].i_type )
        {
            mutex_unlock( &p_this->var_lock );
            return MODULE_ERROR_BADVAR;
        }

        p_this->p_vars[i_new].i_usage++;
		if( i_type & VAR_ISCOMMAND ){
            p_this->p_vars[i_new].i_type |= VAR_ISCOMMAND;
		}

        mutex_unlock( &p_this->var_lock );
        return MODULE_SUCCESS;
    }

	//Variable 구조체의 String 값과 비교할 String 값이 같은 Variable 구조체의 다음 Index 값 = i_new
	i_new = Insert( p_this->p_vars, p_this->i_vars, psz_name );

    if( (p_this->i_vars & 15) == 15 )
    {
        p_this->p_vars = realloc( p_this->p_vars, (p_this->i_vars+17) * sizeof(variable_t));
    }

	//p_vars의 i_new+1번째에 p_var의 i_new를 i_vars-i_new만큼 복사함, i_new 번째에 새로운 Variable 구조체를 넣기 위해
	memmove( p_this->p_vars + i_new + 1,
             p_this->p_vars + i_new,
             (p_this->i_vars - i_new) * sizeof(variable_t) );

    p_this->i_vars++;

    p_var = &p_this->p_vars[i_new];
    memset( p_var, 0, sizeof(*p_var) );

    p_var->i_hash = HashString( psz_name );
    p_var->psz_name = strdup( psz_name );
    p_var->psz_text = NULL;

    p_var->i_type = i_type & ~VAR_DOINHERIT;
    memset( &p_var->val, 0, sizeof(value_t) );

    p_var->pf_dup = DupDummy;
    p_var->pf_free = FreeDummy;

    p_var->i_usage = 1;

    p_var->i_default = -1;
    p_var->choices.i_count = 0;
    p_var->choices.p_values = NULL;
    p_var->choices_text.i_count = 0;
    p_var->choices_text.p_values = NULL;

    p_var->b_incallback = false;
    p_var->i_entries = 0;
    p_var->p_entries = NULL;

	switch( i_type & VAR_TYPE )
    {
		case VAR_BOOL:
            p_var->pf_cmp = CmpBool;
            p_var->val.b_bool = false;
            break;
        case VAR_INTEGER:
            p_var->pf_cmp = CmpInt;
            p_var->val.i_int = 0;
            break;
		case VAR_STRING:
        case VAR_MODULE:
        case VAR_VARIABLE:
            p_var->pf_cmp = CmpString;
            p_var->pf_dup = DupString;
            p_var->pf_free = FreeString;
            p_var->val.psz_string = "";
            break;
        case VAR_FLOAT:
            p_var->pf_cmp = CmpFloat;
            p_var->val.f_float = 0.0;
            break;
        case VAR_TIME:
            p_var->pf_cmp = CmpTime;
            p_var->val.i_time = 0;
            break;
        case VAR_ADDRESS:
            p_var->pf_cmp = CmpAddress;
            p_var->val.p_address = NULL;
            break;
		case VAR_MUTEX:
            p_var->pf_cmp = CmpAddress;
            p_var->pf_free = FreeMutex;
            p_var->val.p_address = malloc(sizeof(mutex_t));
            mutex_init((mutex_t*)p_var->val.p_address );
            break;
	}

	p_var->pf_dup( &p_var->val );

	if( i_type & VAR_DOINHERIT )
    {
        value_t val;

        if( InheritValue( p_this, psz_name, &val, p_var->i_type ) == MODULE_SUCCESS ) //여기서 값을 가지고 온다.
        {
            p_var->pf_free( &p_var->val );
            p_var->val = val;

            if( i_type & VAR_HASCHOICE )
            {
                p_var->i_default = 0;

                INSERT_ELEM( p_var->choices.p_values, p_var->choices.i_count,
                             0, val );
                INSERT_ELEM( p_var->choices_text.p_values,
                             p_var->choices_text.i_count, 0, val );
                p_var->pf_dup( &p_var->choices.p_values[0] );
                p_var->choices_text.p_values[0].psz_string = NULL;
            }
        }
    }

	mutex_unlock( &p_this->var_lock );
	return MODULE_SUCCESS;
}


static int GetUnused(module_object_t *p_this, const char *psz_name )
{
    int i_var, i_tries = 0;

    while( true )
    {
        i_var = Lookup( p_this->p_vars, p_this->i_vars, psz_name );
        if( i_var < 0 )
        {
			return MODULE_ERROR_BADVAR;
        }

        if( ! p_this->p_vars[i_var].b_incallback )
        {
            return i_var;
        }

        if( i_tries++ > 100 )
        {
            return MODULE_ERROR_TIMEOUT;
        }

        mutex_unlock( &p_this->var_lock );
        msleep( THREAD_SLEEP );
        mutex_lock( &p_this->var_lock );
    }
}


int var_destroy( module_object_t* p_this, const char* psz_name )
{
    int i_var, i;
    variable_t *p_var;

    mutex_lock( &p_this->var_lock );

    i_var = GetUnused( p_this, psz_name );
    if( i_var < 0 )
    {
        mutex_unlock( &p_this->var_lock );
        return i_var;
    }

    p_var = &p_this->p_vars[i_var];

    if( p_var->i_usage > 1 )
    {
        p_var->i_usage--;
        mutex_unlock( &p_this->var_lock );
        return MODULE_SUCCESS;
    }

    p_var->pf_free( &p_var->val );

    if( p_var->choices.i_count )
    {
        for( i = 0 ; i < p_var->choices.i_count ; i++ )
        {
            p_var->pf_free( &p_var->choices.p_values[i] );
            FREE( p_var->choices_text.p_values[i].psz_string );
        }
        FREE( p_var->choices.p_values );
        FREE( p_var->choices_text.p_values );
    }

    if( p_var->p_entries )
    {
        FREE( p_var->p_entries );
    }

    FREE( p_var->psz_name );
    FREE( p_var->psz_text );

    memmove( p_this->p_vars + i_var,
             p_this->p_vars + i_var + 1,
             (p_this->i_vars - i_var - 1) * sizeof(variable_t) );

    if( (p_this->i_vars & 15) == 0 )
    {
        p_this->p_vars = realloc( p_this->p_vars, (p_this->i_vars) * sizeof( variable_t ) );
    }

    p_this->i_vars--;

    mutex_unlock( &p_this->var_lock );

    return MODULE_SUCCESS;
}

/**
	@brief Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 String Type 값을 얻어오는 함수
	@details Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 String 값을 리턴
	@param[in] p_this Module Load를 찾기 위한 Module Object 구조체
	@param[in] psz_name Module Load가 가지고 있는 Module Configuration 구조체를 String Type 값을 바탕으로 찾기 위한 정보
	@return String이 같은 Module Load의 Module Configuration 구조체의 String 값
*/
char* config_get_psz( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;
    char *psz_value = NULL;

	//Module Configuration 구조체 얻음
    p_config = config_find_config( p_this, psz_name );

	//안정성 체크
    /* sanity checks */
    if( !p_config )
    {
        return NULL;
    }
    if( (p_config->i_type!=CONFIG_ITEM_STRING) &&
        (p_config->i_type!=CONFIG_ITEM_MODULE) )
    {
        return NULL;
    }

    mutex_lock( p_config->p_lock );
    if( p_config->psz_value ) psz_value = strdup( p_config->psz_value );
    mutex_unlock( p_config->p_lock );

    return psz_value;
}

/**
	@brief Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 Float Type 값을 얻어오는 함수
	@details Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 Float Type 값을 리턴
	@param[in] p_this Module Load를 찾기 위한 Module Object 구조체
	@param[in] psz_name Module Load가 가지고 있는 Module Configuration 구조체를 String 값을 바탕으로 찾기 위한 정보
	@return String이 같은 Module Load의 Module Configuration 구조체의 Float Type 값
*/
float config_get_float( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;

	//Module Configuration 구조체 얻음
    p_config = config_find_config( p_this, psz_name );

    if( !p_config )
    {
        return -1;
    }
    if( p_config->i_type != CONFIG_ITEM_FLOAT )
	{
	}
        return -1;

    return p_config->f_value;
}

/**
	@brief Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 Integer Type 값을 얻어오는 함수
	@details Module Load가 가지고 있는 String값을 바탕으로 얻어온 Module Configuration 구조체가 가지고 있는 Integer Type 값을 리턴
	@param[in] p_this Module Load를 찾기 위한 Module Object 구조체
	@param[in] psz_name Module Load가 가지고 있는 Module Configuration 구조체를 String 값을 바탕으로 찾기 위한 정보
	@return String이 같은 Module Load의 Module Configuration 구조체의 Integer Type 값
*/
int config_get_int( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;

	//Module Configuration 구조체 얻음
    p_config = config_find_config( p_this, psz_name );

    /* sanity checks */
    if( !p_config )
    {
        return -1;
    }
    if( (p_config->i_type!=CONFIG_ITEM_INTEGER) &&
        (p_config->i_type!=CONFIG_ITEM_BOOL) )
    {
        return -1;
    }

    return p_config->i_value;
}

/**
	@brief Module Load가 가지고 있는 Module Configuration 구조체를 String값을 바탕으로 얻어오는 함수
	@details Module Object가 가지고 있는 Module Load에 대한 List를 얻어와 이름이 String 값과 같은 Module Load가 가지고 있는 Module Configuration의 Module Configuration 구조체를 리턴함
	@param[in] p_this Module Load를 찾기 위한 Module Object 구조체
	@param[in] psz_name Module Load가 가지고 있는 Module Configuration 구조체를 String 값을 바탕으로 찾기 위한 정보
	@return String이 같은 Module Load의 Module Configuration 구조체
*/
module_config_t* config_find_config( module_object_t* p_this, const char* psz_name )
{
    module_list_t *p_list = NULL;
    moduleload_t *p_parser = NULL;
    module_config_t *p_item = NULL;
    int i_index = -1;

    if( !psz_name ) return NULL;

	p_list = moduleListFind( p_this, OBJECT_MODULE_LOAD, FIND_ANYWHERE );

    for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
        p_parser = (moduleload_t *)p_list->p_values[i_index].p_object ;

        if( !p_parser->i_config_items )
            continue;

        for( p_item = p_parser->p_config;
             p_item->i_type != CONFIG_HINT_END;
             p_item++ )
        {
            if( p_item->i_type & CONFIG_HINT )
                continue;

			if(p_item->psz_name == NULL) continue;

            if( !strcmp( psz_name, p_item->psz_name ) )
            {
                moduleListRelease( p_list );
                return p_item;
            }
        }
    }

	moduleListRelease( p_list );

    return NULL;
}

int var_get( module_object_t* p_this, const char* psz_name, value_t* p_val )
{
    int i_var;
    variable_t *p_var;

    mutex_lock( &p_this->var_lock );

    i_var = Lookup( p_this->p_vars, p_this->i_vars, psz_name );

    if( i_var < 0 )
    {
        mutex_unlock( &p_this->var_lock );
		return MODULE_ERROR_BADVAR;
    }

    p_var = &p_this->p_vars[i_var];

    *p_val = p_var->val;
    p_var->pf_dup( p_val );

    mutex_unlock( &p_this->var_lock );

    return MODULE_SUCCESS;
}


int var_set( module_object_t* p_this, const char* psz_name, value_t val )
{
    int i_var;
    variable_t *p_var;
    value_t oldval;

    mutex_lock( &p_this->var_lock );

    i_var = GetUnused( p_this, psz_name );
    if( i_var < 0 )
    {
        mutex_unlock( &p_this->var_lock );
        return i_var;
    }

    p_var = &p_this->p_vars[i_var];

    p_var->pf_dup( &val );

    oldval = p_var->val;

    CheckValue( p_var, &val );

    p_var->val = val;

    if( p_var->i_entries )
    {
        int i_var;
        int i_entries = p_var->i_entries;
        callback_entry_t *p_entries = p_var->p_entries;

        p_var->b_incallback = true;
        mutex_unlock( &p_this->var_lock );

        for( ; i_entries-- ; )
        {
            p_entries[i_entries].pf_callback( p_this, psz_name, oldval, val, p_entries[i_entries].v_data );
        }

        mutex_lock( &p_this->var_lock );

        i_var = Lookup( p_this->p_vars, p_this->i_vars, psz_name );
        if( i_var < 0 )
        {
            mutex_unlock( &p_this->var_lock );
			return MODULE_ERROR_BADVAR;
        }

        p_var = &p_this->p_vars[i_var];
        p_var->b_incallback = false;
    }

    p_var->pf_free( &oldval );

    mutex_unlock( &p_this->var_lock );

	return MODULE_SUCCESS;
}

/**
	@brief variable_t 구조체와 value_t 구조체를 체크하는 함수
	@details Variable 
	@param[in] p_var variable_t 구조체를 가리키는 포인터
	@param[in] p_val value_t 구조체를 가리키는 포인터
*/
static void CheckValue (variable_t *p_var, value_t *p_val)
{
    if( p_var->i_type & VAR_HASCHOICE && p_var->choices.i_count )
    {
        int i;

        for( i = p_var->choices.i_count ; i-- ; )
        {
            if( p_var->pf_cmp( *p_val, p_var->choices.p_values[i] ) == 0 )
            {
                break;
            }
        }

        if( i < 0 )
        {
            p_var->pf_free( p_val );
            *p_val = p_var->choices.p_values[p_var->i_default >= 0
                                          ? p_var->i_default : 0 ];
            p_var->pf_dup( p_val );
        }
    }


    switch( p_var->i_type & VAR_TYPE )
    {
        case VAR_INTEGER:
            if( p_var->i_type & VAR_HASSTEP && p_var->step.i_int && (p_val->i_int % p_var->step.i_int) )
            {
                p_val->i_int = (p_val->i_int + (p_var->step.i_int / 2)) / p_var->step.i_int * p_var->step.i_int;
            }

            if( p_var->i_type & VAR_HASMIN && p_val->i_int < p_var->min.i_int )
            {
                p_val->i_int = p_var->min.i_int;
            }

            if( p_var->i_type & VAR_HASMAX && p_val->i_int > p_var->max.i_int )
            {
                p_val->i_int = p_var->max.i_int;
            }
            break;
        case VAR_FLOAT:
            if( p_var->i_type & VAR_HASSTEP && p_var->step.f_float )
            {
                float f_round = p_var->step.f_float * (float)(int)( 0.5 + p_val->f_float / p_var->step.f_float );
                if( p_val->f_float != f_round )
                {
                    p_val->f_float = f_round;
                }
            }

            if( p_var->i_type & VAR_HASMIN && p_val->f_float < p_var->min.f_float )
            {
                p_val->f_float = p_var->min.f_float;
            }

            if( p_var->i_type & VAR_HASMAX && p_val->f_float > p_var->max.f_float )
            {
                p_val->f_float = p_var->max.f_float;
            }
            break;
        case VAR_TIME:

            break;
    }
}
