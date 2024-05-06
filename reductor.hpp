#pragma once

#include "definitions.hpp"
#include "expression.hpp"

#include <unordered_set>


Def::FuncTable Reduct(const Def::FuncTable&);

std::unordered_set<std::string> GetUsedVars(Expression*);