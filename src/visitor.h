#ifndef _VISITOR_H
#define _VISITOR_H

ASTProgram *root = NULL;

class Visitor {

};

void visit(ASTExpression* ast) {
}

class printableVisitor : public Visitor {
public:
	void visit (ASTProgram* ast) {
		visit(ast->declBlock);
		visit(ast->codeBlock);
	}
	void visit (ASTDeclBlock* ast) {
	}
	void visit (ASTCodeBlock* ast) {
		std::cout << "Starting Code Block {" << std::endl;
		for (std::vector<ASTStatement*>::iterator i = ast->statements->end()-1; i >= ast->statements->begin(); i--) {
			visit(*i);
		}
		std::cout << "} Ending Code Block" << std::endl;
	}

	void visit (ASTIdentifier* ast) {
		ASTSingleIdentifier *single_id = dynamic_cast<ASTSingleIdentifier *>(ast);
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (single_id) {
			std::cout << single_id->id;
		}
		if (array_id) {
			std::cout << array_id->id;
			std::cout << "[";
			visit(array_id->index);
			std::cout << "]";
		}
	}

	void visit (ASTExpression* ast) {
		ASTIntegerLiteral *int_literal = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *bin_exp = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTBooleanExpression *bool_exp = dynamic_cast<ASTBooleanExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);
		if (int_literal) {
			std::cout << int_literal->value;
		}
		else if (bin_exp) {
			visit(bin_exp->left_child);
			std::cout << "[" << bin_exp->op << "]";
			visit(bin_exp->right_child);
		}
		else if (bool_exp) {
			visit(bool_exp->left_child);
			std::cout << "[" << bool_exp->op << "]";
			visit(bool_exp->right_child);
		}
		else if (id) {
			visit(id);
		}
		else if (bool_exp) {
		}
		else {
			std::cerr<< "Unknown Type of expression!";
		}
	}

	void visit (ASTStatement* ast) {
		ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(ast);
		ASTReadStatement *readStatement = dynamic_cast<ASTReadStatement *>(ast);
		ASTWhileStatement *whileStatement = dynamic_cast<ASTWhileStatement *>(ast);
		ASTIfStatement *ifStatement = dynamic_cast<ASTIfStatement *>(ast);
		if (assignmentStatement) {
			std::cout << "Assignment Statement:";
			visit(assignmentStatement->id);
			std::cout<<" = ";
			visit(assignmentStatement->rhs);
			std::cout<<std::endl;
		}
		else if (readStatement) {
			std::cout << "Read ";
			for (std::vector<ASTIdentifier*>::iterator i = readStatement->ids->begin(); i != readStatement->ids->end(); i++) {
				visit(*i);
				std::cout << " ";
			}
			std::cout << std::endl;
		}
		else if (whileStatement) {
			std::cout << "WHILE ";
			visit (whileStatement->cond);
			std::cout << std::endl;
			visit (whileStatement->codeBlock);
		}
		else if (ifStatement) {
			std::cout << "IF ";
			visit (ifStatement->cond);
			std::cout << std::endl;
			visit (ifStatement->then_block);
			std::cout << std::endl;
			if (ifStatement->else_block) {
				std::cout << "ELSE" << std::endl;
				visit (ifStatement->else_block);
				std::cout << std::endl;
			}
		}
		else {
			std::cerr << "Unknown statement error";
		}
	}
};
#endif
