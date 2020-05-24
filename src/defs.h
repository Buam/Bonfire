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

	// Gets the type size in bytes
	uint32_t get_type_size(Type type) {
		switch (type) {
		case Type::VOID:	return 0;
			// One byte types (byte, char)
		case Type::BOOLEAN:
		case Type::INT8:
		case Type::UINT8:	return 1;
			// Two byte types (short)
		case Type::INT16:
		case Type::UINT16:	return 2;
			// Four byte types (int, float)
		case Type::INT32:
		case Type::UINT32:
		case Type::FLOAT:	return 4;
			// Eight byte types (long, double)
		case Type::INT64:
		case Type::UINT64:
		case Type::DOUBLE:	return 8;
		}
	}

	bool is_integer_type(Type type) {
		return type == Type::BOOLEAN || type == Type::UINT8 || type == Type::UINT16 || type == Type::UINT32
			|| type == Type::UINT64 || type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64;
	}

	bool is_unsigned_integer_type(Type type) {
		return type == Type::UINT8 || type == Type::UINT16 || type == Type::UINT32 || type == Type::UINT64;
	}

	Type get_type_for_op(Type lhs, Type rhs) {
		bool can_be_unsigned = is_unsigned_integer_type(lhs) && is_unsigned_integer_type(rhs);
		Type biggest;
		if (can_be_unsigned && (lhs == Type::UINT64 || rhs == Type::UINT64)) return Type::UINT64;
		if (!can_be_unsigned && (lhs == Type::UINT64 || rhs == Type::UINT64 || lhs == Type::INT64 || rhs == Type::INT64)) return Type::INT64;
		if (can_be_unsigned && (lhs == Type::UINT32 || rhs == Type::UINT32)) return Type::UINT32;
		if (!can_be_unsigned && (lhs == Type::UINT32 || rhs == Type::UINT32 || lhs == Type::INT32 || rhs == Type::INT32)) return Type::INT32;
		if (can_be_unsigned && (lhs == Type::UINT16 || rhs == Type::UINT16)) return Type::UINT16;
		if (!can_be_unsigned && (lhs == Type::UINT16 || rhs == Type::UINT16 || lhs == Type::INT16 || rhs == Type::INT16)) return Type::INT32;
		if (can_be_unsigned && (lhs == Type::UINT8 || rhs == Type::UINT8)) return Type::UINT8;
		if (!can_be_unsigned && (lhs == Type::UINT8 || rhs == Type::UINT8 || lhs == Type::INT8 || rhs == Type::INT8)) return Type::INT8;
	}

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
		VAR_VALUE,
		ARITHMETIC_OP
	};

	enum class Operation {
		NONE,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		POW,
		EQ,
		LT,
		LTE,
		GT,
		GTE,
		ANDL,
		AND,
		ORL,
		OR
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
		ExpressionST* condition;
		ExpressionST* then_body;
		ExpressionST* else_body;

		IfST(ExpressionST* condition, ExpressionST* then_body, Type return_type) {
			this->condition = condition;
			this->return_type = return_type;
			type = AstType::IF;
			has_else = false;
			this->then_body = then_body;
		}

		IfST(ExpressionST* condition, ExpressionST* then_body, ExpressionST* else_body, Type return_type) {
			this->condition = condition;
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

	struct OperationST : public ExpressionST {
		ExpressionST* lhs;
		ExpressionST* rhs;
		Operation op;

		OperationST() {}
		
		OperationST(Operation op, ExpressionST* lhs, ExpressionST* rhs) {
			this->op = op;
			this->lhs = lhs;
			this->rhs = rhs;
			type = AstType::ARITHMETIC_OP;
			switch (lhs->return_type) {
			case Type::INT8: case Type::INT16: case Type::INT32:
			case Type::UINT8: case Type::UINT16: case Type::UINT32:
				
				return_type = Type::INT32;
			}
		}
	};

	struct FunctionDefST : public AbstractSyntaxTree {
		std::string name = "";
		BlockST* statement = NULL;

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