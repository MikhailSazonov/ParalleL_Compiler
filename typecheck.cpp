#include "typecheck.hpp"

#include "errors.hpp"
#include "builder.hpp"
#include "definitions.hpp"

std::shared_ptr<Type> GetTermType(const Def::TypeTable& typesTable, const Expression& expr) {
    if (expr.type == ExpressionType::CONST) {
        // TODO: proper type
        const auto& constExp = (const ConstExpression&)expr;
        return constExp.correspondingType;
    }
    if (expr.type == ExpressionType::VAR) {
        // TODO: proper type
        const auto& varExp = (const VarExpression&)expr;
        if (!typesTable.contains(varExp.name)) {
            throw UndefinedVariableError{};
        }
        return typesTable.at(varExp.name);
    }
    const auto& appExp = (const AppExpression&)expr;
    auto leftType = GetTermType(typesTable, *appExp.fun);
    auto rightType = GetTermType(typesTable, *appExp.arg);
    const auto* fun = dynamic_cast<const Arrow*>(&*leftType);
    if (fun == nullptr) {
        throw IsNotFuncError{};
    }
    if (*fun->left != *rightType) {
        throw TypeMismatchError{};
    }
    return fun->right;
}

void CheckType(Def::TypeTable& typesTable, const std::string& varName,
               size_t argsN, Type* varType, const Expression& expr) {
    // TODO: type existence check
    if (argsN == 0 && varType->type == TermType::POD) {
        typesTable[varName] = std::make_shared<Pod>(dynamic_cast<const Pod*>(varType)->typeName);
        return;
    }
    for (size_t currentArgN = 0; currentArgN < argsN; ++currentArgN) {
        if (varType->type == TermType::POD) {
            throw TooManyArgsError{};
        }
        auto fun = dynamic_cast<const Arrow*>(varType);
        typesTable[std::to_string(currentArgN) + Def::MANGLING_SYMBOL] = fun->left;
        varType = fun->right.get();
    }
    if (*GetTermType(typesTable, expr) != *varType) {
        throw TypeMismatchError{};
    }
    for (size_t currentArgN = 0; currentArgN < argsN; ++currentArgN) {
        typesTable.erase(std::to_string(currentArgN));
    }
}
