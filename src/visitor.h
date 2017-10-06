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
		for (std::vector<ASTStatement*>::iterator i = ast->statements->begin(); i != ast->statements->end(); i++) {
			visit(*i);
		}
	}

	void visit (ASTIdentifier* ast) {
		ASTSingleIdentifier *single_id = dynamic_cast<ASTSingleIdentifier *>(ast);
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (single_id) {
			std::cout << single_id->id;
		}
		if (array_id) {
			std::cout << array_id->id;
			std::cout << "[" << array_id->index << "]";
		}
	}

	void visit (ASTExpression* ast) {
		ASTIntegerLiteral *int_literal = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *bin_exp = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);
		if (int_literal) {
			std::cout << int_literal->value;
		}
		else if (bin_exp) {
			visit(bin_exp->left_child);
			std::cout << "[" << bin_exp->op << "]";
			visit(bin_exp->right_child);
		}
		else if (id) {
			visit(id);
		}
		else {
			std::cerr<< "Unknown Type of expression!";
		}
	}

	void visit (ASTStatement* ast) {
		ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(ast);
		if (assignmentStatement) {
			std::cout << "Assignment Statement:";
			visit(assignmentStatement->id);
			std::cout<<" = ";
			visit(assignmentStatement->rhs);
			std::cout<<std::endl;
		}
	}
};
#endif
