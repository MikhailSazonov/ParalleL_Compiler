#pragma once

#include <string>
#include "base.hpp"

#include "../definitions.hpp"

namespace BaseLib::Ops {
    std::string GetOperator(const std::string_view);

    std::optional<NativeDesc> GetNative(const std::string&);

    void LoadBaseTypes(Def::TypeTable&);
}
