#pragma once
#include <string>
namespace Bonfire {
	enum class AsmType {
		PROGRAM,
		LABEL,
		SETUP_SF,
		CLOSE_SF,
		RETURN,
		CALL,
		MOVE_REG_MEM,
		MOVE_MEM_REG,
		MOVE_MEM_MEM,
		MOVE_REG_REG,
		MOVE_MEM_CONST,
		MOVE_REG_CONST,
		MOVEZX_REG_MEM,
		MOVESX_REG_MEM,
		MOVEZX_MEM_REG,
		MOVESX_MEM_REG,
		COMP_MEM_CONST,
		COMP_MEM_REG,
		COMP_MEM_MEM,
		COMP_CONST_CONST,
		COMP_CONST_MEM,
		COMP_CONST_REG,
		COMP_REG_CONST,
		COMP_REG_MEM,
		COMP_REG_REG,
		JUMP,
		JUMP_EQ,
		JUMP_NEQ,
		JUMP_GT,
		JUMP_GTE,
		JUMP_LT,
		JUMP_LTE
	};

	struct AssemblyInstruction {
		AsmType type;

		AssemblyInstruction() {}

		AssemblyInstruction(AsmType type) {
			this->type = type;
		}
	};

	template<class T>
	struct Asm1 : AssemblyInstruction {
		T data;
		Asm1(AsmType type, T data) {
			this->type = type;
			this->data = data;
		}
	};

	template<class T, class E>
	struct Asm2 : AssemblyInstruction {
		T data1;
		E data2;
		Asm2(AsmType type, T data1, E data2) {
			this->type = type;
			this->data1 = data1;
			this->data2 = data2;
		}
	};

	template<class T, class E, class F>
	struct Asm3 : AssemblyInstruction {
		T data1;
		E data2;
		F data3;
		Asm3(AsmType type, T data, E data2, F data3) {
			this->type = type;
			this->data1 = data1;
			this->data2 = data2;
			this->data3 = data3;
		}
	};

	template<class T, class E, class F, class G>
	struct Asm4 : AssemblyInstruction {
		T data1;
		E data2;
		F data3;
		G data4;
		Asm4(AsmType type, T data, E data2, F data3, G data4) {
			this->type = type;
			this->data1 = data1;
			this->data2 = data2;
			this->data3 = data3;
			this->data4 = data4;
		}
	};
}