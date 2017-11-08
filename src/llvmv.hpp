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
static unique_ptr<Module> TheModule;
static map<string, Value *> NamedValues;

class CodeGenVisitor {
public:
	virtual void visit(ASTProgram* ast) = 0;
	virtual void visit(ASTDeclBlock* ast) = 0;
	virtual void visit(ASTCodeBlock* ast) = 0;
	virtual Value* visit(ASTIntegerLiteral* ast) = 0;
	virtual void visit(ASTIdentifier* ast) = 0;
	virtual void visit(ASTBinaryExpression* ast) = 0;
	virtual void visit(ASTBooleanExpression* ast) = 0;
	virtual void visit(ASTAssignmentStatement* ast) = 0;
	virtual void visit(ASTPrintStatement* ast) = 0;
	virtual void visit(ASTReadStatement* ast) = 0;
	virtual void visit(ASTWhileStatement* ast) = 0;
	virtual void visit(ASTIfStatement* ast) = 0;
	virtual void visit(ASTForStatement* ast) = 0;
};

class llvmVisitor : public CodeGenVisitor {
public:
	llvmVisitor(ASTProgram *ast) {
		TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
		TheModule->setTargetTriple("x86_64-pc-linux-gnu");
		visit(ast);
		TheModule->print(errs(), nullptr);
	}
	void visit(ASTCodeBlock *ast) {

	}
	void visit(ASTProgram *ast) {
		visit(ast->decl_block);
		visit(ast->code_block);
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
	Value* visit(ASTIntegerLiteral *ast) {
		return ConstantInt::get(Type::getInt32Ty(TheContext), ast->value);
	}
	void visit(ASTIdentifier *ast) {

	}
	void visit(ASTForStatement *ast) {

	}
};

#endif
