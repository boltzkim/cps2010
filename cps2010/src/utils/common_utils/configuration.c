/**
	@file configuration.c
	@date 2013.04.01
	@author ETRI
	@version 1.0
	@brief �ش������� ȯ�� ���� ������ �о ó���ϴ� ����� ����
	@details ȯ�� ���� ������ �о ó���ϴ� �κ�
*/
#include <core.h>

static void processConfiguration(FILE *p_file, module_t *p_module);
static void CheckValue(variable_t *p_var, value_t *p_val);

/**
	@brief Module Object�� ���� ȯ�� �����ϴ� �Լ�
	@details 'cps.config' ���Ͽ��� ������ ���� �о�� Module Object ȯ�� ����
	@param[in] p_module Module Object�� ����Ű�� ������
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
	@brief ���Ͽ��� ������ Configuration ������ Module Configuration ���� �����ϴ� �Լ�
	@details ���Ͽ��� ������ ���� �о�� Configure Type�� ���� Module Object���� ã�� Module Load ����ü�� ���� Module Configure ����ü�� ���� ������
	@param[in] p_file File�� ����Ű�� ������
	@param[in] p_module Module Object�� ����Ű�� ������
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

		//������ ������ �����͸� �ű�
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

							//���ڿ��� ���ڷ� �ٲ���
							p_item->i_value = strtol( p_value, 0, 0 );
							break;
						case CONFIG_ITEM_FLOAT:
							if( !*p_value ) break;

							//���ڿ� �μ��� �ش��ϴ� double�� ��ȯ
							p_item->f_value = (float) atof( p_value);
							break;
						default:
							mutex_lock(p_item->p_lock);

							if( p_item->psz_value )
								FREE( p_item->psz_value );

							//���ο� �޸𸮸� �Ҵ��ϰ� ���ڿ��� ������ �� �� �����͸� ����
							//������ string�� �ʿ䰡 ���� ��� free�Լ��� strdup�� �Ҵ��� �޸𸮸� �ݵ�� ������ �־�� ��
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
	@brief Module Configure�� Module Load�� �����ϴ� �Լ�
	@details �ܺο��� �Է� ���� Module Configure�� Module Load�� ������
	@param[in] p_moduleload Module Load ����ü�� ����Ű�� ������
	@param[in] p_orig Module Load�� ������ ���� Module Configure ����ü�� ����Ű�� ������
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
	@brief Module Load�� ������ �ִ� Module Configure�� �����ϴ� �Լ�
	@details Module Load�� ������ �ִ� Module Configure�� ���� �޸� ���� ����
	@param[in] p_moduleload Module Configure�� ���� �� Module Load ����ü�� ����Ű�� ������
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
	@brief value_t ����ü �� Bool �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� Boolean ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpBool(value_t v, value_t w)
{ 
	//v.b_bool�� w.b_bool�� true�� 0�� �����ϰ�
	//v.b_bool�� true �̰� w.b_bool�� false�̸� 1�� �����ϰ�
	//v.b_bool�� false�̰� w.b_bool�� true�̸� -1�� �����ϰ�
	//�� �̿��� ��쿡�� 0�� ������
	return v.b_bool ? w.b_bool ? 0 : 1 : w.b_bool ? -1 : 0;
}

/**
	@brief value_t ����ü �� Int �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� Integer ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpInt(value_t v, value_t w)
{
	//v.i_int�� w.i_int�� ������ 0�� �����ϰ�
	//v.i_int�� w.i_int���� ũ�� 1�� �����ϰ�
	//�� �̿��� ��쿡�� -1�� ������
	return v.i_int == w.i_int ? 0 : v.i_int > w.i_int ? 1 : -1;
}

/**
	@brief value_t ����ü �� Time �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� Time ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpTime(value_t v, value_t w)
{
	//v.i_int�� w.i_int�� ������ 0�� �����ϰ�
	//v.i_int�� w.i_int���� ũ�� 1�� �����ϰ�
	//�� �̿��� ��쿡�� -1�� ������
    return v.i_time == w.i_time ? 0 : v.i_time > w.i_time ? 1 : -1;
}

/**
	@brief value_t ����ü �� String �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� String ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpString(value_t v, value_t w)
{ 
	//v.psz_string�� w.psz_string ���� ������ 0�� �����ϰ�
	//v.psz_string�� w.psz_string ���� ũ�� 1�� �����ϰ�
	//v.psz_string�� w.psz_string ���� ������ -1�� ������
	return strcmp( v.psz_string, w.psz_string );
}

/**
	@brief value_t ����ü �� Float �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� Float ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpFloat(value_t v, value_t w)
{
	//v.f_float�� w.f_float�� ������ 0�� �����ϰ�
	//v.f_float�� w.f_float���� ũ�� 1�� �����ϰ�
	//�� �̿��� ��쿡�� -1�� ������
	return v.f_float == w.f_float ? 0 : v.f_float > w.f_float ? 1 : -1;
}

/**
	@brief value_t ����ü �� Address �ڷ����� ��ġ�ϴ� �� ���ϴ� �Լ�
	@details value_t ����ü�� ������ �ִ� Address ���� ���� ��
	@param[in] v ���� value_t ����ü
	@param[in] w ���� value_t ����ü
	@return ���� int �� ��� ��
*/
static int CmpAddress(value_t v, value_t w)
{
	//v.p_address�� w.p_address�� ������ 0�� �����ϰ�
	//v.p_address�� w.p_address���� ũ�� 1�� �����ϰ�
	//�� �̿��� ��쿡�� -1�� ������
	return v.p_address == w.p_address ? 0 : v.p_address > w.p_address ? 1 : -1;
}

/**
	@brief value_t ����ü�� Void ���·� ��ȯ�ϴ� �Լ�
	@details value_t ����ü�� Void ���·� Ÿ�� ĳ���� ��
	@param[in] p_val value_t ����ü�� ����Ű�� ������
	@return value_t ����ü�� Ÿ�� ĳ���� �� void �� ��� ��
*/
static void DupDummy(value_t *p_val) 
{ 
	(void)p_val;
}

/**
	@brief �ڽ��� value_t ����ü�� ������ �ִ� psz_string ���� ���ο� ������ �Ҵ��ϴ� �Լ�
	@details �ڽ��� value_t ����ü �� psz_string ���� ���ο� �ּҿ� ���� ���ڿ� �����͸� ������
	@param[in] p_val value_t ����ü�� ����Ű�� ������
*/
static void DupString(value_t *p_val)
{ 
	p_val->psz_string = strdup( p_val->psz_string );
}

/**
	@brief value_t ����ü�� Void ���·� ��ȯ�ϴ� �Լ�
	@details value_t ����ü�� Void ���·� Ÿ�� ĳ���� ��
	@param[in] p_val value_t ����ü�� ����Ű�� ������
	@return value_t ����ü�� Ÿ�� ĳ���� �� void �� ��� ��
*/
static void FreeDummy(value_t *p_val)
{ 
	(void)p_val;
}

/**
	@brief value_t ����ü �� psz_string ���� �޸� �����ϴ� �Լ�
	@details value_t ����ü �� psz_string ���� �޸� ���� ��
	@param[in] p_val value_t ����ü�� ����Ű�� ������
*/
static void FreeString(value_t *p_val) 
{ 
	FREE( p_val->psz_string );
}

/**
	@brief value_t ����ü �� address ���� �޸� �����ϴ� �Լ�
	@details value_t ����ü �� address ���� ���� mutex_t ����ü�� ����ȯ �Ͽ� Mutex�� �����ϰ� �޸� ���� ��
	@param[in] p_val value_t ����ü�� ����Ű�� ������
*/
static void FreeMutex(value_t *p_val )
{ 
	mutex_destroy((mutex_t*)p_val->p_address );
	FREE(p_val->p_address);
}

/**
	@brief String ���� Hash ������ ��ȯ�ϴ� �Լ�
	@details 4byte �̻� �� ���ڿ��� 4byte�� hash ������ �ٲ�
	@param[in] psz_string Hash ������ ��ȯ �� String
	@return String ���� Hash ������ ��ȯ�� ��
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
	@brief Variable ����ü���� ���� Hash ���� ������� ��ġ�� �ִ��� Variable ����ü�� Index�� ã�� �Լ�
	@details Variable ����ü�� Hash ���� ���� Hash ���� ���Ͽ� ù��° Variable ����ü�� hash ������ ���� Hash ���� ������ 0�� �����ϰ� 
	Variable ����ü�� Hash ���� �Է¹��� Hash ���� ũ�� �� Variable ����ü�� Index�� ������
	@param[in] p_vars Hash ���� ������ �ִ� Variable ����ü
	@param[in] i_count Hash ���� ���� Variable ����ü�� Index
	@param[in] i_hash ���� Hash ��
	@return �˻��� Hash ��
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
	@brief Variable ����ü�� String ���� ���� String ���� ���� Variable ����ü�� Index ���� ���ϴ� �Լ�
	@details Variable ����ü�� String ���� �Է¹��� String ���� ���Ͽ� ���� ���� Variable ����ü�� Index�� ������
	@param[in] p_vars String ���� ������ �ִ� Variable ����ü
	@param[in] i_count String ���� ���� Variable ����ü�� Index
	@param[in] psz_name Hash ������ ��ȯ �� String
	@return String�� ���� Variable ����ü�� Index ��
*/
static int Lookup( variable_t *p_vars, int i_count, const char *psz_name )
{
    uint32_t i_hash;
    int i, i_pos;

    if( i_count == 0 )
    {
        return -1;
    }

	//String ���� Hash ������ ��ȯ
    i_hash = HashString( psz_name );

	//Hash ���� �̿��Ͽ� Variable ����ü�� Index�� ã��
    i_pos = LookupInner( p_vars, i_count, i_hash );

    if( i_hash != p_vars[i_pos].i_hash )
    {
        return -1;
    }

	//String ���� Variable�� Index ��°�� String ���� ���� ��� Index���� ����
    if( !strcmp( psz_name, p_vars[i_pos].psz_name ) )
    {
        return i_pos;
    }

	//Index �� ���� �Ʒ��� For ���� ���鼭 Hash ���� ������ ������ ���Ͽ� �������� ã���� ��� �� Index�� ����
    for( i = i_pos - 1 ; i > 0 && i_hash == p_vars[i].i_hash ; i-- )
    {
        if( !strcmp( psz_name, p_vars[i].psz_name ) )
        {
            return i;
        }
    }

	//Index �� ���� ���� For ���� ���鼭 Hash ���� ������ ������ ���Ͽ� �������� ã���� ��� �� Index�� ����
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
	@brief Variable ����ü���� ���� Hash ���� ������� ��ġ�� �ִ��� Variable ����ü�� ���� Index�� ã�� �Լ�
	@details Variable ����ü�� Hash ���� ���� Hash ���� ���Ͽ� ù��° Variable ����ü�� hash ������ ���� Hash ���� ������ 0�� �����ϰ� 
	Variable ����ü�� Hash ���� �Է¹��� Hash ���� ũ�� �� Variable ����ü�� ���� Index(Index+1)�� ������
	@param[in] p_vars Hash ���� ������ �ִ� Variable ����ü
	@param[in] i_count Hash ���� ���� Variable ����ü�� Index
	@param[in] i_hash ���� Hash ��
	@return �˻��� Hash ��
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
	@brief Variable ����ü�� String ���� ���� String ���� ���� Variable ����ü�� ���� Index ���� ���ϴ� �Լ�
	@details Variable ����ü�� String ���� �Է¹��� String ���� ���Ͽ� ���� ���� Variable ����ü�� ���� Index(Index + 1)�� ������
	@param[in] p_vars String ���� ������ �ִ� Variable ����ü
	@param[in] i_count String ���� ���� Variable ����ü�� Index
	@param[in] psz_name Hash ������ ��ȯ �� String
	@return String�� ���� Variable ����ü�� Index ��
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
	@brief Module Object�� �θ� Module�� Value ����ü�� �Ӽ����� �����ϴ� �Լ�
	@details Module Object�� �θ� Module�� ���� ��쿡 Value ����ü�� �Ӽ����� ������
	@param[in] p_this Value ����ü�� �Ӽ����� ���� �� Module Object
	@param[in] psz_name Module Object�� ���� �θ� ��尡 ������ �ִ� Module Configuration ����ü�� String Type ���� �������� ã�� ���� ����
	@param[in] p_val ������ Value ����ü
	@param[in] i_type Module Object���� ����ϴ� �ڷ����� Type
	@return String�� ���� Variable ����ü�� Index ��
*/
static int InheritValue(module_object_t *p_this, const char *psz_name, value_t *p_val, int i_type )
{
    int i_var;
    variable_t *p_var;

	//Module Object�� �θ� Module�� ���� ��쿡�� ����
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

	//p_vars�� String ���� psz_name�� ���� Variable ����ü�� Index ���� = i_var
    i_var = Lookup( p_this->p_parent->p_vars, p_this->p_parent->i_vars, psz_name);

	//i_var ã���� ��� p_var ���� p_var�� val������ ���Ҵ� ��
    if( i_var >= 0 )
    {
        p_var = &p_this->p_parent->p_vars[i_var];

        *p_val = p_var->val;

        p_var->pf_dup( p_val );

        mutex_unlock( &p_this->p_parent->var_lock );
        return MODULE_SUCCESS;
    }
	
	mutex_unlock( &p_this->p_parent->var_lock );

	//Module Object�� �θ� ��⿡ ���� �Լ� call
    return InheritValue( p_this->p_parent, psz_name, p_val, i_type );
}

/**
	@brief Module Object�� Variable ����ü�� �����ϴ� �Լ�
	@details 
	@param[in] p_this Module Load�� ã�� ���� Module Object ����ü
	@param[in] psz_name Module Load�� ������ �ִ� Module Configuration ����ü�� String Type ���� �������� ã�� ���� ����
	@param[in] i_type Module Object���� ����ϴ� �ڷ����� Type
	@return String�� ���� Module Load�� Module Configuration ����ü�� String ��
*/
int var_create( module_object_t* p_this, const char* psz_name, int i_type )
{
	int i_new;
    variable_t *p_var = NULL;
    static module_list_t dummy_null_list = {0, NULL, NULL};

    mutex_lock( &p_this->var_lock );

	//p_this�� p_vars�� String ���� psz_name�� ���� Variable ����ü�� Index ���� = i_new
	i_new = Lookup( p_this->p_vars, p_this->i_vars, psz_name );

	//i_new ã���� ��� p_var�� i_usage ���� 1 ������Ű�� type�� VAR_ISCOMMAND(0x2000)�� OR ������
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

	//Variable ����ü�� String ���� ���� String ���� ���� Variable ����ü�� ���� Index �� = i_new
	i_new = Insert( p_this->p_vars, p_this->i_vars, psz_name );

    if( (p_this->i_vars & 15) == 15 )
    {
        p_this->p_vars = realloc( p_this->p_vars, (p_this->i_vars+17) * sizeof(variable_t));
    }

	//p_vars�� i_new+1��°�� p_var�� i_new�� i_vars-i_new��ŭ ������, i_new ��°�� ���ο� Variable ����ü�� �ֱ� ����
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

        if( InheritValue( p_this, psz_name, &val, p_var->i_type ) == MODULE_SUCCESS ) //���⼭ ���� ������ �´�.
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
	@brief Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� String Type ���� ������ �Լ�
	@details Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� String ���� ����
	@param[in] p_this Module Load�� ã�� ���� Module Object ����ü
	@param[in] psz_name Module Load�� ������ �ִ� Module Configuration ����ü�� String Type ���� �������� ã�� ���� ����
	@return String�� ���� Module Load�� Module Configuration ����ü�� String ��
*/
char* config_get_psz( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;
    char *psz_value = NULL;

	//Module Configuration ����ü ����
    p_config = config_find_config( p_this, psz_name );

	//������ üũ
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
	@brief Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� Float Type ���� ������ �Լ�
	@details Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� Float Type ���� ����
	@param[in] p_this Module Load�� ã�� ���� Module Object ����ü
	@param[in] psz_name Module Load�� ������ �ִ� Module Configuration ����ü�� String ���� �������� ã�� ���� ����
	@return String�� ���� Module Load�� Module Configuration ����ü�� Float Type ��
*/
float config_get_float( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;

	//Module Configuration ����ü ����
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
	@brief Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� Integer Type ���� ������ �Լ�
	@details Module Load�� ������ �ִ� String���� �������� ���� Module Configuration ����ü�� ������ �ִ� Integer Type ���� ����
	@param[in] p_this Module Load�� ã�� ���� Module Object ����ü
	@param[in] psz_name Module Load�� ������ �ִ� Module Configuration ����ü�� String ���� �������� ã�� ���� ����
	@return String�� ���� Module Load�� Module Configuration ����ü�� Integer Type ��
*/
int config_get_int( module_object_t* p_this, const char* psz_name )
{
    module_config_t *p_config;

	//Module Configuration ����ü ����
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
	@brief Module Load�� ������ �ִ� Module Configuration ����ü�� String���� �������� ������ �Լ�
	@details Module Object�� ������ �ִ� Module Load�� ���� List�� ���� �̸��� String ���� ���� Module Load�� ������ �ִ� Module Configuration�� Module Configuration ����ü�� ������
	@param[in] p_this Module Load�� ã�� ���� Module Object ����ü
	@param[in] psz_name Module Load�� ������ �ִ� Module Configuration ����ü�� String ���� �������� ã�� ���� ����
	@return String�� ���� Module Load�� Module Configuration ����ü
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
	@brief variable_t ����ü�� value_t ����ü�� üũ�ϴ� �Լ�
	@details Variable 
	@param[in] p_var variable_t ����ü�� ����Ű�� ������
	@param[in] p_val value_t ����ü�� ����Ű�� ������
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
