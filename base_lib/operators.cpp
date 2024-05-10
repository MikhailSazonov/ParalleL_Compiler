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
    }
    return "";
}


std::optional<BaseLib::NativeDesc> BaseLib::Ops::GetNative(const std::string& name) {
    if (name == "add") {
        return std::optional<BaseLib::NativeDesc>({{},
                                       {"int add (std::tuple<Int, Int>& args) {\n"
                                        "    return std::get<0>(args) + std::get<1>(args);\n"
                                        "}\n\n",
                                       std::make_unique<Arrow>(new Pod("Int"),
                                                               new Arrow(
                                                                       new Pod("Int"),
                                                                       new Pod("Int")
                                                                       )),
                                       2}});
    }
    if (name == "sub") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"int sub (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) - std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "mul") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"int mul (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) * std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "div") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"int div (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) / std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "mod") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"int mod (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) % std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Int")
                                                                                   )),
                                                           2}});
    }
    if (name == "ls") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool ls (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) < std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "lse") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool ls (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) <= std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "gt") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool gt (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) > std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "gte") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool ls (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) >= std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "eq") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool eq (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) == std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
                                                           2}});
    }
    if (name == "neq") {
        return std::optional<BaseLib::NativeDesc>({{},
                                                   {"bool neq (std::tuple<Int, Int>& args) {\n"
                                                    "    return std::get<0>(args) != std::get<1>(args);\n"
                                                    "}\n\n",
                                                           std::make_unique<Arrow>(new Pod("Int"),
                                                                                   new Arrow(
                                                                                           new Pod("Int"),
                                                                                           new Pod("Bool")
                                                                                   )),
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
    typeTable["eq"] = std::make_shared<Arrow>(new Pod("Int"),
                                              new Arrow(new Pod("Int"), new Pod("Bool")));
    typeTable["neq"] = std::make_shared<Arrow>(new Pod("Int"),
                                               new Arrow(new Pod("Int"), new Pod("Bool")));
}
