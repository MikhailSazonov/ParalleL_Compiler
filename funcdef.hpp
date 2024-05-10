#pragma once

#include "expression.hpp"

struct FuncDef {
    std::unique_ptr<Expression> cond;
    std::unique_ptr<Expression> expr;
    size_t argNo;
};
