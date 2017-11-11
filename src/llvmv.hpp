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
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

class llvmVisitor : public Visitor {
	llvm::LLVMContext TheContext;
	llvm::Module *TheModule;
	llvm::BasicBlock *mainBlock;
	stack<llvm::BasicBlock*> blockStack;

	llvm::Function *mainFunction;
	llvm::Function *printFunction;
	llvm::Function *scanFunction;

	ASTProgram *root;
	map<ASTIdentifier, llvm::Value*> variableTable;
	map<ASTIdentifier, llvm::Value*> labelTable;
public:
	llvmVisitor(ASTProgram *ast) {
		TheModule = new llvm::Module("mainModule", TheContext);
		TheModule->setTargetTriple("x86_64-pc-linux-gnu");
		mainFunction = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), false), llvm::GlobalValue::ExternalLinkage, "main", TheModule);
		mainBlock = llvm::BasicBlock::Create(TheContext, "mainFunction", mainFunction);
		printFunction = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), true), llvm::GlobalValue::ExternalLinkage, string("printf"), TheModule);
		scanFunction = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt64Ty(TheContext), true), llvm::GlobalValue::ExternalLinkage, string("scanf"), TheModule);
		root = ast;
	}

	void *visit(ASTProgram *ast) {
		blockStack.push(mainBlock);
		ast->decl_block->accept(this);
		ast->code_block->accept(this);
		llvm::ReturnInst::Create(TheContext, blockStack.top());
		blockStack.pop();
		TheModule->print(llvm::errs(), nullptr);
		return NULL;
	}

	void *visit(ASTCodeBlock *ast) {
		for (auto i = ast->statements->begin(); i != ast->statements->end(); i++)
			(*i)->accept(this);
		return NULL;
	}

	void *visit(ASTLabel *ast) {
		// llvm::GlobalVariable *variable = new llvm::GlobalVariable(*TheModule, llvm::Type::getInt64Ty(TheContext), false, llvm::GlobalValue::CommonLinkage, NULL, ast->id);
		// variable->setInitializer(llvm::ConstantInt::get(TheContext, llvm::APInt(64, llvm::StringRef("0"), 10)));
		// variableTable.insert(make_pair((*i)->id, variable));
		return NULL;
	}

	void *visit(ASTDeclBlock *ast) {
		for (auto i = ast->declarations->begin(); i != ast->declarations->end(); i++) {
			ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(*i);
			// if (isDeclared(*i)) {
			// 	cerr << "Error: variable " << (*i)->id << " has already been declared" <<endl;
			// 	exit(-1);
			// }
			if (array_id) {
				llvm::GlobalVariable *variable = new llvm::GlobalVariable(*TheModule, llvm::ArrayType::get(llvm::Type::getInt64Ty(TheContext), 10), false, llvm::GlobalValue::CommonLinkage, NULL, array_id->id);
				ASTIntegerLiteral *index = dynamic_cast<ASTIntegerLiteral*>(array_id->index);
				if (!index) {
					cerr << "Error: Array declaration index must be a number: " << array_id->id << endl;
					exit(-1);
				}
				variable->setInitializer(llvm::ConstantAggregateZero::get(llvm::ArrayType::get(llvm::Type::getInt64Ty(TheContext), 10)));
                variableTable.insert(make_pair(array_id->id, variable));
			}
			else {
				llvm::GlobalVariable *variable = new llvm::GlobalVariable(*TheModule, llvm::Type::getInt64Ty(TheContext), false, llvm::GlobalValue::CommonLinkage, NULL, (*i)->id);
                variable->setInitializer(llvm::ConstantInt::get(TheContext, llvm::APInt(64, llvm::StringRef("0"), 10)));
                variableTable.insert(make_pair((*i)->id, variable));
			}
		}
		return NULL;
	}

	bool isDeclared(ASTIdentifier *ast) {
		return variableTable.find(*ast) != variableTable.end();
	}

	void *visit(ASTAssignmentStatement *ast) {
		if (!isDeclared(ast->id)) {
			cerr << "Error: variable " << ast->id->id << " not declared" <<endl;
			exit(-1);
		}
		llvm::Value *return_val = static_cast<llvm::Value*>(ast->rhs->accept(this));
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast->id);
		if (array_id) {
			std::vector <llvm::Value*> index;
			index.push_back(llvm::ConstantInt::get(TheContext, llvm::APInt(64, llvm::StringRef("0"), 10)));
			index.push_back(static_cast<llvm::Value *>(array_id->index->accept(this)));
			llvm::Value *location = llvm::GetElementPtrInst::CreateInBounds(variableTable[*array_id], index, "tmp", blockStack.top());
            return new llvm::StoreInst(return_val, location, false, blockStack.top());
		}
		else {
			return new llvm::StoreInst(return_val, variableTable[*(ast->id)], false, blockStack.top());
		}
		return NULL;
	}

	llvm::Value *convertToValue(string text) {
		llvm::GlobalVariable *variable = new llvm::GlobalVariable(*TheModule, llvm::ArrayType::get(llvm::IntegerType::get(TheContext, 8), text.size() + 1), true, llvm::GlobalValue::InternalLinkage, NULL, "string");
		variable->setInitializer(llvm::ConstantDataArray::getString(TheContext, text, true));
		return variable;
	}

	void *visit(ASTReadStatement *ast) {
		// vector<llvm::Value*> scan_list;
		// scan_list.push_back(convertToValue("%d"));
		// scan_list.push_back(convertToValue(""));
		// for (auto i = ast->ids->begin(); i!= ast->ids->end(); i++) {
		// 	scan_list[1] = static_cast<llvm::Value*>((*i)->accept(this));
		// 	llvm::CallInst::Create(scanFunction, llvm::makeArrayRef(scan_list), string("scanf"), blockStack.top());
		// }
		return NULL;
	}

	void *visit(ASTPrintStatement *printStatement) {
		vector<llvm::Value*> print_list;
		print_list.push_back(convertToValue(" "));
		string format_string = "";
		for (auto i = printStatement->printable->begin(); i != printStatement->printable->end(); i++) {
			if ((*i)->id == NULL) {
				format_string.append((*i)->text);
			}
			else {
				format_string.append("%d");

				print_list.push_back(static_cast<llvm::Value*>((*i)->id->accept(this)));
			}
			format_string.append(" ");
		}
		format_string.append(printStatement->delim);
		print_list[0] = convertToValue(format_string);
	    llvm::CallInst::Create(printFunction, llvm::makeArrayRef(print_list), string("printf"), blockStack.top());
		return NULL;
	}

	void *visit(ASTWhileStatement *ast) {
		llvm::BasicBlock *entry_block = blockStack.top();
		llvm::BasicBlock *header_block = llvm::BasicBlock::Create(TheContext, "loop_header", entry_block->getParent(), 0);
		llvm::BasicBlock *body_block = llvm::BasicBlock::Create(TheContext, "loop_body", entry_block->getParent(), 0);
		llvm::BasicBlock *after_loop_block = llvm::BasicBlock::Create(TheContext, "after_loop", entry_block->getParent(), 0);

		blockStack.push(header_block);
		llvm::Value *condition = static_cast<llvm::Value*>(ast->cond->accept(this));
		llvm::ICmpInst *comparison = new llvm::ICmpInst(*header_block, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0, true), "tmp");
		blockStack.pop();

		llvm::BranchInst::Create(body_block, after_loop_block, comparison, header_block);
		llvm::BranchInst::Create(header_block, entry_block);

		blockStack.push(body_block);
		ast->code_block->accept(this);
		body_block = blockStack.top();
		blockStack.pop();
		if (!body_block->getTerminator()) {
			llvm::BranchInst::Create(header_block, body_block);
		}

		blockStack.push(after_loop_block);
		return NULL;
	}

	void *visit(ASTIfStatement *ast) {
		llvm::BasicBlock *entry_block = blockStack.top();
		llvm::Value *condition = static_cast<llvm::Value *>(ast->cond->accept(this));
		llvm::ICmpInst *comparison = new llvm::ICmpInst(*entry_block, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0, true), "tmp");
		llvm::BasicBlock *if_block = llvm::BasicBlock::Create(TheContext, "if_block", entry_block->getParent());
		llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(TheContext, "merge_block", entry_block->getParent());

		llvm::BasicBlock *returned_block = NULL;

		blockStack.push(if_block);
		ast->then_block->accept(this);
		returned_block = blockStack.top();
		blockStack.pop();
		if (!returned_block->getTerminator()) {
			llvm::BranchInst::Create(merge_block, returned_block);
		}
		if (ast->else_block) {
			llvm::BasicBlock *else_block = llvm::BasicBlock::Create(TheContext, "else_block", entry_block->getParent());
			blockStack.push(else_block);
			ast->else_block->accept(this);
			returned_block = blockStack.top();
			blockStack.pop();
			if (!returned_block->getTerminator()) {
				llvm::BranchInst::Create(merge_block, returned_block);
			}
			llvm::BranchInst::Create(if_block, else_block, comparison, entry_block);
		} else {
		llvm::BranchInst::Create(if_block, merge_block, comparison, entry_block);
		}
		blockStack.push(merge_block);
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
		return llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), ast->value);
	}

	void *visit(ASTIdentifier *ast) {
		if (!isDeclared(ast)) {
			cerr << "Error: variable " << ast->id << " not declared" <<endl;
			exit(-1);
		}
		ASTArrayIdentifier *array_id = dynamic_cast<ASTArrayIdentifier *>(ast);
		if (array_id) {
            std::vector <llvm::Value*> index;
            index.push_back(llvm::ConstantInt::get(TheContext, llvm::APInt(64, llvm::StringRef("0"), 10)));
            index.push_back(static_cast<llvm::Value *>(array_id->index->accept(this)));
            llvm::Value *val = variableTable[*array_id];
            llvm::Value *offset = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", blockStack.top());
            if (val) {
                return new llvm::LoadInst(offset, "tmp", blockStack.top());
            }
			else
				return NULL;
		}
		else {
            llvm::Value *value = variableTable[*ast];
			return new llvm::LoadInst(value, "tmp", blockStack.top());
		}
	}

	void *visit(ASTForStatement *ast) {
		llvm::BasicBlock *entry_block = blockStack.top();
		llvm::BasicBlock *header_block = llvm::BasicBlock::Create(TheContext, "loop_header", entry_block->getParent(), 0);
		llvm::BasicBlock *body_block = llvm::BasicBlock::Create(TheContext, "loop_body", entry_block->getParent(), 0);
		llvm::BasicBlock *after_loop_block = llvm::BasicBlock::Create(TheContext, "after_loop", entry_block->getParent(), 0);

		ast->init->accept(this);
		llvm::Value *val = new llvm::LoadInst(variableTable[*(ast->init->id)], "load", header_block);
		llvm::ICmpInst *comparison = new llvm::ICmpInst(*header_block, llvm::ICmpInst::ICMP_NE, val, static_cast<llvm::Value *>(ast->limit->accept(this)), "tmp");
		llvm::BranchInst::Create(body_block, after_loop_block, comparison, header_block);
		llvm::BranchInst::Create(header_block, entry_block);

		blockStack.push(body_block);
		ast->code_block->accept(this);
		(new ASTAssignmentStatement(ast->init->id, new ASTBinaryExpression(ast->init->id, ast->step, BinOp::PLUS)))->accept(this);
		body_block = blockStack.top();
		blockStack.pop();
		if (!body_block->getTerminator()) {
			llvm::BranchInst::Create(header_block, body_block);
		}

		blockStack.push(after_loop_block);
		return NULL;
	}
};

#endif
