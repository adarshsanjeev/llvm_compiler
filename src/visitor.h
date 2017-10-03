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
		ASTSingleIdentifier *single_id = dynamic_cast<ASTSingleIdentifier *>(ast);
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
		ASTSingleIdentifier *single_id = dynamic_cast<ASTSingleIdentifier *>(ast);
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
public:
	map<ASTIdentifier*, int> variableTable;

	void interpret(ASTProgram *ast) {
		interpret(ast->declBlock);
		interpret(ast->codeBlock);
	}

	void interpret(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			variableTable[*i] = 0;
		}
	}

	void interpret(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			interpret(*i);
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
		}
	}
};

#endif
