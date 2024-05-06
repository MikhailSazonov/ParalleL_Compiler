#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "type.hpp"

enum class ExpressionType {
    VAR,
    APP,
    CONST
};

struct Expression {
    const ExpressionType type;

    virtual ~Expression() = default;

    Expression(ExpressionType type) : type(type) {}
};

struct VarExpression : public Expression {
    std::string name;

    VarExpression(std::string name) : Expression(ExpressionType::VAR), name(std::move(name)) {}
};

struct ConstExpression : public Expression {
    std::string value;
    std::shared_ptr<Type> correspondingType;

    ConstExpression(std::string value, const std::string& type)
        :
      Expression(ExpressionType::CONST)
    , value(std::move(value))
    , correspondingType(new Pod(type))
        {}
};

ConstExpression* CreateConstant(const std::string_view);

bool IsConstant(const std::string_view);

struct AppExpression : Expression {
    std::unique_ptr<Expression> fun;
    std::unique_ptr<Expression> arg;

    AppExpression() : Expression(ExpressionType::APP) {}

    AppExpression(Expression* fun, Expression* arg)
        :
          Expression(ExpressionType::APP)
        , fun(fun)
        , arg(arg)
    {}
};