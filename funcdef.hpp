#pragma once

#include "expression.hpp"

struct FuncDef {
    std::unique_ptr<AnnotatedExpression> cond;
    std::unique_ptr<AnnotatedExpression> annExpr;
    size_t argNo;
    bool racing{false};
    Color color;
};
