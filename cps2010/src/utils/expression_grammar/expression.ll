%{
/*
	This program is created by pws.
	Copyright reserved by ETRI
	2011-11-14
*/


//#include "expression.tab.hh"


%}

INTEGERVALUE        	[+-]?[0-9]+
FLOATVALUE          	[+-]?({INTEGERVALUE}"."{INTEGERVALUE})(e[+-]?{INTEGERVALUE})?
CHARVALUE  	       	\'[^\']\'
STR			\"([^"]*\"\")*[^"]*\"
STR2			\'([^']*\'\')*[^']*\'
STR3			\`([^']*\'\')*[^']*\'
IDENTIFIER		[a-zA-Z][a-zA-Z0-9_]*

%%

";"		{ return (SEMI); }
","		{ return (COMMA); }
">="		{ return (GREATEREQUAL); }
"<="		{ return (LESSEQUAL); }
"<>"		{ return (NOTEQUAL); }
"*"		{ return (MUL); }
"("		{ return (LRPAR); }
")"             { return (RRPAR); }
"="		{ return (EQUAL); }
">"             { return (GREATER); }
"<"		{ return (LESS); }


like		{return like;}
ORDER		{return ORDER;}
BY		{return BY;}
WHERE		{return WHERE;}
SELECT		{return SELECT;}
FROM		{return FROM;}
AS		{return AS;}
INNER		{return INNER;}
NATURAL		{return NATURAL;}
JOIN		{return JOIN;}
AND		{return AND;}
OR		{return OR;}
NOT		{return NOT;}
BETWEEN		{return BETWEEN;}
order		{return ORDER;}
by		{return BY;}
where		{return WHERE;}
select		{return SELECT;}
from		{return FROM;}
as		{return AS;}
inner		{return INNER;}
natural		{return NATURAL;}
join		{return JOIN;}
and		{return AND;}
or		{return OR;}
not		{return NOT;}
between		{return BETWEEN;}

{INTEGERVALUE}  { yylval.Integer = atol(yytext); return INTEGERVALUE;}
{FLOATVALUE}	{yylval.Float = (double)atof(yytext); return FLOATVALUE;}
{CHARVALUE}	{ yylval.Char = yytext[1]; return CHARVALUE;}
{STR} | {STR2} | {STR3}	{yylval.String = (char*)malloc((unsigned)strlen(yytext)+1);  strcpy(yylval.String,yytext); return STRING;}


{IDENTIFIER}	{
			yylval.String = (char*)malloc((unsigned)strlen(yytext)+1); 
			strcpy(yylval.String,yytext);
			return IDENTIFIER;
		}


.
%%
int yywrap()
{
return 1;
}