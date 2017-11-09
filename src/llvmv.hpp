#ifndef _LLVMV_H
#define _LLVMV_H

#include <iostream>
#include "ast.hpp"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

using namespace std;
using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static unique_ptr<Module> TheModule;
static map<string, Value *> NamedValues;
static BasicBlock *mainBlock;
static Function *mainFunction;

class CodeGenVisitor {
public:
	virtual void* visit(ASTProgram* ast) = 0;
	virtual void* visit(ASTDeclBlock* ast) = 0;
	virtual void* visit(ASTCodeBlock* ast) = 0;
	virtual void* visit(ASTIntegerLiteral* ast) = 0;
	virtual void* visit(ASTIdentifier* ast) = 0;
	virtual void* visit(ASTBinaryExpression* ast) = 0;
	virtual void* visit(ASTBooleanExpression* ast) = 0;
	virtual void* visit(ASTAssignmentStatement* ast) = 0;
	virtual void* visit(ASTPrintStatement* ast) = 0;
	virtual void* visit(ASTReadStatement* ast) = 0;
	virtual void* visit(ASTWhileStatement* ast) = 0;
	virtual void* visit(ASTIfStatement* ast) = 0;
	virtual void* visit(ASTForStatement* ast) = 0;
};

class llvmVisitor : public CodeGenVisitor {
	map<ASTIdentifier, llvm::Value*> variable_table;
public:
	llvmVisitor(ASTProgram *ast) {
		TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
		TheModule->setTargetTriple("x86_64-pc-linux-gnu");
		llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), false);
		mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", TheModule);
		mainBlock = llvm::BasicBlock::Create(TheContext, Twine("mainFunction"), mainFunction);

		visit(ast);
		TheModule->print(errs(), nullptr);
	}
	void *visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(*i);
			if (assignmentStatement) {
				visit(assignmentStatement);
			}
		}
	}

	void *visit(ASTProgram *ast) {
		visit(ast->decl_block);
		visit(ast->code_block);
		return NULL;
	}

	void *visit(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(*i);
			if (array_id) {
			}
			else {
				llvm::GlobalVariable *globalInteger = new llvm::GlobalVariable(*TheModule, llvm::Type::getInt64Ty(TheContext), false, llvm::GlobalValue::CommonLinkage, NULL, (*i)->id);
                globalInteger->setInitializer(llvm::ConstantInt::get(TheContext, llvm::APInt(64, llvm::StringRef("0"), 10)));
                variable_table.insert(make_pair((*i)->id, globalInteger));
			}
		}
		return NULL;
	}

	void *visit(ASTAssignmentStatement *ast) {
		ASTIntegerLiteral *number = dynamic_cast<ASTIntegerLiteral *>(ast->rhs);
		if (number) {
			Value* return_val = (Value*)visit(number);
			cout << "VALUE" << return_val << endl;
			variable_table[(*ast->id)] = return_val;
			return new llvm::StoreInst(return_val, variable_table[*(ast->id)], false, mainBlock);
		}
		return NULL;
	}
	void *visit(ASTReadStatement *ast) {
		return NULL;

	}
	void *visit(ASTPrintStatement *ast) {
		return NULL;

	}
	void *visit(ASTWhileStatement *ast) {
		return NULL;

	}
	void *visit(ASTIfStatement *ast) {
		return NULL;

	}
	void *visit(ASTBinaryExpression *ast) {
// 		switch(ast->op) {
// 		case BinOp::PLUS:
// //			return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
// 			break;
// 		}
	}
	void *visit(ASTBooleanExpression *ast) {
		return NULL;

	}
	void *visit(ASTIntegerLiteral *ast) {
		return ConstantInt::get(Type::getInt32Ty(TheContext), ast->value);
	}
	void *visit(ASTIdentifier *ast) {
		// ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(id);
		// if (array_id) {

		// }
		// else {
		// 	llvm::AllocaInst * allocaInst = new llvm::AllocaInst(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getId(), symbolTable.topBlock());
		// 	new llvm::StoreInst(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), allocaInst, false, symbolTable.topBlock());
		// 	symbolTable.declareLocalVariables(node->getId(), allocaInst);
		// 	return allocaInst;
		// }

	}
	void *visit(ASTForStatement *ast) {

		return NULL;
	}
};

#endif
