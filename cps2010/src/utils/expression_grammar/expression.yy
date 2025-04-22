%{
/*
	This program is created by pws.
	Copyright reserved by ETRI
	2011-11-14
*/


#include <stdio.h>
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include <assert.h>

int yyerror(char *msg);
int yylex();

#define YY_NEVER_INTERACTIVE 1


expression_t *p_expr;


%}


%union {
    char		*String;
    char		Char;
    long		Integer;
    double		Float;
    expression_t	*ExpressionT;
    expression_list_t   *List;
    function_type	Ftype;
}


%token like
%token ORDER BY
%token WHERE
%token SELECT
%token FROM
%token AS
%token INNER NATURAL
%token JOIN
%token AND
%token OR
%token NOT
%token BETWEEN

%token COMMA
%token GREATER LESS
%token GREATEREQUAL LESSEQUAL NOTEQUAL EQUAL
%token SEMI MUL
%token LRPAR RRPAR

%token <String> STRING IDENTIFIER
%token <Char> CHARVALUE
%token <Float> FLOATVALUE
%token <Integer> INTEGERVALUE
%type  <ExpressionT>	FilterExpression TopicExpression QueryExpression
			SelectFrom Where Condition Aggregation Selection
			FIELDNAME TOPICNAME PARAMETER SubjectFieldSpec
			Parameter Range BetweenPredicate Predicate ComparisonPredicate
			JoinItem
%type  <List>		SubjectFieldSpec_list FIELDNAME_list
%type  <Ftype>		RelOp NaturalJoin

%%

Expression : TopicExpression	{ p_expr = $1; }
	| QueryExpression	{ p_expr = $1; }
	| FilterExpression	{ p_expr = $1; }
;

FilterExpression : Condition {$$ = F1(FilterExpression_TYPE,$1);}
;

TopicExpression : SelectFrom Where Last
	{$$ = F2(TopicExpression_TYPE,$1,$2);}
;

Last :
	| SEMI

QueryExpression : Condition ORDER BY FIELDNAME_list {$$ = F1(QueryExpression_TYPE,$1);}
;


FIELDNAME_list :
	FIELDNAME { $$ = List1($1); }
	| FIELDNAME_list COMMA FIELDNAME  {$$ = expression_append($1,$3); }
;

SelectFrom : SELECT Aggregation FROM Selection  { $$ = F2(SELECT_TYPE,$2, $4); }
;

Aggregation : MUL  { $$ = L0(Aggregation_TYPE); }
	| SubjectFieldSpec_list { $$ = L1(Aggregation_TYPE,$1); }
;

SubjectFieldSpec_list :
	SubjectFieldSpec	 { $$ = List1($1); printf("SubjectFieldSpec\r\n");} 
	| SubjectFieldSpec_list COMMA SubjectFieldSpec {$$ = expression_append($1,$3); printf("SubjectFieldSpec_list\r\n");}
;

SubjectFieldSpec : FIELDNAME AS FIELDNAME { $$ = F2(SubjectFieldSpec_AS_TYPE,$1, $3); }
	| FIELDNAME FIELDNAME { $$ = F2(SubjectFieldSpec_TYPE,$1, $2); }
	| FIELDNAME { $$ = F1(SubjectFieldSpec_TYPE,$1); }
;

Selection : TOPICNAME				{ $$ = $1;}
	| TOPICNAME NaturalJoin JoinItem	{ $$ = F2($2,$1,$3); }
;

JoinItem : TOPICNAME					{ $$ = $1;}
	| TOPICNAME NaturalJoin JoinItem		{ $$ = F2($2,$1,$3); }
	| LRPAR TOPICNAME NaturalJoin JoinItem RRPAR	{ $$ = F2($3,$2,$4); }
;

NaturalJoin : INNER NATURAL JOIN	{ $$ = NaturalJoin_TYPE; }
	| NATURAL JOIN			{ $$ = NaturalJoin_TYPE; }
	| NATURAL INNER JOIN		{ $$ = NaturalJoin_TYPE; }
;

Where : WHERE Condition	{ $$ = F1(WHERE_TYPE,$2); }
;


Condition : Predicate			{ $$ = $1; }
	| Condition AND Condition	{ $$ = F2(AND_TYPE,$1,$3); }
	| Condition OR Condition	{ $$ = F2(OR_TYPE,$1,$3); }
	| NOT Condition			{ $$ = F1(NOT_TYPE,$2); }
	| LRPAR Condition RRPAR		{ $$ = $2; }
;

Predicate : ComparisonPredicate	{ $$ = $1; }
	| BetweenPredicate	{  $$ = $1; }
;

ComparisonPredicate : FIELDNAME RelOp Parameter	{ $$ = F2($2,$1,$3); }
	| Parameter RelOp FIELDNAME		{ $$ = F2($2,$1,$3); }
	| FIELDNAME RelOp FIELDNAME		{ $$ = F2($2,$1,$3); }
;

BetweenPredicate : FIELDNAME BETWEEN Range	{ $$ = F2(BetweenPredicate_TYPE,$1,$3); }
	| FIELDNAME NOT BETWEEN Range		{ $$ = F2(BetweenPredicate_NOT_TYPE,$1,$4); }
;

RelOp : GREATEREQUAL	{ $$ = GREATEREQUAL_TYPE; }
	| LESSEQUAL	{ $$ = LESSEQUAL_TYPE; }
	| NOTEQUAL	{ $$ = NOTEQUAL_TYPE; }
	| EQUAL		{ $$ = EQUAL_TYPE; }
	| GREATER	{ $$ = GREATER_TYPE; }
	| LESS		{ $$ = LESS_TYPE; }
	| like		{ $$ = LIKE_TYPE; }
;

Range : Parameter AND Parameter	 { $$ = F2(Range_TYPE,$1,$3); }
;

Parameter : INTEGERVALUE	{$$ = expression_newInteger($1);}
	| CHARVALUE		{$$ = expression_newCharacter($1);}
	| FLOATVALUE		{$$ = expression_newDouble($1);}
	| STRING		{$$ = expression_newString($1); FREE($1);}
	| PARAMETER		{$$ = $1;}
;


FIELDNAME : IDENTIFIER { $$ = F1(FIELDNAME_TYPE,expression_newIdendifier($1)); FREE($1);}
;
	
TOPICNAME : IDENTIFIER  { $$ = F1(TOPICNAME_TYPE,expression_newIdendifier($1)); FREE($1);}
	|  STRING	{ $$ = F1(TOPICNAME_TYPE,expression_newString($1)); FREE($1);}
;

PARAMETER : IDENTIFIER  { $$ = F1(PARAMETER_TYPE,expression_newIdendifier($1)); FREE($1);}
;



%%


#include "lex.yy.c"


static mutex_t				structure_lock;
static bool structure_lock_initialize = false;



void expression_parse (const char *expression)
{

	if(!structure_lock_initialize)
	{
		structure_lock_initialize = true;
		mutex_init(&structure_lock);
	}
	assert(expression != NULL);

	mutex_lock(&structure_lock);
	p_expr  = NULL;
	yy_scan_string((char *) expression);
	yyparse();
	yy_delete_buffer(YY_CURRENT_BUFFER);
	expression_print(p_expr,0);
	mutex_unlock(&structure_lock);
}


int main()
{
	expression_parse("SELECT flight_name, x, y, z AS height FROM 'Location' NATURAL JOIN 'FlightPlan' WHERE height < 1000 AND x <23");
	
	getchar();
	return 0;
}


int yyerror(char *msg)
{
	printf("Error encountered: %s %s\n", msg,yytext);
	return 0;
}