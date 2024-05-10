#include "codegen.hpp"

#include "../../definitions.hpp"
#include "../../errors.hpp"

#include "../../base_lib/base.hpp"
#include "../../utils.hpp"

#include <fstream>

void CppCodegen::Generate(Def::FuncTable& funcTable,
Def::TypeTable& typeTable, bool skipIntermediate) {
    std::ofstream genFile("a.cpp", std::ios::trunc);
    if (!genFile) {
        throw FailToCreateFile{};
    }
    GenerateFunc("main", funcTable, typeTable);
    Print(genFile);
}

bool CppCodegen::TypedefCreated(Type* type) {
    return std::any_of(typedefsFuncs.begin(), typedefsFuncs.end(), [type](auto& elem){ return *elem.first == *type; }) ||
            std::any_of(typedefsPods.begin(), typedefsPods.end(), [type](auto& elem){ return *elem.first == *type; });
}

void CppCodegen::GenerateTypedef(const std::string& name, Type* type, size_t argNo) {
    if (type->type == TermType::POD) {
        GeneratePodTypedef(type);
        return;
    }
    GenerateTypedefForCppFunc(name, type, argNo);
    GenerateTypedef(type);
}


void CppCodegen::GenerateTypedef(Type* type) {
    if (TypedefCreated(type)) {
        return;
    }
    auto* arrow = dynamic_cast<Arrow*>(type);
    if (arrow->left->type == TermType::POD) {
        GeneratePodTypedef(arrow->left.get());
    }
    if (arrow->right->type == TermType::POD) {
        GeneratePodTypedef(arrow->right.get());
    }
    std::string generatedTypeName = GenerateTypedefName(type);
    typedefsFuncs.emplace_back(type,
               std::make_pair(CreateTypedef(arrow), generatedTypeName));
}


bool CppCodegen::GeneratePodTypedef(Type* type) {
    if (TypedefCreated(type)) {
        return true;
    }
    auto* pod = dynamic_cast<Pod*>(type);
    if (pod == nullptr) {
        return false;
    }
    if (BaseLib::BASE_TYPES.contains(pod->typeName)) {
        typedefsPods.emplace_back(type,
                  std::make_pair(BaseLib::BASE_TYPES.at(pod->typeName), pod->typeName));
        return true;
    }
    // TODO: user types
    throw UndefinedTypeError{};
}


std::string CppCodegen::CreateTypedef(Type* type) {
    std::string leftType, rightType;
    if (type->type == TermType::ARROW) {
        auto* arr = dynamic_cast<Arrow*>(type);
        leftType = GetTypedef(arr->left.get());
        rightType = CreateTypedef(arr->right.get());
    } else {
        leftType = "Void";
        rightType = GetTypedef(type);
    }
    return "Func<" + leftType + ", " + rightType + ">";
}

std::string CppCodegen::GetTypedef(Type* type) {
    if (type->type == TermType::ARROW) {
        for (const auto& elem : typedefsFuncs) {
            if (*elem.first == *type) {
                return elem.second.second;
            }
        }
        GenerateTypedef(type);
    } else {
        for (const auto& elem : typedefsPods) {
            if (*elem.first == *type) {
                return elem.second.second;
            }
        }
        GeneratePodTypedef(type);
    }
    return GetTypedef(type);
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
    std::string args = "std::tuple<";
    for (size_t i = 0; i < argTypedefs.size(); ++i) {
        if (i > 0) {
            args += ", ";
        }
        args += argTypedefs[i];
    }
    args += ">";
    cppFuncList[name] = {retType, args, nameCpp, argTypedefs};
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
                              Def::FuncTable& funcTable,
                              Def::TypeTable& typeTable) {
    if (!cppFuncList.contains(name)) {
        auto nativeDesc = BaseLib::GetNative(name);
        if (nativeDesc) {
            cppNatives.includes.merge(nativeDesc->includes);
            cppNatives.nativeDefs.push_back(nativeDesc->fun);
            typeTable[name] = nativeDesc->fun.plType;
            funcTable[name].push_back({{}, nullptr, nativeDesc->fun.argNo});
        }
        GenerateTypedef(name, typeTable.at(name).get(), funcTable.at(name)[0].argNo);
        if (nativeDesc) {
            funcList[name] = nativeDesc->fun.nativeDef;
            funcsOrdered.push_back(nativeDesc->fun.nativeDef);
            return;
        }
    }
    std::string result;
    if (name == "main") {
        result = "int main() {\n\t";
    } else {
        const auto& funcDesc = cppFuncList[name];
        result = funcDesc.retType + " " + name + " (" + funcDesc.args + "& args) {\n\t";
    }
    size_t startingDatasIdx = 0;
    for (const auto& def : funcTable.at(name)) {
        std::string resultingExpr;
        if (def.cond != nullptr) {
            resultingExpr += "{\n\t\t";
            std::vector<std::string> datas;
            auto checkExpr = GenerateExpr(def.cond.get(), funcTable, typeTable, datas, startingDatasIdx);
            for (const auto& str : datas) {
                resultingExpr += str;
            }
            startingDatasIdx += datas.size();
            resultingExpr += "if (" + checkExpr += ") {\n\t\t\t";
        }
        if (cppFuncList[name].retType != "Void" && name != "main") {
            resultingExpr += "return ";
        }
        auto* expr = def.expr.get();
        std::vector<std::string> datas;
        resultingExpr += GenerateExpr(expr, funcTable, typeTable, datas, startingDatasIdx);
        resultingExpr += ";\n}\n";
        for (const auto& str : datas) {
            result += str;
        }
        result += resultingExpr;
        if (def.cond != nullptr) {
            result += "}\n";
        }
    }
    result += "\n";
    funcList[name] = result;
    funcsOrdered.push_back(result);
}


std::string CppCodegen::GenerateExpr(Expression* expr,
                                     Def::FuncTable& funcTable,
                                     Def::TypeTable& typeTable,
                                     std::vector<std::string>& datas,
                                     const size_t startingIdx) {
    if (expr->type == ExpressionType::LITERAL) {
        auto* constExpr = dynamic_cast<ConstExpression*>(expr);
        return constExpr->value;
    } else if (expr->type == ExpressionType::APP) {
        auto* appExpr = dynamic_cast<AppExpression*>(expr);
        return "(" + GenerateExpr(appExpr->fun.get(), funcTable, typeTable, datas, startingIdx) + ").Eval(" +
                GenerateExpr(appExpr->arg.get(), funcTable, typeTable, datas, startingIdx) + ").Eval()";
    }
    auto* varExpr = dynamic_cast<VarExpression*>(expr);
    if (IsMangledName({varExpr->name})) {
        std::string demangled = varExpr->name;
        demangled.pop_back();
        return "std::move(std::get<" + demangled + ">(args))";
    }
    if (!funcList.contains(varExpr->name)) {
        funcList[varExpr->name]; // recursion protection
        GenerateFunc(varExpr->name, funcTable, typeTable);
    }
    std::string data = "Data<" + cppFuncList[varExpr->name].retType;
    for (const auto& arg : cppFuncList[varExpr->name].argVect) {
        data += ", " + arg;
    }
    std::string dataName = "data" + std::to_string(datas.size() + startingIdx);
    data += "> " + dataName + "{.fun = &" + varExpr->name + "};\n\t";
    datas.push_back(data);
    return GetTypedef(typeTable.at(varExpr->name).get()) + "{&" + dataName + "}";
}


void CppCodegen::Print(std::ofstream& file) {
    for (const auto& inc : cppNatives.includes) {
        file << inc;
    }
    file << BaseLib::GetBaseCode();

    file << "\n\n";
    for (const auto& typeDef : typedefsPods) {
        file << "typedef " << typeDef.second.first << " " << typeDef.second.second << ";\n";
    }
    for (const auto& typeDef : typedefsFuncs) {
        file << "typedef " << typeDef.second.first << " " << typeDef.second.second << ";\n\n";
    }
    for (const auto& func : funcsOrdered) {
        file << func << "\n";
    }
    file << "\n";
}
