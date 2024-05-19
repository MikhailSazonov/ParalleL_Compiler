#include "datarace.hpp"
#include "expression.hpp"
#include "base_lib/base.hpp"

#include <unordered_set>
#include <unordered_map>
#include <string>

//void DataRaceCheck(const Def::FuncTable& funcTable) {
//    ResourcesContainer funcsResources;
//    for (const auto& func : funcTable) {
//        if (!funcsResources.contains(func.first)) {
//            DataRaceCheckFunc(func.first, funcTable, funcsResources);
//        }
//    }
//}
//
//void DataRaceCheckFunc(const std::string& funcName, const Def::FuncTable& funcTable, ResourcesContainer& rc) {
//    const auto& func = funcTable.at(funcName);
//}
//
//BaseLib::Resources GetResources(const std::string& funcName, const Def::FuncTable& funcTable) {
//    auto native = BaseLib::GetNative(funcName);
//    if (native.has_value()) {
//        return native->resources;
//    }
//    if (!funcTable.contains(funcName)) {
//        return {};
//    }
//    auto& func = funcTable.at(funcName);
//    BaseLib::Resources totalRes;
//    for (const auto& funcDef : func) {
//        if (funcDef.cond) {
//            for (const auto& [_, exprs] : *funcDef.cond) {
//                BaseLib::Resources res;
//                for (const auto& expr : exprs) {
//                    if (expr->type == ExpressionType::VAR) {
//                        auto* varExpr = dynamic_cast<VarExpression*>(expr.get());
//                        res.merge(GetResources(varExpr->name, funcTable));
//                    } else if (expr->type == ExpressionType::APP) {
//                        auto* appExpr = dynamic_cast<AppExpression*>(expr.get());
//                        res.merge(GetResources(appExpr->name, funcTable));
//                    }
//                }
//            }
//        }
//    }
//}