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
	ASTCodeBlock *codeBlock;
	ASTProgram *program;
	ASTStatement *statement;
	vector<ASTStatement*> *statements;
	ASTIdentifier *id;
	vector<ASTIdentifier*> *ids;
	ASTExpression *expr;
	BoolOp boolOp;
};

%type	<ival>			NUMBER
%type	<sval>			IDENTIFIER
%type	<sval>			STRING
%type	<id>			identifier
%type	<expr>			expr
%type	<statement>		assignment
%type	<ids>			identifiers
%type	<statement>		read
%type	<boolOp>		relop
%type	<expr>			cond
%type	<statement>		while
%type	<statement>		codeline
%type	<statements>	codelines
%type	<codeBlock>		code_block
%type	<program>		program

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
program: 		decl_block code_block { $$ = new ASTProgram(new ASTDeclBlock(), $2); root = $$; }

decl_block: 	DECL_BLOCK '{' declarations '}'
code_block: 	CODE_BLOCK '{' codelines '}' { $$ = new ASTCodeBlock($3); }

declarations: 	declaration ';' declarations | %empty
declaration: 	INT identifiers

identifier: 	IDENTIFIER { $$ = new ASTSingleIdentifier($1); }
		| 		IDENTIFIER '[' expr ']' { $$ = new ASTArrayIdentifier($1, $3); }
identifiers: 	identifier { $$ = new vector<ASTIdentifier*>; $$->push_back($1); }
		| 		identifier ',' identifiers { $3->push_back($1); $$ = $3; }

codelines: 		codeline codelines { $2->push_back($1); $$ = $2; } | %empty { $$ = new vector<ASTStatement*>; }
codeline: 		assignment ';' { $$ = $1; }
//		| 		print ';'
		| 		read ';'
		| 		while
//		| 		for
//		| 		if
//		| 		label
//		| 		goto ';' | ';'
// label: 			IDENTIFIER ':'
// goto: 			GOTO IDENTIFIER | GOTO IDENTIFIER IF cond

assignment: 	identifier '=' expr { $$ = new ASTAssignmentStatement($1, $3); }
//			 print: 			PRINT value_list
//			| 		PRINTLN value_list
read: 			READ identifiers { $$ = new ASTReadStatement($2); }
// value_list: 	value | value ',' value_list
// value: 			STRING | identifier
expr:			expr '+' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::plus); visit($$);}
		|		expr '-' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::minus); }
		|		expr '*' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::product); }
		|		expr '/' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::divide); }
		|       '(' expr ')' { $$ = $2; }
		| 		NUMBER { $$ = new ASTIntegerLiteral($1); }
		|		identifier { $$ = $1; }

cond:   expr relop expr { $$ = new ASTBooleanExpression($1, $3, $2); }
		| '(' expr relop expr ')' { $$ = new ASTBooleanExpression($2, $4, $3); }
relop:  CMP { $$ = BoolOp::equalequal; }
		|		NE { $$ = BoolOp::notequal; }
		|       '>' { $$ = BoolOp::greaterthan; }
		|   	'<' { $$ = BoolOp::lessthan; }
		|		GE { $$ = BoolOp::greaterequal; }
		|		LE { $$ = BoolOp::lessequal; }

while: 			WHILE cond '{' codelines '}' { $$ = new ASTWhileStatement($2, $4); }
// for: 			FOR assignment ',' expr ',' expr '{' codelines '}' | FOR assignment ',' expr '{' codelines '}'
//				 if: 			IF cond '{' codelines '}' | IF cond '{' codelines '}' ELSE '{' codelines '}'
%%

void yyerror (char const *s)
{
       fprintf (stderr, "%s\n", s);
}

int main (int argc, char *argv[])
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

	printableVisitor *visitor = new printableVisitor();
	if (root)
		visitor->visit(root);
}
