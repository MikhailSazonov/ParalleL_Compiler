#pragma once

#include "../definitions.hpp"

#include <unordered_map>
#include <string>

class ICodegen {
    public:
        virtual ~ICodegen() = default;

        virtual void Generate(const Def::FuncTable&,
                              const Def::TypeTable&,
                              bool) = 0;
};
