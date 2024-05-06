#pragma once


#include "../i_codegen.hpp"
#include "../../definitions.hpp"
#include "../../base_lib/base.hpp"

#include <unordered_set>

struct FuncDesc {
    std::string retType;
    std::string args;
};

struct CppNatives {
    std::unordered_set<std::string> includes;
    std::vector<BaseLib::NativeFun> nativeDefs;
};

class CppCodegen : public ICodegen {
    public:
        void Generate(const Def::FuncTable&,
                      const Def::TypeTable&,
                      bool);

        void GenerateTypedef(const std::string&, Type*, size_t);

        std::string CreateTypedef(Type*, const std::string&, std::vector<std::string>&);

        typedef std::vector<std::pair<Type*, std::pair<std::string, std::string>>> TypedefList;
        typedef std::unordered_map<std::string, FuncDesc> CppFuncList;
        typedef std::unordered_map<std::string, std::string> FuncList;
        typedef std::vector<std::string> FuncListOrdered;

        bool TypedefCreated(Type*);

        std::string GetTypedef(Type*);

        std::string GenerateTypedefName(Type*);

        std::string GenerateTypedefNameCpp(Type* type) {
            return GenerateTypedefName(type) + "Cpp";
        }

        std::string GenerateTypedefForCppFunc(const std::string&, Type* type, size_t);

        void GenerateFunc(const std::string&, const Def::FuncTable&, const Def::TypeTable&);

        std::string GenerateExpr(Expression*, const Def::FuncTable&,
                                 const Def::TypeTable&, bool insertBracket = false);

        void Print(std::ofstream&);

    private:
        TypedefList typedefs;
        FuncList funcList;
        FuncListOrdered funcsOrdered;
        CppFuncList cppFuncList;
        CppNatives cppNatives;
};

