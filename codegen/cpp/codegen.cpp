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
    bool needForMT = false;
    GenerateFunc("main", funcTable, typeTable, needForMT);
    Print(genFile, needForMT);
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
        leftType = "void";
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
                              Def::TypeTable& typeTable,
                              bool& needForMT) {
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
        result = "int main() {\n";
    } else {
        const auto& funcDesc = cppFuncList[name];
        result = funcDesc.retType + " " + name + " (" + funcDesc.args + "& args) {\n";
    }
    size_t startingDatasIdx = 0;
    for (const auto& def : funcTable.at(name)) {
        std::string resultingExpr;
        if (def.cond != nullptr) {
            resultingExpr += "{\n\t\t";
            std::vector<std::string> datas;
            auto checkExpr = GenerateExpr2(*def.cond, funcTable, typeTable, needForMT);
            resultingExpr += checkExpr.first;
            startingDatasIdx += datas.size();
            resultingExpr += "if (" + checkExpr.second += ") {\n\t\t\t";
        }
        auto* expr = def.annExpr.get();
        std::vector<std::string> datas;
        auto valExpr = GenerateExpr2(*expr, funcTable, typeTable, needForMT);
        resultingExpr += valExpr.first;
        if (name != "main") {
            resultingExpr += "\treturn " + valExpr.second + ";\n";
        }
        result += resultingExpr + "}\n";
    }
    result += "\n";
    funcList[name] = result;
    funcsOrdered.push_back(result);
}

std::string CppCodegen::GetTypeForData(const std::string& name) {
    std::string result = cppFuncList[name].retType;
    for (const auto& arg : cppFuncList[name].argVect) {
        result += ", " + arg;
    }
    return result;
}

std::pair<std::string, std::string> CppCodegen::GenerateExpr2(const AnnotatedExpression& annExpr,
Def::FuncTable& ft, Def::TypeTable& typet, bool& needForMT) {
    std::string result;
    size_t i, j = 0;
    for (const auto& [num, exprs] : annExpr) {
        i = num;
        if (exprs.size() > 1) {
            // multithreaded
            needForMT = true;
            auto counterName = GenerateCounterName();
            result += BaseLib::GetMTPreludeCode(counterName, std::to_string(exprs.size()));
            for (const auto& expr : exprs) {
                std::vector<std::string> datas;
                auto genExpr = GenerateExpr3(expr.get(), typet, ft, num, j, datas, needForMT) + ";\n";
                for (const auto& data : datas) {
                    result += data;
                }
                auto argName = GenerateAnnotatedName(num, j);
                result += BaseLib::GetFuncMTCode(argName, genExpr) +
                        BaseLib::GetEvalMTCode(argName, counterName);
                ++j;
            }
            result += BaseLib::GetMTCodaCode(counterName, std::to_string(exprs.size()));
        } else {
            const auto& expr = exprs.begin();
            // single-threaded
            std::vector<std::string> datas;
            auto genExpr = GenerateExpr3(expr->get(), typet, ft, num, j, datas, needForMT);
            if (num == (size_t)-1) {
                j = 0;
            }
            auto argName = GenerateAnnotatedName(num, j);
            for (const auto& data : datas) {
                result += data;
            }
            result += BaseLib::GetFuncSTCode(argName, genExpr);
        }
    }
    auto evalvalue = "arg" + GenerateAnnotatedName(i, j) + "ret";
    newCounter = 0;
    return {result, evalvalue};
}


std::string CppCodegen::GenerateExpr3(Expression* expr,
Def::TypeTable& typeTable, Def::FuncTable& funcTable,
size_t argLeft, size_t& argRight, std::vector<std::string>& datas, bool& needForMT) {
    auto annotatedName = GenerateAnnotatedName(argLeft, argRight);
    if (expr->type == ExpressionType::VAR) {
        auto* varExpr = dynamic_cast<VarExpression*>(expr);
        if (IsMangledName(varExpr->name)) {
            std::string demangled = varExpr->name;
            demangled.pop_back();
            return "std::move(std::get<" + demangled + ">(args))";
        } else if (IsAnnotatedName(varExpr->name)) {
            return "std::move(arg" + varExpr->name + "ret)";
        }
        if (!funcList.contains(varExpr->name)) {
            funcList[varExpr->name]; // recursion protection
            GenerateFunc(varExpr->name, funcTable, typeTable, needForMT);
        }
        ++argRight;
        datas.push_back(BaseLib::GetDataCode(GetTypeForData(varExpr->name),
                                             annotatedName, varExpr->name));
        return GetTypedef(typeTable.at(varExpr->name).get()) + "{&data" + annotatedName + "}";
    } else if (expr->type == ExpressionType::LITERAL) {
        auto litExpr = dynamic_cast<ConstExpression*>(expr);
        return litExpr->value;
    }
    auto* appExpr = dynamic_cast<AppExpression*>(expr);
    return "(" + GenerateExpr3(appExpr->fun.get(), typeTable, funcTable, argLeft,
                               argRight, datas, needForMT) +
           ").Eval(" + GenerateExpr3(appExpr->arg.get(), typeTable, funcTable, argLeft,
                                     argRight, datas, needForMT) + ")";
}


std::string CppCodegen::GenerateCounterName() {
    return "counter" + std::to_string(newCounter++);
}

void CppCodegen::Print(std::ofstream& file, bool needForMT) {
    auto includes = cppNatives.includes;
    std::string baseCode = BaseLib::GetBaseCode(includes);
    std::string baseMTCode;

    if (needForMT) {
        baseMTCode = BaseLib::GetMTCode(includes);
    }
    for (const auto& inc : includes) {
        file << inc;
    }

    file << baseCode << baseMTCode;

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
