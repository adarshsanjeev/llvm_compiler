#ifndef _AST_H
#define _AST_H

#include <vector>
#include <string>
#include <map>

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
	virtual void visit(ASTProgram* ast) = 0;
	virtual void visit(ASTDeclBlock* ast) = 0;
	virtual void visit(ASTCodeBlock* ast) = 0;
	virtual int visit(ASTExpression* ast) = 0;
	virtual void visit(ASTStatement* ast) = 0;
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
};

class ASTProgram : public ASTNode {
public:
	ASTDeclBlock *decl_block;
	ASTCodeBlock *code_block;
	ASTProgram(ASTDeclBlock *decl_block, ASTCodeBlock *code_block) {
		this->decl_block = decl_block;
		this->code_block = code_block;
	}
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTCodeBlock : public ASTNode {
public:
	vector<ASTStatement*> *statements;
	ASTCodeBlock (vector<ASTStatement*> *statements) {
		this->statements = statements;
	}
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTExpression : public ASTNode {
public:
	ASTExpression() {
	}
	virtual ~ASTExpression () {}
	int accept(Visitor *visitor) {
		return visitor->visit(this);
	}
};

class ASTIntegerLiteral : public ASTExpression {
public:
	int value;
	ASTIntegerLiteral(int value) {
		this->value = value;
	}
	void accept(Visitor *visitor) {
		visitor->visit(this);
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
	void accept(Visitor *visitor) {
		visitor->visit(this);
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
	void accept(Visitor *visitor) {
		visitor->visit(this);
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
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTArrayIdentifier : public ASTIdentifier {
public:
	ASTExpression* index;
ASTArrayIdentifier(string id, ASTExpression *index) : ASTIdentifier (id) {
		this->index = index;
	}
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTDeclBlock : public ASTNode {
public:
	vector<ASTIdentifier*> *declarations;
	ASTDeclBlock(vector<ASTIdentifier*> *declarations) {
		this->declarations = declarations;
	}
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTStatement : public ASTNode {
public:
	virtual ~ASTStatement() {}
	void accept(Visitor *visitor) {
		visitor->visit(this);
	}
};

class ASTAssignmentStatement : public ASTStatement {
public:
	ASTIdentifier *id;
	ASTExpression *rhs;
	ASTAssignmentStatement(ASTIdentifier *id, ASTExpression *rhs) {
		this->id = id;
		this->rhs = rhs;
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
};

class ASTPrintStatement : public ASTStatement {
public:
	string delim = "";
	vector<ASTPrintable*> *printable;
	ASTPrintStatement(vector<ASTPrintable*> *printable, string delim = "") {
		this->printable = printable;
		this->delim = delim;
	}
};

class ASTReadStatement : public ASTStatement {
public:
	vector<ASTIdentifier*> *ids;
	ASTReadStatement(vector<ASTIdentifier*> *ids) {
		this->ids = ids;
	}
};

class ASTLabel : public ASTStatement {
public:
	string label_name;
	ASTLabel(string label_name) {
		this->label_name = label_name;
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
};

class ASTForStatement : public ASTStatement {
public:
	ASTStatement *init;
	ASTExpression *limit;
	ASTStatement *step;
	ASTCodeBlock *code_block;
	ASTForStatement(ASTStatement *init, ASTExpression *limit, ASTStatement *step, vector<ASTStatement*> *statements) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(limit);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->init = init;
			this->limit = bool_exp;
			this->step = step;
			this->code_block = new ASTCodeBlock(statements);
		}
	}
	ASTForStatement(ASTStatement *init, ASTExpression *limit, vector<ASTStatement*> *statements) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(limit);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->init = init;
			this->limit = bool_exp;
			this->code_block = new ASTCodeBlock(statements);
			this->step = NULL;
		}
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
};

#endif
