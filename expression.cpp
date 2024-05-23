#include "expression.hpp"
#include "base_lib/base.hpp"
#include "utils.hpp"
#include "errors.hpp"

ConstExpression* CreateConstant(const std::string_view view) {
    if (IsBool(view)) {
        return new ConstExpression(std::string(view), BaseLib::BOOL_TYPE.first);
    }
    if (IsNum(view)) {
        return new ConstExpression(std::string(view), BaseLib::INT_TYPE.first);
    }
    if (IsChar(view)) {
        return new ConstExpression(std::string(view), BaseLib::CHAR_TYPE.first);
    }
    if (IsFloat(view)) {
        return new ConstExpression(std::string(view), BaseLib::FLOAT_TYPE.first);
    }
    if (IsString(view)) {
        return new ConstExpression(std::string(view), BaseLib::STRING_TYPE.first);
    }
    if (IsVoid(view)) {
        return new ConstExpression(std::string(view), BaseLib::VOID_TYPE.first);
    }
    if (IsNull(view)) {
        return new ConstExpression(std::string(view), BaseLib::NULL_TYPE);
    }
    throw UnexpectedSymbol{};
}

bool IsConstant(const std::string_view view) {
    return IsBool(view) || IsNum(view) || IsChar(view) || IsFloat(view) ||
        IsString(view) || IsNull(view) || IsVoid(view);
}

bool IsAnnotation(const std::string_view view) {
    return IsPositiveNum({&view[0], view.size() - 1}) && view.back() == '!';
}

bool IsAbstract(const std::string& src) {
    return std::all_of(src.begin(), src.end(), [](char c) {
        return 'a' <= c && c <= 'z';
    });
}
