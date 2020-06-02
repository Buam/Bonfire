#pragma once
namespace Bonfire {
	enum struct TokenType {
		FAIL,
		IDENTIFIER,		// Function names, variable names and constants
		PAR_OPEN,		// (
		PAR_CLOSE,		// )
		BRACE_OPEN,		// {
		BRACE_CLOSE,	// }
		RETURN,			// <-
		RETURN_TYPE,	// ->
		CONSTANT,		// Hard-Coded Constants like 2 or '\n'
		COLON,			// : (type) or else
		EQUALS,			// =
		EQUALS2,		// ==
		NEQUALS,		// !=
		IF,				// ?
		// Arithmetic operations
		PLUS,			// +
		MINUS,			// -
		MUL,			// *
		SLASH,			// /
		MODULO,			// %
		POW,			// ^
		// Boolean operations
		OR,				// |
		ORL,			// || (lazy or)
		AND,			// &
		ANDL			// && (lazy and)
	};

	struct Token
	{
		TokenType type;
		std::string value;

		Token(TokenType type, std::string value) {
			this->type = type;
			this->value = value;
		}

		const char* to_string() const {
			switch (type) {
			case TokenType::IDENTIFIER:
				return value.c_str();
			case TokenType::PAR_OPEN:
				return "(";
			case TokenType::PAR_CLOSE:
				return ")";
			case TokenType::BRACE_OPEN:
				return "{";
			case TokenType::BRACE_CLOSE:
				return "}";
			case TokenType::RETURN:
				return "<-";
			case TokenType::RETURN_TYPE:
				return "->";
			case TokenType::IF:
				return "?";
			case TokenType::CONSTANT:
				return value.c_str();
			default:
				return "Unknown token";
			}
		}

		static Token fail() {
			return Token(TokenType::FAIL, "");
		};
	};
}