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

				if (c == '(') {
					tokens_out.push_back(Token(TokenType::PAR_OPEN, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == ')') {
					tokens_out.push_back(Token(TokenType::PAR_CLOSE, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == '{') {
					tokens_out.push_back(Token(TokenType::BRACE_OPEN, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == '}') {
					tokens_out.push_back(Token(TokenType::BRACE_CLOSE, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == '-' && source[cursor + 1] == '>') {
					tokens_out.push_back(Token(TokenType::RETURN_TYPE, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				else if (c == '<' && source[cursor + 1] == '-') {
					tokens_out.push_back(Token(TokenType::RETURN, ""));
					token_indices.push_back(cursor);
					cursor += 2;
				}
				else if (c == ':') {
					tokens_out.push_back(Token(TokenType::TYPE_DEF, ""));
					token_indices.push_back(cursor);
					++cursor;
				}
				else if (c == '=') {
					tokens_out.push_back(Token(TokenType::EQUALS, ""));
					token_indices.push_back(cursor);
					++cursor;
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
					// Constant
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