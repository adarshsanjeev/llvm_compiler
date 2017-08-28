%{
#include "parser.tab.h"
%}

delim [ \t\n]
ws {delim}+
letter [A-Za-z]
digit [0-9]
id {letter}({letter}|{digit})*
number {digit}+

%%
declblock {
	//printf("declblock keyword read\n");
	return DECL_BLOCK;
}

codeblock {
	//printf("codeblock keyword read\n");
	return CODE_BLOCK;
}

\( {
	//printf("opening paranthesis read\n");
	return '(';
}

\) {
	//printf("closing paranthesis read\n");
	return ')';
}

\[ {
	//printf("opening sqrb read\n");
	return '[';
}

\] {
	//printf("closing sqrb read\n");
	return ']';
}

\{ {
	//printf("opening brace read\n");
	return '{';
}

\} {
	//printf("closing brace read\n");
	return '}';
}

int {
	//printf("int keyword read\n");
	return INT;
}

print {
	//printf("Print statement\n");
	return PRINT;
}

println {
	//printf("Print statement\n");
	return PRINTLN;
}

while {
	//printf ("While statement\n");
	return WHILE;
}

for {
	//printf ("For statement\n");
	return FOR;
}

if {
	//printf("if statement\n");
	return IF;
}

else {
	//printf("else statement\n");
	return ELSE;
}

read {
	//printf("Read statement\n");
	return READ;
}

goto {
	//printf("goto \n");
	return GOTO;
}

\".*\" {
	//printf("print string\n");
	return STRING;
}

\; {
	//printf("semicolon read\n");
	return ';';
}

\+	    {
	//printf("+ read\n");
	return '+';
}

\-	    {
	//printf("- read\n");
	return '-';
}

\* 	    {
	//printf("* read\n");
	return '*';
}

\/	    {
	//printf("/ read\n");
	return '/';
}

\=\= {
    return CMP;
}

\!\= {
    return NE;
}

\>\= {
    return GE;
}

\<\= {
    return LE;
}

\= {
	//printf("= read\n");
	return '=';
}

\> {
	//printf("> read\n");
	return '>';
}

\< {
	//printf("< read\n");
	return '<';
}

\, {
	//printf(", read\n");
	return ',';
}

: {
	//printf(": read\n");
	return ':';
}

[0-9][0-9]*	{
			//printf("Token type: Number, Lexeme/Token Value: %s\n", yytext);  
			return NUMBER; 
}

[a-zA-Z][a-zA-Z0-9]* {  
			//printf("Token type: Identifier, Lexeme/Token Value: %s\n", yytext);  
			return IDENTIFIER; 
		     } 

{ws}		{ /* Do nothing */ }

.		{ 
		  //printf("Unexpected token encountered: %s\n", yytext); 
		  return ETOK;
		}
