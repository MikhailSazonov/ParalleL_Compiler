#include "codegen.hpp"

#include "../../definitions.hpp"
#include "../../errors.hpp"
#include "../../type.hpp"

#include "../../base_lib/base.hpp"
#include "../../utils.hpp"

#include <fstream>


static bool ClassContainerContains(const std::string& className, CppCodegen::ClassListOrdered& classList) {
    return std::any_of(classList.begin(), classList.end(), [&className](std::pair<std::string, std::string>& elem) {
        return elem.first == className;
    });
}

void CppCodegen::Generate(Def::FuncTable& funcTable,
Def::TypeTable& typeTable, Def::ClassTable& classTable, bool skipIntermediate) {
    std::ofstream genFile("a.cpp", std::ios::trunc);
    if (!genFile) {
        throw FailToCreateFile{};
    }
    bool needForMT = false;
    GenerateFunc("main", funcTable, typeTable, classTable, needForMT);
    Print(genFile, needForMT);
}

bool CppCodegen::TypedefCreated(std::shared_ptr<Type>& type) {
    return std::any_of(typedefsFuncs.begin(), typedefsFuncs.end(), [&type](auto& elem){ return *elem.first == *type; }) ||
            std::any_of(typedefsPods.begin(), typedefsPods.end(), [&type](auto& elem){ return *elem.first == *type; });
}

void CppCodegen::GenerateTypedef(const std::string& name, std::shared_ptr<Type>& type,
size_t argNo, Def::ClassTable& classTable, bool generateTrueTypedef) {
    if (type->type == TermType::POD) {
        auto* podType = dynamic_cast<Pod*>(type.get());
        if (!BaseLib::IsDefaultType(podType->typeName) &&
        !ClassContainerContains(podType->typeName, classListOrdered)) {
            const auto* neededClass = FindClass(podType->typeName, classTable);
            classListOrdered.emplace_back(neededClass->name, BaseLib::GenerateClassCode(neededClass->name,
                                                  neededClass->values));
            return;
        }
        GeneratePodTypedef(type, classTable);
        return;
    }
    GenerateTypedefForCppFunc(name, &type, argNo, classTable, generateTrueTypedef);
    if (generateTrueTypedef) {
        GenerateTypedef(type, classTable);
    }
}


void CppCodegen::GenerateTypedef(std::shared_ptr<Type>& type, Def::ClassTable& clTable) {
    if (TypedefCreated(type)) {
        return;
    }
    auto* arrow = dynamic_cast<Arrow*>(type.get());
    if (arrow->left->type == TermType::POD) {
        GeneratePodTypedef(arrow->left, clTable);
    }
    if (arrow->right->type == TermType::POD) {
        GeneratePodTypedef(arrow->right, clTable);
    }
    std::string generatedTypeName = GenerateTypedefName(type, clTable);
    typedefsFuncs.emplace_back(type,
               std::make_pair(CreateTypedef(type, clTable), generatedTypeName));
}


bool CppCodegen::GeneratePodTypedef(std::shared_ptr<Type>& type, Def::ClassTable& classTable) {
    if (TypedefCreated(type)) {
        return true;
    }
    auto* pod = dynamic_cast<Pod*>(type.get());
    if (pod == nullptr) {
        return false;
    }
    if (BaseLib::BASE_TYPES.contains(pod->typeName)) {
        typedefsPods.emplace_back(type,
                  std::make_pair(BaseLib::BASE_TYPES.at(pod->typeName), pod->typeName));
        return true;
    }
    if (ClassContainerContains(pod->typeName, classListOrdered)) {
        return true;
    }
    if (ClassIsPresent(pod->typeName, classTable)) {
        auto* cl = FindClass(pod->typeName, classTable);
        classListOrdered.emplace_back(pod->typeName, BaseLib::GenerateClassCode(cl->name, cl->values));
        return true;
    }
    throw UndefinedTypeError{};
}


std::string CppCodegen::CreateTypedef(std::shared_ptr<Type>& type, Def::ClassTable& clTable) {
    std::string leftType, rightType;
    if (type->type == TermType::ARROW) {
        auto* arr = dynamic_cast<Arrow*>(type.get());
        leftType = GetTypedef(arr->left, clTable);
        rightType = CreateTypedef(arr->right, clTable);
    } else {
        leftType = "void";
        rightType = GetTypedef(type, clTable);
    }
    return "Func<" + leftType + ", " + rightType + ">";
}

std::string CppCodegen::GetTypedef(std::shared_ptr<Type>& type, Def::ClassTable& clTable) {
    if (type->type == TermType::ARROW) {
        for (const auto& elem : typedefsFuncs) {
            if (*elem.first == *type) {
                return elem.second.second;
            }
        }
        GenerateTypedef(type, clTable);
    } else if (type->type == TermType::ABSTRACT) {
        int a = 5;
    } else {
        auto* podType = dynamic_cast<Pod*>(type.get());
        if (ClassIsPresent(podType->typeName, clTable)) {
            return podType->typeName;
        }
        for (const auto& elem : typedefsPods) {
            if (*elem.first == *type) {
                return elem.second.second;
            }
        }
        GeneratePodTypedef(type, clTable);
    }
    return GetTypedef(type, clTable);
}


std::string CppCodegen::GenerateTypedefForCppFunc(const std::string& name, std::shared_ptr<Type>* type, size_t argNo,
Def::ClassTable& clTable, bool generateCppTypedef) {
    std::vector<std::string> argTypedefs;
    std::string nameCpp;
    if (generateCppTypedef) {
        nameCpp = GenerateTypedefNameCpp(*type, clTable);
    }
    for (size_t i = 0; i < argNo; ++i) {
        auto* arrow = dynamic_cast<Arrow*>(type->get());
        if (arrow->left->type == TermType::ABSTRACT) {
            auto* absType = dynamic_cast<Abstract*>(arrow->left.get());
            argTypedefs.push_back(absType->abstractName);
        } else {
            argTypedefs.push_back(GetTypedef(arrow->left, clTable));
        }
        type = &arrow->right;
    }
    std::string retType;
    if ((*type)->type == TermType::ABSTRACT) {
        auto* absType = dynamic_cast<Abstract*>(type->get());
        retType = absType->abstractName;
    } else {
        retType = GetTypedef(*type, clTable);
    }
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


std::string CppCodegen::GenerateTypedefName(std::shared_ptr<Type>& type, Def::ClassTable& clTable) {
    if (type->type == TermType::POD) {
        return GetTypedef(type, clTable);
    }
    auto* arr = dynamic_cast<Arrow*>(type.get());
    return "B" + GenerateTypedefName(arr->left, clTable) + GenerateTypedefName(arr->right, clTable) + "B";
}

void CppCodegen::GenerateFunc(const std::string& name,
                              Def::FuncTable& funcTable,
                              Def::TypeTable& typeTable,
                              Def::ClassTable& classTable,
                              bool& needForMT) {
    std::string result;
    if (!cppFuncList.contains(name)) {
        auto nativeDesc = BaseLib::GetNative(name);
        if (nativeDesc) {
            cppNatives.includes.merge(nativeDesc->includes);
            cppNatives.nativeDefs.push_back(nativeDesc->fun);
            typeTable[name] = nativeDesc->fun.plType;
            funcTable[name].push_back({{}, nullptr, nativeDesc->fun.argNo});
        }
        std::unordered_set<std::string> templateArgs;
        GatherAbstractTypes(typeTable.at(name).get(), templateArgs);
        GenerateTypedef(name, typeTable.at(name), funcTable.at(name)[0].argNo,
                        classTable, templateArgs.empty());
        if (!templateArgs.empty()) {
            result += BaseLib::GenerateTemplateFromTemplate(templateArgs);
        }
        if (nativeDesc) {
            funcList[name] = nativeDesc->fun.nativeDef;
            funcsOrdered.push_back(nativeDesc->fun.nativeDef);
            return;
        }
    }
    if (name == "main") {
        result += "int main() {\n";
    } else {
        const auto& funcDesc = cppFuncList[name];
        result += funcDesc.retType + " " + name + " (" + funcDesc.args + "& args) {\n";
        funcDeclarations.push_back(funcDesc.retType + " " + name + " (" + funcDesc.args + "& args);\n\n");
    }
    size_t startingDatasIdx = 0;
    size_t retIdx = 0;
    for (const auto& def : funcTable.at(name)) {
        std::string resultingExpr = "\n{\n";
        if (def.cond != nullptr) {
            std::vector<std::string> datas;
            auto checkExpr = GenerateExpr2(*def.cond, funcTable, typeTable, classTable, needForMT, retIdx, name);
            resultingExpr += checkExpr.first;
            startingDatasIdx += datas.size();
            resultingExpr += "if (" + checkExpr.second += ".value) {\n\t\t";
        }
        auto* expr = def.annExpr.get();
        std::vector<std::string> datas;
        auto valExpr = GenerateExpr2(*expr, funcTable, typeTable, classTable, needForMT, retIdx, name);
        resultingExpr += valExpr.first;
        if (name != "main") {
            resultingExpr += "\treturn " + valExpr.second + ".Replicate();\n";
        }
        result += resultingExpr + "}\n}\n";
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

std::string CppCodegen::GetTypeForData(std::shared_ptr<Type>* type, Def::ClassTable& classTable,
size_t argNo) {
    std::string result;
    std::string resultEnding;
    for (size_t i = 0; i < argNo; ++i) {
        if ((*type)->type != TermType::ARROW) {
            throw TypeMismatchError{};
        }
        auto* arrType = dynamic_cast<Arrow*>(type->get());
        resultEnding += ", " + GetTypedef(arrType->left, classTable);
        type = &arrType->right;
    }
    result = GetTypedef(*type, classTable) + resultEnding;
    return result;
}

std::string CppCodegen::GetTypeForTemplateData(Def::TypeTable& typeTable, Expression* expr) {
    if (expr->type == ExpressionType::VAR) {
        auto* varExpr = dynamic_cast<VarExpression*>(expr);
        if (cppFuncList.contains(varExpr->name)) {
            return cppFuncList[varExpr->name].nameCpp;
        }
        auto* varType = dynamic_cast<Pod*>(typeTable[varExpr->name].get());
        return varType->typeName;
    }
    if (expr->type == ExpressionType::LITERAL) {
        auto* litExpr = dynamic_cast<ConstExpression*>(expr);
        auto* podType = dynamic_cast<Pod*>(litExpr->correspondingType.get());
        return podType->typeName;
    }
    auto arrowType = dynamic_cast<AppExpression*>(expr);
    return GetTypeForTemplateData(typeTable, arrowType->arg.get()) +
        ", " + GetTypeForTemplateData(typeTable, arrowType->fun.get());
}

std::pair<std::string, std::string> CppCodegen::GenerateExpr2(const AnnotatedExpression& annExpr,
Def::FuncTable& ft, Def::TypeTable& typet, Def::ClassTable& classTable, bool& needForMT, size_t& retIdx,
const std::string& name) {
    std::string result;
    size_t i, j = 0, k;
    for (const auto& [num, exprs] : annExpr) {
        i = num;
        k = 0;
        size_t stIdx = 0;
        if (exprs.size() > 1) {
            // multithreaded
            needForMT = true;
            auto counterName = GenerateCounterName();
            result += BaseLib::GetMTPreludeCode(counterName, std::to_string(exprs.size()));
            for (const auto& expr : exprs) {
                std::vector<std::string> datas;
                std::vector<std::string> storages;
                std::vector<std::string> packStorages;
                auto genExpr = GenerateExpr3(expr.get(), typet, ft, classTable, num, j,
                                             datas, needForMT, true,
                                             name, storages, stIdx, packStorages, retIdx) + ";\n";

                for (const auto& data : datas) {
                    result += data;
                }

                for (const auto& storage : storages) {
                    result += storage;
                }

                for (const auto& pack : packStorages) {
                    result += pack;
                }

                auto argName = GenerateAnnotatedName(num, k);
                result += BaseLib::GetFuncMTCode(argName, genExpr) +
                        BaseLib::GetEvalMTCode(argName, counterName);
                ++k;
            }
            result += BaseLib::GetMTCodaCode(counterName, std::to_string(exprs.size()));
        } else {
            const auto& expr = exprs.begin();
            // single-threaded
            std::vector<std::string> datas;
            std::vector<std::string> storages;
            std::vector<std::string> packStorages;
            auto genExpr = GenerateExpr3(expr->get(), typet, ft, classTable,
                                         num, j, datas, needForMT, true,
                                         name, storages, stIdx, packStorages, retIdx);
            j = 0;
            if (num == (size_t)-1) {
                j = retIdx;
                k = retIdx;
            }
            auto argName = GenerateAnnotatedName(num, k);
            for (const auto& data : datas) {
                result += data;
            }
            for (const auto& storage : storages) {
                result += storage;
            }
            for (const auto& pack : packStorages) {
                result += pack;
            }
            result += BaseLib::GetFuncSTCode(argName, genExpr);
        }
        j = 0;
    }
    auto evalvalue = "arg" + GenerateAnnotatedName(i, k) + "ret";
    newCounter = 0;
    return {result, evalvalue};
}

static size_t numPackedToGen = 0;
static size_t storageNoX = 0;

std::string CppCodegen::GenerateExpr3(Expression* expr,
Def::TypeTable& typeTable, Def::FuncTable& funcTable,
Def::ClassTable& classTable,
size_t argLeft, size_t& argRightData,
std::vector<std::string>& datas, bool& needForMT, bool topLevel, const std::string& name,
std::vector<std::string>& fbipStorages, size_t& storageNo,
std::vector<std::string>& packStorages,
size_t& retIdx) {
    if (expr->type == ExpressionType::PACKED) {
        auto* packExpr = dynamic_cast<PackedExpression*>(expr);

        auto retType = cppFuncList[name].retType;
        auto packName = Def::PACKED_GENERATED + std::to_string(numPackedToGen++);

        std::vector<std::string> elemsEvals;

        for (const auto& annExpr : packExpr->exprs) {
            auto [res, evalValue] = GenerateExpr2(*annExpr, funcTable, typeTable, classTable, needForMT,
                                                  retIdx, name);
            // TODO : idcs mappings
            if (res.ends_with("Null{}.Eval();\n")) {
                continue;
            }
            retIdx++;
            packStorages.push_back(res);
            elemsEvals.push_back(evalValue);
        }

        packStorages.push_back(BaseLib::GeneratePackedClassCode(elemsEvals,
                    FindClass(retType, classTable)->values, retType, packName));
        return packName;

    } else if (expr->type == ExpressionType::VAR) {
        auto* varExpr = dynamic_cast<VarExpression*>(expr);
        if (varExpr->name.find(Def::MANGLING_SYMBOL) != std::string::npos) {

            auto argNo = std::stoi(varExpr->name.substr(0, varExpr->name.find(Def::MANGLING_SYMBOL)));
            std::string arg = cppFuncList[name].argVect[argNo];

            auto res = GenerateUnmangledCode(varExpr->name, arg, {}, classTable);
            std::string result = res.first;

//            if (result == "std::move(std::get<0>(args).Get().Getvalue())") {
//                int a = 5;
//            }

            // do we need second check?
            if (FindClass(res.second, classTable)) {
                GenerateCopyCode(storageNoX, result, fbipStorages);
                result = "std::move(copy" + std::to_string(storageNoX) + ")";
                storageNoX++;
            }

            return result;
        } else if (IsAnnotatedName(varExpr->name)) {
            return "std::move(arg" + varExpr->name + "ret)";
        }
        if (!funcList.contains(varExpr->name)) {
            funcList[varExpr->name]; // recursion protection
            GenerateFunc(varExpr->name, funcTable, typeTable, classTable, needForMT);
        }
        auto annotatedName = GenerateAnnotatedName(argLeft, argRightData);
        ++argRightData;
        datas.push_back(BaseLib::GetDataCode(GetTypeForData(varExpr->name),
                                             annotatedName, varExpr->name));
        return GetTypedef(typeTable.at(varExpr->name), classTable) + "{&data" + annotatedName + "}";
    } else if (expr->type == ExpressionType::VAR_ABSTRACTED) {
        auto* absExpr = dynamic_cast<AbstractVarExpression*>(expr);
        auto type = ResolveAbstractTypes(typeTable[absExpr->name], absExpr->resolveTable);
        auto tempTypedef = GetTypedef(type, classTable);
        if (!funcList.contains(absExpr->name)) {
            funcList[absExpr->name]; // recursion protection
            GenerateFunc(absExpr->name, funcTable, typeTable, classTable, needForMT);
        }
        auto annotatedName = GenerateAnnotatedName(argLeft, argRightData);
        ++argRightData;
        datas.push_back(BaseLib::GetDataCode(GetTypeForData(&type, classTable,
                                                            funcTable.at(absExpr->name)[0].argNo),
                                             annotatedName, absExpr->name));
        return tempTypedef + "{&data" + annotatedName + "}";
    } else if (expr->type == ExpressionType::LITERAL) {
        auto litExpr = dynamic_cast<ConstExpression*>(expr);
        if (litExpr->value == "{}") {
            auto podType = dynamic_cast<Pod*>(litExpr->correspondingType.get());
            return podType->typeName + litExpr->value;
        }
        return BaseLib::GenerateConstCode(litExpr->value,
                          GetTypedef(litExpr->correspondingType, classTable));
    }
    auto* appExpr = dynamic_cast<AppExpression*>(expr);
    std::string result = "(" + GenerateExpr3(appExpr->fun.get(), typeTable, funcTable, classTable, argLeft,
                                         argRightData, datas, needForMT,
                                         false, name, fbipStorages, storageNo, packStorages, retIdx) +
                     ").Eval(" + GenerateExpr3(appExpr->arg.get(), typeTable, funcTable, classTable, argLeft,
                                               argRightData, datas, needForMT,
                                               false, name, fbipStorages, storageNo, packStorages, retIdx) + ")";
    if (!topLevel) {
        result += ".Eval()";
    }
    return result;
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
        file << "typedef SimpleContainer<" << typeDef.second.first << "> " << typeDef.second.second << ";\n";
    }
    for (const auto& cl : classListOrdered) {
        file << cl.second << "\n";
    }
    for (const auto& typeDef : typedefsFuncs) {
        file << "typedef " << typeDef.second.first << " " << typeDef.second.second << ";\n\n";
    }
    for (const auto& decl : funcDeclarations) {
        file << decl;
    }
    for (const auto& func : funcsOrdered) {
        file << func << "\n";
    }
    file << "\n";
}

std::pair<std::string, std::string> CppCodegen::GenerateUnmangledCode(const std::string_view baseString,
const std::string& className, const std::vector<std::pair<std::string, std::string>>& values,
Def::ClassTable& classTable) {
    if (baseString.empty()) {
        return {"", ""};
    }
    size_t pos;
    if ((pos = baseString.find(Def::MANGLING_SYMBOL)) != std::string_view::npos) {
        auto numStr = std::string(&baseString[0], pos);
        auto num = std::stoi(numStr);
        auto* thisClass = FindClass(className, classTable);
        std::pair<std::string, std::string> rest{"", className};
        if (thisClass) {
            auto res = GenerateUnmangledCode({&baseString[pos + 1], baseString.size() - (pos + 1)},
                                         thisClass->values[num].first, thisClass->values, classTable);
            rest.first = res.first;
            rest.second = (res.second.empty() ? rest.second : res.second);
        }
        return {"std::move(std::get<" + numStr + ">(args)" + rest.first + ")", rest.second};
    }
    pos = baseString.find(Def::UNPACKING_SYMBOL, 1);
    size_t num;
    std::pair<std::string, std::string> rest{"", className};
    if (pos == std::string::npos) {
        num = std::stoi(std::string(&baseString[1]));
    } else {
        num = std::stoi(std::string(&baseString[1], pos - 1));
    }
    auto* thisClass = FindClass(className, classTable);
    if (thisClass && pos != std::string::npos) {
        auto res = GenerateUnmangledCode({&baseString[pos + 1], baseString.size() - (pos + 1)},
                                     thisClass->values[num].first, thisClass->values, classTable);
        rest.first = res.first;
        rest.second = (res.second.empty() ? rest.second : res.second);
    }
    return {".Get().Get" + values[num].second + "()" + rest.first, rest.second};
}

void CppCodegen::GenerateCopyCode(size_t argNo, const std::string& src,
std::vector<std::string>& storageVector) {
    storageVector.push_back(BaseLib::GenerateFbipCode(std::to_string(argNo), src, storageVector.empty()));
}
