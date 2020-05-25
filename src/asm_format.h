#pragma once

#define ASM_8 "BYTE"
#define ASM_16 "WORD"
#define ASM_32 "DWORD"
#define ASM_64 "QWORD"

/* FUNCTIONS   
 %s:
 ^^
 Function name
 %s           < Function body
*/
#define ASM_FORMAT_LABEL "%s:\n"

/* CREATE STACKFRAME
 push ebp
 mov ebp, esp
*/
#define ASM_SETUP_STACK_FRAME "\tpush ebp\n\tmov ebp, esp\n"

/* PROGRAM
 .intel_syntax noprefix
 .global _main

 .text
*/
#define ASM_FORMAT_PROGRAM "\t.intel_syntax noprefix\n\t.global _main\n\n\t.text\n"

/* VARIABLE DECLARATION & INITIALIZATION, CONSTANT
 sub %d, esp
     ^^
     Bytes to allocate
 mov %s PTR [esp], %d
     ^^            ^^
    Size         Constant
*/
#define ASM_FORMAT_VAR_DEC_INIT "\tmov %s PTR [ebp-%u], %d\n"

/* VARIABLE DECLARATION & INITIALIZATION, VARIABLE
 sub %d, esp
     ^^
     Bytes to allocate
 mov DWORD PTR [esp], %s PTR [ebp-%d]
                                  ^^
                     Stack offset of rhs variable
*/
#define ASM_FORMAT_VAR_DEC_INIT_VAR "\tmov ebx, %s PTR [ebp-%u]\n\tmov DWORD PTR [ebp-%d], ebx\n"

/* VARIABLE DECLARATION & INITIALIZATION, EAX
 sub %d, esp
 mov DWORD PTR [esp], eax
*/
#define ASM_FORMAT_VAR_DEC_RETURN "\tmov DWORD PTR [ebp-%u], eax\n"

// [SIZE] [CONST]
#define ASM_FORMAT_VAR_AS_CONST "\tmov %s PTR [ebp-%u], %d\n"

// [SIZE] [MEM FROM] [SIZE] [MEM TO]
#define ASM_FORMAT_VAR_AS_VAR "\tmov ebx, %s PTR [ebp-%u]\n\tmov %s PTR [ebp-%u], ebx\n"

/* RETURN
 ret
*/
#define ASM_RETURN "\tpop ebp\n\tret\n"

/* SET RETURN VALUE AND RETURN, VARIABLE
 mov eax, DWORD PTR [ebp-%d]
                         ^^
                         Stack offset
 mov ebp, esp   ; leave stack frame
 pop ebp
 ret            ; return
 */
#define ASM_FORMAT_RETURN_VAR "\tmov eax, DWORD PTR [ebp-%u]\n"

#define ASM_FORMAT_RETURN_VAR_ZX "\tmovzx eax, %s PTR [ebp-%u]\n"

#define ASM_FORMAT_RETURN_VAR_SX "\tmovsx eax, %s PTR [ebp-%u]\n"

/* SET RETURN VALUE AND RETURN, CONSTANT
 mov eax, %d
          ^^
          Value
 mov ebp, esp  ; leave stack frame
 pop ebp
 ret           ; return
 */
#define ASM_FORMAT_RETURN_CONST "\tmov eax, %s\n"

/* CALL LABEL
 call %s
*/
#define ASM_FORMAT_CALL "\tcall %s\n"

/* COMPARE MEM & CONSTANT
 cmp DWORD PTR [ebp-%u], %d
*/
#define ASM_FORMAT_CMP_MEM_CONST "\tcmp %s PTR [ebp-%u], %s\n"

/* COMPARE CONSTANT & MEM
 cmp %d, DWORD PTR [ebp-%u]
*/
#define ASM_FORMAT_CMP_CONST_MEM "\tcmp %s, %s PTR [ebp-%u]\n"

/* COMPARE MEM & MEM
 mov ebx, DWORD PTR [ebp-%u]
 cmp ebx, DWORD PTR [ebp-%u]
*/
#define ASM_FORMAT_CMP_MEM_MEM "\tmov ebx, %s PTR [ebp-%u]\n\tcmp ebx, %s PTR [ebp-%u]\n"

/* UNCONDITIONAL JUMP
 jmp %s
*/
#define ASM_FORMAT_JMP "\tjmp %s\n"

/* JUMP EQUAL
 je %s
*/
#define ASM_FORMAT_JMP_EQ "\tje %s\n"

/* JUMP NOT EQUAL
 jne %s
*/
#define ASM_FORMAT_JMP_NEQ "\tjne %s\n"

#define ASM_FORMAT_JMP_GT "\tjg %s\n"
#define ASM_FORMAT_JMP_GTE "\tjge %s\n"

#define ASM_FORMAT_JMP_LT "\tjl %s\n"
#define ASM_FORMAT_JMP_LTE "\tjle %s\n"

/* STORE VARIABLE VALUE IN REGISTER
 mov %s, DWORD PTR [ebp-%u]
*/
#define ASM_FORMAT_STORE_MEM_REG "\tmov %s, DWORD PTR [ebp-%u]\n"