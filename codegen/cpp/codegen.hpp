#pragma once


#include "../i_codegen.hpp"
#include "../../definitions.hpp"
#include "../../base_lib/base.hpp"

#include <unordered_set>

struct FuncDesc {
    std::string retType;
    std::string args;
    std::string nameCpp;
    std::vector<std::string> argVect;
};

struct CppNatives {
    std::unordered_set<std::string> includes;
    std::vector<BaseLib::NativeFun> nativeDefs;
};

class CppCodegen : public ICodegen {
    public:
        void Generate(Def::FuncTable&,
                      Def::TypeTable&,
                      bool);

        void GenerateTypedef(const std::string&, Type*, size_t);

        void GenerateTypedef(Type*);

        bool GeneratePodTypedef(Type*);

        std::string CreateTypedef(Type*);

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

        void GenerateFunc(const std::string&, Def::FuncTable&, Def::TypeTable&);

        std::string GenerateExpr(Expression*, Def::FuncTable&,
                                 Def::TypeTable&,
                                 std::vector<std::string>&,
                                 const size_t);

        void Print(std::ofstream&);

    private:
        TypedefList typedefsFuncs;
        TypedefList typedefsPods;
        FuncList funcList;
        FuncListOrdered funcsOrdered;
        CppFuncList cppFuncList;
        CppNatives cppNatives;
};

