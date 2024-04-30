#pragma once

#include <string>
#include <vector>
#include <memory>

enum class ExpressionType {
	VAR,
	APP
}

struct Expression {
	const ExpressionType type;
};

struct VarExpression {
	std::string name;

	VarExpression() : type(ExpressionType.VAR) {}
};

struct AppExpression {
	std::string fun;
	std::unique_ptr<Expression> arg;

	AppExpression() : type(ExpressionType.APP) {}
};
