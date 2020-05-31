#pragma once

#define ASM_SIZE_8 "BYTE"
#define ASM_SIZE_16 "WORD"
#define ASM_SIZE_32 "DWORD"
#define ASM_SIZE_64 "QWORD"

#define ASM_PROGRAM "\t.intel_syntax noprefix\n\t.global _main\n\t.text\n"

#define ASM_SETUP_STACK_FRAME "\t;Setup stack frame\n"
#define ASM_CLOSE_STACK_FRAME "\t;Close stack frame\n"
#define ASM_RETURN "\tret\n"

#define ASM_LABEL "%s:\n"
#define ASM_CALL "\tcall %s\n"

#define ASM_MOVE_REG_MEM "\tmov %s, %s PTR [ebp-%u]\n"
#define ASM_MOVE_REG_REG "\tmov %s, %s\n"
#define ASM_MOVE_REG_CONST "\tmov %s, %s\n"

#define ASM_MOVE_MEM_MEM "\tmov %s, %s PTR [ebp-%u]\n\tmov %s PTR [ebp-%u], %s\n"
#define ASM_MOVE_MEM_REG "\tmov %s PTR [ebp-%u], %s\n"
#define ASM_MOVE_MEM_CONST "\tmov %s PTR [ebp-%u], %s\n"

#define CMP_CONST_CONST "\tcmp %s, %s\n"
#define CMP_CONST_MEM "\tcmp %s, %s PTR [ebp-%u]\n"
#define CMP_CONST_REG "\tcmp %s, %s\n"

#define CMP_MEM_CONST "\tcmp %s PTR [ebp-%u], %s\n"
#define CMP_MEM_MEM "\tmov %s, %s PTR [ebp-%u]\n\tcmp %s, %s PTR [ebp-%u]"
#define CMP_MEM_REG "\tcmp %s PTR [ebp-%u], %s\n"

#define CMP_REG_CONST "\tcmp %s, %s\n"
#define CMP_REG_MEM "\tcmp %s, %s PTR [ebp-%u]\n"
#define CMP_REG_REG "\tcmp %s, %s\n"

#define JMP "\tjmp %s\n"
#define JMP_EQ "\tje %s\n"
#define JMP_NEQ "\tjne %s\n"
#define JMP_GT "\tjg %s\n"
#define JMP_GTE "\tjge %s\n"
#define JMP_LT "\tjl %s\n"
#define JMP_LTE "\tjle %s\n"