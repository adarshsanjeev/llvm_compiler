#ifndef _AST_H
#define _AST_H

#include <vector>
#include <string>
#include <map>

#include <iostream>
#include "ast.hpp"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

using namespace std;

class ASTNode;

class ASTProgram;
class ASTDeclBlock;
class ASTCodeBlock;

class ASTExpression;
class ASTBinaryExpression;
class ASTBooleanExpression;
class ASTIntegerLiteral;

class ASTIdentifier;
class ASTArrayIdentifier;
class ASTPrintable;

class ASTStatement;
class ASTAssignmentStatement;
class ASTPrintStatement;
class ASTReadStatement;
class ASTLabel;
class ASTGoToStatement;
class ASTIfStatement;
class ASTWhileStatement;
class ASTForStatement;

class Visitor {
public:
	virtual void* visit(ASTProgram* ast) = 0;
	virtual void* visit(ASTDeclBlock* ast) = 0;
	virtual void* visit(ASTCodeBlock* ast) = 0;
	virtual void* visit(ASTIntegerLiteral* ast) = 0;
	virtual void* visit(ASTIdentifier* ast) = 0;
	virtual void* visit(ASTBinaryExpression* ast) = 0;
	virtual void* visit(ASTBooleanExpression* ast) = 0;
	virtual void* visit(ASTAssignmentStatement* ast) = 0;
	virtual void* visit(ASTPrintStatement* ast) = 0;
	virtual void* visit(ASTLabel* ast) = 0;
	virtual void* visit(ASTReadStatement* ast) = 0;
	virtual void* visit(ASTWhileStatement* ast) = 0;
	virtual void* visit(ASTIfStatement* ast) = 0;
	virtual void* visit(ASTForStatement* ast) = 0;
};

enum BinOp {
    PLUS,
    MINUS,
    PRODUCT,
    DIVIDE,
	MODULUS,
	EXPONENT
};

enum BoolOp {
    LESSTHAN,
    GREATERTHAN,
    LESSEQUAL,
    GREATEREQUAL,
    NOTEQUAL,
    EQUALEQUAL,
    AND_OP,
    OR_OP
};

class ASTNode {
	virtual void* accept(Visitor *visitor) = 0;
};

class ASTProgram : public ASTNode {
public:
	ASTDeclBlock *decl_block;
	ASTCodeBlock *code_block;
	ASTProgram(ASTDeclBlock *decl_block, ASTCodeBlock *code_block) {
		this->decl_block = decl_block;
		this->code_block = code_block;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTCodeBlock : public ASTNode {
public:
	vector<ASTStatement*> *statements;
	ASTCodeBlock (vector<ASTStatement*> *statements) {
		this->statements = statements;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTExpression : public ASTNode {
public:
	ASTExpression() {
	}
	virtual ~ASTExpression () {}
	void* accept(Visitor *visitor) {
	}
};

class ASTIntegerLiteral : public ASTExpression {
public:
	int value;
	ASTIntegerLiteral(int value) {
		this->value = value;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTBinaryExpression : public ASTExpression {
public:
	ASTExpression *left_child;
	ASTExpression *right_child;
	BinOp op;

	ASTBinaryExpression(ASTExpression *left, ASTExpression *right, BinOp op) {
		this->left_child = left;
		this->right_child = right;
		this->op = op;
	};
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTBooleanExpression : public ASTExpression {
public:
	ASTExpression *left_child;
	ASTExpression *right_child;
	BoolOp op;

	ASTBooleanExpression(ASTExpression *left, ASTExpression *right, BoolOp op) {
		this->left_child = left;
		this->right_child = right;
		this->op = op;
	};
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTIdentifier : public ASTExpression {
public:
	string id;
	ASTIdentifier(string id) {
		this->id = id;
	}
	bool operator<( const ASTIdentifier& other) const
	{
		return this->id < other.id;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTArrayIdentifier : public ASTIdentifier {
public:
	ASTExpression* index;
ASTArrayIdentifier(string id, ASTExpression *index) : ASTIdentifier (id) {
		this->index = index;
	}
	bool operator<( const ASTIdentifier& other) const
	{
		return this->id < other.id;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTDeclBlock : public ASTNode {
public:
	vector<ASTIdentifier*> *declarations;
	ASTDeclBlock(vector<ASTIdentifier*> *declarations) {
		this->declarations = declarations;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTStatement : public ASTNode {
public:
	virtual ~ASTStatement() {}
	virtual void* accept(Visitor *visitor) = 0;
};

class ASTAssignmentStatement : public ASTStatement {
public:
	ASTIdentifier *id;
	ASTExpression *rhs;
	ASTAssignmentStatement(ASTIdentifier *id, ASTExpression *rhs) {
		this->id = id;
		this->rhs = rhs;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTPrintable : public ASTNode {
public:
	string text;
	ASTIdentifier *id;

	ASTPrintable(string text) {
		this->text = text;
		this->id = NULL;
	}
	ASTPrintable(ASTIdentifier *id) {
		this->id = id;
	}
	void* accept(Visitor *visitor) {
	}
};

class ASTPrintStatement : public ASTStatement {
public:
	string delim = "";
	vector<ASTPrintable*> *printable;
	ASTPrintStatement(vector<ASTPrintable*> *printable, string delim = "") {
		this->printable = printable;
		this->delim = delim;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTReadStatement : public ASTStatement {
public:
	vector<ASTIdentifier*> *ids;
	ASTReadStatement(vector<ASTIdentifier*> *ids) {
		this->ids = ids;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTLabel : public ASTStatement {
public:
	string label_name;
	ASTLabel(string label_name) {
		this->label_name = label_name;
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTGoToStatement : public ASTStatement {
public:
	string label_name;
	ASTBooleanExpression* cond;
	ASTGoToStatement(string label_name) {
		this->label_name = label_name;
		this->cond = NULL;
	}
	ASTGoToStatement(string label_name, ASTExpression* cond) {
		this->label_name = label_name;
		this->cond = dynamic_cast<ASTBooleanExpression *>(cond);
	}
	void* accept(Visitor *visitor) {
	}
};

class ASTWhileStatement : public ASTStatement {
public:
	ASTBooleanExpression *cond;
	ASTCodeBlock *code_block;
	ASTWhileStatement(ASTExpression *cond, vector<ASTStatement*> *statements) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(cond);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->cond = bool_exp;
			this->code_block = new ASTCodeBlock(statements);
		}
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTForStatement : public ASTStatement {
public:
	ASTAssignmentStatement *init;
	ASTExpression *limit;
	ASTExpression *step;
	ASTCodeBlock *code_block;
	ASTForStatement(ASTAssignmentStatement *init, ASTExpression *limit, ASTExpression *step, vector<ASTStatement*> *statements) {
		this->init = init;
		this->limit = limit;
		this->step = step;
		this->code_block = new ASTCodeBlock(statements);
	}

	ASTForStatement(ASTAssignmentStatement *init, ASTExpression *limit, vector<ASTStatement*> *statements) {
		this->init = init;
		this->limit = limit;
		this->code_block = new ASTCodeBlock(statements);
		this->step = new ASTIntegerLiteral(1);
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTIfStatement : public ASTStatement {
public:
	ASTBooleanExpression *cond;
	ASTCodeBlock *then_block;
	ASTCodeBlock *else_block;

	ASTIfStatement(ASTExpression *cond, vector<ASTStatement*> *then_block) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(cond);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->cond = bool_exp;
			this->then_block = new ASTCodeBlock(then_block);
			this->else_block = NULL;
		}
	}

	ASTIfStatement(ASTExpression *cond, vector<ASTStatement*> *then_block, vector<ASTStatement*> *else_block) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(cond);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->cond = bool_exp;
			this->then_block = new ASTCodeBlock(then_block);
			this->else_block = new ASTCodeBlock(else_block);
		}
	}
	void* accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

#endif
