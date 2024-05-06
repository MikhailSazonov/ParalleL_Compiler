#include "parse.hpp"
#include "type.hpp"
#include "funcdef.hpp"
#include "definitions.hpp"

#include <unordered_map>
#include <iostream>

#include "codegen/create_gen.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "File for compilation not provided.\n";
        return 1;
    }
    Def::TypeTable typeTable;
    typeTable["main"] = std::make_shared<Pod>("Void");
    Def::FuncTable funcTable;
    if (Parse(typeTable, funcTable, std::string(argv[1])) != 0) {
        return 1;
    }
    if (!funcTable.contains(Def::MAIN_NAME)) {
        std::cerr << "Main module not found.\n";
        return 1;
    }
    CreateCodegen(CodegenType::CPP)->Generate(funcTable, typeTable, true);
}
