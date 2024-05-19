#include "operators.hpp"

#include "../errors.hpp"

std::string BaseLib::Ops::GetOperator(const std::string_view src) {
    if (src == "+") {
        return "add";
    } else if (src == "-") {
        return "sub";
    } else if (src == "/") {
        return "div";
    } else if (src == "*") {
        return "mul";
    } else if (src == "%") {
        return "mod";
    } else if (src == "<") {
        return "ls";
    } else if (src == "<=") {
        return "lse";
    } else if (src == ">") {
        return "gt";
    } else if (src == ">=") {
        return "gte";
    } else if (src == "==") {
        return "eq";
    } else if (src == "!=") {
        return "neq";
    } else if (src == "!") {
        return "not";
    } else if (src == "~=") {
        return "isNull";
    } else if (src == ">>") {
        return "seq";
    }
    return "";
}


std::optional<BaseLib::NativeDesc> BaseLib::Ops::GetNative(const std::string& name) {
    if (name == "add") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                       {"Int add (std::tuple<Int, Int>& args) {\n"
                                        "    return {std::get<0>(args).value + std::get<1>(args).value};\n"
                                        "}\n\n",
                                       std::make_unique<Arrow>(new Pod("Int"),
                                                               new Arrow(
                                                                       new Pod("Int"),
                                                                       new Pod("Int")
                                                                       )),
                                       2}});
    }
    if (name == "sub") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Int sub (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value - std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "mul") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Int mul (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value * std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "div") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Int div (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value / std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "mod") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Int mod (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value % std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "ls") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Bool ls (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value < std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "lse") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Bool lse (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value <= std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "gt") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Bool gt (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value > std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "gte") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Bool ls (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value >= std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "eq") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"template <typename a>"
                                                    "Bool eq (std::tuple<a, a>& args) {\n"
                                                    "    return {std::get<0>(args).value == std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Abstract("a"),
                                                                                   new Arrow(
                                                                                           new Abstract("a"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "neq") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                   {"Bool neq (std::tuple<Int, Int>& args) {\n"
                                                    "    return {std::get<0>(args).value != std::get<1>(args).value};\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "not") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                    {"Bool not (std::tuple<Bool>& args) {\n"
                                                     "    return {!std::get<0>(args).value};\n"
                                                     "}\n\n",
                                                            std::make_unique<Arrow>(new Pod("Bool"),
                                                                                    new Pod("Bool")
                                                                                    ),
                                                            1}});
    }
    if (name == "isNull") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                    {"template <typename a>"
                                                     "Bool isNull (std::tuple<a>& args) {\n"
                                                     "    return {std::get<0>(args).value == nullptr};\n"
                                                     "}\n\n",
                                                            std::make_unique<Arrow>(new Abstract("a"),
                                                                                    new Pod("Bool")
                                                            ),
                                                            1}});
    }
    if (name == "seq") {
        return std::optional<BaseLib::NativeDesc>({ {},
                                                    {},
                                                    {"Void seq (std::tuple<Void, Void>& args) {\n"
                                                     "    return Void{};\n"
                                                     "}\n\n",
                                                            std::make_unique<Arrow>(new Abstract("a"),
                                                                                    new Arrow(
                                                                        new Abstract("b"), new Pod("Void")
                                                                    )
                                                            ),
                                                            2}});
    }
    return std::nullopt;
}

void BaseLib::Ops::LoadBaseTypes(Def::TypeTable& typeTable) {
    typeTable["add"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Int")));
    typeTable["sub"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Int")));
    typeTable["mul"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Int")));
    typeTable["div"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Int")));
    typeTable["mod"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Int")));
    typeTable["ls"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["lse"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["gt"] = std::make_shared<Arrow>(new Pod("Int"),
                                              new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["gte"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["eq"] = std::make_shared<Arrow>(new Abstract("a"),
                                              new Arrow(new Abstract("a"), new Pod("Bool")));
    typeTable["neq"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["not"] = std::make_shared<Arrow>(new Pod("Bool"), new Pod("Bool"));
    typeTable["seq"] = std::make_shared<Arrow>(new Abstract("a"), new Arrow(
                new Abstract("b"), new Pod("Void")
            ));
    typeTable["isNull"] = std::make_shared<Arrow>(new Abstract("a"), new Pod("Bool"));
}
