#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <map>

#include "type.hpp"

enum class ExpressionType {
    VAR,
    VAR_ABSTRACTED,
    APP,
    LITERAL
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

struct AbstractVarExpression : public Expression {
    std::string name;
    std::shared_ptr<Type> type;
    Def::ResolveTable resolveTable;

    AbstractVarExpression(std::string name) : Expression(ExpressionType::VAR_ABSTRACTED),
        name(std::move(name)) {}
};

bool IsAbstract(const std::string&);

struct ConstExpression : public Expression {
    std::string value;
    std::shared_ptr<Type> correspondingType;

    ConstExpression(std::string value, const std::string& type)
        :
      Expression(ExpressionType::LITERAL)
    , value(std::move(value))
    {
        if (IsAbstract(type)) {
            correspondingType = std::make_shared<Abstract>(type);
        } else {
            correspondingType = std::make_shared<Pod>(type);
        }
    }
};

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

typedef std::map<size_t, std::vector<std::unique_ptr<Expression>>> AnnotatedExpression;

ConstExpression* CreateConstant(const std::string_view);

bool IsConstant(const std::string_view);

bool IsAnnotation(const std::string_view);
