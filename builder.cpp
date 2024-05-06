#include "builder.hpp"

#include "utils.hpp"
#include "errors.hpp"
#include "base_lib/base.hpp"
#include "definitions.hpp"

#include <cctype>

// TODO: allow space absence
std::unique_ptr<Expression> BuildExpression(const std::string_view view,
const std::unordered_map<std::string, size_t>& varPos) {
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

    auto next_token_pos = GetNextTokenPos(stripped);
    std::string_view next_token(&stripped[next_token_pos.first], next_token_pos.second);

    size_t delimeter_pos = stripped.find(Def::EXPR_DELIMETER, next_token_pos.first + next_token_pos.second);

    if (delimeter_pos == std::string_view::npos) {
        throw UnexpectedSymbol{};
    }

    size_t pos = delimeter_pos + Def::EXPR_DELIMETER.size();
    auto rest = std::string_view(&stripped[pos],
                                 stripped.size() - pos);
    return std::make_unique<AppExpression>(BuildExpression(next_token, varPos).release(),
                                   BuildExpression(rest, varPos).release());
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


std::pair<size_t, size_t> GetNextTokenPos(const std::string_view view) {
    if (view[0] == '(') {
        size_t i = 1;
        size_t balance = i;
        for (; i < view.size() && balance > 0; ++i) {
            if (view[i] == '(') {
                balance += 1;
            } else if (view[i] == ')') {
                balance -= 1;
            }
        }
        if (i == view.size()) {
            SyntaxError();
        }
        i -= 1;
        return std::make_pair(1, i - 1);
    }
    return std::make_pair(0, std::min(view.find(' '), view.size()));
}
