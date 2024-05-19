#include "typecheck.hpp"

#include "errors.hpp"
#include "builder.hpp"
#include "definitions.hpp"
#include "utils.hpp"

std::shared_ptr<Type> GetTermType(const Def::TypeTable& typesTable, const Expression& expr,
const AnnotatedExpression& annExpr, Def::ResolveTable** resolveTable) {
    if (expr.type == ExpressionType::LITERAL) {
        const auto& constExp = (const ConstExpression&)expr;
        return constExp.correspondingType;
    }
    if (expr.type == ExpressionType::VAR) {
        auto& varExp = (VarExpression&)expr;
        if (IsAnnotatedName(varExp.name)) {
            size_t pos = varExp.name.find(Def::ANNOT_DELIM_SYMBOL);
            size_t fi = std::stoi(varExp.name.substr(0, pos));
            size_t si = std::stoi(varExp.name.substr(pos + 1));
            return GetTermType(typesTable, *annExpr.at(fi)[si], annExpr, resolveTable);
        }
        if (!typesTable.contains(varExp.name)) {
            throw UndefinedVariableError{};
        }
        return typesTable.at(varExp.name);
    }
    if (expr.type == ExpressionType::VAR_ABSTRACTED) {
        auto& absExpr = (AbstractVarExpression&)expr;
        *resolveTable = &absExpr.resolveTable;
        return typesTable.at(absExpr.name);
    }
    const auto& appExp = (const AppExpression&)expr;
    auto leftType = GetTermType(typesTable, *appExp.fun, annExpr, resolveTable);
    auto rightType = GetTermType(typesTable, *appExp.arg, annExpr, resolveTable);
    auto* fun = dynamic_cast<Arrow*>(&*leftType);
    if (fun == nullptr) {
        throw IsNotFuncError{};
    }
    // TODO: different abstracts in one call
    if (fun->left->type == TermType::ABSTRACT) {
        auto* abs = dynamic_cast<Abstract*>(fun->left.get());
        if (rightType->type == TermType::ABSTRACT) {
            if (*rightType != *fun->left) {
                throw TypeMismatchError{};
            }
//            return (**resolveTable)[abs->abstractName];
        } else {
            if ((**resolveTable).contains(abs->abstractName) &&
            *(**resolveTable)[abs->abstractName] != *rightType) {
                throw TypeMismatchError{};
            }
            (**resolveTable)[abs->abstractName] = rightType;
        }
//        if (rightType->type == TermType::ABSTRACT && *rightType != *fun->left) {
//            throw TypeMismatchError{};
//        }
//        if (rightType->type != TermType::ABSTRACT) {
//            if ((**resolveTable).contains(abs->abstractName) &&
//            *(**resolveTable)[abs->abstractName] != *rightType) {
//                throw TypeMismatchError{};
//            }
//        }
    } else if (*fun->left != *rightType) {
        throw TypeMismatchError{};
    }
    if (fun->right->type == TermType::ABSTRACT) {
        auto* abs = dynamic_cast<Abstract*>(fun->left.get());
        if (!(**resolveTable).contains(abs->abstractName)) {
            throw AbstractTypeUnresolved{};
        }
        return (**resolveTable)[abs->abstractName];
    }
    return fun->right;
}

void CheckType(Def::TypeTable& typesTable, const std::string& varName,
               size_t argsN, Type* varType, const Expression& expr, const AnnotatedExpression& annExpr) {
    if (expr.type != ExpressionType::APP && argsN == 0 && varType->type == TermType::POD) {
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
    Def::ResolveTable* tablePtr = nullptr;
    if (*GetTermType(typesTable, expr, annExpr, &tablePtr) != *varType) {
        throw TypeMismatchError{};
    }
//    auto typeTableClone = typesTable;
//    for (size_t currentArgN = 0; currentArgN < argsN; ++currentArgN) {
//        typesTable.erase(std::to_string(currentArgN));
//    }
//    for (const auto& [name, type] : typeTableClone) {
//        if (name.find('%') != std::string::npos) {
//            typesTable.erase(name);
//        }
//    }
}

void CheckAnnotatedType(Def::TypeTable& typeTable, const std::string& varName,
size_t argN, Type* varType, const AnnotatedExpression& annExpr) {
    CheckType(typeTable, varName, argN, varType, *annExpr.at((size_t)-1).back().get(), annExpr);
}
