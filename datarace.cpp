#include "datarace.hpp"
#include "expression.hpp"
#include "errors.hpp"
#include "base_lib/base.hpp"

#include <unordered_set>
#include <unordered_map>
#include <string>

void DataRace::DataRaceCheck(const Def::FuncTable& funcTable, const Def::TypeTable& typeTable,
const Def::ClassTable& classTable) {
    CheckFunc(Def::MAIN_NAME, funcTable.at(Def::MAIN_NAME), funcTable,
              typeTable, classTable, {});
}

BaseLib::Resources DataRace::CheckFunc(const std::string& name, const std::vector<FuncDef>& fds,
const Def::FuncTable& ft, const Def::TypeTable& tt, const Def::ClassTable& clt,
const std::deque<std::shared_ptr<Expression>>& args) {
    BaseLib::Resources res;
    for (const auto& def : fds) {
        res.merge(DataRace::CheckAnnExpr(*def.cond, ft, tt, clt, name, def.racing, args));
        res.merge(DataRace::CheckAnnExpr(*def.annExpr, ft, tt, clt, name, def.racing, args));
    }
    return res;
}

BaseLib::Resources DataRace::CheckAnnExpr(const AnnotatedExpression& annExpr,
const Def::FuncTable& ft, const Def::TypeTable& typet, const Def::ClassTable& clt,
const std::string& name, bool racing, const std::deque<std::shared_ptr<Expression>>& args) {
    BaseLib::Resources res;
    for (const auto& [_, exprs] : annExpr) {
        BaseLib::Resources stageRes;
        for (const auto& expr : exprs) {
            auto newRes = CheckExpr(expr.get(), ft, typet, clt, args);
            CompareResources(stageRes, newRes, racing, name);
            stageRes.merge(newRes);
        }
        res.merge(stageRes);
    }
    return res;
}

BaseLib::Resources DataRace::CheckExpr(const Expression* expr,
const Def::FuncTable& funcTable, const Def::TypeTable& typeTable,
const Def::ClassTable& classTable, const std::deque<std::shared_ptr<Expression>>& args) {
    std::vector<std::string> names;
    std::vector<std::deque<std::shared_ptr<Expression>>> newArgs;

    for (;;) {
        if (expr->type == ExpressionType::VAR) {

        }
        if (expr->type == ExpressionType::APP) {
//            if (name.empty()) {
//                name = GetFunName(expr);
//            }
        }
    }
}

void DataRace::FillExpr(const Expression* expr,
const Def::FuncTable& funcTable, const Def::TypeTable& typeTable,
const Def::ClassTable& classTable, std::string& name,
std::deque<std::string>& args) {
    if (expr->type == ExpressionType::VAR) {
        const auto* varExpr = (const VarExpression*)expr;
        if (name.empty()) {
            name = varExpr->name;
        } else {
            args.push_back(varExpr->name);
        }
        return;
    }
    const auto* appExpr = (const AppExpression*)expr;
    FillExpr(appExpr->fun.get(), funcTable, typeTable, classTable, name, args);
    FillExpr(appExpr->arg.get(), funcTable, typeTable, classTable, name, args);
}

BaseLib::Resources DataRace::ReplaceResources(const BaseLib::Resources& baseResources,
const std::deque<std::string>& args) {
    BaseLib::Resources result;
    for (const auto& [arg, res] : baseResources) {
        if (arg.find(Def::MANGLING_SYMBOL) != std::string::npos) {
            auto argCopy = arg;
            argCopy.pop_back();
            result.emplace(args[std::stoi(argCopy)], res);
        } else {
            result.emplace(arg, res);
        }
    }
    return result;
}

BaseLib::Resources DataRace::CompareResources(const BaseLib::Resources& left, const BaseLib::Resources& right,
bool racing, const std::string& funName) {
    if (!racing && !(left.empty() && right.empty())) {
        throw DataRaceButNotRacing(funName);
    }
    for (const auto& [srcLeft, opTypeLeft] : left) {
        auto rightResRead = right.find({srcLeft, BaseLib::RedOpType::READ});
        auto rightResWrite = right.find({srcLeft, BaseLib::RedOpType::WRITE});
        if (rightResRead != right.end() && opTypeLeft == BaseLib::RedOpType::WRITE ||
        rightResWrite != right.end()) {
            throw CertainDataRace(funName);
        }
    }
}

std::string DataRace::GetFunName(const Expression* expr) {
    if (expr->type == ExpressionType::APP) {
        auto* appExpr = (const AppExpression*)expr;
//        return GetFunName(appExpr->fun.get());
    }
    auto* varExpr = (const VarExpression*)expr;
    return varExpr->name;
}


void DataRace::CheckForColors(const Def::ColorTable& colorTable, const AnnotatedExpression& annExpr) {
    for (const auto& [_, exprs] : annExpr) {
        Color color = Color::BLUE;
        for (const auto& expr : exprs) {
            if (expr->color == Color::RED) {
                if (color == Color::RED) {
                    throw ColorDataRace{};
                } else {
                    color = Color::RED;
                }
            }
        }
    }
}
