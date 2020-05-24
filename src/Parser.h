#pragma once
#include <vector>
#include <set>
#include "Lexer.h"
#include "Token.h"
#include "defs.h"
#include "asm_format.h"

namespace Bonfire {
	namespace Parser {

		Type parse_type(std::string source) {
			if (!source.compare("i8"))		return Type::INT8;
			if (!source.compare("i16"))		return Type::INT16;
			if (!source.compare("i32"))		return Type::INT32;
			if (!source.compare("i64"))		return Type::INT64;
			if (!source.compare("u8"))		return Type::UINT8;
			if (!source.compare("u16"))		return Type::UINT16;
			if (!source.compare("u32"))		return Type::UINT32;
			if (!source.compare("u64"))		return Type::UINT64;
			if (!source.compare("bool"))	return Type::BOOLEAN;
			// TODO: Exceptions
			return Type::VOID;
		}

		// Exception to throw if something could not be parsed
		class parse_exception : std::exception {};

		// Exception to throw when a code block was fully parsed
		class block_done_exception : std::exception {};

		// Exception to throw if there is a token that stops us from compiling
		// (unlike parse_exception, which only exists so we can try parsing something else)
		class unexpected_token : std::exception {
		public:
			int64_t index;
			unexpected_token(int64_t index) {
				this->index = index;
			}
		};

		class unexpected_c : std::exception {
		public:
			uint64_t index;
			unexpected_c(uint64_t index) {
				this->index = index;
			}
		};

		ExpressionST* parse_expression(std::vector<Token>& tokens, uint64_t& cursor, Type return_type);

		Operation parse_operation(std::vector<Token>& tokens, uint64_t& cursor) {
			++cursor;
			// If the token at the cursor is an operation, it is only one token: We use a switch
			switch (tokens[cursor - 1].type) {
			case TokenType::AND:
				return Operation::AND;
			case TokenType::ANDL:
				return Operation::ANDL;
			case TokenType::OR:
				return Operation::OR;
			case TokenType::ORL:
				return Operation::ORL;
			case TokenType::PLUS:
				return Operation::ADD;
			case TokenType::MINUS:
				return Operation::SUB;
			case TokenType::MUL:
				return Operation::MUL;
			case TokenType::SLASH:
				return Operation::DIV;
			case TokenType::MODULO:
				return Operation::MOD;
			case TokenType::POW:
				return Operation::POW;
			default:
				--cursor;
				return Operation::NONE;
			}
		}

		ConstantST* parse_constant(std::vector<Token>& tokens, uint64_t& cursor, Type expected_type) {
			if (cursor + 1 >= tokens.size()) throw parse_exception();
			if (tokens[cursor].type == TokenType::CONSTANT) {
				std::string value = tokens[cursor].value;
				++cursor;
				return new ConstantST(expected_type, value);
			}
			throw parse_exception();
		}

		// <- expression
		ReturnST* parse_return(std::vector<Token>& tokens, uint64_t& cursor, Type return_type) {
			if (cursor + 2 >= tokens.size()) throw parse_exception();
			if (tokens[cursor].type == TokenType::RETURN) {
				++cursor;
				ExpressionST* expression = parse_expression(tokens, cursor, return_type);
				if (expression->return_type == Type::VOID || expression->return_type != return_type) {
					// TODO: Exception, for now: just fail
					throw parse_exception();
				}

				ReturnST* return_st = new ReturnST(expression);
				return return_st;
			}
			throw parse_exception();
		}

		VariableValST* parse_variable_value(std::vector<Token>& tokens, uint64_t& cursor, Type expected_type) {
			if (cursor >= tokens.size()) throw parse_exception();
			if (tokens[cursor].type == TokenType::IDENTIFIER) {
				VariableValST* var_val_st = new VariableValST(tokens[cursor].value, expected_type);
				++cursor;
				return var_val_st;
			}
			throw parse_exception();
		}

		IfST* parse_if(std::vector<Token>& tokens, uint64_t& cursor, Type expected_type) {
			if (cursor + 3 >= tokens.size()) throw parse_exception();
			if (tokens[cursor].type == TokenType::IF) {
				++cursor;
				if (tokens[cursor].type == TokenType::PAR_OPEN) {
					++cursor;
					ExpressionST* condition = parse_expression(tokens, cursor, Type::BOOLEAN);
					if (tokens[cursor].type == TokenType::PAR_CLOSE) {
						++cursor;
						ExpressionST* then_body = parse_expression(tokens, cursor, expected_type);

						IfST* if_st = new IfST(condition, then_body, expected_type);
						// Check if there is an else body after :
						if (tokens[cursor].type == TokenType::COLON) {
							// There is an else body
							++cursor;
							ExpressionST* else_body = parse_expression(tokens, cursor, expected_type);
							if_st->has_else = true;
							if_st->else_body = else_body;
						}
						return if_st;
					}
				}
			}
			throw parse_exception();
		}

		VariableDeclarationST* parse_variable_declaration(std::vector<Token>& tokens, uint64_t& cursor) {
			if (cursor + 4 >= tokens.size()) throw parse_exception();
			if (tokens[cursor].type == TokenType::IDENTIFIER) {
				std::string var_name = tokens[cursor].value;
				++cursor;
				if (tokens[cursor].type == TokenType::COLON) {
					++cursor;
					if (tokens[cursor].type == TokenType::IDENTIFIER) {
						Type var_type = parse_type(tokens[cursor].value);
						if (var_type == Type::VOID) { throw parse_exception(); }
						++cursor;
						if (tokens[cursor].type == TokenType::EQUALS) {
							++cursor;
							// This is the expression, we have to assign the variable to
							ExpressionST* expression = parse_expression(tokens, cursor, var_type);
							if (expression->return_type != var_type) { throw parse_exception(); }

							VariableDeclarationST* var_st = new VariableDeclarationST(var_name, var_type, expression);
							return var_st;
						}
					}
				}
			}
			throw parse_exception();
		}

		BlockST* parse_code_block(std::vector<Token>& tokens, uint64_t& cursor, Type return_type);

		// Parses exactly one ST
		ExpressionST* parse_expression(std::vector<Token>& tokens, uint64_t& cursor, Type return_type) {
			uint64_t start_cursor = cursor; // We have to reset the cursor everytime the parser fails to parse
			ExpressionST* expression;
			try {
				expression = parse_return(tokens, cursor, return_type);
			}
			catch (parse_exception) {
				cursor = start_cursor;
				try {
					expression = parse_code_block(tokens, cursor, return_type);
				}
				catch (parse_exception) {
					cursor = start_cursor;
					try {
						expression = parse_variable_declaration(tokens, cursor);
					}
					catch (parse_exception) {
						cursor = start_cursor;
						try {
							expression = parse_variable_value(tokens, cursor, return_type);
						}
						catch (parse_exception) {
							cursor = start_cursor;
							try {
								expression = parse_constant(tokens, cursor, return_type);
							}
							catch (parse_exception) {
								if (tokens[cursor].type == TokenType::BRACE_CLOSE) {
									++cursor;
									throw block_done_exception();
								}
								throw unexpected_token(cursor);
							}
						}
					}
				}
			}
			// Check for operations
			Operation op = parse_operation(tokens, cursor);
			if (op != Operation::NONE) {
				// There is an operation. expression is lhs
				ExpressionST* expression_rhs = parse_expression(tokens, cursor, return_type);
				OperationST* operation = new OperationST(op, expression, expression_rhs);
				return operation;
			}
			// There is no operation, return expression
			return expression;
		}

		BlockST* parse_code_block(std::vector<Token>& tokens, uint64_t& cursor, Type return_type) {
			if (tokens[cursor].type == TokenType::RETURN_TYPE) {
				// See if it is the same type
				++cursor;
				if (tokens[cursor].type == TokenType::IDENTIFIER) {
					if (parse_type(tokens[cursor].value) != return_type) {
						throw parse_exception();
					}
					++cursor;
				}
			}

			if (tokens[cursor].type == TokenType::BRACE_OPEN) {
				// Parse everything inside the code block and put it as the children of the code block
				// put cursor at the beginning
				++cursor;
				std::vector<ExpressionST*> block_children;

				while (1) {
					try {
						block_children.push_back(parse_expression(tokens, cursor, return_type));
					}
					catch (block_done_exception) {
						// This exception tells us that we found a '}' during parsing, which means that
						// this code block is fully parsed
						break;
					}
					// The cursor will be outside of the code block when this loop finishes
				}

				// Create the Code Block
				ExpressionST** block_children_arr = new ExpressionST*[block_children.size()];
				for (int i = 0; i < block_children.size(); i++) {
					block_children_arr[i] = block_children[i];
				}

				return new BlockST(return_type, block_children_arr, block_children.size());
			}
			throw parse_exception();
		}

		// Returns a pointer because of slicing
		BlockST* parse_code_block(std::vector<Token>& tokens, uint64_t& cursor) {
			if (tokens[cursor].type == TokenType::RETURN_TYPE) {
				// Parse return type
				++cursor;
				if (tokens[cursor].type == TokenType::IDENTIFIER) {
					Type return_type = parse_type(tokens[cursor].value);
					++cursor;
					// Cursor is now at '{'
					return parse_code_block(tokens, cursor, return_type);
				}
			}
			else {
				// Return type is void
				return parse_code_block(tokens, cursor, Type::VOID);
			}
			throw parse_exception();
		}

		FunctionDefST* parse_function(std::vector<Token>& tokens, uint64_t& cursor) {
			FunctionDefST* fun = new FunctionDefST();

			if (tokens[cursor].type == TokenType::IDENTIFIER) {
				fun->name = tokens[cursor].value;
				++cursor;
				if (tokens[cursor].type == TokenType::PAR_OPEN) {
					++cursor;
					if (tokens[cursor].type == TokenType::PAR_CLOSE) {
						++cursor;
						// Token at cursor is now '{', the beginning of the code, parse the code block
						fun->statement = parse_code_block(tokens, cursor);
						return fun;
					}
				}
			}
			throw unexpected_token(cursor);
		}

		static ProgramST* parse(std::vector<Token>& tokens) {
			uint64_t cursor = 0;

			// First thing we expect is a function
			return new ProgramST(parse_function(tokens, cursor));
		}
	}
}