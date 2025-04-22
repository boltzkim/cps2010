#include <core.hh>


namespace cps
{



////////////////////////////////////////////////

ReturnCode_t FooTypeSupport2::register_type(in_dds DomainParticipant2 participant, in_dds string type_name)
{
	return p_fooTypeSupport->register_type(p_fooTypeSupport, participant.getDomainParticipant(), type_name);
}

string FooTypeSupport2::get_type_name()
{
	return p_fooTypeSupport->get_type_name(p_fooTypeSupport);
}

bool FooTypeSupport2::insert_parameter(string parameter_name, SUPPORT_TYPE type, bool is_key)
{
	return p_fooTypeSupport->insert_parameter(p_fooTypeSupport, parameter_name, type, is_key);

	/*int size = 0;
	int offset_size = 0;

	switch(type)
	{
		case DDS_STRING_TYPE:
			size = sizeof(dds_string);
			offset_size = sizeof(dds_string);
			break;
		case DDS_BYTE_TYPE:
		case DDS_INTEGER8_TYPE:
		case DDS_UINTEGER8_TYPE:
			size = sizeof(int8_t);
			offset_size = 4;
			break;
		case DDS_INTEGER16_TYPE:
		case DDS_UINTEGER16_TYPE:
			size = sizeof(int16_t);
			offset_size = 4;
			break;
		case DDS_INTEGER32_TYPE:
		case DDS_UINTEGER32_TYPE:
			size = sizeof(int32_t);
			offset_size = 4;
			break;
		case DDS_INTEGER64_TYPE:
		case DDS_UINTEGER64_TYPE:
			size = sizeof(int64_t);
			offset_size = 8;
			break;
		case DDS_FLOAT_TYPE:
			size = sizeof(int32_t);
			offset_size = 4;
			break;
		case DDS_DOUBLE_TYPE:
			size = sizeof(int64_t);
			offset_size = 8;
			break;
		case DDS_BOOLEAN_TYPE:
			size = 1;
			offset_size = 4;
			break;
		case DDS_CHARACTER_TYPE:
			size = 1;
			offset_size = 4;
			break;
		case DDS_STRUCT_TYPE:
			trace_msg(NULL,TRACE_ERROR,"if you want struct type, use fuction insert_parameter_struct.");
			return false;
			break;
	}

	dds_parameter_t *p_para;

	if(is_key)
	{
		p_para = get_new_parameter_key(parameter_name, type, size);
	}else{
		p_para = get_new_parameter(parameter_name, type, size);
	}

	p_para->offset = offset_size;
	
	INSERT_PARAM(p_fooTypeSupport->pp_parameters,p_fooTypeSupport->i_parameters, p_fooTypeSupport->i_parameters, p_para);
	p_fooTypeSupport->offsetcount += p_para->offset;
	p_fooTypeSupport->i_size += p_para->i_size;*/
	//return true;
}

bool FooTypeSupport2::insert_parameter_struct(string parameter_name, int size, bool is_key)
{


	return p_fooTypeSupport->insert_parameter_struct(p_fooTypeSupport, parameter_name, size, is_key);

	/*dds_parameter_t *p_para;
	int offset_size = size;

	if(is_key)
	{
		p_para = get_new_parameter_key(parameter_name, DDS_STRUCT_TYPE, size);
	}else{
		p_para = get_new_parameter(parameter_name, DDS_STRUCT_TYPE, size);
	}

	if(size % 4)
	{
		offset_size += (4 - (size % 4));
	}

	p_para->offset = offset_size;


	INSERT_PARAM(p_fooTypeSupport->pp_parameters,p_fooTypeSupport->i_parameters, p_fooTypeSupport->i_parameters, p_para);

	return true;*/
}

void FooTypeSupport2::setFootypeSize(int size)
{
	p_fooTypeSupport->i_size = size;
}

////////////////////////////////////////////////



FooRet Foo2::setString(string parameter, string value)
{

	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_STRING_TYPE) return TYPE_WRONG;

			uint8_t *p_offset = (uint8_t *)p_tsFoo;

			set_string((dds_string*)(p_offset+offset), value);
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

//FooRet Foo2::setInt(string parameter, int value)
//{
//	dds_parameter_t *p_para;
//	int offset = 0;
//
//
//	if(hasTypesupport == false)
//	{
//		return FooTypeSupport2_NOT_EXIST;
//	}
//
//	if(p_tsFoo == NULL)
//	{
//		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
//		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
//	}
//
//	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
//	{
//		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];
//
//		if(!strcmp(parameter, p_para->p_field_name))
//		{
//			if(p_para->type != DDS_INTEGER32_TYPE) return TYPE_WRONG;
//			uint8_t *p_offset = (uint8_t *)p_tsFoo;
//
//
//			int *p_value = (int *)(p_offset+offset);
//
//			*p_value = value;
//			
//			return OK;
//		}
//
//		offset += p_para->i_size;
//	}
//
//	return PARAMETER_NOT_EXIST;
//}

FooRet Foo2::setByte(string parameter, int8_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_BYTE_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			int8_t *p_value = (int8_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setInt8(string parameter, int8_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER8_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			int8_t *p_value = (int8_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setInt16(string parameter, int16_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER16_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			int16_t *p_value = (int16_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setInt32(string parameter, int32_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER32_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			int *p_value = (int32_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setInt64(string parameter, int64_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER64_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			int64_t *p_value = (int64_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}


FooRet Foo2::setUint8(string parameter, uint8_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER8_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			uint8_t *p_value = (uint8_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setUint16(string parameter, uint16_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER16_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			uint16_t *p_value = (uint16_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setUint32(string parameter, uint32_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER32_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			uint32_t *p_value = (uint32_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setUint64(string parameter, uint64_t value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER64_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			uint64_t *p_value = (uint64_t *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setDouble(string parameter, double value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_DOUBLE_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			double *p_value = (double *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}

FooRet Foo2::setBoolean(string parameter, bool value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_BOOLEAN_TYPE) return TYPE_WRONG;
			uint8_t *p_offset = (uint8_t *)p_tsFoo;


			bool *p_value = (bool *)(p_offset+offset);

			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}


FooRet Foo2::setFloat(string parameter, float value)
{
	dds_parameter_t *p_para;
	int offset = 0;


	if(hasTypesupport == false)
	{
		return FooTypeSupport2_NOT_EXIST;
	}

	if(p_tsFoo == NULL)
	{
		p_tsFoo = (Foo *)malloc(typysupport.getFooTypeSupport()->i_size);
		memset(p_tsFoo, 0, typysupport.getFooTypeSupport()->i_size);
	}

	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_FLOAT_TYPE) return TYPE_WRONG;

			uint8_t *p_offset = (uint8_t *)p_tsFoo;

			float *p_value = (float *)(p_offset+offset);
			*p_value = value;
			
			return OK;
		}

		offset += p_para->i_size;
	}

	return PARAMETER_NOT_EXIST;
}


string Foo2::getString(string parameter)
{

	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return NULL;
	}

	if(p_foo == NULL)
	{
		return NULL;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_STRING_TYPE) return NULL;
			uint8_t *p_offset = (uint8_t *)p_foo;
			dds_string *p_dds_string = (dds_string *)(p_offset+offset);

			return p_dds_string->value;
			
		}

		offset += p_para->i_size;
	}

	return NULL;
}

//int Foo2::getInt(string parameter)
//{
//	dds_parameter_t *p_para;
//	int offset = 0;
//
//	if(hasTypesupport == false)
//	{
//		return 0;
//	}
//
//	if(p_foo == NULL)
//	{
//		return 0;
//	}
//
//
//	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
//	{
//		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];
//
//		if(!strcmp(parameter, p_para->p_field_name))
//		{
//			if(p_para->type != DDS_INTEGER32_TYPE) return 0;
//			uint8_t *p_offset = (uint8_t *)p_foo;
//			int *p_value = (int *)(p_offset+offset);
//
//			return *p_value;
//			
//		}
//
//		offset += p_para->i_size;
//	}
//
//	return 0;
//}

int8_t Foo2::getByte(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_BYTE_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			int8_t *p_value = (int8_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

int8_t Foo2::getInt8(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER8_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			int8_t *p_value = (int8_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

int16_t Foo2::getInt16(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER16_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			int16_t *p_value = (int16_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}


int32_t Foo2::getInt32(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER32_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			int32_t *p_value = (int32_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

int64_t Foo2::getInt64(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_INTEGER64_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			int64_t *p_value = (int64_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}


uint8_t Foo2::getUint8(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER8_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			uint8_t *p_value = (uint8_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

uint16_t Foo2::getUint16(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER16_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			uint16_t *p_value = (uint16_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

uint32_t Foo2::getUint32(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER32_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			uint32_t *p_value = (uint32_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

uint64_t Foo2::getUint64(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_UINTEGER64_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			uint64_t *p_value = (uint64_t *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

double Foo2::getDouble(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_DOUBLE_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			double *p_value = (double *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}

bool Foo2::getBoolean(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return false;
	}

	if(p_foo == NULL)
	{
		return false;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_BOOLEAN_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			bool *p_value = (bool *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return false;
}



float Foo2::getFloat(string parameter)
{
	dds_parameter_t *p_para;
	int offset = 0;

	if(hasTypesupport == false)
	{
		return 0;
	}

	if(p_foo == NULL)
	{
		return 0;
	}


	for(int i=0; i < typysupport.getFooTypeSupport()->i_parameters; i++)
	{
		p_para = typysupport.getFooTypeSupport()->pp_parameters[i];

		if(!strcmp(parameter, p_para->p_field_name))
		{
			if(p_para->type != DDS_FLOAT_TYPE) return 0;
			uint8_t *p_offset = (uint8_t *)p_foo;
			float *p_value = (float *)(p_offset+offset);

			return *p_value;
			
		}

		offset += p_para->i_size;
	}

	return 0;
}


Foo *Foo2::getFoo()
{



	if(p_tsFoo)
	{
		return p_tsFoo;
	}

	return p_foo;
}


//////////////////////////////////////////////

}

