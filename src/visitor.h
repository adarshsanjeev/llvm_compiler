#ifndef _VISITOR_H
#define _VISITOR_H

#include <cmath>

ASTProgram *root = NULL;

class interpreterVisitor : public Visitor {
	map<ASTIdentifier, int> variableTable;
public:

    void print_map() {
		for (auto i = variableTable.begin(); i != variableTable.end(); i++) {
			cout << (*i).first.id << "=" << (*i).second << ", ";
		}
		cout << endl;
	}

	void visit(ASTProgram *ast) {
		(ast->decl_block->accept(this));
		(ast->code_block->accept(this));
	}

	void visit(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(*i);
			if (array_id) {
				ASTIntegerLiteral *literal = dynamic_cast<ASTIntegerLiteral *>(array_id->index);
				if (!literal) {
					cerr << "Wrong declaration syntax";
					exit(-1);
				}
				for (int j=0; j<literal->value; j++) {
					variableTable[*(new ASTIdentifier(array_id->id+"["+to_string(j)+"]"))];
				}
			}
			variableTable[**i] = 0;
		}
	}

	void visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			((*i)->accept(this));
			// print_map();
		}
	}

	void visit(ASTStatement *ast) {
		ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(ast);
		ASTReadStatement *readStatement = dynamic_cast<ASTReadStatement *>(ast);
		ASTWhileStatement *whileStatement = dynamic_cast<ASTWhileStatement *>(ast);
		ASTIfStatement *ifStatement = dynamic_cast<ASTIfStatement *>(ast);
		ASTForStatement *forStatement = dynamic_cast<ASTForStatement *>(ast);
		/* ASTLabel *label = dynamic_cast<ASTLabel *>(ast); */
		/* ASTGoToStatement *goTo = dynamic_cast<ASTGoToStatement *>(ast); */
		ASTPrintStatement *printStatement = dynamic_cast<ASTPrintStatement *>(ast);

		if (assignmentStatement) {
			if (variableTable.find(*(assignmentStatement->id)) != variableTable.end()) {
				variableTable[*(assignmentStatement->id)] = (assignmentStatement->rhs->accept(this));
			}
			else {
				cerr << "Compilation error: Undeclared variable used:" << (assignmentStatement->id)->id << endl;
				exit(-1);
			}
		}
		else if (printStatement) {
			for (auto i = printStatement->printable->begin(); i != printStatement->printable->end(); i++) {
				if ((*i)->id == NULL)
					cout << (*i)->text;
				else
					cout << variableTable[*((*i)->id)];
				cout << " ";
			}
			cout << printStatement->delim;
		}
		else if (readStatement) {
			for (auto i = readStatement->ids->begin(); i != readStatement->ids->end(); i++) {
				cin >> variableTable[**i];
			}
		}
		else if (ifStatement) {
			if (((ASTExpression*)ifStatement->cond)->accept(this)) {
				(ifStatement->then_block->accept(this));
			}
			else {
				(ifStatement->else_block->accept(this));
			}
		}
		else if (whileStatement) {
			while (((ASTExpression*)whileStatement->cond)->accept(this)) {
				(whileStatement->code_block->accept(this));
			}
		}
		else if (forStatement) {
			(forStatement->init->accept(this));
			while ((forStatement->limit->accept(this))) {
				(forStatement->code_block->accept(this));
				if (forStatement->step)
					(forStatement->step->accept(this));
			}
		}
	}

	int visit(ASTExpression *ast) {
		ASTIntegerLiteral *int_literal = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *bin_exp = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTBooleanExpression *bool_exp = dynamic_cast<ASTBooleanExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);

		if (int_literal) {
			return int_literal->value;
		}
		else if (id) {
			if (variableTable.find(*id) != variableTable.end()) {
				return variableTable[*id];
			}
			else {
				cerr << "Compilation error: Undeclared variable used: " << id->id << endl;
				exit(-1);
			}
		}
		else if (bin_exp) {
			switch(bin_exp->op) {
			case PLUS: return (bin_exp->left_child->accept(this)) + (bin_exp->right_child->accept(this));
				break;
			case MINUS: return (bin_exp->left_child->accept(this)) - (bin_exp->right_child->accept(this));
				break;
			case PRODUCT: return (bin_exp->left_child->accept(this)) * (bin_exp->right_child->accept(this));
				break;
			case DIVIDE: return (bin_exp->left_child->accept(this)) / (bin_exp->right_child->accept(this));
				break;
			case MODULUS: return (bin_exp->left_child->accept(this)) % (bin_exp->right_child->accept(this));
				break;
			case EXPONENT: return pow((bin_exp->left_child->accept(this)), (bin_exp->right_child->accept(this)));
				break;
			}
		}
		else if (bool_exp) {
			switch(bool_exp->op) {
			case LESSTHAN: return (bool_exp->left_child->accept(this)) < (bool_exp->right_child->accept(this));
				break;
			case GREATERTHAN: return (bool_exp->left_child->accept(this)) > (bool_exp->right_child->accept(this));
				break;
			case LESSEQUAL: return (bool_exp->left_child->accept(this)) <= (bool_exp->right_child->accept(this));
				break;
			case GREATEREQUAL: return (bool_exp->left_child->accept(this)) >= (bool_exp->right_child->accept(this));
				break;
			case NOTEQUAL: return (bool_exp->left_child->accept(this)) != (bool_exp->right_child->accept(this));
				break;
			case EQUALEQUAL: return (bool_exp->left_child->accept(this)) == (bool_exp->right_child->accept(this));
				break;
			case AND_OP: return (bool_exp->left_child->accept(this)) and (bool_exp->right_child->accept(this));
				break;
			case OR_OP: return (bool_exp->left_child->accept(this)) or (bool_exp->right_child->accept(this));
				break;
			default: cerr << "Compiler Error: Unknown case of operator" << endl;
				exit(-1);
			}
		}
		return -1;
	}
};

#endif
