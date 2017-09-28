%{
  #include <iostream>
  #include <string>
  #include "ast.h"
  #include "visitor.h"
  extern "C" FILE *yyin;
  extern "C" int yylex ();
  extern "C" int yyparse ();

  using namespace std;

  void yyerror (char const *s);
  #include "lex.yy.c"
%}

%union {
	int ival;
	char* sval;
	ASTStatement *statement;
	ASTIdentifier *id;
	ASTExpression *expr;
};

%type	<ival>			NUMBER
%type	<sval>			IDENTIFIER
%type	<id>			identifier
%type	<expr>			expr

%token NUMBER
%token STRING
%token IDENTIFIER

%token DECL_BLOCK
%token CODE_BLOCK

%token CMP
%token NE
%token GE
%token LE

%token INT
%token PRINT
%token PRINTLN
%token READ
%token GOTO
%token WHILE
%token FOR
%token IF
%token ELSE
%token ETOK

%left '+' '-'
%left '*' '/'
%%
program: 		decl_block code_block

decl_block: 	DECL_BLOCK '{' declarations '}'
code_block: 	CODE_BLOCK '{' codelines '}'

declarations: 	declaration ';' declarations | %empty
declaration: 	INT identifiers

identifier: 	IDENTIFIER { $$ = new ASTSingleIdentifier($1); }
		| 		IDENTIFIER '[' expr ']' { $$ = new ASTArrayIdentifier($1, $3); }
identifiers: 	identifier | identifier ',' identifiers

codelines: 		codeline codelines | %empty
codeline: 		assignment ';'
		| 		print ';'
		| 		read ';'
		| 		while
		| 		for
		| 		if
		| 		label
		| 		goto ';' | ';'
label: 			IDENTIFIER ':'
goto: 			GOTO IDENTIFIER | GOTO IDENTIFIER IF cond

assignment: 	identifier '=' expr
print: 			PRINT value_list | PRINTLN value_list
read: 			READ identifiers
value_list: 	value | value ',' value_list
value: 			STRING | identifier
expr:			expr '+' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::plus); visit($$);}
		|		expr '-' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::minus); }
		|		expr '*' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::product); }
		|		expr '/' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::divide); }
		|       '(' expr ')' { $$ = $2; }
		| 		NUMBER { $$ = new ASTIntegerLiteral($1); }
		|		identifier { $$ = $1; }

cond:   expr relop expr
		| '(' expr relop expr ')'
relop:  CMP | NE | '>' | '<' | GE | LE

while: 			WHILE cond '{' codelines '}'
for: 			FOR assignment ',' expr ',' expr '{' codelines '}' | FOR assignment ',' expr '{' codelines '}'
if: 			IF cond '{' codelines '}' | IF cond '{' codelines '}' ELSE '{' codelines '}'
%%

void yyerror (char const *s)
{
       fprintf (stderr, "%s\n", s);
}

int main(int argc, char *argv[])
{
	if (argc == 1 ) {
		fprintf(stderr, "Correct usage: bcc filename\n");
		exit(1);
	}

	if (argc > 2) {
		fprintf(stderr, "Passing more arguments than necessary.\n");
		fprintf(stderr, "Correct usage: bcc filename\n");
	}

	yyin = fopen(argv[1], "r");

	yyparse();
}
