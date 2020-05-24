#pragma once
#include <string>
#include <sstream>
#include "asm_format.h"
#include "defs.h"

#define ASM_ERR "err"

namespace Bonfire {
	namespace Assembler {
		void assemble_expression(std::stringstream& stream, ExpressionST* expression, uint32_t& stack_offset);

		void assemble_code_block(std::stringstream& stream, ExpressionST* block, uint32_t& stack_offset) {
			BlockST* block_st = static_cast<BlockST*>(block);
			for (int i = 0; i < block_st->num_children; i++) {
				assemble_expression(stream, block_st->children[i], stack_offset);
			}
		}

		void assemble_return(std::stringstream& stream, ExpressionST* ret, uint32_t& stack_offset) {
			ReturnST* ret_st = static_cast<ReturnST*>(ret);
			if (ret_st) {
				switch (ret_st->expression->type) {
				case AstType::CONSTANT:
				{
					ConstantST* constant = static_cast<ConstantST*>(ret_st->expression);
					stream << string_format(ASM_FORMAT_RETURN_CONST, constant->get_value().c_str());
					return;
				}
				case AstType::VAR_VALUE:
				{
					//VariableValST* var_st = static_cast<VariableValST*>(ret_st->expression);
					stream << string_format(ASM_FORMAT_RETURN_VAR, stack_offset);
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
					ConstantST* constant = static_cast<ConstantST*>(var_st->value);
					stack_offset += 4;
					stream << string_format(ASM_FORMAT_VAR_DEC_INIT, stack_offset, constant->get_value().c_str());
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
			}
		}

		void assemble_function(std::stringstream& stream, FunctionDefST* function, uint32_t& stack_offset) {
			stream << string_format(ASM_FORMAT_FUNCTION, !function->name.compare("main") ? "_main" : function->name.c_str());
			assemble_expression(stream, function->statement, stack_offset);
		}

		static std::string assemble(ProgramST* program) {
			std::stringstream stream;
			stream << ASM_FORMAT_PROGRAM;

			uint32_t stack_offset = 0;

			assemble_function(stream, program->main, stack_offset);
			return stream.str();
		}
	}
}