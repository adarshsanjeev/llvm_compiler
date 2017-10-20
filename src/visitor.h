#ifndef _VISITOR_H
#define _VISITOR_H

#include <cmath>

ASTProgram *root = NULL;

class interpreterVisitor : public Visitor {
	map<ASTIdentifier, int> variableTable;
	stack<int> eval_stack;
public:

    void print_map() {
		for (auto i = variableTable.begin(); i != variableTable.end(); i++) {
			cout << (*i).first.id << "=" << (*i).second << ", ";
		}
		cout << endl;
	}

	int evaluate_and_return(ASTExpression *ast) {
		ast->accept(this);
		int temp = eval_stack.top();
		eval_stack.pop();
		return temp;
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

	void visit(ASTAssignmentStatement *assignmentStatement) {
		if (variableTable.find(*(assignmentStatement->id)) != variableTable.end()) {
			assignmentStatement->rhs->accept(this);
			variableTable[*(assignmentStatement->id)] = eval_stack.top();
			eval_stack.pop();
		}
		else {
			cerr << "Compilation error: Undeclared variable used:" << (assignmentStatement->id)->id << endl;
			exit(-1);
		}
	}

	void visit(ASTReadStatement *readStatement) {
		for (auto i = readStatement->ids->begin(); i != readStatement->ids->end(); i++) {
			cin >> variableTable[**i];
		}
	}

	void visit(ASTWhileStatement *whileStatement) {
		while (evaluate_and_return((ASTExpression*)whileStatement->cond)) {
			(whileStatement->code_block->accept(this));
		}
	}

	void visit(ASTIfStatement *ifStatement) {
		if (evaluate_and_return(((ASTExpression*)ifStatement->cond))) {
			(ifStatement->then_block->accept(this));
		}
		else {
			(ifStatement->else_block->accept(this));
		}
	}

	void visit(ASTForStatement *forStatement) {
		(forStatement->init->accept(this));
		while (evaluate_and_return(forStatement->limit)) {
			(forStatement->code_block->accept(this));
			if (forStatement->step)
				(forStatement->step->accept(this));
		}
	}

	void visit(ASTPrintStatement *printStatement) {
		for (auto i = printStatement->printable->begin(); i != printStatement->printable->end(); i++) {
			if ((*i)->id == NULL)
				cout << (*i)->text;
			else
				cout << variableTable[*((*i)->id)];
			cout << " ";
		}
		cout << printStatement->delim;
	}

	void visit(ASTIntegerLiteral *int_literal) {
		eval_stack.push(int_literal->value);
	}

	void visit(ASTBinaryExpression *bin_exp) {
		switch(bin_exp->op) {
		case PLUS:
			eval_stack.push(evaluate_and_return(bin_exp->left_child) + evaluate_and_return(bin_exp->right_child));
			break;
		case MINUS:
			eval_stack.push(evaluate_and_return(bin_exp->left_child) - evaluate_and_return(bin_exp->right_child));
			break;
		case PRODUCT:
			eval_stack.push(evaluate_and_return(bin_exp->left_child) * evaluate_and_return(bin_exp->right_child));
			break;
		case DIVIDE:
			eval_stack.push(evaluate_and_return(bin_exp->left_child) / evaluate_and_return(bin_exp->right_child));
			break;
		case MODULUS:
			eval_stack.push(evaluate_and_return(bin_exp->left_child) % evaluate_and_return(bin_exp->right_child));
			break;
		case EXPONENT:
			eval_stack.push(pow(evaluate_and_return(bin_exp->left_child), evaluate_and_return(bin_exp->right_child)));
			break;
		}
	}

	void visit(ASTBooleanExpression *bool_exp) {
		switch(bool_exp->op) {
		case LESSTHAN:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) < evaluate_and_return(bool_exp->right_child));
			break;
		case GREATERTHAN:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) > evaluate_and_return(bool_exp->right_child));
			break;
		case LESSEQUAL:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) <= evaluate_and_return(bool_exp->right_child));
			break;
		case GREATEREQUAL:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) >= evaluate_and_return(bool_exp->right_child));
			break;
		case NOTEQUAL:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) != evaluate_and_return(bool_exp->right_child));
			break;
		case EQUALEQUAL:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) == evaluate_and_return(bool_exp->right_child));
			break;
		case AND_OP:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) and evaluate_and_return(bool_exp->right_child));
			break;
		case OR_OP:
			eval_stack.push(evaluate_and_return(bool_exp->left_child) or evaluate_and_return(bool_exp->right_child));
			break;
		default: cerr << "Compiler Error: Unknown case of operator" << endl;
			exit(-1);
		}
	}

	void visit(ASTIdentifier *id) {
		if (variableTable.find(*id) != variableTable.end()) {
			eval_stack.push(variableTable[*id]);
		}
		else {
			cerr << "Compilation error: Undeclared variable used: " << id->id << endl;
			exit(-1);
		}
	}
};

#endif
