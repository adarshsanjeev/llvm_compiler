#ifndef _VISITOR_H
#define _VISITOR_H

#include <cmath>

ASTProgram *root = NULL;

class Visitor {
};

class interpreterVisitor : public Visitor {
map<ASTIdentifier, int> variableTable;
public:

    void print_map() {
		for (auto i = variableTable.begin(); i != variableTable.end(); i++) {
			cout << (*i).first.id << "=" << (*i).second << ", ";
		}
		cout << endl;
	}

	void interpret(ASTProgram *ast) {
		interpret(ast->decl_block);
		interpret(ast->code_block);
	}

	void interpret(ASTDeclBlock *ast) {
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

	void interpret(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			interpret(*i);
			// print_map();
		}
	}

	void interpret(ASTStatement *ast) {
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
				variableTable[*(assignmentStatement->id)] = interpret(assignmentStatement->rhs);
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
			if (interpret(ifStatement->cond)) {
				interpret(ifStatement->then_block);
			}
			else {
				interpret(ifStatement->else_block);
			}
		}
		else if (whileStatement) {
			while (interpret(whileStatement->cond)) {
				interpret(whileStatement->code_block);
			}
		}
		else if (forStatement) {
			interpret(forStatement->init);
			while (interpret(forStatement->limit)) {
				interpret(forStatement->code_block);
				if (forStatement->step)
					interpret(forStatement->step);
			}
		}
	}

	int interpret(ASTExpression *ast) {
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
			case PLUS: return interpret(bin_exp->left_child) + interpret(bin_exp->right_child);
				break;
			case MINUS: return interpret(bin_exp->left_child) - interpret(bin_exp->right_child);
				break;
			case PRODUCT: return interpret(bin_exp->left_child) * interpret(bin_exp->right_child);
				break;
			case DIVIDE: return interpret(bin_exp->left_child) / interpret(bin_exp->right_child);
				break;
			case MODULUS: return interpret(bin_exp->left_child) % interpret(bin_exp->right_child);
				break;
			case EXPONENT: return pow(interpret(bin_exp->left_child), interpret(bin_exp->right_child));
				break;
			}
		}
		else if (bool_exp) {
			switch(bool_exp->op) {
			case LESSTHAN: return interpret(bool_exp->left_child) < interpret(bool_exp->right_child);
				break;
			case GREATERTHAN: return interpret(bool_exp->left_child) > interpret(bool_exp->right_child);
				break;
			case LESSEQUAL: return interpret(bool_exp->left_child) <= interpret(bool_exp->right_child);
				break;
			case GREATEREQUAL: return interpret(bool_exp->left_child) >= interpret(bool_exp->right_child);
				break;
			case NOTEQUAL: return interpret(bool_exp->left_child) != interpret(bool_exp->right_child);
				break;
			case EQUALEQUAL: return interpret(bool_exp->left_child) == interpret(bool_exp->right_child);
				break;
			case AND_OP: return interpret(bool_exp->left_child) and interpret(bool_exp->right_child);
				break;
			case OR_OP: return interpret(bool_exp->left_child) or interpret(bool_exp->right_child);
				break;
			}
		}
	}
};

#endif
