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
                      Def::ClassTable&,
                      bool);

        void GenerateTypedef(const std::string&, std::shared_ptr<Type>&,
            size_t, Def::ClassTable&, bool generateCppFuncTypedef = true);

        void GenerateTypedef(std::shared_ptr<Type>&, Def::ClassTable&);

        bool GeneratePodTypedef(std::shared_ptr<Type>&, Def::ClassTable&);

        std::string CreateTypedef(std::shared_ptr<Type>&, Def::ClassTable&);

        typedef std::vector<std::pair<std::shared_ptr<Type>, std::pair<std::string, std::string>>> TypedefList;
        typedef std::unordered_map<std::string, FuncDesc> CppFuncList;
        typedef std::unordered_map<std::string, std::string> FuncList;
        typedef std::vector<std::string> FuncListOrdered;
        typedef std::vector<std::pair<std::string, std::string>> ClassListOrdered;

        bool TypedefCreated(std::shared_ptr<Type>&);

        std::string GetTypedef(std::shared_ptr<Type>&, Def::ClassTable&);

        std::string GenerateTypedefName(std::shared_ptr<Type>&, Def::ClassTable&);

        std::string GenerateTypedefNameCpp(std::shared_ptr<Type>& type, Def::ClassTable& clTable) {
            return GenerateTypedefName(type, clTable) + "Cpp";
        }

        std::string GenerateTypedefForCppFunc(const std::string&, std::shared_ptr<Type>* type, size_t, Def::ClassTable&,
                                              bool generateCppTypedef = true);

        void GenerateFunc(const std::string&, Def::FuncTable&, Def::TypeTable&, Def::ClassTable&, bool&);

        std::pair<std::string, std::string> GenerateExpr2(const AnnotatedExpression&, Def::FuncTable&,
                             Def::TypeTable&, Def::ClassTable&, bool&, size_t&, const std::string&);

        std::string GenerateExpr3(Expression*,
                                  Def::TypeTable&,
                                  Def::FuncTable&,
                                  Def::ClassTable&,
                                  size_t, size_t&,
                                  std::vector<std::string>&, bool&, bool,
                                  const std::string&,
                                  std::vector<std::string>&, size_t&,
                                  std::vector<std::string>&, size_t&);

        void Print(std::ofstream&, bool);

        std::string GetTypeForData(const std::string&);

        std::string GetTypeForData(std::shared_ptr<Type>*, Def::ClassTable&, size_t);

        std::string GetTypeForTemplateData(Def::TypeTable&, Expression*);

        std::string GenerateCounterName();

        std::pair<std::string, std::string> GenerateUnmangledCode(const std::string_view, const std::string&,
                                          const std::vector<std::pair<std::string, std::string>>&,
                                          Def::ClassTable&);

        void GenerateCopyCode(size_t, const std::string&, std::vector<std::string>&);

    private:
        TypedefList typedefsFuncs;
        TypedefList typedefsPods;
        FuncList funcList;
        FuncListOrdered funcsOrdered;
        CppFuncList cppFuncList;
        CppNatives cppNatives;
        ClassListOrdered classListOrdered;
        std::vector<std::string> funcDeclarations;
        size_t newCounter;
};

