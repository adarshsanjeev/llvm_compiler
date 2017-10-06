#ifndef _AST_H
#define _AST_H
#include <vector>
#include <string>

class ASTNode;

class ASTProgram;
class ASTDeclBlock;
class ASTCodeBlock;

class ASTExpression;
class ASTBinaryExpression;
class ASTIntegerLiteral;

class ASTStatement;
class ASTAssignmentStatement;

enum BinOp {
    plus,
    minus,
    product,
    divide
};

enum BoolOp {
    lessthan,
    greaterthan,
    lessequal,
    greaterequal,
    notequal,
    equalequal,
    and_op,
    or_op
};

class ASTNode {
};

class ASTProgram : public ASTNode {
public:
	ASTDeclBlock *declBlock;
	ASTCodeBlock *codeBlock;
	ASTProgram(ASTDeclBlock *declBlock, ASTCodeBlock *codeBlock) {
		this->declBlock = declBlock;
		this->codeBlock = codeBlock;
	}
};

class ASTDeclBlock : public ASTNode {
};

class ASTCodeBlock : public ASTNode {
public:
	std::vector<ASTStatement*> *statements;
	ASTCodeBlock (std::vector<ASTStatement*> *statements) {
		this->statements = statements;
	}
};

class ASTExpression : public ASTNode {
public:
	ASTExpression() {
	}
	virtual ~ASTExpression () {}
};

class ASTIntegerLiteral : public ASTExpression {
public:
	int value;
	ASTIntegerLiteral(int value) {
		this->value = value;
	}
};

class ASTBinaryExpression : public ASTExpression {
public:
	ASTExpression *left_child;
	ASTExpression *right_child;
	BinOp op;

	ASTBinaryExpression (ASTExpression *left, ASTExpression *right, BinOp op) {
		this->left_child = left;
		this->right_child = right;
		this->op = op;
	};
};

class ASTBooleanExpression : public ASTExpression {
public:
	ASTExpression *left_child;
	ASTExpression *right_child;
	BoolOp op;

	ASTBooleanExpression (ASTExpression *left, ASTExpression *right, BoolOp op) {
		this->left_child = left;
		this->right_child = right;
		this->op = op;
	};
};

class ASTIdentifier : public ASTExpression {
};

class ASTSingleIdentifier : public ASTIdentifier {
public:
	std::string id;
	ASTSingleIdentifier(std::string id) {
		this->id = id;
	}
};

class ASTArrayIdentifier : public ASTIdentifier {
public:
	std::string id;
	ASTExpression* index;
	ASTArrayIdentifier(std::string id, ASTExpression *index) {
		this->id = id;
		this->index = index;
	}
};

class ASTStatement : public ASTNode {
public:
	virtual ~ASTStatement() {}
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

class ASTPrintStatement : public ASTStatement {
public:
	std::vector<std::string> values;
	void add_to_list(std::string value) {
		values.push_back(value);
	}
};

class ASTReadStatement : public ASTStatement {
public:
	std::vector<ASTIdentifier*> *ids;
	ASTReadStatement(std::vector<ASTIdentifier*> *ids) {
		this->ids = ids;
	}
};

class ASTWhileStatement : public ASTStatement {
public:
	ASTBooleanExpression *cond;
	std::vector<ASTStatement> *codeBlock;
	ASTWhileStatement(ASTBooleanExpression *cond, std::vector<ASTStatement> *codeBlock) {
		this->cond = cond;
		this->codeBlock = codeBlock;
	}
};

#endif
