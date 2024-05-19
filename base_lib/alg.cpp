#include "alg.hpp"

void BaseLib::Alg::LoadBaseTypes(Def::TypeTable& typeTable) {
    typeTable["Try"] = std::make_shared<Arrow>(new Abstract("a"), new Arrow(
                new Abstract("b"),
                new Pod("Void")
            ));
}

std::optional<BaseLib::NativeDesc> BaseLib::Alg::GetNative(const std::string& name) {
    if (name == "Try") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {},
                                                   {"template <typename a, typename b>\n"
                                                    "Void Try(std::tuple<a, b>& args) {\n"
                                                    "    try {\n"
                                                    "        std::get<0>(args).Eval();\n"
                                                    "    } catch (...) {\n"
                                                    "\n"
                                                    "    }\n"
                                                    "    return Void{};\n"
                                                    "}\n",
                                                           std::make_unique<Arrow>(new Abstract("a"), new Arrow(
                                                                   new Abstract("b"),
                                                                   new Pod("Void")
                                                           )),
                                                           2}});
    };
    return std::nullopt;
}
