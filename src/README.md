# Bonfire Compiler
The Bonfire Compiler compiles to x86 Assembly for the GNU Assembler.

There are four compile stages:
1. **Pre-Processor**: Looks through the code and removes comments
2. **Tokenizer/Lexer**: Converts the code into tokens
3. **Parser**: Parses constructs like if statements or functions. Creates an Abstract Syntax Tree
4. **Assembler**: Assembles the final Assembly Code from the Abstract Syntax Tree

The compiler currently doesn't optimize the final code.
