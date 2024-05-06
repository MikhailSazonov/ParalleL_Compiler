#include "create_gen.hpp"
#include "cpp/codegen.hpp"

std::unique_ptr<ICodegen> CreateCodegen(CodegenType type) {
    switch (type) {
        case CodegenType::CPP:
            return std::unique_ptr<ICodegen>(new CppCodegen());
        default:
            throw std::runtime_error{"Not implemented."};
    }
    return nullptr;
}
