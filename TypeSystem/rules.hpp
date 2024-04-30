#pragma once

#include "types.hpp"
#include "term.hpp"

void TypeMismatchError();


Type RuleVar(const Var& var);


Type RuleApp(const Arrow& fun, const Type& arg);


Type Evaluate(const Term& term);