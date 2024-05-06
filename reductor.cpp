#include "reductor.hpp"

#include "utils.hpp"

#include <unordered_set>

//Def::FuncTable Reduct(const Def::FuncTable& funcTable) {
//    std::unordered_set<std::string> usedFunctions;
//
//    for (const auto& fun : funcTable) {
//        for (const auto& def : fun.second) {
//            // TODO: conditions (pattern matching)
//            usedFunctions.merge(GetUsedVars(def.expr.get()));
//        }
//    }
//
//    Def::FuncTable result;
//
//    for (const auto& fun : funcTable) {
//        if (usedFunctions.contains(fun.first)) {
//            result.insert(fun);
//        }
//    }
//    return result;
//}
//
//std::unordered_set<std::string> GetUsedVars(Expression* expr) {
//    std::unordered_set<std::string> result;
//    if (expr->type == ExpressionType::VAR) {
//        auto* varExpr = dynamic_cast<VarExpression*>(expr);
//        if (!IsMangledName(varExpr->name)) {
//            result.insert(varExpr->name);
//        }
//        return result;
//    } else if (expr->type == ExpressionType::APP) {
//        auto* appExpr = dynamic_cast<AppExpression*>(expr);
//        result = GetUsedVars(appExpr->fun.get());
//        result.merge(GetUsedVars(appExpr->arg.get()));
//        return result;
//    }
//    return result;
//}
