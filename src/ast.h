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

class ASTStatementList;

enum BinOp {
    plus,
    minus,
    product,
    divide,
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
	std::vector<ASTStatement> statements;
	void add_to_list(ASTStatement *statement) {
		statements.push_back(*statement);
	}
};

class ASTExpression : public ASTNode {
public:
	ASTExpression() {
	}
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
};

class ASTAssignmentStatement : public ASTStatement {
public:
	ASTIdentifier *id;
	ASTExpression *rhs;
	ASTAssignmentStatement(ASTExpression *rhs) {

	}
};
#endif
