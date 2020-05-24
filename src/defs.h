#pragma once
#include <cstdint>

namespace Bonfire {
	enum class Type {
		VOID,
		BOOLEAN,
		INT8,
		INT16,
		INT32,
		INT64,
		UINT8,
		UINT16,
		UINT32,
		UINT64,
		FLOAT,
		DOUBLE
	};

	enum class ExpressionType {
		VAR_DECLARE,
		VAR_ASSIGN,
		VAR_VALUE,
		FUNCTION_CALL,
		RETURN
	};

	// Represents a variable definition, including its runtime position on the stack
	struct VariableDef {
		const char* identifier;
		Type type;
		uint64_t stack_offset;
	};

	// Represents a function definition
	struct FunctionDef {
		const char* identifier;
		uint8_t num_args = 0;
		Type* arg_types;
		Type ret_type;
		const char* contents; // Code contents in assembly
	};

	enum class AstType {
		NONE,
		PROGRAM,
		BLOCK,
		IF,
		IF_BODY,
		IF_ELSE_BODY,
		RETURN,
		CONSTANT,
		FUNCTION,
		FUNCTION_CALL,
		VAR_ASSIGNMENT,
		VAR_DECLARATION,
		VAR_DECL_INIT,
		VAR_VALUE
	};

	struct AbstractSyntaxTree {
		AstType type = AstType::NONE;

		AbstractSyntaxTree() {}

		AbstractSyntaxTree(AstType type) {
			this->type = type;
		}

		~AbstractSyntaxTree() {
			// TODO
		}
	};

	struct ExpressionST : public AbstractSyntaxTree {
		Type return_type = Type::VOID;

		ExpressionST() {}

		ExpressionST(AstType type, Type return_type) {
			this->type = type;
			this->return_type = return_type;
		}
	};

	struct BlockST : public ExpressionST {
		uint32_t num_children = 0;
		ExpressionST** children = NULL;

		BlockST() {}

		BlockST(Type return_type, ExpressionST* children[], uint32_t num_children) {
			this->return_type = return_type;
			this->type = AstType::BLOCK;
			this->num_children = num_children;
			this->children = children;
		}
	};

	struct IfST : public ExpressionST {
		bool has_else = false;
		ExpressionST* then_body;
		ExpressionST* else_body;

		IfST(ExpressionST* then_body, Type return_type) {
			this->return_type = return_type;
			type = AstType::IF;
			has_else = false;
			this->then_body = then_body;
		}

		IfST(ExpressionST* then_body, ExpressionST* else_body, Type return_type) {
			this->return_type = return_type;
			type = AstType::IF;
			has_else = true;
			this->then_body = then_body;
			this->else_body = else_body;
		}
	};
	
	struct FunctionCallST : public ExpressionST {
		// TODO: Implement Function calls
	};

	struct VariableValST : public ExpressionST {
		std::string identifier = "";

		VariableValST(std::string identifier, Type var_type) {
			this->identifier = identifier;
			this->return_type = var_type;
			this->type = AstType::VAR_VALUE;
		}
	};

	struct VariableDeclarationST : public ExpressionST {
		std::string identifier = "";
		Type var_type;
		ExpressionST* value;

		VariableDeclarationST(std::string identifier, Type var_type, ExpressionST* value) {
			this->identifier = identifier;
			this->var_type = var_type;
			this->value = value;
			type = AstType::VAR_DECLARATION;
		}
	};

	struct ConstantST : public ExpressionST {
		std::string constant;

		ConstantST() {
			return_type = Type::VOID;
			constant = nullptr;
			type = AstType::CONSTANT;
		}

		ConstantST(Type const_type, std::string constant) {
			this->return_type = const_type;
			this->constant = constant;
			type = AstType::CONSTANT;
		}
		std::string get_value() const {
			return constant;
		}
	};

	struct FunctionDefST : public AbstractSyntaxTree {
		std::string name = "";
		BlockST* statement;

		FunctionDefST() {}

		FunctionDefST(std::string name, BlockST* statement) {
			this->name = name;
			this->statement = statement;
		}
	};

	struct ProgramST : public AbstractSyntaxTree {
		FunctionDefST* main;

		ProgramST(FunctionDefST* main) {
			this->main = main;
		}
	};

	struct ReturnST : public ExpressionST {
		ExpressionST* expression;

		ReturnST(ExpressionST* expression) {
			this->return_type = Type::VOID;
			this->type = AstType::RETURN;
			this->expression = expression;
		}
	};
}