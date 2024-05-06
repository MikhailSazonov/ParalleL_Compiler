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
    throw UnexpectedSymbol{};
}

bool IsConstant(const std::string_view view) {
    return IsBool(view) || IsNum(view) || IsChar(view) || IsFloat(view) || IsString(view);
}
