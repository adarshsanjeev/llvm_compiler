%{
  #include "llvm/IR/Value.h"
  #include <iostream>
  #include <string>
  #include <stack>
  #include "ast.hpp"
  #include "visitor.hpp"
  #include "llvmv.hpp"

  extern "C" FILE *yyin;
  extern "C" int yylex();
  extern "C" int yyparse();

  using namespace std;

  void yyerror(char const *s);
  #include "lex.yy.c"
%}

%union {
	int ival;
	char* sval;
	ASTCodeBlock *code_block;
	ASTDeclBlock *decl_block;
	ASTProgram *program;
	ASTStatement *statement;
	ASTAssignmentStatement *assignment;
	vector<ASTStatement*> *statements;
	ASTIdentifier *id;
	vector<ASTIdentifier*> *ids;
	ASTBooleanExpression *bool_expr;
	ASTExpression *expr;
	BoolOp boolOp;
	ASTPrintable *printable;
	vector<ASTPrintable*> *printables;
};

%type	<ival>			NUMBER
%type	<sval>			IDENTIFIER
%type	<sval>			STRING
%type	<id>			identifier
%type	<expr>			expr
%type	<assignment>	assignment
%type	<ids>			identifiers
%type	<statement>		read
%type	<boolOp>		relop
%type	<expr>			cond
%type	<statement>		while
%type	<statement>		codeline
%type	<statements>	codelines
%type	<code_block>	code_block
%type	<program>		program
%type	<statement>		if
%type	<statement>		for
%type	<statement>		label
%type	<statement>		goto
%type	<statement>		print
%type	<printable>		value
%type	<printables>	value_list;
%type	<ids>			declaration;
%type	<ids>			declarations;
%type	<decl_block>	decl_block;

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
%left '%'
%left '^'
%precedence NEG

%%
program: 		decl_block code_block { $$ = new ASTProgram($1, $2); root = $$; }

decl_block: 	DECL_BLOCK '{' declarations '}' { $$ = new ASTDeclBlock($3); }
code_block: 	CODE_BLOCK '{' codelines '}' { $$ = new ASTCodeBlock($3); }

declarations: 	declaration ';' declarations { $3->insert($3->end(), $1->begin(), $1->end()); $$ = $3; } | %empty { $$ = new vector<ASTIdentifier*>; }
declaration: 	INT identifiers { $$ = $2; }

identifier: 	IDENTIFIER { $$ = new ASTIdentifier($1); }
		| 		IDENTIFIER '[' expr ']' { $$ = new ASTArrayIdentifier($1, $3); }
identifiers: 	identifier { $$ = new vector<ASTIdentifier*>; $$->push_back($1); }
		| 		identifier ',' identifiers { $3->push_back($1); $$ = $3; }

codelines: 		codelines codeline { $1->push_back($2); $$ = $1; } | %empty { $$ = new vector<ASTStatement*>; }
codeline: 		assignment ';' { $$ = $1; }
		| 		print ';' { $$ = $1; }
		| 		read ';' { $$ = $1; }
		| 		while { $$ = $1; }
		| 		for { $$ = $1; }
		| 		if { $$ = $1; }
		| 		label { $$ = $1; }
		| 		goto ';' { $$ = $1; }
		|       ';' { $$ = NULL; }

label: 			IDENTIFIER ':' { $$ = new ASTLabel($1); }
goto: 			GOTO IDENTIFIER { $$ = new ASTGoToStatement($2); }
		| 		GOTO IDENTIFIER IF cond { $$ = new ASTGoToStatement($2, $4); }

assignment: 	identifier '=' expr { $$ = new ASTAssignmentStatement($1, $3); }
print: 			PRINT value_list { $$ = new ASTPrintStatement($2); }
		| 		PRINTLN value_list { $$ = new ASTPrintStatement($2, "\n"); }
read: 			READ identifiers { $$ = new ASTReadStatement($2); }
value_list: 	value { $$ = new vector<ASTPrintable*>; $$->push_back($1); } | value_list ',' value { $1->push_back($3); $$=$1; }
value:			STRING { $$ = new ASTPrintable($1); }
		|		identifier { $$ = new ASTPrintable($1); }
expr:			expr '+' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::PLUS);}
		|		expr '-' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::MINUS); }
		|		expr '*' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::PRODUCT); }
		|		expr '/' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::DIVIDE); }
		|		expr '%' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::MODULUS); }
		|		expr '^' expr { $$ = new ASTBinaryExpression($1, $3, BinOp::EXPONENT); }
		|       '-' expr %prec NEG { $$ = new ASTBinaryExpression(new ASTIntegerLiteral(-1), $2, BinOp::PRODUCT); }
		|       '('expr ')' { $$ = $2; }
		| 		NUMBER { $$ = new ASTIntegerLiteral($1); }
		|		identifier { $$ = $1; }

cond:   		expr relop expr { $$ = new ASTBooleanExpression($1, $3, $2); }
		| '(' cond ')' { $$ = $2; }
relop:  		CMP { $$ = BoolOp::EQUALEQUAL; }
		|		NE { $$ = BoolOp::NOTEQUAL; }
		|       '>' { $$ = BoolOp::GREATERTHAN; }
		|   	'<' { $$ = BoolOp::LESSTHAN; }
		|		GE { $$ = BoolOp::GREATEREQUAL; }
		|		LE { $$ = BoolOp::LESSEQUAL; }

while: 			WHILE cond '{' codelines '}' { $$ = new ASTWhileStatement($2, $4); }
for: 			FOR assignment ',' cond '{' codelines '}' { $$ = new ASTForStatement($2, $4, $6); }
		| 		FOR assignment ',' cond ',' assignment '{' codelines '}' { $$ = new ASTForStatement($2, $4, $6, $8); }
		|		FOR '(' assignment ',' cond ')' '{' codelines '}' { $$ = new ASTForStatement($3, $5, $8); }
		| 		FOR '(' assignment ',' cond ',' assignment ')' '{' codelines '}' { $$ = new ASTForStatement($3, $5, $7, $10); }
if: 			IF cond '{' codelines '}' { $$ = new ASTIfStatement($2, $4); }
		| 		IF cond '{' codelines '}' ELSE '{' codelines '}' { $$ = new ASTIfStatement($2, $4, $8); }
%%

void yyerror(char const *s)
{
       fprintf(stderr, "%s\n", s);
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

	llvmVisitor *visitor = new llvmVisitor(root);
}
