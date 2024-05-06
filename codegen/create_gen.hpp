#pragma once

#include "i_codegen.hpp"

enum class CodegenType {
    CPP,
    LLVM
};

std::unique_ptr<ICodegen> CreateCodegen(CodegenType);