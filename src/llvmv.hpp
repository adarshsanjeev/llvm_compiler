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

class llvmVisitor : public Visitor {
	BasicBlock *mainBlock;
	stack<BasicBlock*> blockStack;

	Function *mainFunction;
	Function *printFunction;
	Function *scanFunction;
	map<ASTIdentifier, llvm::Value*> variable_table;
public:
	llvmVisitor(ASTProgram *ast) {
		TheModule = new llvm::Module("mainModule", TheContext);
		TheModule->setTargetTriple("x86_64-pc-linux-gnu");
		llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), false);
		mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", TheModule);
		mainBlock = llvm::BasicBlock::Create(TheContext, Twine("mainFunction"), mainFunction);
		printFunction = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), true), llvm::GlobalValue::ExternalLinkage, string("printf"), TheModule);
		blockStack.push(mainBlock);
		ast->accept(this);
		llvm::ReturnInst::Create(TheContext, blockStack.top());
		blockStack.pop();
		TheModule->print(errs(), nullptr);
	}
	void *visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++) {
			ASTAssignmentStatement *assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(*i);
			ASTPrintStatement *printStatement = dynamic_cast<ASTPrintStatement *>(*i);
			ASTIfStatement *ifStatement = dynamic_cast<ASTIfStatement*>(*i);
			ASTForStatement *forStatement = dynamic_cast<ASTForStatement *>(*i);
			ASTWhileStatement *whileStatement = dynamic_cast<ASTWhileStatement *>(*i);
			if (assignmentStatement) {
				assignmentStatement->accept(this);
			}
			else if (ifStatement) {
				ifStatement->accept(this);
			}
			else if (printStatement) {
				printStatement->accept(this);
			}
			else if (forStatement) {
				forStatement->accept(this);
			}
			else if (whileStatement) {
				whileStatement->accept(this);
			}
		}
	}

	void *visit(ASTProgram *ast) {
		ast->decl_block->accept(this);
		ast->code_block->accept(this);
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
		Value* return_val = (Value*)ast->rhs->accept(this);
		if (return_val != NULL)
			return new llvm::StoreInst(return_val, variable_table[*(ast->id)], false, blockStack.top());
		else
			cout << "NULL GOT" << endl;
	}

	void *visit(ASTExpression *ast) {
		ASTIntegerLiteral *number = dynamic_cast<ASTIntegerLiteral *>(ast);
		ASTBinaryExpression *binary = dynamic_cast<ASTBinaryExpression *>(ast);
		ASTBooleanExpression *boolean = dynamic_cast<ASTBooleanExpression *>(ast);
		ASTIdentifier *id = dynamic_cast<ASTIdentifier *>(ast);
		if (number) {
			return (Value*)number->accept(this);
		}
		if (binary) {
			return (Value*)binary->accept(this);
		}
		if (boolean) {
			return (Value*)boolean->accept(this);
		}
		if (id) {
			return (Value*)id->accept(this);
		}
	}

	void *visit(ASTReadStatement *ast) {
		return NULL;

	}

	void print_function_call (vector<Value*> arguments) {
	    llvm::CallInst::Create(printFunction, llvm::makeArrayRef(arguments), string("printf"), blockStack.top());
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

				print_list.push_back((Value*)(*i)->id->accept(this));
			}
			format_string.append(" ");
		}
		format_string.append(printStatement->delim);
		print_list[0] = convert_to_value(format_string);
		print_function_call(print_list);
		return NULL;
	}

	void *visit(ASTWhileStatement *ast) {
		llvm::BasicBlock * entryBlock = blockStack.top();
		llvm::BasicBlock * headerBlock = llvm::BasicBlock::Create(TheContext, "loop_header", entryBlock->getParent(), 0);
		llvm::BasicBlock * bodyBlock = llvm::BasicBlock::Create(TheContext, "loop_body", entryBlock->getParent(), 0);
		llvm::BasicBlock * afterLoopBlock = llvm::BasicBlock::Create(TheContext, "after_loop", entryBlock->getParent(), 0);

		// symbolTable.pushBCS(afterLoopBlock, headerBlock);

		blockStack.push(headerBlock);
		llvm::Value *condition = (Value*)ast->cond->accept(this);
		llvm::ICmpInst * comparison = new llvm::ICmpInst(*headerBlock, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0, true), "tmp");
		blockStack.pop();

		llvm::BranchInst::Create(bodyBlock, afterLoopBlock, comparison, headerBlock);
		llvm::BranchInst::Create(headerBlock, entryBlock);

		blockStack.push(bodyBlock);
		ast->code_block->accept(this);
		bodyBlock = blockStack.top();
		blockStack.pop();
		if (!bodyBlock->getTerminator()) {
			llvm::BranchInst::Create(headerBlock, bodyBlock);
		}

		// // symbolTable.popBCS();

		blockStack.push(afterLoopBlock);

		return NULL;
	}

	void *visit(ASTIfStatement *ast) {
		llvm::BasicBlock *entryBlock = blockStack.top();
		llvm::Value *condition = static_cast<llvm::Value *>(ast->cond->accept(this));
		llvm::ICmpInst * comparison = new llvm::ICmpInst(*entryBlock, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0, true), "tmp");
		llvm::BasicBlock * ifBlock = llvm::BasicBlock::Create(TheContext, "ifBlock", entryBlock->getParent());
		llvm::BasicBlock * mergeBlock = llvm::BasicBlock::Create(TheContext, "mergeBlock", entryBlock->getParent());

		llvm::BasicBlock * returnedBlock = NULL;

		blockStack.push(ifBlock);
		ast->then_block->accept(this);
		returnedBlock = blockStack.top();
		blockStack.pop();
		if (!returnedBlock->getTerminator()) {
			llvm::BranchInst::Create(mergeBlock, returnedBlock);
		}
		if (ast->else_block) {
			llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(TheContext, "elseBlock", entryBlock->getParent());
			blockStack.push(elseBlock);
			ast->else_block->accept(this);
			returnedBlock = blockStack.top();
			blockStack.pop();
			if (!returnedBlock->getTerminator()) {
				llvm::BranchInst::Create(mergeBlock, returnedBlock);
			}
			llvm::BranchInst::Create(ifBlock, elseBlock, comparison, entryBlock);
		} else {
		llvm::BranchInst::Create(ifBlock, mergeBlock, comparison, entryBlock);
		}
		blockStack.push(mergeBlock);
		return NULL;
	}

	void *visit(ASTBinaryExpression *ast) {
		switch(ast->op) {
		case BinOp::PLUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		case BinOp::MINUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::Sub, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		case BinOp::PRODUCT:
			return llvm::BinaryOperator::Create(llvm::Instruction::Mul, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		case BinOp::DIVIDE:
			return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		case BinOp::MODULUS:
			return llvm::BinaryOperator::Create(llvm::Instruction::SRem, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		default:
			cerr << "Unidentifier operation" << endl;
			return NULL;
		}
	}

	void *visit(ASTBooleanExpression *ast) {
		switch(ast->op) {
		case BoolOp::LESSTHAN:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::GREATERTHAN:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::LESSEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::GREATEREQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGE, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::NOTEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::EQUALEQUAL:
			return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)),"tmp", blockStack.top()), llvm::Type::getInt64Ty(TheContext), "zext", blockStack.top());
			break;
		case BoolOp::AND_OP:
			return llvm::BinaryOperator::Create(llvm::Instruction::And, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
			break;
		case BoolOp::OR_OP:
			return llvm::BinaryOperator::Create(llvm::Instruction::Or, static_cast<llvm::Value*>(ast->left_child->accept(this)), static_cast<llvm::Value*>(ast->right_child->accept(this)), "tmp", blockStack.top());
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
			return new llvm::LoadInst(value, "tmp", blockStack.top());
		}
	}

	void *visit(ASTForStatement *ast) {
		llvm::BasicBlock * entryBlock = blockStack.top();
		llvm::BasicBlock * headerBlock = llvm::BasicBlock::Create(TheContext, "loop_header", entryBlock->getParent(), 0);
		llvm::BasicBlock * bodyBlock = llvm::BasicBlock::Create(TheContext, "loop_body", entryBlock->getParent(), 0);
		llvm::BasicBlock * afterLoopBlock = llvm::BasicBlock::Create(TheContext, "after_loop", entryBlock->getParent(), 0);

		// symbolTable.pushBCS(afterLoopBlock, headerBlock);

		ast->init->accept(this);
		llvm::Value *val = new llvm::LoadInst(variable_table[*(ast->init->id)], "load", headerBlock);
		llvm::ICmpInst *comparison = new llvm::ICmpInst(*headerBlock, llvm::ICmpInst::ICMP_SLT, val, static_cast<llvm::Value *>(ast->limit->accept(this)), "tmp");
		llvm::BranchInst::Create(bodyBlock, afterLoopBlock, comparison, headerBlock);
		llvm::BranchInst::Create(headerBlock, entryBlock);

		blockStack.push(bodyBlock);
		ast->code_block->accept(this);
		(new ASTAssignmentStatement(ast->init->id, new ASTBinaryExpression(ast->init->id, ast->step, BinOp::PLUS)))->accept(this);
		bodyBlock = blockStack.top();
		blockStack.pop();
		if (!bodyBlock->getTerminator()) {
			llvm::BranchInst::Create(headerBlock, bodyBlock);
		}

		// symbolTable.popBCS();

		blockStack.push(afterLoopBlock);

		return NULL;
	}
};

#endif
