#include "base.hpp"

#include "../errors.hpp"

bool BaseLib::CheckTypeExistance(const std::string& type) {
    // TODO
    return true;
}

std::optional<BaseLib::NativeDesc> BaseLib::GetNative(const std::string& name) {
    if (name == "print") {
        return std::optional<BaseLib::NativeDesc>({{"#include <iostream>\n"},
                                      {"void print (std::tuple<std::string>& args) {"
                                       "    std::cout << std::get<0>(args) << '\n';"
                                       "}\n\n",
                                       std::make_unique<Arrow>(new Pod("String"), new Pod("Void")),
                                       1}});
    }
    return std::nullopt;
}
