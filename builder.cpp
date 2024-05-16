#include "builder.hpp"

#include "utils.hpp"
#include "errors.hpp"
#include "base_lib/base.hpp"
#include "base_lib/operators.hpp"
#include "definitions.hpp"

#include <cctype>

std::unique_ptr<AnnotatedExpression> BuildExpression(const std::string_view view,
const std::unordered_map<std::string, size_t>& varPos) {
    std::unique_ptr<AnnotatedExpression> expr(new AnnotatedExpression());
    (*expr)[(size_t)-1].push_back(BuildExpressionImpl(view, varPos, *expr));
    return expr;
}

std::unique_ptr<Expression> BuildExpressionImpl(const std::string_view view,
const std::unordered_map<std::string, size_t>& varPos,
AnnotatedExpression& expr) {
    std::string_view stripped = RemoveBrackets(Strip(view));

    if (IsConstant(stripped)) {
        return std::unique_ptr<Expression>(CreateConstant(view));
    }

    if (IsName(stripped)) {
        std::string varName(stripped.data(), stripped.size());
        if (varPos.contains(varName)) {
            return std::make_unique<VarExpression>(std::to_string(varPos.at(varName)) + Def::MANGLING_SYMBOL);
        }
        return std::make_unique<VarExpression>(varName);
    }

    auto next_token_pos = GetLastTokenPos(stripped);
    std::string_view next_token(&stripped[next_token_pos.first], next_token_pos.second);

    auto rest = std::string_view(&stripped[0],
                                 next_token_pos.first - 1);

    // TODO: arithmetic operations order
    std::string op;
    if (!(op = BaseLib::Ops::GetOperator(next_token)).empty()) {
        return std::make_unique<AppExpression>(BuildExpressionImpl({&op[0]}, varPos, expr).release(),
                                               BuildExpressionImpl(rest, varPos, expr).release());
    }

    auto first_token_pos = GetNextTokenPos(stripped);
    std::string_view first_token(&stripped[first_token_pos.first], first_token_pos.second);

    if (IsAnnotation(first_token)) {
        std::string_view annotNum{&first_token[0], first_token.size() - 1};
        size_t annotNumInt = std::stoi(std::string(annotNum));
        size_t annotatedArgNum = expr[annotNumInt].size();
        expr[annotNumInt].push_back(nullptr);
        expr[annotNumInt][annotatedArgNum] = BuildExpressionImpl({&stripped[first_token_pos.second],
                                   stripped.size() - (first_token_pos.second)}, varPos, expr);
        return std::make_unique<VarExpression>(std::string(annotNum) + Def::ANNOT_DELIM_SYMBOL +
                            std::to_string(annotatedArgNum));
    }

    return std::make_unique<AppExpression>(BuildExpressionImpl(rest, varPos, expr).release(),
                                           BuildExpressionImpl(next_token, varPos, expr).release());
}


std::unique_ptr<Type> BuildType(const std::string_view view) {
    std::string_view stripped = RemoveBrackets(Strip(view));

    if (IsName(stripped)) {
        std::string name(stripped.data(), stripped.size());
        BaseLib::CheckTypeExistance(name);
        return std::make_unique<Pod>(name);
    }

    auto next_token_pos = GetNextTokenPos(stripped);
    std::string_view next_token(&stripped[next_token_pos.first], next_token_pos.second);

    size_t delimeter_pos = stripped.find(Def::TYPE_DELIMETER, next_token_pos.first + next_token_pos.second + 1);

    if (delimeter_pos == std::string_view::npos) {
        return BuildType(next_token);
    }

    size_t pos = delimeter_pos + Def::TYPE_DELIMETER.size();
    auto rest = std::string_view(&stripped[pos],
                               stripped.size() - pos);
    return std::make_unique<Arrow>(BuildType(next_token).release(),
                                   BuildType(rest).release());
}
