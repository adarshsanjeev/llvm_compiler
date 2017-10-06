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
	vector<ASTStatement*> *statements;
	ASTCodeBlock (vector<ASTStatement*> *statements) {
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

	ASTBinaryExpression(ASTExpression *left, ASTExpression *right, BinOp op) {
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

	ASTBooleanExpression(ASTExpression *left, ASTExpression *right, BoolOp op) {
		this->left_child = left;
		this->right_child = right;
		this->op = op;
	};
};

class ASTIdentifier : public ASTExpression {
};

class ASTSingleIdentifier : public ASTIdentifier {
public:
	string id;
	ASTSingleIdentifier(string id) {
		this->id = id;
	}
};

class ASTArrayIdentifier : public ASTIdentifier {
public:
	string id;
	ASTExpression* index;
	ASTArrayIdentifier(string id, ASTExpression *index) {
		this->id = id;
		this->index = index;
	}
};

map<string, int> variableTable;

class SymbolTable : public ASTNode {
public:
	void addToMap(vector<ASTIdentifier*> *ids) {
		for (auto i = ids->begin(); i!=ids->end(); i++) {
			ASTSingleIdentifier* single_id = dynamic_cast<ASTSingleIdentifier *>(*i);
			ASTArrayIdentifier* array_id = dynamic_cast<ASTArrayIdentifier *>(*i);
			if (single_id != NULL) {
				if (checkIfDeclared(single_id))
					cerr<<"Already Declared" << endl;
				else
					variableTable[single_id->id] = 0;
			}
			if (array_id != NULL) {
				if (checkIfDeclared(array_id))
					cerr<<"Already Declared" << endl;
				else
					variableTable[array_id->id] = 0;
			}
		}
	}
	bool checkIfDeclared (ASTIdentifier *id) {
		ASTSingleIdentifier* single_id = dynamic_cast<ASTSingleIdentifier *>(id);
		ASTArrayIdentifier* array_id = dynamic_cast<ASTArrayIdentifier *>(id);
		if (single_id != NULL) {
			variableTable.find(single_id->id) != variableTable.end();
		}
		if (array_id != NULL) {
			variableTable.find(array_id->id) != variableTable.end();
		}
	}
}*symbolTable;

class ASTStatement : public ASTNode {
public:
	virtual ~ASTStatement() {}
};

class ASTAssignmentStatement : public ASTStatement {
public:
	ASTIdentifier *id;
	ASTExpression *rhs;
	ASTAssignmentStatement(ASTIdentifier *id, ASTExpression *rhs) {
		if (!symbolTable->checkIfDeclared(id)) {
			cerr << "Undeclared variable used" << endl;
			exit(-1);
		}
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
		if (!symbolTable->checkIfDeclared(id)) {
			cerr << "Undeclared variable used" << endl;
			exit(-1);
		}
		this->id = id;
	}
};

class ASTPrintStatement : public ASTStatement {
public:
	vector<ASTPrintable*> *printable;
	ASTPrintStatement(vector<ASTPrintable*> *printable) {
		this->printable = printable;
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
	ASTCodeBlock *codeBlock;
	ASTWhileStatement(ASTExpression *cond, vector<ASTStatement*> *statements) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(cond);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->cond = bool_exp;
			this->codeBlock = new ASTCodeBlock(statements);
		}
	}
};

class ASTForStatement : public ASTStatement {
public:
	ASTStatement *init;
	ASTExpression *limit;
	ASTExpression *step;
	ASTCodeBlock *codeBlock;
	ASTForStatement(ASTStatement *init, ASTExpression *limit, ASTExpression *step, vector<ASTStatement*> *statements) {
		ASTBooleanExpression* bool_exp = dynamic_cast<ASTBooleanExpression *>(limit);
		if (!bool_exp) {
			cerr << "Found non boolean expression" << endl;
		}
		else {
			this->init = init;
			this->limit = bool_exp;
			this->step = step;
			this->codeBlock = new ASTCodeBlock(statements);
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
			this->codeBlock = new ASTCodeBlock(statements);
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
