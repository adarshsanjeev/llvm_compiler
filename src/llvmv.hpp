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
static Module* TheModule;
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
	virtual void* visit(ASTExpression* ast) = 0;
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
		TheModule = new llvm::Module("mainModule", TheContext); //llvm::make_unique<Module>("my cool jit", TheContext);
		TheModule->setTargetTriple("x86_64-pc-linux-gnu");
		llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), false);
		mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", TheModule);
		mainBlock = llvm::BasicBlock::Create(TheContext, Twine("mainFunction"), mainFunction);
		visit(ast);
		llvm::ReturnInst::Create(TheContext, mainBlock);
		TheModule->print(errs(), nullptr);
	}
	void *visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(*i);
			ASTPrintStatement *printStatement = dynamic_cast<ASTPrintStatement *>(*i);
			if (assignmentStatement) {
				visit(assignmentStatement);
			}
			if (printStatement) {
				visit(printStatement);
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
		Value* return_val = (Value*)visit(ast->rhs);
		if (return_val != NULL)
			return new llvm::StoreInst(return_val, variable_table[*(ast->id)], false, mainBlock);
		else
			cout << "NULL GOT" << endl;
	}

	void *visit(ASTExpression *ast) {
		ASTIntegerLiteral *number = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *binary = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTBooleanExpression *boolean = dynamic_cast<ASTBooleanExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);
		if (number) {
			return (Value*)visit(number);
		}
		if (binary) {
			return (Value*)visit(binary);
		}
		if (boolean) {
			return (Value*)visit(boolean);
		}
		if (id) {
			return (Value*)visit(id);
		}
	}

	void *visit(ASTReadStatement *ast) {
		return NULL;

	}

	void print_function_call (vector<Value*> arguments) {
		string methodName = "printf";
		llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), true);
		Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, methodName, TheModule);
		llvm::CallInst *call = llvm::CallInst::Create(function, llvm::makeArrayRef(arguments), methodName, mainBlock);
	}

	Value *convert_to_value(string text) {
		llvm::GlobalVariable* variable = new llvm::GlobalVariable(*TheModule, llvm::ArrayType::get(llvm::IntegerType::get(TheContext, 8), text.size() + 1), true, llvm::GlobalValue::InternalLinkage, NULL, "string");
		variable->setInitializer(llvm::ConstantDataArray::getString(TheContext, text, true));
		return variable;
	}

	void *visit(ASTPrintStatement *printStatement) {
		vector<Value*> print_list;
		print_list.push_back(convert_to_value(" "));
		string format_string = "";
		for (auto i = printStatement->printable->begin(); i != printStatement->printable->end(); i++) {
			if ((*i)->id == NULL) {
				format_string.append((*i)->text);
			}
			else {
				format_string.append("%d");

				print_list.push_back((Value*)visit((*i)->id));
			}
			format_string.append(" ");
		}
		format_string.append(printStatement->delim);
		print_list[0] = convert_to_value(format_string);
		print_function_call(print_list);
		return NULL;
	}

	void *visit(ASTWhileStatement *ast) {
		return NULL;

	}
	void *visit(ASTIfStatement *ast) {
		return NULL;

	}

	void *visit(ASTBinaryExpression *ast) {
		switch(ast->op) {
		case BinOp::PLUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		case BinOp::MINUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::Sub, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		case BinOp::PRODUCT:
			return llvm::BinaryOperator::Create(llvm::Instruction::Mul, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		case BinOp::DIVIDE:
			return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		case BinOp::MODULUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::SRem, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		default:
			cerr << "Unidentifier operation" << endl;
			return NULL;
		}
	}

	void *visit(ASTBooleanExpression *ast) {
		switch(ast->op) {
		case BoolOp::LESSTHAN:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::GREATERTHAN:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::LESSEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::GREATEREQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGE, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::NOTEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::EQUALEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)),"tmp", mainBlock), llvm::Type::getInt64Ty(TheContext), "zext", mainBlock);
			break;
		case BoolOp::AND_OP:
			return llvm::BinaryOperator::Create(llvm::Instruction::And, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		case BoolOp::OR_OP:
			return llvm::BinaryOperator::Create(llvm::Instruction::Or, static_cast<llvm::Value*>(this->visit(ast->left_child)), static_cast<llvm::Value*>(this->visit(ast->right_child)), "tmp", mainBlock);
			break;
		default:
			cerr << "Unidentifier operation" << endl;
			return NULL;
		}
	}
	void *visit(ASTIntegerLiteral *ast) {
		return ConstantInt::get(Type::getInt64Ty(TheContext), ast->value);
	}
	void *visit(ASTIdentifier *ast) {
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (array_id) {

		}
		else {
            llvm::Value *value = variable_table[*ast];
			return new llvm::LoadInst(value, "tmp", mainBlock);
		}
	}
	void *visit(ASTForStatement *ast) {

		return NULL;
	}
};

#endif
