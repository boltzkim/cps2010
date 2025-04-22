#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <assert.h>



typedef struct expression_value
{
	int type;
	union{
		long longer;
	}value;
}expression_value;

expression_function_t *alloc_function_list(function_type f_type, expression_list_t *p_list)
{
	expression_function_t *p_ex_function = (expression_function_t *)malloc(sizeof(expression_function_t));
	p_ex_function->f_type = f_type;
	p_ex_function->p_list = p_list;
	return p_ex_function;
}



expression_t *expression_newFunction(function_type f_type, expression_list_t *p_list)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));

    p_expr->kind = FUNCTION_KIND;
    p_expr->value.function = alloc_function_list(f_type, p_list);
    return p_expr;
}

expression_list_t *expression_insert(expression_list_t *list, expression_t *p_expr)
{
    expression_list_t *head;
    assert(p_expr != NULL);
    head = (expression_list_t *)malloc(sizeof(expression_list_t));
    head->next = list;
	head->p_expr = p_expr;
    return head;
}

expression_t *expression_newIdendifier(char *string)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));
    p_expr->kind = IDENTIFIER_KIND;
    p_expr->value.string = strdup(string);
    return p_expr;
}

expression_t *expression_newString(char *string)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));
    p_expr->kind = STRING_KIND;
    p_expr->value.string = (char*)malloc(strlen(string));
    return p_expr;
}

char *get_yytext();

expression_t *expression_newInteger(long value)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));
	char *yytext = get_yytext();
	if((yytext-1)[0] == '%')
	{
		p_expr->kind = INTEGER_PARAMETER_KIND;
	}else{
		p_expr->kind = INTEGER_KIND;
	}
	
    p_expr->value.integer = value;
    return p_expr;
}

expression_t *expression_newDouble(double value)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));
    p_expr->kind = FLOAT_KIND;
    p_expr->value.real = value;
    return p_expr;
}

expression_t *expression_newCharacter(char value)
{
    expression_t *p_expr = (expression_t *)malloc(sizeof(expression_t));
    p_expr->kind = CHARACTER_KIND;
    p_expr->value.character = value;
    return p_expr;
}

expression_list_t *expression_append(expression_list_t *list, expression_t *p_expr)
{
    expression_list_t **ptr;
    assert(p_expr != NULL);
    if (list == NULL) {
        list = (expression_list_t *)malloc(sizeof(expression_list_t));
        list->next = NULL;
        list->p_expr = p_expr;
    } else {
        for (ptr = &list->next; *ptr != NULL; ptr = &(*ptr)->next);
        *ptr = (expression_list_t *)malloc(sizeof(expression_list_t));
        (*ptr)->next = NULL;
        (*ptr)->p_expr = p_expr;
    }
    return list;
}




void expression_print(expression_t *p_expr, long tab)
{
	int i;
	expression_list_t	*next = NULL;
	expression_t type;
	assert(p_expr != NULL);
	type.kind = INTEGER_KIND;


	
	for(i=0; i < tab; i++)
	{
		printf("\t");
	}

	if(p_expr->kind == STRING_KIND)
	{
		printf("STRING_KIND : %s", p_expr->value.string);
	}

	if(p_expr->kind == IDENTIFIER_KIND)
	{
		printf("IDENTIFIER_KIND : %s", p_expr->value.string);
	}

	if(p_expr->kind == INTEGER_KIND)
	{
		printf("INTEGER_KIND : %ld", p_expr->value.integer);
	}

	if(p_expr->kind == INTEGER_PARAMETER_KIND)
	{
		printf("INTEGER_PARAMETER_KIND : %ld", p_expr->value.integer);
	}
	
	if(p_expr->kind == FLOAT_KIND)
	{
		printf("INTEGER_KIND : %f", p_expr->value.real);
	}

	if(p_expr->kind == CHARACTER_KIND)
	{
		printf("CHARACTER_KIND : %c", p_expr->value.character);
	}

	if(p_expr->kind == FUNCTION_KIND)
	{
		switch(p_expr->value.function->f_type)
		{
		case FilterExpression_TYPE:
			printf("FilterExpression_TYPE");
			break;
		case TopicExpression_TYPE:
			printf("TopicExpression_TYPE");
			break;
		case QueryExpression_TYPE:
			printf("QueryExpression_TYPE");
			break;
		case SELECT_TYPE:
			printf("SELECT_TYPE");
			break;
		case Aggregation_TYPE:
			printf("Aggregation_TYPE");
			break;
		case SubjectFieldSpec_AS_TYPE:
			printf("SubjectFieldSpec_AS_TYPE");
			break;
		case SubjectFieldSpec_TYPE:
			printf("SubjectFieldSpec_TYPE");
			break;
		case Selection_NaturalJoin_TYPE:
			printf("Selection_NaturalJoin_TYPE");
			break;
		case JoinItem_TYPE:
			printf("JoinItem_TYPE");
			break;
		case NaturalJoin_TYPE:
			printf("NaturalJoin_TYPE");
			break;
		case WHERE_TYPE:
			printf("WHERE_TYPE");
			break;
		case OR_TYPE:
			printf("OR_TYPE");
			break;
		case ComparisonPredicate_TYPE:
			printf("ComparisonPredicate_TYPE");
			break;
		case BetweenPredicate_TYPE:
			printf("BetweenPredicate_TYPE");
			break;
		case BetweenPredicate_NOT_TYPE:
			printf("BetweenPredicate_NOT_TYPE");
			break;
		case GREATEREQUAL_TYPE:
			printf("GREATEREQUAL_TYPE");
			break;
		case LESSEQUAL_TYPE:
			printf("LESSEQUAL_TYPE");
			break;
		case NOTEQUAL_TYPE:
			printf("NOTEQUAL_TYPE");
			break;
		case EQUAL_TYPE:
			//expression_function_equal(
			printf("EQUAL_TYPE");
			break;
		case GREATER_TYPE:
			printf("GREATER_TYPE");
			break;
		case LESS_TYPE:
			printf("LESS_TYPE");
			break;
		case LIKE_TYPE:
			printf("LIKE_TYPE");
			break;
		case AND_TYPE:
			printf("AND_TYPE");
			break;
		case NOT_TYPE:
			printf("NOT_TYPE");
			break;
		case Range_TYPE:
			printf("Range_TYPE");
			break;
		case FIELDNAME_TYPE:
			printf("FIELDNAME_TYPE");
			break;
		case TOPICNAME_TYPE:
			printf("TOPICNAME_TYPE");
			break;
		case PARAMETER_TYPE:
			printf("PARAMETER_TYPE");
			break;
		}

		printf("  ");

		if(p_expr->value.function->p_list)
		{
			expression_print(p_expr->value.function->p_list->p_expr, tab);
		}

		

		printf("\r\n");
		
		next = p_expr->value.function->p_list->next;

		while(next)
		{
			expression_print(next->p_expr, ++tab);
			next = next->next;
		}

		
	}
}



void expression_check_field(expression_t *p_expr, FooTypeSupport *p_footypesupport)
{
	int i;
	expression_list_t	*next = NULL;
	expression_t type;
	assert(p_expr != NULL);
	assert(p_footypesupport != NULL);
	type.kind = INTEGER_KIND;



	if(p_expr->kind == STRING_KIND)
	{
		//printf("STRING_KIND : %s", p_expr->value.string);
	}

	if(p_expr->kind == IDENTIFIER_KIND)
	{
		//printf("IDENTIFIER_KIND : %s", p_expr->value.string);
		for(i =0; i < p_footypesupport->i_parameters;i++)
		{
			if(!strcmp(p_expr->value.string, p_footypesupport->pp_parameters[i]->p_field_name))
			{
				return;
			}
		}
		printf("Filed name is not exist.... Check topic support type.\r\n");
		assert(false);
	}

	if(p_expr->kind == INTEGER_KIND)
	{
		//printf("INTEGER_KIND : %d", p_expr->value.integer);
	}
	
	if(p_expr->kind == FLOAT_KIND)
	{
		//printf("INTEGER_KIND : %f", p_expr->value.real);
	}

	if(p_expr->kind == CHARACTER_KIND)
	{
		//printf("CHARACTER_KIND : %c", p_expr->value.character);
	}

	if(p_expr->kind == FUNCTION_KIND)
	{
		switch(p_expr->value.function->f_type)
		{
		case FilterExpression_TYPE:
			break;
		case TopicExpression_TYPE:
			break;
		case QueryExpression_TYPE:
			break;
		case SELECT_TYPE:
			break;
		case Aggregation_TYPE:
			break;
		case SubjectFieldSpec_AS_TYPE:
			break;
		case SubjectFieldSpec_TYPE:
			break;
		case Selection_NaturalJoin_TYPE:
			break;
		case JoinItem_TYPE:
			break;
		case NaturalJoin_TYPE:
			break;
		case WHERE_TYPE:
			break;
		case OR_TYPE:
			break;
		case ComparisonPredicate_TYPE:
			break;
		case BetweenPredicate_TYPE:
			break;
		case BetweenPredicate_NOT_TYPE:
			break;
		case GREATEREQUAL_TYPE:
			break;
		case LESSEQUAL_TYPE:
			break;
		case NOTEQUAL_TYPE:
			break;
		case EQUAL_TYPE:
			break;
		case GREATER_TYPE:
			break;
		case LESS_TYPE:
			break;
		case LIKE_TYPE:
			break;
		case AND_TYPE:
			break;
		case NOT_TYPE:
			break;
		case Range_TYPE:
			break;
		case FIELDNAME_TYPE:
			break;
		case TOPICNAME_TYPE:
			break;
		case PARAMETER_TYPE:
			break;
		}


		if(p_expr->value.function->p_list)
		{
			expression_check_field(p_expr->value.function->p_list->p_expr, p_footypesupport);
		}

		
		next = p_expr->value.function->p_list->next;

		while(next)
		{
			expression_check_field(next->p_expr, p_footypesupport);
			next = next->next;
		}

		
	}
}


expression_t expression_function_EQUAL(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer == b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real == b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character == b.value.character;
				break;
			case BOOLEAN_KIND:
				type.value.boolean = a.value.boolean == b.value.boolean;
				break;
			case STRING_KIND:
				type.value.boolean = !strcmp(a.value.string, b.value.string);
				break;
		}
	}

	return type;
}


expression_t expression_function_GREATEREQUAL(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer >= b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real >= b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character >= b.value.character;
				break;

		}
	}

	return type;
}


expression_t expression_function_LESSEQUAL(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer <= b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real <= b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character <= b.value.character;
				break;

		}
	}

	return type;
}


expression_t expression_function_NOTEQUAL(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer != b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real != b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character != b.value.character;
				break;

		}
	}

	return type;
}


expression_t expression_function_GREATER(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer > b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real > b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character > b.value.character;
				break;

		}
	}

	return type;
}

expression_t expression_function_LESS(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case INTEGER_KIND:
				type.value.boolean = a.value.integer < b.value.integer;
				break;
			case FLOAT_KIND:
				type.value.boolean = a.value.real < b.value.real;
				break;
			case CHARACTER_KIND:
				type.value.boolean = a.value.character < b.value.character;
				break;

		}
	}

	return type;
}

expression_t expression_function_LIKE(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case STRING_KIND:
				type.value.boolean = !strcmp(a.value.string, b.value.string);
				break;

		}
	}

	return type;
}


expression_t expression_function_AND(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case BOOLEAN_KIND:
				type.value.boolean = a.value.boolean & b.value.boolean;
				break;

		}
	}

	return type;
}


expression_t expression_function_OR(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case BOOLEAN_KIND:
				type.value.boolean = a.value.boolean | b.value.boolean;
				break;

		}
	}

	return type;
}

expression_t expression_function_NOT(expression_t a, expression_t b)
{
	expression_t type;
	type.kind = BOOLEAN_KIND;
	type.value.boolean = false;

	if(a.kind == b.kind)
	{
		switch(a.kind)
		{
			case BOOLEAN_KIND:
				type.value.boolean = a.value.boolean ^ b.value.boolean;
				break;

		}
	}

	return type;
}


typedef struct string_temp
{
	int32_t size;
	char dummy[1024];
}string_temp;


expression_t get_value_from_RTPS_to_TypeSupport(char* fieldname, FooTypeSupport *p_typeSupport, char *value, int i_size)
{
	int i;
	int32_t offset = 0;
	int32_t len = 0;
	dds_parameter_t *p_parameter;
	char *string_offset = NULL;
	string_temp temp_string;
	expression_t type;
	type.kind = UNDEFINED_KIND;
	
	
	
	for(i=0; i < p_typeSupport->i_parameters; i++)
	{
		p_parameter = p_typeSupport->pp_parameters[i];


		switch(p_parameter->type)
		{
			case DDS_STRING_TYPE:
				//temp_string.p_dummy = malloc(1024);
				//memset(temp_string.p_dummy,0,1024);
				string_offset =  &value[len+4];
				len += set_string_rtps_to_type_support((dds_string2*)(&temp_string), &value[len], p_parameter->i_size, i_size, offset);
				//FREE(temp_string.p_dummy);
				break;
			default:
				
				//len += p_parameter->i_size;
				break;
		}

		if(!strcmp(p_parameter->p_field_name, fieldname)){
			

			switch(p_parameter->type)
			{
				case DDS_STRING_TYPE:
					type.kind = STRING_KIND;
					type.value.string = string_offset;
					break;
				case DDS_BYTE_TYPE:
				case DDS_INTEGER8_TYPE:
				case DDS_INTEGER16_TYPE:
				case DDS_INTEGER32_TYPE:
				case DDS_INTEGER64_TYPE:
				case DDS_UINTEGER8_TYPE:
				case DDS_UINTEGER16_TYPE:
				case DDS_UINTEGER32_TYPE:
				case DDS_UINTEGER64_TYPE:
					type.kind = INTEGER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_FLOAT_TYPE:
				case DDS_DOUBLE_TYPE:
					type.kind = FLOAT_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_CHARACTER_TYPE:
				case DDS_BOOLEAN_TYPE:
					type.kind = CHARACTER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
			}

			return type;
		}


		if(p_parameter->type != DDS_STRING_TYPE)
		{
			len += p_parameter->i_size;
		}




		offset += p_parameter->i_size;
	}

	return type;
}

expression_t expression_caculate(expression_t *p_expr, FooTypeSupport *p_typeSupport, char *p_value, int i_size)
{
	expression_list_t	*next = NULL;
	expression_t type;
	expression_t typeA;
	expression_t typeB;

	assert(p_expr != NULL);
	assert(p_typeSupport != NULL);
	assert(p_value != NULL);
	assert(i_size > 0);

	type.kind = INTEGER_KIND;



	if(p_expr->kind == STRING_KIND)
	{
		//printf("STRING_KIND : %s", p_expr->value.string);
		type.kind = STRING_KIND;
		type.value.string = p_expr->value.string;
	}

	if(p_expr->kind == IDENTIFIER_KIND)
	{
	//	printf("IDENTIFIER_KIND : %s", p_expr->value.string);
		//
		type.kind = STRING_KIND;
		type.value.string = p_expr->value.string;
	}


	if(p_expr->kind == INTEGER_PARAMETER_KIND)
	{
		//printf("INTEGER_KIND : %d", p_expr->value.integer);
		type.kind = INTEGER_KIND;
		type.value.integer = p_expr->value.integer;
	}

	if(p_expr->kind == INTEGER_KIND)
	{
		//printf("INTEGER_KIND : %d", p_expr->value.integer);
		type.kind = INTEGER_KIND;
		type.value.integer = p_expr->value.integer;
	}
	
	if(p_expr->kind == FLOAT_KIND)
	{
		//printf("INTEGER_KIND : %f", p_expr->value.real);
		type.kind = FLOAT_KIND;
		type.value.real = p_expr->value.real;
	}

	if(p_expr->kind == CHARACTER_KIND)
	{
		//printf("CHARACTER_KIND : %c", p_expr->value.character);
		type.kind = CHARACTER_KIND;
		type.value.character = p_expr->value.character;
	}

	if(p_expr->kind == FUNCTION_KIND)
	{
		switch(p_expr->value.function->f_type)
		{
		case FilterExpression_TYPE:
			//printf("FilterExpression_TYPE");
			break;
		case TopicExpression_TYPE:
			//printf("TopicExpression_TYPE");
			break;
		case QueryExpression_TYPE:
			//printf("QueryExpression_TYPE");
			break;
		case SELECT_TYPE:
			//printf("SELECT_TYPE");
			break;
		case Aggregation_TYPE:
			//printf("Aggregation_TYPE");
			break;
		case SubjectFieldSpec_AS_TYPE:
			//printf("SubjectFieldSpec_AS_TYPE");
			break;
		case SubjectFieldSpec_TYPE:
			//printf("SubjectFieldSpec_TYPE");
			break;
		case Selection_NaturalJoin_TYPE:
			//printf("Selection_NaturalJoin_TYPE");
			break;
		case JoinItem_TYPE:
			//printf("JoinItem_TYPE");
			break;
		case NaturalJoin_TYPE:
			//printf("NaturalJoin_TYPE");
			break;
		case WHERE_TYPE:
			//printf("WHERE_TYPE");
			break;
		case OR_TYPE:
			//printf("OR_TYPE");
			break;
		case ComparisonPredicate_TYPE:
			//printf("ComparisonPredicate_TYPE");
			break;
		case BetweenPredicate_TYPE:
			//printf("BetweenPredicate_TYPE");
			break;
		case BetweenPredicate_NOT_TYPE:
			//printf("BetweenPredicate_NOT_TYPE");
			break;
		case GREATEREQUAL_TYPE:
			//printf("GREATEREQUAL_TYPE");
			break;
		case LESSEQUAL_TYPE:
			//printf("LESSEQUAL_TYPE");
			break;
		case NOTEQUAL_TYPE:
			//printf("NOTEQUAL_TYPE");
			break;
		case EQUAL_TYPE:
			//expression_function_equal(
			//printf("EQUAL_TYPE");
			break;
		case GREATER_TYPE:
			//printf("GREATER_TYPE");
			break;
		case LESS_TYPE:
			//printf("LESS_TYPE");
			break;
		case LIKE_TYPE:
			//printf("LIKE_TYPE");
			break;
		case AND_TYPE:
			//printf("AND_TYPE");
			break;
		case NOT_TYPE:
			//printf("NOT_TYPE");
			break;
		case Range_TYPE:
			//printf("Range_TYPE");
			break;
		case FIELDNAME_TYPE:
			//printf("FIELDNAME_TYPE");
			break;
		case TOPICNAME_TYPE:
			//printf("TOPICNAME_TYPE");
			break;
		case PARAMETER_TYPE:
			//printf("PARAMETER_TYPE");
			break;
		}

		//printf("  ");

		if(p_expr->value.function->p_list)
		{
			typeA = expression_caculate(p_expr->value.function->p_list->p_expr, p_typeSupport, p_value, i_size);
		}

		

		

		//printf("\r\n");
		
		next = p_expr->value.function->p_list->next;

		while(next)
		{
			typeB = expression_caculate(next->p_expr, p_typeSupport, p_value, i_size);
			next = next->next;
		}


		if(p_expr->kind == FUNCTION_KIND)
		{
			type = typeA;


			switch(p_expr->value.function->f_type)
			{
				case FIELDNAME_TYPE:
					//필드에서 해당 값을 가져와서 대입 시킨다.
					if(type.kind == STRING_KIND)
					{
						type = get_value_from_RTPS_to_TypeSupport(type.value.string, p_typeSupport, p_value, i_size);
					}
					break;
				case TOPICNAME_TYPE:
					//printf("TOPICNAME_TYPE");
					break;
				case EQUAL_TYPE:
					return expression_function_EQUAL(typeA, typeB);
					break;
				case LESS_TYPE:
					return expression_function_LESS(typeA, typeB);
					break;
				case LESSEQUAL_TYPE:
					return expression_function_LESSEQUAL(typeA, typeB);
					break;
				case GREATEREQUAL_TYPE:
					return expression_function_GREATEREQUAL(typeA, typeB);
					break;
				case NOTEQUAL_TYPE:
					return expression_function_NOTEQUAL(typeA, typeB);
					break;
				case GREATER_TYPE:
					return expression_function_GREATER(typeA, typeB);
					break;
				case LIKE_TYPE:
					return expression_function_LIKE(typeA, typeB);
					break;
				case AND_TYPE:
					return expression_function_AND(typeA, typeB);
					break;
				case OR_TYPE:
					return expression_function_OR(typeA, typeB);
					break;
				case NOT_TYPE:
					return expression_function_NOT(typeA, typeB);
					break;
			}
		}

		
	}

	return type;
}



expression_t get_value_from_message(char* fieldname, FooTypeSupport *p_typeSupport, char *value, int i_size)
{
	int i;
	int32_t offset = 0;
	int32_t len = 0;
	dds_parameter_t *p_parameter;
	char *string_offset = NULL;
	//string_temp temp_string;
	expression_t type;
	type.kind = UNDEFINED_KIND;
	
	
	
	for(i=0; i < p_typeSupport->i_parameters; i++)
	{
		p_parameter = p_typeSupport->pp_parameters[i];


		switch(p_parameter->type)
		{
			case DDS_STRING_TYPE:
				//temp_string.p_dummy = malloc(1024);
				//memset(temp_string.p_dummy,0,1024);
				string_offset =  &value[len+4];
				len += p_parameter->i_size;
				//FREE(temp_string.p_dummy);
				break;
			default:
				
				//len += p_parameter->i_size;
				break;
		}

		if(!strcmp(p_parameter->p_field_name, fieldname)){
			

			/*switch(p_parameter->type)
			{
				case DDS_STRING_TYPE:
					type.kind = STRING_KIND;
					type.value.string = string_offset;
					break;
				case DDS_INTEGER32_TYPE:
					type.kind = INTEGER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_FLOAT_TYPE:
					type.kind = FLOAT_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_CHARACTER_TYPE:
					type.kind = CHARACTER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
			}*/

			switch(p_parameter->type)
			{
				case DDS_STRING_TYPE:
					type.kind = STRING_KIND;
					type.value.string = string_offset;
					break;
				case DDS_BYTE_TYPE:
				case DDS_INTEGER8_TYPE:
				case DDS_INTEGER16_TYPE:
				case DDS_INTEGER32_TYPE:
				case DDS_INTEGER64_TYPE:
				case DDS_UINTEGER8_TYPE:
				case DDS_UINTEGER16_TYPE:
				case DDS_UINTEGER32_TYPE:
				case DDS_UINTEGER64_TYPE:
					type.kind = INTEGER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_FLOAT_TYPE:
				case DDS_DOUBLE_TYPE:
					type.kind = FLOAT_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
				case DDS_CHARACTER_TYPE:
				case DDS_BOOLEAN_TYPE:
					type.kind = CHARACTER_KIND;
					memcpy(&type.value, &value[len], p_parameter->i_size);
					break;
			}

			return type;
		}


		if(p_parameter->type != DDS_STRING_TYPE)
		{
			len += p_parameter->i_size;
		}




		offset += p_parameter->i_size;
	}

	return type;
}



expression_t expression_caculate_for_message(expression_t *p_expr, FooTypeSupport *p_typeSupport, char *p_value, int i_size, StringSeq query_parameters)
{
	expression_list_t	*next = NULL;
	expression_t type;
	expression_t typeA;
	expression_t typeB;

	assert(p_expr != NULL);
	assert(p_typeSupport != NULL);
	assert(p_value != NULL);
	assert(i_size > 0);

	type.kind = INTEGER_KIND;



	if(p_expr->kind == STRING_KIND)
	{
		//printf("STRING_KIND : %s", p_expr->value.string);
		type.kind = STRING_KIND;
		type.value.string = p_expr->value.string;
	}

	if(p_expr->kind == IDENTIFIER_KIND)
	{
	//	printf("IDENTIFIER_KIND : %s", p_expr->value.string);
		//
		type.kind = STRING_KIND;
		type.value.string = p_expr->value.string;
	}

	if(p_expr->kind == INTEGER_KIND)
	{
		//printf("INTEGER_KIND : %d", p_expr->value.integer);
		type.kind = INTEGER_KIND;
		type.value.integer = p_expr->value.integer;
	}


	if(p_expr->kind == INTEGER_PARAMETER_KIND)
	{
		if(query_parameters.i_string < p_expr->value.integer)
		{
			//parameter의 수가 정의한 수보다는 같거나 많아야 한다.
			assert(false);
		}
		//printf("INTEGER_KIND : %d", p_expr->value.integer);
		type.kind = INTEGER_KIND;

		

		type.value.integer = atol(query_parameters.pp_string[p_expr->value.integer]);
	}
	
	if(p_expr->kind == FLOAT_KIND)
	{
		//printf("INTEGER_KIND : %f", p_expr->value.real);
		type.kind = FLOAT_KIND;
		type.value.real = p_expr->value.real;
	}

	if(p_expr->kind == CHARACTER_KIND)
	{
		//printf("CHARACTER_KIND : %c", p_expr->value.character);
		type.kind = CHARACTER_KIND;
		type.value.character = p_expr->value.character;
	}

	if(p_expr->kind == FUNCTION_KIND)
	{
		switch(p_expr->value.function->f_type)
		{
		case FilterExpression_TYPE:
			//printf("FilterExpression_TYPE");
			break;
		case TopicExpression_TYPE:
			//printf("TopicExpression_TYPE");
			break;
		case QueryExpression_TYPE:
			//printf("QueryExpression_TYPE");
			break;
		case SELECT_TYPE:
			//printf("SELECT_TYPE");
			break;
		case Aggregation_TYPE:
			//printf("Aggregation_TYPE");
			break;
		case SubjectFieldSpec_AS_TYPE:
			//printf("SubjectFieldSpec_AS_TYPE");
			break;
		case SubjectFieldSpec_TYPE:
			//printf("SubjectFieldSpec_TYPE");
			break;
		case Selection_NaturalJoin_TYPE:
			//printf("Selection_NaturalJoin_TYPE");
			break;
		case JoinItem_TYPE:
			//printf("JoinItem_TYPE");
			break;
		case NaturalJoin_TYPE:
			//printf("NaturalJoin_TYPE");
			break;
		case WHERE_TYPE:
			//printf("WHERE_TYPE");
			break;
		case OR_TYPE:
			//printf("OR_TYPE");
			break;
		case ComparisonPredicate_TYPE:
			//printf("ComparisonPredicate_TYPE");
			break;
		case BetweenPredicate_TYPE:
			//printf("BetweenPredicate_TYPE");
			break;
		case BetweenPredicate_NOT_TYPE:
			//printf("BetweenPredicate_NOT_TYPE");
			break;
		case GREATEREQUAL_TYPE:
			//printf("GREATEREQUAL_TYPE");
			break;
		case LESSEQUAL_TYPE:
			//printf("LESSEQUAL_TYPE");
			break;
		case NOTEQUAL_TYPE:
			//printf("NOTEQUAL_TYPE");
			break;
		case EQUAL_TYPE:
			//expression_function_equal(
			//printf("EQUAL_TYPE");
			break;
		case GREATER_TYPE:
			//printf("GREATER_TYPE");
			break;
		case LESS_TYPE:
			//printf("LESS_TYPE");
			break;
		case LIKE_TYPE:
			//printf("LIKE_TYPE");
			break;
		case AND_TYPE:
			//printf("AND_TYPE");
			break;
		case NOT_TYPE:
			//printf("NOT_TYPE");
			break;
		case Range_TYPE:
			//printf("Range_TYPE");
			break;
		case FIELDNAME_TYPE:
			//printf("FIELDNAME_TYPE");
			break;
		case TOPICNAME_TYPE:
			//printf("TOPICNAME_TYPE");
			break;
		case PARAMETER_TYPE:
			//printf("PARAMETER_TYPE");
			break;
		}

		//printf("  ");

		if(p_expr->value.function->p_list)
		{
			typeA = expression_caculate_for_message(p_expr->value.function->p_list->p_expr, p_typeSupport, p_value, i_size, query_parameters);
		}

		

		

		//printf("\r\n");
		
		next = p_expr->value.function->p_list->next;

		while(next)
		{
			typeB = expression_caculate_for_message(next->p_expr, p_typeSupport, p_value, i_size, query_parameters);
			next = next->next;
		}


		if(p_expr->kind == FUNCTION_KIND)
		{
			type = typeA;

			switch(p_expr->value.function->f_type)
			{
				case FIELDNAME_TYPE:
					//필드에서 해당 값을 가져와서 대입 시킨다.
					if(type.kind == STRING_KIND)
					{
						type = get_value_from_message(type.value.string, p_typeSupport, p_value, i_size);
					}
					break;
				case TOPICNAME_TYPE:
					//printf("TOPICNAME_TYPE");
					break;
				case EQUAL_TYPE:
					return expression_function_EQUAL(typeA, typeB);
					break;
				case LESS_TYPE:
					return expression_function_LESS(typeA, typeB);
					break;
				case LESSEQUAL_TYPE:
					return expression_function_LESSEQUAL(typeA, typeB);
					break;
				case GREATEREQUAL_TYPE:
					return expression_function_GREATEREQUAL(typeA, typeB);
					break;
				case NOTEQUAL_TYPE:
					return expression_function_NOTEQUAL(typeA, typeB);
					break;
				case GREATER_TYPE:
					return expression_function_GREATER(typeA, typeB);
					break;
				case LIKE_TYPE:
					return expression_function_LIKE(typeA, typeB);
					break;
				case AND_TYPE:
					return expression_function_AND(typeA, typeB);
					break;
				case OR_TYPE:
					return expression_function_OR(typeA, typeB);
					break;
				case NOT_TYPE:
					return expression_function_NOT(typeA, typeB);
					break;
			}
		}

		
	}

	return type;
}



