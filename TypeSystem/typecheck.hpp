#pragma once

#include "expression.hpp"
#include "type.hpp"
#include "expression.hpp"

#include <vector>


void VarNotFoundError() {
	// TODO
}


void TypeIsNotFuncError() {
	// TODO
}


void ValueMismatchError() {
	// TODO
}


std::shared_ptr<Type> CheckTerm(const std::unordered_map<std::string, std::shared_ptr<Type>>& typesTable, const Expression& expr) {
	if (expr.code == ExpressionType.VAR) {
		const VarExpression& varExp = (const VarExpression&)expr;
		return typesTable[varExp.name];
	}
	const AppExpression& appExp = (const AppExpression&)expr;
	CheckTerm(typesTable, appExp.fun);
	CheckTerm(typesTable, appExp.arg);
	Arrow* left = dynamic_cast<Arrow*>(&typesTable[appExp.fun]);
	const Type& right = typesTable[appExp.arg] 
	if (left == nullptr) {
		TypeIsNotFuncError();
	}
	if (left->left != right) {
		ValueMismatchError();
	}
	return left->right;
}
