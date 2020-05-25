#pragma once
#include <vector>
#include <set>
#include "Token.h"
#include "Parser.h"
#include "strutils.h"

namespace Bonfire {
	namespace Lexer {

		class unexpected_c : std::exception {
		public:
			uint64_t index;
			unexpected_c(uint64_t index) {
				this->index = index;
			}
		};

		// Tokenize a given source into a vector of tokens
		void tokenize(std::string source, std::vector<Token>& tokens_out, std::vector<uint64_t>& token_indices) {
			uint64_t cursor = 0;

			while (cursor < source.size() - 1) {
				char c = source[cursor];
				if (is_whitespace(c)) {
					++cursor;
					continue;
				}

				// (
				if (c == '(') {
					tokens_out.push_back(Token(TokenType::PAR_OPEN, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// )
				else if (c == ')') {
					tokens_out.push_back(Token(TokenType::PAR_CLOSE, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// {
				else if (c == '{') {
					tokens_out.push_back(Token(TokenType::BRACE_OPEN, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// }
				else if (c == '}') {
					tokens_out.push_back(Token(TokenType::BRACE_CLOSE, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// -
				else if (c == '-' && source[cursor + 1] == '>') {
					tokens_out.push_back(Token(TokenType::RETURN_TYPE, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				// <-
				else if (c == '<' && source[cursor + 1] == '-') {
					tokens_out.push_back(Token(TokenType::RETURN, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				// :
				else if (c == ':') {
					tokens_out.push_back(Token(TokenType::COLON, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == '=' && source[cursor + 1] == '=') {
					tokens_out.push_back(Token(TokenType::EQUALS2, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				// =
				else if (c == '=') {
					tokens_out.push_back(Token(TokenType::EQUALS, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// ?
				else if (c == '?') {
					tokens_out.push_back(Token(TokenType::IF, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// &&
				else if (c == '&' && source[cursor + 1] == '&') {
					tokens_out.push_back(Token(TokenType::ANDL, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				// ||
				else if (c == '|' && source[cursor + 1] == '|') {
					tokens_out.push_back(Token(TokenType::ORL, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				// &
				else if (c == '&') {
					tokens_out.push_back(Token(TokenType::AND, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// |
				else if (c == '|') {
					tokens_out.push_back(Token(TokenType::OR, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// +
				else if (c == '+') {
					tokens_out.push_back(Token(TokenType::PLUS, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// -
				else if (c == '-') {
					tokens_out.push_back(Token(TokenType::MINUS, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// *
				else if (c == '*') {
					tokens_out.push_back(Token(TokenType::MUL, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// /
				else if (c == '/') {
					tokens_out.push_back(Token(TokenType::SLASH, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// %
				else if (c == '%') {
					tokens_out.push_back(Token(TokenType::MODULO, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// ^
				else if (c == '^') {
					tokens_out.push_back(Token(TokenType::POW, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				// true
				else if (cursor + 4 < source.size() &&
					c == 't' && source[cursor + 1] == 'r' && source[cursor + 2] == 'u' && source[cursor + 3] == 'e'
					&& !isalpha(source[cursor + 4])
					) {
					tokens_out.push_back(Token(TokenType::CONSTANT, "1"));
					token_indices.push_back(cursor);
					cursor += 4;
				}
				// false
				else if (cursor + 5 < source.size() &&
					c == 'f' && source[cursor + 1] == 'a' && source[cursor + 2] == 'l' && source[cursor + 3] == 's'
					&& source[cursor + 4] == 'e' && !isalpha(source[cursor + 5])
					) {
					tokens_out.push_back(Token(TokenType::CONSTANT, "0"));
					token_indices.push_back(cursor);
					cursor += 5;
				}
				else if(isalpha(c)) {
					// Identifier
					token_indices.push_back(cursor);
					std::string identifier = "";
					while (isalpha(source[cursor]) || isdigit(source[cursor])) {
						identifier += source[cursor];
						++cursor;
					}
					tokens_out.push_back(Token(TokenType::IDENTIFIER, identifier));
				}
				else if (isdigit(c)) {
					// Number Constant
					std::string number = "";
					while (isdigit(source[cursor])) {
						number += source[cursor];
						++cursor;
					}
					tokens_out.push_back(Token(TokenType::CONSTANT, number));
				}
				else {
					// TODO: Throwing the correct exception (unexpected_char) generates a segfault
					// So we throw a different one
					throw Parser::unexpected_token(0);
				}
			}
		}
	}
}