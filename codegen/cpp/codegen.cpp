#include "codegen.hpp"

#include "../../definitions.hpp"
#include "../../errors.hpp"

#include "../../base_lib/base.hpp"
#include "../../utils.hpp"

#include <fstream>

void CppCodegen::Generate(const Def::FuncTable& funcTable,
const Def::TypeTable& typeTable, bool skipIntermediate) {
    std::ofstream genFile("a.cpp");
    if (!genFile) {
        throw FailToCreateFile{};
    }
    GenerateFunc("main", funcTable, typeTable);
    Print(genFile);
}

bool CppCodegen::TypedefCreated(Type* type) {
    return std::any_of(typedefs.begin(), typedefs.end(), [type](auto& elem){ return *elem.first == *type; });
}

void CppCodegen::GenerateTypedef(const std::string& name, Type* type, size_t argNo) {
    if (TypedefCreated(type)) {
        return;
    }
    if (type->type == TermType::POD) {
        auto* pod = dynamic_cast<Pod*>(type);
        if (BaseLib::BASE_TYPES.contains(pod->typeName)) {
            typedefs.emplace_back(type,
                  std::make_pair(BaseLib::BASE_TYPES.at(pod->typeName), pod->typeName));
            cppFuncList[name] = {"Void", ""};
            return;
        }
        // TODO: user types
        throw UndefinedTypeError{};
    }
    auto* arrow = dynamic_cast<Arrow*>(type);
    std::string execFuncType = GenerateTypedefForCppFunc(name, type, argNo);
    std::vector<std::string> accumulated_types;
    std::string generatedTypeName = GenerateTypedefName(type);
    typedefs.emplace_back(type,
                  std::make_pair(CreateTypedef(arrow, execFuncType, accumulated_types), generatedTypeName));
}


std::string CppCodegen::CreateTypedef(Type* type, const std::string& funName, std::vector<std::string>& accumulated) {
    std::string leftType, rightType;
    if (type->type == TermType::ARROW) {
        auto* arr = dynamic_cast<Arrow*>(type);
        leftType = GetTypedef(arr->left.get());
        accumulated.push_back(leftType);
        rightType = CreateTypedef(arr->right.get(), funName, accumulated);
        accumulated.pop_back();
    } else {
        leftType = "Void";
        rightType = GetTypedef(type);
    }
    std::string result = "Func<" + leftType + ", " + rightType + ", " + funName;
    for (const auto& acc : accumulated) {
        result += ", " + acc;
    }
    return result + ">";
}

std::string CppCodegen::GetTypedef(Type* type) {
    for (const auto& elem : typedefs) {
        if (*elem.first == *type) {
            return elem.second.second;
        }
    }
    return "";
}


std::string CppCodegen::GenerateTypedefForCppFunc(const std::string& name, Type* type, size_t argNo) {
    std::vector<std::string> argTypedefs;
    std::string nameCpp = GenerateTypedefNameCpp(type);
    for (size_t i = 0; i < argNo; ++i) {
        auto* arrow = dynamic_cast<Arrow*>(type);
        argTypedefs.push_back(GetTypedef(arrow->left.get()));
        type = arrow->right.get();
    }
    std::string retType = GetTypedef(type);
    std::string args = "(std::tuple<";
    for (size_t i = 0; i < argTypedefs.size(); ++i) {
        if (i > 0) {
            args += ", ";
        }
        args += argTypedefs[i];
    }
    args += ">";
    cppFuncList[name] = {retType, args};
    return nameCpp;
}


std::string CppCodegen::GenerateTypedefName(Type* type) {
    if (type->type == TermType::POD) {
        return GetTypedef(type);
    }
    auto* arr = dynamic_cast<Arrow*>(type);
    return "B" + GenerateTypedefName(arr->left.get()) + GenerateTypedefName(arr->right.get()) + "B";
}

void CppCodegen::GenerateFunc(const std::string& name,
                              const Def::FuncTable& funcTable,
                              const Def::TypeTable& typeTable) {
    if (!cppFuncList.contains(name)) {
        auto nativeDesc = BaseLib::GetNative(name);
        if (nativeDesc) {
            cppNatives.includes.merge(nativeDesc->includes);
            cppNatives.nativeDefs.push_back(nativeDesc->fun);
        }
        GenerateTypedef(name, typeTable.at(name).get(), funcTable.at(name)[0].argNo);
    }
    auto* expr = funcTable.at(name)[0].expr.get();
    std::string result;
    if (name == "main") {
        result = "int main() {\n\t";
    } else {
        const auto& funcDesc = cppFuncList[name];
        result = funcDesc.retType + " " + name + " (" + funcDesc.args + "&) {\n\t";
        if (funcDesc.retType != "Void") {
            result += "return ";
        }
    }
    result += GenerateExpr(expr, funcTable, typeTable) + ";\n}\n\n";
    funcList[name] = result;
    funcsOrdered.push_back(result);
}


std::string CppCodegen::GenerateExpr(Expression* expr,
                                     const Def::FuncTable& funcTable,
                                     const Def::TypeTable& typeTable,
                                     bool insertBracket) {
    if (expr->type == ExpressionType::CONST) {
        auto* constExpr = dynamic_cast<ConstExpression*>(expr);
        return constExpr->value;
    } else if (expr->type == ExpressionType::APP) {
        auto* appExpr = dynamic_cast<AppExpression*>(expr);
        return GenerateExpr(appExpr->fun.get(), funcTable, typeTable) + (insertBracket ? ")" : "") + ".Eval(" +
                GenerateExpr(appExpr->arg.get(), funcTable, typeTable, true);
    }
    auto* varExpr = dynamic_cast<VarExpression*>(expr);
    if (IsMangledName({varExpr->name})) {
        std::string demangled = varExpr->name;
        demangled.pop_back();
        return "std::get<" + demangled + ">(args)";
    }
    if (!funcList.contains(varExpr->name)) {
        GenerateFunc(varExpr->name, funcTable, typeTable);
    }
    return GetTypedef(typeTable.at(varExpr->name).get()) + "{&" + varExpr->name + "}";
}


void CppCodegen::Print(std::ofstream& file) {
    for (const auto& inc : cppNatives.includes) {
        file << inc;
    }
    file << "\n\n";
    for (const auto& native : cppNatives.nativeDefs) {
        file << native;
    }
    for (const auto& typeDef : typedefs) {
        file << "typedef " << typeDef.second.first << " " << typeDef.second.second << "\n";
    }
    for (const auto& func : funcsOrdered) {
        file << func << "\n";
    }
    file << "\n";
}
