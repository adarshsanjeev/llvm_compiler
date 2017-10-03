#ifndef _VISITOR_H
#define _VISITOR_H
ASTProgram *root = NULL;

class Visitor {

};

void visit(ASTExpression* ast) {
}

class printableVisitor : public Visitor {
public:
	void visit(ASTProgram* ast) {
		visit(ast->declBlock);
		visit(ast->codeBlock);
	}
	void visit(ASTDeclBlock* ast) {
		cout << "Declarations" << endl;
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			visit(*i);
			cout << " ";
		}
		cout << endl;
	}
	void visit(ASTCodeBlock* ast) {
		cout << "Starting Code Block {" << endl;
		for (auto i = ast->statements->end()-1; i >= ast->statements->begin(); i--) {
			visit(*i);
		}
		cout << "} Ending Code Block" << endl;
	}

	void visit(ASTIdentifier* ast) {
		ASTIdentifier *single_id = dynamic_cast<ASTIdentifier *>(ast);
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (single_id) {
			cout << single_id->id;
		}
		if (array_id) {
			cout << array_id->id;
			cout << "[";
			visit(array_id->index);
			cout << "]";
		}
	}

	void visit(ASTPrintStatement* ast) {
		ASTIdentifier *single_id = dynamic_cast<ASTIdentifier *>(ast);
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (single_id) {
			cout << single_id->id;
		}
		if (array_id) {
			cout << array_id->id;
			cout << "[";
			visit(array_id->index);
			cout << "]";
		}
	}

	void visit(ASTExpression* ast) {
		ASTIntegerLiteral *int_literal = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *bin_exp = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTBooleanExpression *bool_exp = dynamic_cast<ASTBooleanExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);
		if (int_literal) {
			cout << int_literal->value;
		}
		else if (bin_exp) {
			visit(bin_exp->left_child);
			cout << "[" << bin_exp->op << "]";
			visit(bin_exp->right_child);
		}
		else if (bool_exp) {
			visit(bool_exp->left_child);
			cout << "[" << bool_exp->op << "]";
			visit(bool_exp->right_child);
		}
		else if (id) {
			visit(id);
		}
		else if (bool_exp) {
		}
		else {
			cerr<< "Unknown Type of expression!";
		}
	}

	void visit(ASTStatement* ast) {
		ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(ast);
		ASTReadStatement *readStatement = dynamic_cast<ASTReadStatement *>(ast);
		ASTWhileStatement *whileStatement = dynamic_cast<ASTWhileStatement *>(ast);
		ASTIfStatement *ifStatement = dynamic_cast<ASTIfStatement *>(ast);
		ASTForStatement *forStatement = dynamic_cast<ASTForStatement *>(ast);
		ASTLabel *label = dynamic_cast<ASTLabel *>(ast);
		ASTGoToStatement *goTo = dynamic_cast<ASTGoToStatement *>(ast);
		ASTPrintStatement *printStatement = dynamic_cast<ASTPrintStatement *>(ast);

		if (assignmentStatement) {
			cout << "Assignment Statement:";
			visit(assignmentStatement->id);
			cout<<" = ";
			visit(assignmentStatement->rhs);
			cout<<endl;
		}
		else if (readStatement) {
			cout << "Read ";
			for (auto i = readStatement->ids->begin(); i != readStatement->ids->end(); i++) {
				visit(*i);
				cout << " ";
			}
			cout << endl;
		}
		else if (whileStatement) {
			cout << "WHILE ";
			visit(whileStatement->cond);
			cout << endl;
			visit(whileStatement->codeBlock);
		}
		else if (ifStatement) {
			cout << "IF ";
			visit(ifStatement->cond);
			cout << endl;
			visit(ifStatement->then_block);
			cout << endl;
			if (ifStatement->else_block) {
				cout << "ELSE" << endl;
				visit(ifStatement->else_block);
				cout << endl;
			}
		}
		else if (forStatement) {
			cout << "FOR ";
			visit(forStatement->init);
			cout << ", ";
			visit(forStatement->limit);
			if (forStatement->step) {
				cout << ", ";
				visit(forStatement->step);
			}
			cout << endl;
			visit(forStatement->codeBlock);
		}
		else if (label) {
			cout<<label->label_name<<":"<<endl;
		}
		else if (goTo) {
			cout<< "goto " << goTo->label_name;
			if (goTo->cond) {
				cout << " if ";
				visit(goTo->cond);
			}
			cout << endl;
		}
		else if (printStatement) {
			cout << "PRINT ";
			for (auto i = (*(printStatement->printable)).begin(); i !=(*(printStatement->printable)).end(); i++) {
				if ((*i)->id == NULL)
					cout << (*i)->text << " ";
				else
					visit((*i)->id);
			}
			cout << endl;
		}
		else {
			cerr << "Unknown statement error";
		}
	}
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
		interpret(ast->declBlock);
		interpret(ast->codeBlock);
	}

	void interpret(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			variableTable[**i] = 0;
		}
	}

	void interpret(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			interpret(*i);
			print_map();
		}
	}

	void interpret(ASTStatement *ast) {
		ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(ast);
		/* ASTReadStatement *readStatement = dynamic_cast<ASTReadStatement *>(ast); */
		/* ASTWhileStatement *whileStatement = dynamic_cast<ASTWhileStatement *>(ast); */
		/* ASTIfStatement *ifStatement = dynamic_cast<ASTIfStatement *>(ast); */
		/* ASTForStatement *forStatement = dynamic_cast<ASTForStatement *>(ast); */
		/* ASTLabel *label = dynamic_cast<ASTLabel *>(ast); */
		/* ASTGoToStatement *goTo = dynamic_cast<ASTGoToStatement *>(ast); */
		/* ASTPrintStatement *printStatement = dynamic_cast<ASTPrintStatement *>(ast); */

		if (assignmentStatement) {
			if (variableTable.find(*(assignmentStatement->id)) != variableTable.end()) {
				variableTable[*(assignmentStatement->id)] = interpret(assignmentStatement->rhs);
			}
			else {
				cerr << "Undeclared variable used:" << endl;
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
				ASTIdentifier *single_id = dynamic_cast<ASTIdentifier *>(ast);
				return variableTable[*id];
			}
			else {
				cerr << "Undeclared variable used:" << endl;
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
			}
		}
		else if (bool_exp) {
			switch(bool_exp->op) {
			case LESSTHAN: return interpret(bin_exp->left_child) < interpret(bin_exp->right_child);
				break;
			case GREATERTHAN: return interpret(bin_exp->left_child) > interpret(bin_exp->right_child);
				break;
			case LESSEQUAL: return interpret(bin_exp->left_child) <= interpret(bin_exp->right_child);
				break;
			case GREATEREQUAL: return interpret(bin_exp->left_child) >= interpret(bin_exp->right_child);
				break;
			case NOTEQUAL: return interpret(bin_exp->left_child) != interpret(bin_exp->right_child);
				break;
			case EQUALEQUAL: return interpret(bin_exp->left_child) == interpret(bin_exp->right_child);
				break;
			case AND_OP: return interpret(bin_exp->left_child) and interpret(bin_exp->right_child);
				break;
			case OR_OP: return interpret(bin_exp->left_child) or interpret(bin_exp->right_child);
				break;
			}
		}
	}
};

/* class SymbolTable : public ASTNode { */
/* public: */
/* 	void addToMap(vector<ASTIdentifier*> *ids) { */
/* 		for (auto i = ids->begin(); i!=ids->end(); i++) { */
/* 			ASTSingleIdentifier* single_id = dynamic_cast<ASTSingleIdentifier *>(*i); */
/* 			ASTArrayIdentifier* array_id = dynamic_cast<ASTArrayIdentifier *>(*i); */
/* 			if (single_id != NULL) { */
/* 				if (checkIfDeclared(single_id)) */
/* 					cerr<<"Already Declared" << endl; */
/* 				else */
/* 					variableTable[single_id->id] = 0; */
/* 			} */
/* 			if (array_id != NULL) { */
/* 				if (checkIfDeclared(array_id)) */
/* 					cerr<<"Already Declared" << endl; */
/* 				else */
/* 					variableTable[array_id->id] = 0; */
/* 			} */
/* 		} */
/* 	} */
/* 	bool checkIfDeclared (ASTIdentifier *id) { */
/* 		ASTSingleIdentifier* single_id = dynamic_cast<ASTSingleIdentifier *>(id); */
/* 		ASTArrayIdentifier* array_id = dynamic_cast<ASTArrayIdentifier *>(id); */
/* 		if (single_id != NULL) { */
/* 			variableTable.find(single_id->id) != variableTable.end(); */
/* 		} */
/* 		if (array_id != NULL) { */
/* 			variableTable.find(array_id->id) != variableTable.end(); */
/* 		} */
/* 		return false; */
/* 	} */
/* }*symbolTable; */

#endif
