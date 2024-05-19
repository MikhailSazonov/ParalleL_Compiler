#include "io.hpp"

void BaseLib::Io::LoadBaseTypes(Def::TypeTable& typeTable) {
    typeTable["print"] = std::make_shared<Arrow>(new Pod("String"), new Pod("Void"));
    typeTable["readStr"] = std::make_shared<Arrow>(new Pod("Void"), new Pod("String"));
    typeTable["readFile"] = std::make_shared<Arrow>(new Pod("String"), new Pod("String"));
    typeTable["writeFile"] = std::make_shared<Arrow>(new Pod("String"),
                                                     new Arrow(new Pod("String"), new Pod("Void")));
}

std::optional<BaseLib::NativeDesc> BaseLib::Io::GetNative(const std::string& name) {
    static const std::string STDIO = "stdio";
    if (name == "print") {
        return std::optional<BaseLib::NativeDesc>({ {{STDIO, RedOpType::WRITE}},
                                                    {"#include <iostream>\n"},
                                                    {"Void print (std::tuple<String>& args) {\n"
                                                    "    std::cout << std::get<0>(args).value << '\\n';\n"
                                                    "    return Void{};\n"
                                                    "}\n\n",
                                                    std::make_unique<Arrow>(new Pod("String"), new Pod("Void")),
                                                    1}});
    }
    if (name == "readStr") {
        return std::optional<BaseLib::NativeDesc>({ {{STDIO, RedOpType::WRITE}},
                                                    {"#include <iostream>\n"},
                                                    {"String print (std::tuple<Void>& args) {\n"
                                                     "    String s;\n"
                                                     "    std::cin >> s.value;\n"
                                                     "    return s;\n"
                                                     "}\n\n",
                                                    std::make_unique<Arrow>(new Pod("Void"), new Pod("String")),
                                                    1}});
    }
    if (name == "readFile") {
        return std::optional<BaseLib::NativeDesc>({ {{"arg#1", RedOpType::READ}},
                                                    {"#include <fstream>\n"},
                                                    {"String readFile(std::tuple<String>& args) {\n"
                                                     "    std::ifstream fs(std::get<0>(args).value);\n"
                                                     "    std::string value((std::istreambuf_iterator<char>(fs)),\n"
                                                     "                      std::istreambuf_iterator<char>());\n"
                                                     "    String s{value};\n"
                                                     "    return s;\n"
                                                     "}\n",
                                                    std::make_unique<Arrow>(new Pod("String"), new Pod("String")),
                                                    1}});
    }
    if (name == "writeFile") {
        return std::optional<BaseLib::NativeDesc>({ {{"arg#1", RedOpType::WRITE}},
                                                    {"#include <fstream>\n"},
                                                    {"Void writeFile(std::tuple<String, String>& args) {\n"
                                                     "    std::ofstream fs(std::get<0>(args).value);\n"
                                                     "    fs << std::get<1>(args).value;\n"
                                                     "    return Void{};\n"
                                                     "}",
                                                    std::make_unique<Arrow>(new Pod("String"),
                                                    new Arrow(new Pod("String"), new Pod("Void"))),
                                                    2}});
    }
    return std::nullopt;
}
