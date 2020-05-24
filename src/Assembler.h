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

			Variable(std::string name, uint32_t stack_offset) {
				this->name = name;
				this->stack_offset = stack_offset;
			}
		};

		std::vector<Variable> glob_vars;
		std::vector<std::string> labels;
		uint32_t num_labels;

		uint32_t get_stack_offset_by_var_name(std::string name) {
			for (Variable v : glob_vars) {
				if (!v.name.compare(name)) {
					return v.stack_offset;
				}
			}
			return NULL;
		}

		void assemble_expression(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset);

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

		BlockST* assemble_code_block(std::stringstream& stream, ExpressionST* block, uint32_t& stack_offset) {
			std::string block_label = get_block_label(num_labels).c_str();
			++num_labels;
			stream << string_format(ASM_FORMAT_CALL, block_label.c_str());

			std::stringstream block_stream;
			block_stream << string_format(ASM_FORMAT_LABEL, block_label.c_str());
			block_stream << ASM_SETUP_STACK_FRAME;
			BlockST* block_st = static_cast<BlockST*>(block);
			for (int i = 0; i < block_st->num_children; i++) {
				assemble_expression(block_stream, block_st->children[i], stack_offset);
			}
			labels.push_back(block_stream.str());
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
					return;
				}
				case AstType::VAR_VALUE:
				{
					VariableValST* var_st = static_cast<VariableValST*>(ret_st->expression);
					stream << string_format(ASM_FORMAT_RETURN_VAR, get_stack_offset_by_var_name(var_st->identifier));
					return;
				}
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
					stream << string_format(ASM_FORMAT_VAR_DEC_INIT, size, num);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset + size));
					return;
				}
				case AstType::VAR_VALUE:
				{
					VariableValST* var_rhs = static_cast<VariableValST*>(var_st->value);
					// Decrease stack pointer by the size of the rhs variable and move the value of rhs into it
					uint8_t size = get_type_size(var_rhs->return_type);
					uint32_t rhs_stack_offset = get_stack_offset_by_var_name(var_rhs->identifier);
					stream << string_format(ASM_FORMAT_VAR_DEC_INIT_VAR, size, rhs_stack_offset);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset + size));
					return;
				}
				case AstType::BLOCK:
					BlockST* block_st = assemble_code_block(stream, var_st->value, stack_offset);

					uint8_t size = get_type_size(block_st->return_type);
					stream << string_format(ASM_FORMAT_VAR_DEC_RETURN, size);

					glob_vars.push_back(Variable(var_st->identifier, stack_offset + size));
					return;
				}
			}
		}

		void assemble_if(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset) {

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
			}
		}

		void assemble_function(std::stringstream& stream, FunctionDefST* function, uint32_t& stack_offset) {
			stream << string_format(ASM_FORMAT_LABEL, !function->name.compare("main") ? "_main" : function->name.c_str());
			stream << ASM_SETUP_STACK_FRAME;
			assemble_code_block(stream, function->statement, stack_offset);
		}

		static std::string assemble(ProgramST* program) {
			std::stringstream stream;
			stream << ASM_FORMAT_PROGRAM;

			uint32_t stack_offset = 0;

			assemble_function(stream, program->main, stack_offset);

			stream << ASM_RETURN;
			stream << "\n";
			for (std::string additional_label : labels) {
				stream << additional_label;
			}

			return stream.str();
		}
	}
}