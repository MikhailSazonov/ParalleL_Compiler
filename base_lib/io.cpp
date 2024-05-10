#include "io.hpp"

void BaseLib::Io::LoadBaseTypes(Def::TypeTable& typeTable) {
    typeTable["print"] = std::make_shared<Arrow>(new Pod("String"), new Pod("Void"));
}

std::optional<BaseLib::NativeDesc> BaseLib::Io::GetNative(const std::string& name) {
    if (name == "print") {
        return std::optional<BaseLib::NativeDesc>({{"#include <iostream>\n"},
                                                   {"void print (std::tuple<String>& args) {\n"
                                                    "    std::cout << std::get<0>(args) << '\\n';\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("String"), new Pod("Void")),
                                                           1}});
    }
    return std::nullopt;
}
