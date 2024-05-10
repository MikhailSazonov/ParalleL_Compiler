#pragma once

#include "base.hpp"

#include <iostream>
#include <utility>

namespace BaseLib::Io {
    void LoadBaseTypes(Def::TypeTable&);

    std::optional<NativeDesc> GetNative(const std::string&);
}
