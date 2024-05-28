#pragma once

#include "base.hpp"

#include <iostream>
#include <utility>

namespace BaseLib::Io {
    void LoadBaseTypes(Def::TypeTable&);

    void LoadBaseColors(Def::ColorTable&);

    std::optional<NativeDesc> GetNative(const std::string&);
}
