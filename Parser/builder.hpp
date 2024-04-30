#pragma once

#include <string>

#include "../TypeSystem/expression.hpp"
#include "../TypeSystem/type.hpp"

const std::string TYPE_DELIMETER = "->"
const std::string EXPR_DELIMETER = "->"

void SyntaxError();

Expression BuildExpression(const std::string&);

std::unique_ptr<Type> BuildType(const std::string&);

std::vector<std::string> ParseExpression(const std::string&);

std::vector<std::string> ParseType(const std::string&);
