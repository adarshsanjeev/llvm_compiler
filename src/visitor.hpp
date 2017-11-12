#ifndef _VISITOR_H
#define _VISITOR_H

#include <cmath>

ASTProgram *root = NULL;

union id_values{
	int value;
	int *array;
};

class interpreterVisitor : public Visitor {
	map<ASTIdentifier, id_values> variable_table;
	stack<int> eval_stack;
public:

    void print_map() {
		for (auto i = variable_table.begin(); i != variable_table.end(); i++) {
			cout << (*i).first.id << "=" << (*i).second.value << ", ";
		}
		cout << endl;
	}

	int evaluate_and_return(ASTExpression *ast) {
		ast->accept(this);
		int temp = eval_stack.top();
		eval_stack.pop();
		return temp;
	}

	void* visit(ASTProgram *ast) {
		(ast->decl_block->accept(this));
		(ast->code_block->accept(this));
		return NULL;
	}

	void* visit(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(*i);
			if (array_id) {
				ASTIntegerLiteral *literal = dynamic_cast<ASTIntegerLiteral *>(array_id->index);
				if (!literal) {
					cerr << "Wrong declaration syntax";
					exit(-1);
				}

				variable_table[**i].array = new int [literal->value];
				for (int j=0; j<literal->value; j++) {
					variable_table[**i].array[j] = 0;
				}
			}
			else
				variable_table[**i].value = 0;
		}
		return NULL;
	}

	void* visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			((*i)->accept(this));
			// print_map();
		}
		return NULL;
	}

	void* visit(ASTLabel *ast) {
	}

	void* visit(ASTGoToStatement *ast) {
	}

	void* visit(ASTAssignmentStatement *assignmentStatement) {
		if (variable_table.find(*(assignmentStatement->id)) != variable_table.end()) {
			assignmentStatement->rhs->accept(this);
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(assignmentStatement->id);
			if (array_id) {
				variable_table[*array_id].array[evaluate_and_return(array_id->index)] = eval_stack.top();
			}
			else {
				variable_table[*(assignmentStatement->id)].value = eval_stack.top();
			}
			eval_stack.pop();
		}
		else {
			cerr << "Compilation error: Undeclared variable used:" << (assignmentStatement->id)->id << endl;
			exit(-1);
		}
		return NULL;
	}

	void* visit(ASTReadStatement *readStatement) {
		for (auto i = readStatement->ids->begin(); i != readStatement->ids->end(); i++) {
			cin >> variable_table[**i].value;
		}
		return NULL;
	}

	void* visit(ASTWhileStatement *whileStatement) {
		while (evaluate_and_return((ASTExpression*)whileStatement->cond)) {
			(whileStatement->code_block->accept(this));
		}
		return NULL;
	}

	void* visit(ASTIfStatement *ifStatement) {
		if (evaluate_and_return(((ASTExpression*)ifStatement->cond))) {
			(ifStatement->then_block->accept(this));
		}
		else {
			(ifStatement->else_block->accept(this));
		}
		return NULL;
	}

	void* visit(ASTForStatement *forStatement) {
		forStatement->init->accept(this);
		while (evaluate_and_return(forStatement->init->id) <= evaluate_and_return(forStatement->limit)) {
			(forStatement->code_block->accept(this));
			(new ASTAssignmentStatement(forStatement->init->id, new ASTBinaryExpression(forStatement->init->id, forStatement->step, BinOp::PLUS)))->accept(this);
		}
		return NULL;
	}

	void* visit(ASTPrintStatement *printStatement) {
		for (auto i = printStatement->printable->begin(); i != printStatement->printable->end(); i++) {
			if ((*i)->id == NULL)
				cout << (*i)->text;
			else {
				ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>((*i)->id);
				if (array_id){
					cout << variable_table[*array_id].array[evaluate_and_return(array_id->index)];
				}
				else {
					cout << variable_table[*((*i)->id)].value;
				}
			}
			cout << " ";
		}
		cout << printStatement->delim;
		return NULL;
	}

	void* visit(ASTIntegerLiteral *int_literal) {
		eval_stack.push(int_literal->value);
		return NULL;
	}

	void* visit(ASTBinaryExpression *bin_exp) {
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
		}
		return NULL;
	}

	void* visit(ASTBooleanExpression *bool_exp) {
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
		return NULL;
	}

	void* visit(ASTIdentifier *id) {
		if (variable_table.find(*id) != variable_table.end()) {
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(id);
			if (array_id) {
				eval_stack.push(variable_table[*id].array[evaluate_and_return(array_id->index)]);
			}
			else
				eval_stack.push(variable_table[*id].value);
		}
		else {
			cerr << "Compilation error: Undeclared variable used: " << id->id << endl;
			exit(-1);
		}
		return NULL;
	}
};

#endif
