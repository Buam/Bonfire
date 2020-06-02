#pragma once
#include "assembler/instructions.h"

namespace Bonfire {
	static void optimize_jumps(std::vector<AssemblyInstruction*>& instructions) {
		std::vector<int> to_remove;
		for(int i = 0; i < instructions.size() - 1; i++) {
			if(instructions[i]->type == AsmType::JUMP) {
				auto jump_instruction = static_cast<Asm1<std::string>*>(instructions[i]);
				// Check if the instruction after that is the label
				if(instructions[i + 1]->type == AsmType::LABEL) {
					auto label_instruction = static_cast<Asm1<std::string>*>(instructions[i]);
					// Check if the jump from before calls the label
					if(!jump_instruction->data1.compare(label_instruction->data1)) {
						// Remove
						to_remove.push_back(i);
						to_remove.push_back(i + 1);
					}
				}
			}
		}
		int offset = 0; // Number of already removed elements
		for(int i : to_remove) {
			instructions.erase(instructions.begin() + i - offset);
			++offset;
		}
	}

	static void optimize(std::vector<AssemblyInstruction*>& instructions) {
		optimize_jumps(instructions);
	}
}