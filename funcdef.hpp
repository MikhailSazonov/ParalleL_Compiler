#pragma once

#include "expression.hpp"

struct Condition {
    // TODO
};

struct FuncDef {
    Condition cond;
    std::unique_ptr<Expression> expr;
    size_t argNo;
};
