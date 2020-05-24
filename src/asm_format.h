#pragma once
/* FUNCTIONS   
 %s:
 ^^
 Function name
 %s           < Function body
*/
#define ASM_FORMAT_FUNCTION "%s:\n\tpush ebp\n\tmov ebp, esp\n"

/* PROGRAM
 
*/
#define ASM_FORMAT_PROGRAM "\t.intel_syntax noprefix\n\t.global _main\n\n\t.text\n"

/* VARIABLE DECLARATION & INITIALIZATION
 mov DWORD PTR [rbp-%d], %d
                    ^^   ^^
          Stack offset   Constant
*/
#define ASM_FORMAT_VAR_DEC_INIT "\tmov DWORD PTR [ebp-%u], %s\n"

/* SET RETURN VALUE AND RETURN, VARIABLE
 mov eax, DWORD PTR [rbp-%d]
                         ^^
               Stack offset
 ret
 */
#define ASM_FORMAT_RETURN_VAR "\tmov eax, DWORD PTR [ebp-%u]\n\tret\n"

/* SET RETURN VALUE AND RETURN, CONSTANT
 mov eax, %d
          ^^
          Value
 ret
 */
#define ASM_FORMAT_RETURN_CONST "\tmov eax, %s\n\tpop ebp\n\tret\n"