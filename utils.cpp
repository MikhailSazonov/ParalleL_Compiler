#include "utils.hpp"

#include <cctype>
#include <algorithm>
#include <unordered_set>

#include "definitions.hpp"
#include "errors.hpp"

void SyntaxError() {

}

std::string_view Strip(const std::string_view src) {
    if (src.empty()) {
        return src;
    }
    size_t left_pos = 0;
    size_t right_pos = src.size();
    while (left_pos < src.size() && std::isspace(src[left_pos])) {
        ++left_pos;
    }
    while (right_pos > 0 && std::isspace(src[right_pos - 1])) {
        --right_pos;
    }
    return {&src[left_pos], right_pos - left_pos};
}


std::string_view LeftStrip(const std::string_view src, char symb) {
    if (src.empty()) {
        return src;
    }
    size_t left_pos = 0;
    while (left_pos < src.size() && src[left_pos] == symb) {
        ++left_pos;
    }
    return {&src[left_pos], src.size() - left_pos};
}


std::string_view RightStrip(const std::string_view src, char symb) {
    if (src.empty()) {
        return src;
    }
    size_t right_pos = src.size();
    while (right_pos > 0 && src[right_pos - 1] == symb) {
        --right_pos;
    }
    return {&src[0], right_pos};
}


std::string_view RemoveBrackets(const std::string_view src) {
    if (src.empty()) {
        return src;
    }
    size_t idx = 0;
    for (; idx < src.size() && src[idx] == '('; ++idx) {}
    size_t balance = idx;
    size_t min_balance = balance;
    if (min_balance > 0) {
        for (; idx < src.size() - 1; ++idx) {
            if (src[idx] == '(') {
                ++balance;
            } else if (src[idx] == ')') {
                --balance;
            }
            min_balance = std::min(balance, min_balance);
        }
    }
    return {&src[min_balance], src.size() - 2 * min_balance};
}

bool IsName(const std::string_view src) {
    return !src.empty() && std::all_of(src.begin(), src.end(), [](char c){ return std::isalpha(c); });
}

bool IsPositiveNum(const std::string_view src, bool allowZero) {
    return !src.empty() && (allowZero || src[0] != '0') &&
    std::all_of(src.begin(), src.end(), [](char c){ return std::isdigit(c); });
}

bool IsNum(const std::string_view src) {
    return src == "0" || IsPositiveNum(src) || src.size() > 1 &&
        src[0] == '-' && IsPositiveNum({&src[1], src.size() - 1});
}


bool IsBool(const std::string_view src) {
    return src == "true" || src == "false";
}


bool IsChar(const std::string_view src) {
    return src.size() == 3 && src[0] == '\'' && src[2] == '\'';
}


bool IsFloat(const std::string_view src) {
    size_t dotPos;
    return (dotPos = src.find('.')) != std::string_view::npos && IsNum({&src[0], dotPos}) &&
        IsPositiveNum({&src[dotPos + 1]}, true);
}


bool IsString(const std::string_view src) {
    return src.size() >= 2 && src.front() == '\"' && src.back() == '\"';
}


bool IsMangledName(const std::string_view src) {
    return !src.empty() && IsPositiveNum({&src[0], src.size() - 1}, true) && src.back() == Def::MANGLING_SYMBOL;
}


bool IsOperator(const std::string_view c) {
    static const std::unordered_set<std::string> ops = {"+", "-", "*", "/", "%", "<", ">",
                                                        "<=", ">="};
    return ops.contains(std::string(c));
}


std::pair<size_t, size_t> GetNextTokenPos(const std::string_view view) {
    if (view[0] == '(') {
        size_t i = 1;
        size_t balance = i;
        for (; i < view.size() && balance > 0; ++i) {
            if (view[i] == '(') {
                balance++;
            } else if (view[i] == ')') {
                balance--;
            }
        }
        if (i == view.size()) {
            SyntaxError();
        }
        i--;
        return std::make_pair(1, i - 1);
    }
    return std::make_pair(0, std::min(view.find(' '), view.size()));
}


std::pair<size_t, size_t> GetLastTokenPos(const std::string_view view) {
    if (view.back() == ')') {
        size_t i = 1;
        size_t balance = i;
        for (; i < view.size() && balance > 0; ++i) {
            if (view[view.size() - i - 1] == '(') {
                balance--;
            } else if (view[view.size() - i - 1] == ')') {
                balance++;
            }
        }
        if (i == view.size()) {
            SyntaxError();
        }
        return std::make_pair(view.size() - i + 1, i - 2);
    }
    if (view.back() == '\"') {
        auto next_quote = view.rfind('\"', view.size() - 2);
        if (next_quote == std::string_view::npos) {
            throw UnexpectedSymbol{};
        }
        return std::make_pair(next_quote, view.size() - next_quote);
    }
    auto delimeter_pos = view.rfind(' ');
    return std::make_pair(delimeter_pos == std::string_view::npos ? 0 : delimeter_pos + 1,
                          view.size() - (delimeter_pos + 1));
}
