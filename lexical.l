%option noyywrap
%{
	#include <stdio.h>
%}

letter		[a-zA-Z]
digit		[0-9]
id			{letter}({letter}|{digit})*
digits		{digit}+
num			{digit}+|{digit}+\.{digits}(E{digits})?
relop		==|!=|>(=)?|<(=)?
assign		=
addop		[\+\-]
mulop		[\*/]

%%

boolean|int|float		printf("%s\n",yytext);
if|else|while			printf("%s\n",yytext);
{id}		printf("id\n");
{num}		printf("num\n");
{relop} 	printf("relop\n");
{assign} 	printf("assign\n");
[;,\(\)\{\}]			printf("%s\n",yytext);
{addop}		printf("addop\n");
{mulop}		printf("mulop\n");
[\n \t]		;
.			printf("Unregcccodnized character %s\n", yytext);

%%

int main(int argc, char *argv[]){
	yyin = fopen(argv[1], "r");
	yylex();
	fclose(yyin);
	return 0;
}