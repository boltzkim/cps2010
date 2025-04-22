#if !defined(__EXPRESSION_H)
#define __EXPRESSION_H

typedef enum expression_kind
{
	INTEGER_KIND,
	FLOAT_KIND,
	STRING_KIND,
	CHARACTER_KIND,
	FUNCTION_KIND,
	IDENTIFIER_KIND,
	BOOLEAN_KIND,
	UNDEFINED_KIND,
	INTEGER_PARAMETER_KIND
}expression_kind;


typedef enum expression_type
{
	EQ_TYPE,
	NE_TYPE,
	LT_TYPE,
	LE_TYPE,
	GT_TYPE,
	GE_TYPE
}expression_type;

typedef enum function_type
{
	FilterExpression_TYPE,
	TopicExpression_TYPE,
	QueryExpression_TYPE,
	SELECT_TYPE,
	Aggregation_TYPE,
	SubjectFieldSpec_AS_TYPE,
	SubjectFieldSpec_TYPE,
	Selection_NaturalJoin_TYPE,
	JoinItem_TYPE,
	NaturalJoin_TYPE,
	WHERE_TYPE,
	OR_TYPE,
	ComparisonPredicate_TYPE,
	BetweenPredicate_TYPE,
	BetweenPredicate_NOT_TYPE,
	GREATEREQUAL_TYPE,
	LESSEQUAL_TYPE,
	NOTEQUAL_TYPE,
	EQUAL_TYPE,
	GREATER_TYPE,
	LESS_TYPE,
	LIKE_TYPE,
	AND_TYPE,
	NOT_TYPE,
	Range_TYPE,
	FIELDNAME_TYPE,
	TOPICNAME_TYPE,
	PARAMETER_TYPE
}function_type;

typedef struct expression_list_t expression_list_t;
typedef struct expression_function_t expression_function_t;

struct expression_function_t {
    function_type		f_type;
    expression_list_t	*p_list;
};

struct expression_t{
	expression_kind kind;

	union{
		char					*string;
        char					character;
        long					integer;
        double					real;
		bool					boolean;
		expression_function_t   *function;
	}value;
};

struct expression_list_t {
    expression_t		*p_expr;
    expression_list_t	*next;
};


#ifdef __cplusplus
extern "C" {
#endif

	expression_t *expression_newFunction(function_type f_type, expression_list_t *p_list);
	expression_list_t *expression_insert(expression_list_t *list, expression_t *p_expr);
	expression_t *expression_newIdendifier(char *string);
	expression_t *expression_newString(char *string);
	expression_t *expression_newInteger(long value);
	expression_t *expression_newDouble(double value);
	expression_t *expression_newCharacter(char value);
	expression_list_t *expression_append(expression_list_t *list, expression_t *p_expr);
	void expression_print(expression_t *p_expr, long tab);
	expression_t expression_caculate(expression_t *p_expr, FooTypeSupport *p_typeSupport, char *p_value, int i_size);
	expression_t expression_caculate_for_message(expression_t *p_expr, FooTypeSupport *p_typeSupport, char *p_value, int i_size, StringSeq query_parameters);

#ifdef __cplusplus
}
#endif

#define L0(f_type) expression_newFunction(f_type,NULL)
#define L1(f_type,list) expression_newFunction(f_type,list)
#define L2(f_type,e1,list) expression_newFunction(f_type,expression_insert(list,e1))
#define L3(f_type,e1,e2,list) L2(f_type,e1,expression_insert(list,e2))
#define L4(f_type,e1,e2,e3,list) L3(f_type,e1,e2,expression_insert(list,e3))

#define List1(e1) expression_insert(NULL,e1)
#define List2(e1,e2) expression_insert(List1(e2),e1)
#define List3(e1,e2,e3) expression_insert(List2(e2,e3),e1)
#define List4(e1,e2,e3,e4) expression_insert(List3(e2,e3,e4),e1)
#define List5(e1,e2,e3,e4,e5) expression_insert(List4(e2,e3,e4,e5),e1)

#define F1(f_type,e1) expression_newFunction(f_type,List1(e1))
#define F2(f_type,e1,e2) expression_newFunction(f_type,List2(e1,e2))
#define F3(f_type,e1,e2,e3) expression_newFunction(f_type,List3(e1,e2,e3))
#define F4(f_type,e1,e2,e3,e4) expression_newFunction(f_type,List4(e1,e2,e3,e4))
#define F5(f_type,e1,e2,e3,e4,e5) expression_newFunction(f_type,List5(e1,e2,e3,e4,e5))

#endif