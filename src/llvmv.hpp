#ifndef _LLVMV_H
#define _LLVMV_H

#include <iostream>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/Value.h>

using namespace std;
using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;

class CodeGenVisitor {
public:
	virtual Value* visit(ASTProgram* ast) = 0;
	virtual Value* visit(ASTDeclBlock* ast) = 0;
	virtual Value* visit(ASTCodeBlock* ast) = 0;
	virtual Value* visit(ASTIntegerLiteral* ast) = 0;
	virtual Value* visit(ASTIdentifier* ast) = 0;
	virtual Value* visit(ASTBinaryExpression* ast) = 0;
	virtual Value* visit(ASTBooleanExpression* ast) = 0;
	virtual Value* visit(ASTAssignmentStatement* ast) = 0;
	virtual Value* visit(ASTPrintStatement* ast) = 0;
	virtual Value* visit(ASTReadStatement* ast) = 0;
	virtual Value* visit(ASTWhileStatement* ast) = 0;
	virtual Value* visit(ASTIfStatement* ast) = 0;
	virtual Value* visit(ASTForStatement* ast) = 0;
};

class llvmVisitor : public Visitor {
public:
	void visit(ASTProgram *ast) {
		// module = new llvm::Module("main", llvm::getGlobalContext());
		// module->setTargetTriple("x86_64-pc-linux-gnu");
	}
	void visit(ASTDeclBlock *ast) {

	}
	void visit(ASTAssignmentStatement *ast) {
	}
	void visit(ASTReadStatement *ast) {

	}
	void visit(ASTPrintStatement *ast) {

	}
	void visit(ASTWhileStatement *ast) {

	}
	void visit(ASTIfStatement *ast) {

	}
	void visit(ASTBinaryExpression *ast) {

	}
	void visit(ASTBooleanExpression *ast) {

	}
	void visit(ASTIntegerLiteral *ast) {
	}
	void visit(ASTIdentifier *ast) {

	}
	void visit(ASTCodeBlock *ast) {

	}
	void visit(ASTForStatement *ast) {

	}
};

#endif
