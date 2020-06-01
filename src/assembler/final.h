#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "assembler/instructions.h"
#include "assembler/format.h"
#include "utils/strutils.h"

namespace Bonfire {

	static std::string final_assemble(std::vector<AssemblyInstruction*>& instructions) {
		std::stringstream stream;
		std::cout << "Amount of Instructions: " << instructions.size() << std::endl;
		for (uint32_t i = 0; i < instructions.size(); i++) {
			std::cout << "instruction " << i << " (";
			switch (instructions[i]->type) {
			case AsmType::PROGRAM:
				stream << ASM_PROGRAM;
				break;
			case AsmType::SETUP_SF:
				stream << ASM_SETUP_STACK_FRAME;
				break;
			case AsmType::CLOSE_SF:
				stream << ASM_CLOSE_STACK_FRAME;
				break;
			case AsmType::RETURN:
				stream << ASM_RETURN;
				break;
			case AsmType::CALL:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(ASM_CALL, as->data1.c_str());
				break;
			}
			/////////////// MOVE
			case AsmType::MOVE_REG_MEM:
			{
				auto as = static_cast<Asm3<std::string, std::string, uint32_t>*>(instructions[i]);
				stream << string_format(ASM_MOVE_REG_MEM, as->data1.c_str(), as->data2.c_str(), as->data3);
				break;
			}
			case AsmType::MOVE_REG_REG:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(ASM_MOVE_REG_REG, as->data1.c_str(), as->data2.c_str());
				break;
			}
			case AsmType::MOVE_REG_CONST:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(ASM_MOVE_REG_CONST, as->data1.c_str(), as->data2.c_str());
				break;
			}
			case AsmType::MOVE_MEM_MEM:
			{
				auto as = static_cast<Asm4<std::string, uint32_t, std::string, uint32_t>*>(instructions[i]);
				stream << string_format(ASM_MOVE_MEM_MEM, "ebx", as->data1.c_str(), as->data2, "ebx", as->data3.c_str(), as->data4);
				break;
			}
			case AsmType::MOVE_MEM_REG:
			{
				auto as = static_cast<Asm3<std::string, uint32_t, std::string>*>(instructions[i]);
				stream << string_format(ASM_MOVE_MEM_REG, as->data1.c_str(), as->data2, as->data3.c_str());
				break;
			}
			case AsmType::MOVE_MEM_CONST:
			{
				auto as = static_cast<Asm3<std::string, uint32_t, std::string>*>(instructions[i]);
				stream << string_format(ASM_MOVE_MEM_CONST, as->data1.c_str(), as->data2, as->data3.c_str());
				break;
			}
			case AsmType::LABEL:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(ASM_LABEL, as->data1.c_str());
				break;
			}
			//////////////// COMPARE
			// CONST
			case AsmType::COMP_CONST_CONST:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(CMP_CONST_CONST, as->data1.c_str(), as->data2.c_str());
				break;
			}
			case AsmType::COMP_CONST_MEM:
			{
				auto as = static_cast<Asm3<std::string, std::string, uint32_t>*>(instructions[i]);
				stream << string_format(CMP_CONST_MEM, as->data1.c_str(), as->data2.c_str(), as->data3);
				break;
			}
			case AsmType::COMP_CONST_REG:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(CMP_CONST_REG, as->data1.c_str(), as->data2.c_str());
				break;
			}
			// MEMORY
			case AsmType::COMP_MEM_CONST:
			{
				auto as = static_cast<Asm3<std::string, uint32_t, std::string>*>(instructions[i]);
				stream << string_format(CMP_MEM_CONST, as->data1.c_str(), as->data2, as->data3.c_str());
				break;
			}
			case AsmType::COMP_MEM_MEM:
			{
				auto as = static_cast<Asm4<std::string, uint32_t, std::string, uint32_t>*>(instructions[i]);
				stream << string_format(CMP_MEM_MEM, "ebx", as->data1.c_str(), as->data2, "ebx", as->data3.c_str(), as->data4);
				break;
			}
			case AsmType::COMP_MEM_REG:
			{
				auto as = static_cast<Asm3<std::string, uint32_t, std::string>*>(instructions[i]);
				stream << string_format(CMP_MEM_REG, as->data1.c_str(), as->data2, as->data3.c_str());
				break;
			}
			// REGISTERS
			case AsmType::COMP_REG_CONST:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(CMP_REG_CONST, as->data1.c_str(), as->data2.c_str());
				break;
			}
			case AsmType::COMP_REG_MEM:
			{
				auto as = static_cast<Asm3<std::string, std::string, uint32_t>*>(instructions[i]);
				stream << string_format(CMP_REG_MEM, as->data1.c_str(), as->data2.c_str(), as->data3);
				break;
			}
			case AsmType::COMP_REG_REG:
			{
				auto as = static_cast<Asm2<std::string, std::string>*>(instructions[i]);
				stream << string_format(CMP_REG_REG, as->data1.c_str(), as->data2.c_str());
				break;
			}
			//////////// JUMP
			case AsmType::JUMP:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_EQ:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_EQ, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_NEQ:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_NEQ, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_GT:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_GT, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_GTE:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_GTE, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_LT:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_LT, as->data1.c_str());
				break;
			}
			case AsmType::JUMP_LTE:
			{
				auto as = static_cast<Asm1<std::string>*>(instructions[i]);
				stream << string_format(JMP_LTE, as->data1.c_str());
				break;
			}
			}
			std::cout << ")" << std::endl;
		}
		std::cout << "I knew it!" << std::endl;
		return stream.str();
	}
}