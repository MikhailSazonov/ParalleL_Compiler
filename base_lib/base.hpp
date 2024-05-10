#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <optional>

#include "../type.hpp"
#include "../funcdef.hpp"
#include "../expression.hpp"
#include "../definitions.hpp"

namespace BaseLib {
    const std::pair<std::string, std::string> VOID_TYPE = {"Void", "void"};
    const std::pair<std::string, std::string> BOOL_TYPE = {"Bool", "bool"};
    const std::pair<std::string, std::string> INT_TYPE = {"Int", "int"};
    const std::pair<std::string, std::string> CHAR_TYPE = {"Char", "char"};
    const std::pair<std::string, std::string> FLOAT_TYPE = {"Float", "float"};
    const std::pair<std::string, std::string> STRING_TYPE = {"String", "std::string"};


    const std::unordered_map<std::string, std::string> BASE_TYPES {
            VOID_TYPE,
            BOOL_TYPE,
            INT_TYPE,
            CHAR_TYPE,
            FLOAT_TYPE,
            STRING_TYPE,
    };

//    std::unordered_map<std::string, std::vector<std::string>> FUNC_INCLUDES {
//            {"print", {"iostream"}}
//    };

    bool CheckTypeExistance(const std::string&);

    struct NativeFun {
        std::string nativeDef;
        std::shared_ptr<Type> plType;
        size_t argNo;
    };

    struct NativeDesc {
        std::unordered_set<std::string> includes;
        NativeFun fun;
    };

    std::optional<NativeDesc> GetNative(const std::string&);

    void LoadBaseTypes(Def::TypeTable&);

    std::string GetBaseCode();
}
