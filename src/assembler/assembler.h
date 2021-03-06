#pragma once
#include <string>
#include <sstream>

#include "assembler/format.h"
#include "assembler/instructions.h"
#include "assembler/optimizations.h"
#include "assembler/final.h"
#include "ast.h"

#define ASM_ERR "err"

namespace Bonfire {
	namespace Assembler {

		struct Variable {
			std::string name;
			uint32_t stack_offset;
			uint8_t size;
			Type type;

			Variable(std::string name, uint32_t stack_offset, Type t) {
				this->name = name;
				this->stack_offset = stack_offset;
				this->type = t;
			}
		};

		std::vector<Variable> glob_vars;
		uint32_t num_labels;
		uint32_t name_counter; // To generate unique names

		uint32_t get_stack_offset_by_var_name(std::string name) {
			for (Variable v : glob_vars) {
				if (!v.name.compare(name)) {
					return v.stack_offset;
				}
			}
			return 0;
		}

		Type get_type_by_var_name(std::string name) {
			for (Variable v : glob_vars) {
				if (!v.name.compare(name)) {
					return v.type;
				}
			}
			return Type::VOID;
		}

		uint8_t get_size_by_var_name(std::string name) {
			return get_type_size(get_type_by_var_name(name));
		}

		std::string get_asm_size(Type t) {
			switch (t) {
			case Type::UINT8: case Type::INT8: return ASM_SIZE_8;
			case Type::UINT16: case Type::INT16: return ASM_SIZE_16;
			case Type::UINT32: case Type::INT32: return ASM_SIZE_32;
			case Type::UINT64: case Type::INT64: return ASM_SIZE_64;
			}
			return "ERR";
		}

		std::string get_asm_size(uint8_t size) {
			switch (size) {
			case 1: return ASM_SIZE_8;
			case 2: return ASM_SIZE_16;
			case 4: return ASM_SIZE_32;
			case 8: return ASM_SIZE_64;
			}
			return "ERR";
		}

		void assemble_expression(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset, bool can_return, const char* code_block_label);
		void assemble_expression(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset) {
			assemble_expression(instructions, expression, stack_offset, false, NULL);
		}
		void assemble_expression_stres(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset);

		// Assemble a simple comparison
		void assemble_compare_by_op(std::vector<AssemblyInstruction*>& instructions, OperationST* op_st, uint32_t& stack_offset) {
			if (op_st->lhs->type == AstType::VAR_VALUE && op_st->rhs->type == AstType::VAR_VALUE) {
				// Compare two variables
				VariableValST* lhs = static_cast<VariableValST*>(op_st->lhs);
				VariableValST* rhs = static_cast<VariableValST*>(op_st->rhs);

				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(lhs->identifier);
				std::string lhs_asm_size = get_asm_size(lhs->return_type);
				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(rhs->identifier);
				std::string rhs_asm_size = get_asm_size(lhs->return_type);

				// Compare lhs and rhs (Stores lhs in ebx)
				//stream << string_format(ASM_FORMAT_CMP_MEM_MEM, lhs_asm_size.c_str(), lhs_stack_offset, rhs_asm_size.c_str(), rhs_stack_offset);
				instructions.push_back(new Asm4<std::string, uint32_t, std::string, uint32_t>(AsmType::COMP_MEM_MEM, lhs_asm_size, lhs_stack_offset, rhs_asm_size, rhs_stack_offset));
			}
			else if (op_st->lhs->type == AstType::VAR_VALUE && op_st->rhs->type == AstType::CONSTANT) {
				// Compare a variable and a constant
				VariableValST* lhs = static_cast<VariableValST*>(op_st->lhs);
				ConstantST* rhs = static_cast<ConstantST*>(op_st->rhs);

				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(lhs->identifier);
				std::string lhs_asm_size = get_asm_size(lhs->return_type);

				//stream << string_format(ASM_FORMAT_CMP_MEM_CONST, lhs_asm_size.c_str(), lhs_stack_offset, rhs->constant.c_str());
				instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::COMP_MEM_CONST, lhs_asm_size, lhs_stack_offset, rhs->constant));
			}
			else if (op_st->lhs->type == AstType::CONSTANT && op_st->rhs->type == AstType::VAR_VALUE) {
				// Compare a variable and a constant
				ConstantST* lhs = static_cast<ConstantST*>(op_st->lhs);
				VariableValST* rhs = static_cast<VariableValST*>(op_st->rhs);
				std::string rhs_asm_size = get_asm_size(rhs->return_type);

				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(rhs->identifier);

				//stream << string_format(ASM_FORMAT_CMP_CONST_MEM, lhs->constant.c_str(), rhs_asm_size.c_str(), rhs_stack_offset);
				instructions.push_back(new Asm3<std::string, std::string, uint32_t>(AsmType::COMP_CONST_MEM, lhs->constant, rhs_asm_size, rhs_stack_offset));
			}
			else {
				assemble_expression_stres(instructions, op_st->lhs, stack_offset);
				// Return value of expression is stored in eax, move it to ebx
				//stream << string_format(ASM_FORMAT_MOVE_REG_REG, "ebx", "eax");
				instructions.push_back(new Asm2<std::string, std::string>(AsmType::MOVE_REG_REG, "ebx", "eax"));
				assemble_expression_stres(instructions, op_st->rhs, stack_offset);
				// Compare the result of the lhs (now in ebx) and the result of rhs (in eax)
				//stream << string_format(ASM_FORMAT_CMP_REG_REG, "ebx", "eax");
				instructions.push_back(new Asm2<std::string, std::string>(AsmType::COMP_REG_REG, "ebx", "eax"));
			}
		}

		void assemble_loop(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset, bool can_return, const char* code_block_label) {
			LoopST* loop_st = static_cast<LoopST*>(expression);

			//bool is_op = loop_st->condition->type == AstType::OPERATION;
			if (loop_st->condition->type == AstType::VAR_VALUE) {
				std::string beginning_label_name = "__w_begin" + std::to_string(name_counter);
				std::string continue_label_name = "__w_continue" + std::to_string(name_counter);
				++name_counter;

				// Set begin label
				//stream << string_format(ASM_FORMAT_LABEL, beginning_label_name.c_str());
				instructions.push_back(new Asm1<std::string>(AsmType::LABEL, beginning_label_name));
				// TODO: Assemble operations and comparisons correctly

				// TODO: Add things other than variables
				// This is a variable, compare with 0 (0 = false, so if equals, we jump to else)
				VariableValST* var_st = static_cast<VariableValST*>(loop_st->condition);
				uint32_t var_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
				std::string asm_size = get_asm_size(var_st->return_type);
				// Compare with 0 (false)
				//stream << string_format(ASM_FORMAT_CMP_MEM_CONST, asm_size.c_str(), var_stack_offset, "0");
				instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::COMP_MEM_CONST, asm_size, var_stack_offset, "0"));
				// If it is 0 (false) then jump to else
				//stream << string_format(ASM_FORMAT_JMP_EQ, continue_label_name.c_str());
				instructions.push_back(new Asm1<std::string>(AsmType::JUMP_EQ, continue_label_name));

				// Loop body
				assemble_expression(instructions, loop_st->body, stack_offset, can_return, code_block_label);
				// jump to beginning (loop)
				//stream << string_format(ASM_FORMAT_JMP, beginning_label_name.c_str());
				instructions.push_back(new Asm1<std::string>(AsmType::JUMP, beginning_label_name));
				// continue label
				//stream << string_format(ASM_FORMAT_LABEL, continue_label_name.c_str());
				instructions.push_back(new Asm1<std::string>(AsmType::LABEL, continue_label_name));
			}
		}

		void assemble_if(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset, bool can_return, const char* code_block_label) {
			IfST* if_st = static_cast<IfST*>(expression);
			bool is_op = if_st->condition->type == AstType::OPERATION;
			if (is_op || if_st->condition->type == AstType::VAR_VALUE) {
				std::string else_label_name = "__else" + std::to_string(name_counter);
				std::string continue_label_name = "__continue" + std::to_string(name_counter);
				++name_counter;

				if (is_op) {
					OperationST* op_st = static_cast<OperationST*>(if_st->condition);
					// Assemble comparison based on the operation
					assemble_compare_by_op(instructions, op_st, stack_offset);
					AsmType jump_type = AsmType::JUMP;
					switch (op_st->op) {
					case Operation::EQ:
						jump_type = AsmType::JUMP_NEQ;
						break;
					case Operation::NEQ:
						jump_type = AsmType::JUMP_EQ;
						break;
					case Operation::LT:
						jump_type = AsmType::JUMP_GTE;
						break;
					case Operation::LTE:
						jump_type = AsmType::JUMP_GT;
						break;
					case Operation::GT:
						jump_type = AsmType::JUMP_LTE;
						break;
					case Operation::GTE:
						jump_type = AsmType::JUMP_LT;
						break;
					}
					//stream << string_format(jump_type, if_st->has_else ? else_label_name.c_str() : continue_label_name.c_str());
					instructions.push_back(new Asm1<std::string>(jump_type, if_st->has_else ? else_label_name : continue_label_name));
				}
				else {
					// This is a variable, compare with 0 (0 = false, so if equals, we jump to else)
					VariableValST* var_st = static_cast<VariableValST*>(if_st->condition);
					uint32_t var_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
					std::string asm_size = get_asm_size(var_st->return_type);
					// Compare with 0 (false)
					//stream << string_format(ASM_FORMAT_CMP_MEM_CONST, asm_size.c_str(), var_stack_offset, "0");
					instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::COMP_MEM_CONST, asm_size, var_stack_offset, "0"));
					// If it is 0 (false) then jump to else or continue
					//stream << string_format(ASM_FORMAT_JMP_EQ, if_st->has_else ? else_label_name.c_str() : continue_label_name.c_str());
					instructions.push_back(new Asm1<std::string>(AsmType::JUMP_EQ, if_st->has_else ? else_label_name : continue_label_name));
				}

				assemble_expression(instructions, if_st->then_body, stack_offset, can_return, code_block_label);
				if (if_st->has_else) {
					// Jump to continue to skip else
					//stream << string_format(ASM_FORMAT_JMP, continue_label_name.c_str());
					instructions.push_back(new Asm1<std::string>(AsmType::JUMP, continue_label_name));
					// Put else label here
					//stream << string_format(ASM_FORMAT_LABEL, else_label_name.c_str());
					instructions.push_back(new Asm1<std::string>(AsmType::LABEL, else_label_name));
					assemble_expression(instructions, if_st->else_body, stack_offset, can_return, code_block_label);
				}
				// Put continue label here
				//stream << string_format(ASM_FORMAT_LABEL, continue_label_name.c_str());
				instructions.push_back(new Asm1<std::string>(AsmType::LABEL, continue_label_name));
			}
		}

		// Converts a string from any constant expression into a number
		std::string const_as_num(std::string constant) {
			/*if (!constant.compare("true")) {
				return "1";
			}
			else if (!constant.compare("false")) {
				return "0";
			}
			else {
				return constant;
			}*/
			return constant;
		}

		std::string get_block_label(size_t block_id) {
			return "__block" + std::to_string(block_id);
		}

		BlockST* assemble_only_code_block(std::vector<AssemblyInstruction*>& instructions, BlockST* block, uint32_t& stack_offset, bool can_return, const char* code_block_label) {
			for (int i = 0; i < block->num_children; i++) {
				assemble_expression(instructions, block->children[i], stack_offset, can_return, code_block_label);
			}
			return block;
		}

		BlockST* assemble_only_code_block(std::vector<AssemblyInstruction*>& instructions, ExpressionST* block, uint32_t& stack_offset, bool can_return, const char* code_block_label) {
			BlockST* block_st = static_cast<BlockST*>(block);
			return assemble_only_code_block(instructions, block_st, stack_offset, can_return, code_block_label);
		}

		BlockST* assemble_code_block(std::vector<AssemblyInstruction*>& instructions, ExpressionST* block, uint32_t& stack_offset, bool can_parent_return) {

			BlockST* block_st = static_cast<BlockST*>(block);
			// See if we have to set up a stack frame
			bool stack_frame = false;
			std::string end_label = "__block" + std::to_string(num_labels) + "_end";
			++num_labels;

			for (int i = 0; i < block_st->num_children; i++) {
				if (block_st->children[i]->type == AstType::VAR_DECLARATION) {
					stack_frame = true;
					break;
				}
			}

			// Determine if this block can return the function
			// If the parent can return and the current block type is void, when using return on this block, we want to return from the function instead of the block
			bool can_return = can_parent_return && block_st->return_type == Type::VOID;

			std::cout << "Can parent return? " << std::boolalpha << can_parent_return << std::endl;
			std::cout << "Return type " << (int)block_st->return_type << std::endl;

			if (stack_frame) {
				uint32_t start_stack_offset = stack_offset;
				assemble_only_code_block(instructions, block_st, stack_offset, can_return, end_label.c_str());
				stack_offset = start_stack_offset;
			}
			else {
				assemble_only_code_block(instructions, block_st, stack_offset, can_return, end_label.c_str());
			}
			// Set block end label, if the last expression of the code block was not a return
			//stream << string_format(ASM_FORMAT_LABEL, end_label.c_str());
			instructions.push_back(new Asm1<std::string>(AsmType::LABEL, end_label));
			return block_st;
		}

		void assemble_return(std::vector<AssemblyInstruction*>& instructions, ExpressionST* ret, uint32_t& stack_offset, bool can_return, const char* code_block_end_label) {
			ReturnST* ret_st = static_cast<ReturnST*>(ret);
			if (ret_st) {
				switch (ret_st->expression->type) {
				case AstType::CONSTANT:
				{
					ConstantST* constant = static_cast<ConstantST*>(ret_st->expression);
					//stream << string_format(ASM_FORMAT_RETURN_CONST, const_as_num(constant->get_value()).c_str());
					instructions.push_back(new Asm2<std::string, std::string>(AsmType::MOVE_REG_CONST, "eax", constant->constant));
					break;
				}
				case AstType::VAR_VALUE:
				{
					VariableValST* var_st = static_cast<VariableValST*>(ret_st->expression);
					Type t = get_type_by_var_name(var_st->identifier);
					std::string asm_size = get_asm_size(t);
					//if (!asm_size.compare(ASM_32)) {
						// Don't zero or sign extend
						//stream << string_format(ASM_FORMAT_RETURN_VAR, get_stack_offset_by_var_name(var_st->identifier));
					instructions.push_back(new Asm3<std::string, std::string, uint32_t>(AsmType::MOVE_REG_MEM, "eax", asm_size, get_stack_offset_by_var_name(var_st->identifier)));
					//}
					break;
				}
				}
				if (can_return) {
					//stream << ASM_RETURN;
					instructions.push_back(new AssemblyInstruction(AsmType::CLOSE_SF));
					instructions.push_back(new AssemblyInstruction(AsmType::RETURN));
				}
				else {
					//stream << string_format(ASM_FORMAT_JMP, code_block_end_label);
					instructions.push_back(new Asm1<std::string>(AsmType::JUMP, code_block_end_label));
				}
			}
		}

		void assemble_var_assignment(std::vector<AssemblyInstruction*>& instructions, ExpressionST* ret, uint32_t& stack_offset, bool can_return) {
			VariableAssignST* var_st = static_cast<VariableAssignST*>(ret);
			switch (var_st->value->type) {
			case AstType::CONSTANT:
			{
				ConstantST* constant = static_cast<ConstantST*>(var_st->value);
				uint32_t var_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
				std::string asm_size = get_asm_size(get_size_by_var_name(var_st->identifier));

				//stream << string_format(ASM_FORMAT_VAR_AS_CONST, asm_size.c_str(), stack_offset, num);
				instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::MOVE_MEM_CONST, asm_size, var_stack_offset, constant->constant));
				return;
			}
			case AstType::VAR_VALUE:
			{
				VariableValST* var_val = static_cast<VariableValST*>(var_st->value);
				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(var_val->identifier);
				std::string rhs_asm_size = get_asm_size(get_size_by_var_name(var_val->identifier));
				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
				std::string lhs_asm_size = get_asm_size(get_size_by_var_name(var_st->identifier));

				//stream << string_format(ASM_FORMAT_VAR_AS_VAR, rhs_asm_size.c_str(), rhs_stack_offset, lhs_asm_size.c_str(), lhs_stack_offset);
				instructions.push_back(new Asm4<std::string, uint32_t, std::string, uint32_t>(AsmType::MOVE_MEM_MEM, lhs_asm_size, lhs_stack_offset, rhs_asm_size, rhs_stack_offset));
				return;
			}
			}
		}

		void assemble_var_declaration(std::vector<AssemblyInstruction*>& instructions, ExpressionST* ret, uint32_t& stack_offset, bool can_return) {
			VariableDeclarationST* var_st = static_cast<VariableDeclarationST*>(ret);
			if (var_st) {
				std::cout << "Assembling variable declaration: " << var_st->identifier << std::endl;
				std::cout << "Variable type: ";
				switch (var_st->value->type) {
				case AstType::CONSTANT:
				{
					std::cout << "Constant" << std::endl;
					ConstantST* constant = static_cast<ConstantST*>(var_st->value);
					// Decrease stack pointer by the size of the constant and move the value into it
					int64_t num = std::stoi(const_as_num(constant->get_value()).c_str());
					uint8_t size = get_type_size(constant->return_type);
					std::string asm_size = get_asm_size(constant->return_type);

					stack_offset += size;
					//stream << string_format(ASM_FORMAT_VAR_DEC_INIT, asm_size.c_str(), stack_offset, num);
					instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::MOVE_MEM_CONST, asm_size, stack_offset, constant->constant));
					glob_vars.push_back(Variable(var_st->identifier, stack_offset, constant->return_type));
					return;
				}
				case AstType::VAR_VALUE:
				{
					std::cout << "Variable" << std::endl;
					VariableValST* var_rhs = static_cast<VariableValST*>(var_st->value);
					// Decrease stack pointer by the size of the rhs variable and move the value of rhs into it
					uint8_t size = get_type_size(var_rhs->return_type);
					uint32_t rhs_stack_offset = get_stack_offset_by_var_name(var_rhs->identifier);
					std::string asm_size = get_asm_size(var_rhs->return_type);
					std::string rhs_asm_size = asm_size;
					stack_offset += size;

					std::cout << "Assembling Variable declaration by variable value" << std::endl;
					std::cout << var_st->identifier << " stack offset: " << stack_offset << std::endl;
					std::cout << var_rhs->identifier << " stack offset: " << rhs_stack_offset << std::endl;

					// RHS FIRST
					//stream << string_format(ASM_FORMAT_VAR_DEC_INIT_VAR, asm_size.c_str(), rhs_stack_offset, stack_offset);
					instructions.push_back(new Asm4<std::string, uint32_t, std::string, uint32_t>(AsmType::MOVE_MEM_MEM, rhs_asm_size, rhs_stack_offset, asm_size, stack_offset));

					glob_vars.push_back(Variable(var_st->identifier, stack_offset, var_rhs->return_type));
					return;
				}
				case AstType::BLOCK:
					std::cout << "Block" << std::endl;
					// This block can't return void since it is used to assign a value to a variable. Therefore it can't return from the parent function
					BlockST* block_st = assemble_code_block(instructions, var_st->value, stack_offset, false);


					uint8_t size = get_type_size(block_st->return_type);
					stack_offset += size;
					std::string asm_size = get_asm_size(size);

					//stream << string_format(ASM_FORMAT_VAR_DEC_RETURN, stack_offset);
					//instructions.push_back(new Asm2<std::string, uin32_t);
					instructions.push_back(new Asm3<std::string, uint32_t, std::string>(AsmType::MOVE_MEM_REG, asm_size, stack_offset, "eax"));

					glob_vars.push_back(Variable(var_st->identifier, stack_offset, block_st->return_type));
					return;
				}
			}
		}

		void assemble_expression(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset, bool can_return, const char* code_block_end_label) {
			switch (expression->type) {
			case AstType::BLOCK:
				assemble_code_block(instructions, expression, stack_offset, can_return);
				return;
			case AstType::RETURN:
				assemble_return(instructions, expression, stack_offset, can_return, code_block_end_label);
				return;
			case AstType::VAR_DECLARATION:
				assemble_var_declaration(instructions, expression, stack_offset, can_return);
				return;
			case AstType::VAR_ASSIGNMENT:
				assemble_var_assignment(instructions, expression, stack_offset, can_return);
				return;
			case AstType::IF:
				assemble_if(instructions, expression, stack_offset, can_return, code_block_end_label);
				return;
			case AstType::LOOP:
				assemble_loop(instructions, expression, stack_offset, can_return, code_block_end_label);
				return;
			}
		}

		// Assemble an expression and store the result in eax
		void assemble_expression_stres(std::vector<AssemblyInstruction*>& instructions, ExpressionST* expression, uint32_t& stack_offset) {
			switch (expression->type) {
			case AstType::BLOCK:
				assemble_code_block(instructions, expression, stack_offset, true);
				return;
			case AstType::CONSTANT:
			{
				ConstantST* constant = static_cast<ConstantST*>(expression);
				//stream << string_format(ASM_FORMAT_RETURN_CONST, constant->constant.c_str());
				instructions.push_back(new Asm2<std::string, std::string>(AsmType::MOVE_REG_CONST, "eax", constant->constant));
				return;
			}
			case AstType::VAR_VALUE:
			{
				VariableValST* var = static_cast<VariableValST*>(expression);
				//stream << string_format(ASM_FORMAT_RETURN_VAR, get_stack_offset_by_var_name(var->identifier));
				instructions.push_back(new Asm2<std::string, uint32_t>(AsmType::MOVE_REG_MEM, "eax", get_stack_offset_by_var_name(var->identifier)));
				return;
			}
			}
		}

		void assemble_function(std::vector<AssemblyInstruction*>& instructions, FunctionDefST* function, uint32_t& stack_offset) {
			//stream << string_format(ASM_FORMAT_LABEL, !function->name.compare("main") ? "_main" : function->name.c_str());
			instructions.push_back(new Asm1<std::string>(AsmType::LABEL, function->name));
			// Setup stack frame for this function
			//stream << ASM_SETUP_STACK_FRAME;
			instructions.push_back(new AssemblyInstruction(AsmType::SETUP_SF));
			// Assemble the code block. It can return, since it is the body of a function
			BlockST* block_st = assemble_only_code_block(instructions, function->statement, stack_offset, true, "");

			// TODO: This only works if the return statement is the last expression in the code block
			if (block_st->children[block_st->num_children - 1]->type != AstType::RETURN) {
				//stream << ASM_RETURN;
				instructions.push_back(new AssemblyInstruction(AsmType::CLOSE_SF));
				instructions.push_back(new AssemblyInstruction(AsmType::RETURN));
			}
		}

		static std::string assemble(ProgramST* program) {

			uint32_t stack_offset = 0;
			std::vector<AssemblyInstruction*> instructions;
			instructions.push_back(new AssemblyInstruction(AsmType::PROGRAM));
			assemble_function(instructions, program->main, stack_offset);

			std::cout << "Generated Assembly:" << std::endl;
			for(AssemblyInstruction* i : instructions) {
				std::cout << "\t" << asmtype_to_string(i->type) << std::endl;
			}

			optimize(instructions);

			return final_assemble(instructions);
		}
	}
}