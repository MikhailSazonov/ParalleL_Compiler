#pragma once

#include <string>
#include <string_view>

#include "../TypeSystem/expression.hpp"
#include "../TypeSystem/type.hpp"

const std::string TYPE_DELIMETER = "->"
const std::string EXPR_DELIMETER = " "

void SyntaxError();

Expression BuildExpression(const std::string&);

std::unique_ptr<Type> BuildType(const std::string_view);

std::pair<size_t, size_t> GetNextTokenPos(const std::string_view);

bool IsName(const std::string_view);
