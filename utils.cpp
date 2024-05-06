#include "utils.hpp"

#include <cctype>

#include <algorithm>

#include "definitions.hpp"

void SyntaxError() {

}

//std::vector<std::string> Split(const std::string& src, const std::string& delim) {
//    size_t first_idx = 0;
//    size_t last_idx = 0;
//    std::vector<std::string> result;
//    while (last_idx != src.size()) {
//        first_idx = last_idx;
//        last_idx = src.find(last_idx, delim);
//        result.push_back(src.substr(first_idx, last_idx));
//    }
//    return result;
//}


//std::vector<std::string> Tokenize(const std::string& src, const std::string& delim) {
//    std::vector<std::string> result;
//    size_t left_idx = 0;
//    size_t right_idx = 0;
//    for (size_t idx = 0; idx < src.size();) {
//        auto brackets_idcs = FindBrackets(src, idx);
//        std::string bracketed;
//        if (brackets_idcs) {
//            std::string bracketed = src.substr(brackets_idcs->first + 1, brackets_idcs->second - 1);
//            Strip(bracketed);
//            left_idx = idx;
//            right_idx = brackets_idcs->first - 1;
//            idx = brackets_idcs->second + 1;
//        }
//        std::vector<std::string> splittedBetween = Split(src.substr(left_idx, right_idx), delim);
//        for (auto& token : splittedBetween) {
//            Strip(token);
//            if (!token.empty()) {
//                result.push_back(token);
//            }
//        }
//        if (!bracketed.empty()) {
//            result.push_back(token);
//        }
//    }
//}


//std::optional<std::pair<size_t, size_t>> FindBrackets(const std::string& src, size_t startPos) {
//    size_t left_bracket = src.find(startPos, '(');
//    if (left_bracket == std::string::npos) {
//        return std::nullopt;
//    }
//    size_t balance = 1;
//    for (size_t right_bracket = left_bracket + 1; i < src.size(); ++i) {
//        if (src[right_bracket] == '(') {
//            balance += 1;
//        } else if (src[right_bracket] == ')') {
//            if (balance == 0) {
//                SyntaxError();
//            }
//            balance -= 1;
//        }
//        if (balance == 0) {
//            return std::make_pair(left_bracket, right_bracket);
//        }
//    }
//    SyntaxError();
//    return std::nullopt;
//}


//void LeftStrip(std::string&, char symb = ' ') {
//    while (!src.empty() && src.front() == symb) {
//        src.erase(src.begin());
//    }
//}
//
//void RightStrip(std::string&, char symb = ' ') {
//    while (!src.empty() && src.back() == symb) {
//        src.pop_back();
//    }
//}


//void Strip(std::string& src, char symb) {
//    LeftStrip(src, symb);
//    RightStrip(src, symb);
//}


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
    return IsPositiveNum(src) || src.size() > 1 &&
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
    return !src.empty() && IsName({&src[0], src.size() - 1}) && src.back() == Def::MANGLING_SYMBOL;
}
