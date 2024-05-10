#pragma once

#include "../definitions.hpp"

#include <unordered_map>
#include <string>

class ICodegen {
    public:
        virtual ~ICodegen() = default;

        virtual void Generate(Def::FuncTable&,
                              Def::TypeTable&,
                              bool) = 0;
};
