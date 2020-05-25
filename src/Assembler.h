#pragma once
#include <string>
#include <sstream>
#include "asm_format.h"
#include "defs.h"

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
		std::vector<std::string> labels;
		uint32_t num_labels;
		uint32_t name_counter; // To generate unique names

		uint32_t get_stack_offset_by_var_name(std::string name) {
			for (Variable v : glob_vars) {
				if (!v.name.compare(name)) {
					return v.stack_offset;
				}
			}
			return NULL;
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
			case Type::UINT8: case Type::INT8: case Type::BOOLEAN: return ASM_8;
			case Type::UINT16: case Type::INT16: return ASM_16;
			case Type::UINT32: case Type::INT32: return ASM_32;
			case Type::UINT64: case Type::INT64: return ASM_64;
			}
			return "ERR";
		}

		std::string get_asm_size(uint8_t size) {
			switch (size) {
			case 1: return ASM_8;
			case 2: return ASM_16;
			case 4: return ASM_32;
			case 8: return ASM_64;
			}
			return "ERR";
		}

		void assemble_expression(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset);

		// Assemble a boolean condition, puts the result into reg
		void assemble_compare_by_op(std::stringstream& stream, OperationST* op_st, uint32_t& stack_offset) {
			if (op_st->lhs->type == AstType::VAR_VALUE && op_st->rhs->type == AstType::VAR_VALUE) {
				// Compare two variables
				VariableValST* lhs = static_cast<VariableValST*>(op_st->lhs);
				VariableValST* rhs = static_cast<VariableValST*>(op_st->rhs);

				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(lhs->identifier);
				std::string lhs_asm_size = get_asm_size(lhs->return_type);
				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(rhs->identifier);
				std::string rhs_asm_size = get_asm_size(lhs->return_type);

				// Compare lhs and rhs (Stores lhs in ebx)
				stream << string_format(ASM_FORMAT_CMP_MEM_MEM, lhs_asm_size.c_str(), lhs_stack_offset, rhs_asm_size.c_str(), rhs_stack_offset);
			}
			else if (op_st->lhs->type == AstType::VAR_VALUE && op_st->rhs->type == AstType::CONSTANT) {
				// Compare a variable and a constant
				VariableValST* lhs = static_cast<VariableValST*>(op_st->lhs);
				ConstantST* rhs = static_cast<ConstantST*>(op_st->rhs);

				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(lhs->identifier);
				std::string lhs_asm_size = get_asm_size(lhs->return_type);

				stream << string_format(ASM_FORMAT_CMP_MEM_CONST, lhs_asm_size.c_str(), lhs_stack_offset, rhs->constant.c_str());
			}
			else if (op_st->lhs->type == AstType::CONSTANT && op_st->rhs->type == AstType::VAR_VALUE) {
				// Compare a variable and a constant
				ConstantST* lhs = static_cast<ConstantST*>(op_st->lhs);
				VariableValST* rhs = static_cast<VariableValST*>(op_st->rhs);
				std::string rhs_asm_size = get_asm_size(rhs->return_type);

				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(rhs->identifier);

				stream << string_format(ASM_FORMAT_CMP_CONST_MEM, lhs->constant.c_str(), rhs_asm_size.c_str(), rhs_stack_offset);
			}
		}

		void assemble_if(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset) {
			IfST* if_st = static_cast<IfST*>(expression);
			bool is_op = if_st->condition->type == AstType::ARITHMETIC_OP;
			if (is_op || if_st->condition->type == AstType::VAR_VALUE) {
				std::string else_label_name = "__else" + std::to_string(name_counter);
				std::string continue_label_name = "__continue" + std::to_string(name_counter);

				++name_counter;
				if (is_op) {
					OperationST* op_st = static_cast<OperationST*>(if_st->condition);
					// Assemble comparison based on the operation
					assemble_compare_by_op(stream, op_st, stack_offset);
					const char* jump_format = "";
					switch (op_st->op) {
					case Operation::EQ:
						jump_format = ASM_FORMAT_JMP_NEQ;
						break;
					case Operation::NEQ:
						jump_format = ASM_FORMAT_JMP_EQ;
						break;
					case Operation::LT:
						jump_format = ASM_FORMAT_JMP_GTE;
						break;
					case Operation::LTE:
						jump_format = ASM_FORMAT_JMP_GT;
						break;
					case Operation::GT:
						jump_format = ASM_FORMAT_JMP_LTE;
						break;
					case Operation::GTE:
						jump_format = ASM_FORMAT_JMP_LT;
						break;
					}
					stream << string_format(jump_format, else_label_name.c_str());
				}
				else {
					// This is a variable, compare with 0 (0 = false, so if equals, we jump to else)
					VariableValST* var_st = static_cast<VariableValST*>(if_st->condition);
					uint32_t var_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
					std::string asm_size = get_asm_size(var_st->return_type);
					// Compare with 0 (false)
					stream << string_format(ASM_FORMAT_CMP_MEM_CONST, asm_size.c_str(), var_stack_offset, "0");
					// If it is 0 (false) then jump to else
					stream << string_format(ASM_FORMAT_JMP_EQ, if_st->has_else ? else_label_name.c_str() : continue_label_name.c_str());
				}

				assemble_expression(stream, if_st->then_body, stack_offset);
				if (if_st->has_else) {
					// Jump to continue to skip else
					stream << string_format(ASM_FORMAT_JMP, continue_label_name.c_str());
					// Put else label here
					stream << string_format(ASM_FORMAT_LABEL, else_label_name.c_str());
					assemble_expression(stream, if_st->else_body, stack_offset);
				}
				// Put continue label here
				stream << string_format(ASM_FORMAT_LABEL, continue_label_name.c_str());
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

		BlockST* assemble_only_code_block(std::stringstream& stream, BlockST* block, uint32_t& stack_offset) {
			for (int i = 0; i < block->num_children; i++) {
				assemble_expression(stream, block->children[i], stack_offset);
			}
			return block;
		}

		BlockST* assemble_only_code_block(std::stringstream& stream, ExpressionST* block, uint32_t& stack_offset) {
			BlockST* block_st = static_cast<BlockST*>(block);
			return assemble_only_code_block(stream, block_st, stack_offset);
		}

		BlockST* assemble_code_block(std::stringstream& stream, ExpressionST* block, uint32_t& stack_offset) {

			BlockST* block_st = static_cast<BlockST*>(block);
			// See if we have to set up a stack frame
			bool stack_frame = false;

			for (int i = 0; i < block_st->num_children; i++) {
				if (block_st->children[i]->type == AstType::VAR_DECLARATION) {
					stack_frame = true;
					break;
				}
			}

			if (stack_frame) {
				std::stringstream block_stream;

				std::string block_label = get_block_label(num_labels).c_str();
				++num_labels;

				stream << string_format(ASM_FORMAT_CALL, block_label.c_str());

				block_stream << string_format(ASM_FORMAT_LABEL, block_label.c_str());
				block_stream << ASM_SETUP_STACK_FRAME;
				assemble_only_code_block(block_stream, block_st, stack_offset);
				if (block_st->children[block_st->num_children - 1]->type != AstType::RETURN) {
					block_stream << ASM_RETURN;
				}

				labels.push_back(block_stream.str());
			}
			else {
				assemble_only_code_block(stream, block_st, stack_offset);
			}
			return block_st;
		}

		void assemble_return(std::stringstream& stream, ExpressionST* ret, uint32_t& stack_offset) {
			ReturnST* ret_st = static_cast<ReturnST*>(ret);
			if (ret_st) {
				switch (ret_st->expression->type) {
				case AstType::CONSTANT:
				{
					ConstantST* constant = static_cast<ConstantST*>(ret_st->expression);
					stream << string_format(ASM_FORMAT_RETURN_CONST, const_as_num(constant->get_value()).c_str());
					break;
				}
				case AstType::VAR_VALUE:
				{
					VariableValST* var_st = static_cast<VariableValST*>(ret_st->expression);
					Type t = get_type_by_var_name(var_st->identifier);
					std::string asm_size = get_asm_size(t);
					if (!asm_size.compare(ASM_32)) {
						// Don't zero or sign extend
						stream << string_format(ASM_FORMAT_RETURN_VAR, get_stack_offset_by_var_name(var_st->identifier));
					}
					else if(is_unsigned_integer_type(t)) {
						// Zero extend
						stream << string_format(ASM_FORMAT_RETURN_VAR_ZX, asm_size.c_str(), get_stack_offset_by_var_name(var_st->identifier));
					}
					else {
						// Sign extend
						stream << string_format(ASM_FORMAT_RETURN_VAR_SX, asm_size.c_str(), get_stack_offset_by_var_name(var_st->identifier));
					}
					break;
				}
				}
				stream << ASM_RETURN;
			}
		}

		void assemble_var_assignment(std::stringstream& stream, ExpressionST* ret, uint32_t& stack_offset) {
			VariableAssignST* var_st = static_cast<VariableAssignST*>(ret);
			switch (var_st->value->type) {
			case AstType::CONSTANT:
			{
				ConstantST* constant = static_cast<ConstantST*>(var_st->value);
				int64_t num = std::stoi(const_as_num(constant->get_value()).c_str());
				uint32_t stack_offset = get_stack_offset_by_var_name(var_st->identifier);
				std::string asm_size = get_asm_size(get_size_by_var_name(var_st->identifier));

				stream << string_format(ASM_FORMAT_VAR_AS_CONST, asm_size.c_str(), stack_offset, num);
				return;
			}
			case AstType::VAR_VALUE:
			{
				VariableValST* var_val = static_cast<VariableValST*>(var_st->value);
				uint32_t rhs_stack_offset = get_stack_offset_by_var_name(var_val->identifier);
				std::string rhs_asm_size = get_asm_size(get_size_by_var_name(var_val->identifier));
				uint32_t lhs_stack_offset = get_stack_offset_by_var_name(var_st->identifier);
				std::string lhs_asm_size = get_asm_size(get_size_by_var_name(var_st->identifier));

				stream << string_format(ASM_FORMAT_VAR_AS_VAR, rhs_asm_size.c_str(), rhs_stack_offset, lhs_asm_size.c_str(), lhs_stack_offset);
				return;
			}
			}
		}

		void assemble_var_declaration(std::stringstream& stream, ExpressionST* ret, uint32_t& stack_offset) {
			VariableDeclarationST* var_st = static_cast<VariableDeclarationST*>(ret);
			if (var_st) {
				switch (var_st->value->type) {
				case AstType::CONSTANT:
				{
					ConstantST* constant = static_cast<ConstantST*>(var_st->value);
					// Decrease stack pointer by the size of the constant and move the value into it
					int64_t num = std::stoi(const_as_num(constant->get_value()).c_str());
					uint8_t size = get_type_size(constant->return_type);
					std::string asm_size = get_asm_size(constant->return_type);

					stack_offset += size;
					stream << string_format(ASM_FORMAT_VAR_DEC_INIT, asm_size.c_str(), stack_offset, num);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset, constant->return_type));
					return;
				}
				case AstType::VAR_VALUE:
				{
					VariableValST* var_rhs = static_cast<VariableValST*>(var_st->value);
					// Decrease stack pointer by the size of the rhs variable and move the value of rhs into it
					uint8_t size = get_type_size(var_rhs->return_type);
					uint32_t rhs_stack_offset = get_stack_offset_by_var_name(var_rhs->identifier);
					std::string asm_size = get_asm_size(var_rhs->return_type);
					stack_offset += size;

					stream << string_format(ASM_FORMAT_VAR_DEC_INIT_VAR, asm_size.c_str(), rhs_stack_offset, stack_offset);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset, var_rhs->return_type));
					return;
				}
				case AstType::BLOCK:
					BlockST* block_st = assemble_code_block(stream, var_st->value, stack_offset);

					uint8_t size = get_type_size(block_st->return_type);
					stack_offset += size;

					stream << string_format(ASM_FORMAT_VAR_DEC_RETURN, stack_offset);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset, block_st->return_type));
					return;
				}
			}
		}

		void assemble_expression(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset) {
			switch (expression->type) {
			case AstType::BLOCK:
				assemble_code_block(stream, expression, stack_offset);
				return;
			case AstType::RETURN:
				assemble_return(stream, expression, stack_offset);
				return;
			case AstType::VAR_DECLARATION:
				assemble_var_declaration(stream, expression, stack_offset);
				return;
			case AstType::VAR_ASSIGNMENT:
				assemble_var_assignment(stream, expression, stack_offset);
				return;
			case AstType::IF:
				assemble_if(stream, expression, stack_offset);
				return;
			}
		}

		void assemble_function(std::stringstream& stream, FunctionDefST* function, uint32_t& stack_offset) {
			stream << string_format(ASM_FORMAT_LABEL, !function->name.compare("main") ? "_main" : function->name.c_str());
			// Setup stack frame for this function
			stream << ASM_SETUP_STACK_FRAME;
			BlockST* block_st = assemble_only_code_block(stream, function->statement, stack_offset);

			if (block_st->children[block_st->num_children - 1]->type != AstType::RETURN) {
				stream << ASM_RETURN;
			}
		}

		static std::string assemble(ProgramST* program) {
			std::stringstream stream;
			stream << ASM_FORMAT_PROGRAM;

			uint32_t stack_offset = 0;

			assemble_function(stream, program->main, stack_offset);

			for (std::string additional_label : labels) {
				stream << additional_label;
			}

			return stream.str();
		}
	}
}