#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "expression.hpp"
#include "type.hpp"

void SyntaxError();

std::unique_ptr<Expression> BuildExpression(const std::string_view, const std::unordered_map<std::string, size_t>&);

std::unique_ptr<Type> BuildType(const std::string_view);

//std::pair<size_t, size_t> GetNextTokenPos(const std::string_view);
//
//std::pair<size_t, size_t> GetLastTokenPos(const std::string_view);
