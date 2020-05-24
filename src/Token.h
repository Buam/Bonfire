#pragma once
namespace Bonfire {
	enum struct TokenType {
		FAIL,
		IDENTIFIER,		// Identifiers like function or variable names
		PAR_OPEN,		// (
		PAR_CLOSE,		// )
		BRACE_OPEN,		// {
		BRACE_CLOSE,	// }
		RETURN,			// <-
		RETURN_TYPE,	// ->
		CONSTANT,		// Hard-Coded Constants like 2 or '\n'
		TYPE_DEF,		// : (type) only :
		EQUALS			// =
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
			case TokenType::CONSTANT:
				return value.c_str();
			}
		}

		static Token fail() {
			return Token(TokenType::FAIL, "");
		};
	};
}