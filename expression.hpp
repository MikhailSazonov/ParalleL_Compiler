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
    LITERAL,
    PACKED
};

enum class Color {
    BLUE,
    RED
};

struct Expression {
    const ExpressionType type;
    Color color{Color::BLUE};

    virtual ~Expression() = default;

    Expression(ExpressionType type) : type(type) {}

    Expression(ExpressionType type, Color color) : type(type), color(color) {}
};

struct VarExpression : public Expression {
    std::string name;

    VarExpression(std::string name) : Expression(ExpressionType::VAR), name(std::move(name)) {}

    VarExpression(std::string name, Color color) : Expression(ExpressionType::VAR, color), name(std::move(name)) {}
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

    AppExpression(Expression* fun, Expression* arg, Color color)
            :
            Expression(ExpressionType::APP, color)
            , fun(fun)
            , arg(arg)
    {}
};

typedef std::map<size_t, std::vector<std::unique_ptr<Expression>>> AnnotatedExpression;

struct PackedExpression : Expression {
    std::vector<std::shared_ptr<AnnotatedExpression>> exprs;
    std::shared_ptr<Type> correspondingType;

    PackedExpression() : Expression(ExpressionType::PACKED) {}
};

ConstExpression* CreateConstant(const std::string_view);

bool IsConstant(const std::string_view);

bool IsAnnotation(const std::string_view);
