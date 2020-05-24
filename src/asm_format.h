#pragma once
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
 mov DWORD PTR [esp], %d
                      ^^
                Constant
*/
#define ASM_FORMAT_VAR_DEC_INIT "\tsub esp, %d\n\tmov DWORD PTR [esp], %d\n"

/* VARIABLE DECLARATION & INITIALIZATION, VARIABLE
 sub %d, esp
     ^^
     Bytes to allocate
 mov DWORD PTR [esp], DWORD PTR [ebp-%d]
                                     ^^
                     Stack offset of rhs variable
*/
#define ASM_FORMAT_VAR_DEC_INIT_VAR "\tsub esp, %d\n\tmov DWORD PTR [esp], DWORD PTR [ebp-%d]\n"

/* VARIABLE DECLARATION & INITIALIZATION, EAX
 sub %d, esp
 mov DWORD PTR [esp], eax
*/
#define ASM_FORMAT_VAR_DEC_RETURN "\tsub esp, %d\n\tmov DWORD PTR [esp], eax\n"

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
#define ASM_FORMAT_RETURN_VAR "\tmov eax, DWORD PTR [ebp-%u]\n\tpop ebp\n\tret\n"

/* SET RETURN VALUE AND RETURN, CONSTANT
 mov eax, %d
          ^^
          Value
 mov ebp, esp  ; leave stack frame
 pop ebp
 ret           ; return
 */
#define ASM_FORMAT_RETURN_CONST "\tmov eax, %s\n\tpop ebp\n\tret\n"

/* CALL LABEL
 call %s
*/
#define ASM_FORMAT_CALL "\tcall %s\n"