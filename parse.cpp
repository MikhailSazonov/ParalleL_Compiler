#include "parse.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "builder.hpp"
#include "typecheck.hpp"
#include "definitions.hpp"
#include "datarace.hpp"

#include "base_lib/base.hpp"

#include <cctype>
#include <fstream>
#include <iostream>

int Parse(Def::TypeTable& typeTable,
          Def::FuncTable& defTable,
          Def::AnnTable& annTable,
          Def::ClassTable& classTable,
          Def::ColorTable& colorTable,
          const std::string& fileName) {
    if (!fileName.ends_with(Def::FILE_EXT)) {
        throw FileWrongFormat{};
    }
    std::ifstream file(fileName.c_str());
    if (!file) {
        throw FileNotFoundError{};
    }
    std::string line;
    size_t lineNo = 1;
    std::unordered_set<std::string> racingFuns;
    while (std::getline(file, line)) {
        try {
            Analyze(typeTable, defTable, annTable, classTable, colorTable, line, false, racingFuns);
            ++lineNo;
        } catch (const ParalleLCompilerError& er) {
            std::cerr << "Error on line " << lineNo << ": " << er.what() << '\n';
            return 1;
        } catch (...) {
            std::cerr << "Internal compiler error D:\n";
            return 400;
        }
    }
    return 0;
}

void Analyze(Def::TypeTable& typeTable,
             Def::FuncTable& defTable,
             Def::AnnTable& annTable,
             Def::ClassTable& classTable,
             Def::ColorTable& colorTable,
             const std::string& line,
             bool desugared,
             std::unordered_set<std::string>& racingFuns) {
    if (line.empty() || line.starts_with("%%")) {
        return;
    }
    Color color;
    auto lineView = Strip(line);
    size_t idx = 0;
    auto name = Strip(GetToken(lineView, idx));
    if (!classTable.empty() && classTable.back().opened) {
        auto& lastClass = classTable.back();
        if (name == "}") {
            lastClass.opened = false;
            auto token = Strip(GetToken(lineView, idx));
            if (!token.empty()) {
                throw UnexpectedSymbol{};
            }
            return;
        }
        auto value = Strip(GetToken(lineView, idx));
        if (!BaseLib::CheckTypeExistence(std::string(name), classTable)) {
            throw TypeUndefined{};
        }
        CheckNamingSyntax(std::string(value));
        classTable.back().values.emplace_back(std::string(name), std::string(value));
        return;
    }
    if (name == Def::CLASS_KEYWORD) {
        if (!classTable.empty() && classTable.back().opened) {
            throw UnexpectedSymbol{};
        }
        auto className = std::string(Strip(GetToken(lineView, idx)));
        CheckNamingSyntax(className);
        if (typeTable.contains(className) ||
        BaseLib::CheckTypeExistence(className, classTable)) {
            throw DoubleDefinition{};
        }
        auto nextToken = Strip(GetToken(lineView, idx));
        if (nextToken != "{") {
            throw UnexpectedSymbol{};
        }
        classTable.emplace_back(className);
        if (idx != lineView.size()) {
            nextToken = Strip(GetToken(lineView, idx));
            if (nextToken == "}") {
                classTable.back().opened = false;
            } else {
                throw UnexpectedSymbol{};
            }
        }
        return;
    }
    if (!IsName(name)) {
        throw ImproperName{};
    }
    if (name == Def::ANNOTATE_KEYWORD) {
        AddAnnotation(annTable, {&lineView[idx]});
        return;
    }

    bool racing = name == Def::RACING_KEYWORD;
    if (racing) {
        name = Strip(GetToken(lineView, idx));
        racingFuns.insert(std::string(name));
    }

    std::string funName(name);

    for (; idx < lineView.size() && std::isspace(lineView[idx]); ++idx) {}
    if (idx == lineView.size()) {
        throw UnexpectedEndOfString{};
    }
    if (lineView[idx] == ':') {
        if (typeTable.contains(std::string(name)) ||
        BaseLib::CheckTypeExistence(std::string(name), classTable)) {
            throw DoubleDefinition{};
        }
        std::string_view typeDef(&lineView[idx + 1]);
        CheckTypeSyntax(Strip(typeDef));
        typeTable[std::string(name)] = BuildType(typeDef, classTable);
        return;
    } else /*if (std::isalpha(lineView[idx]) || lineView[idx] == '=')*/ {
        if (racing) {
            throw UnexpectedSymbol{};
        }

        std::unordered_map<std::string, std::string> argsMapping;
        if (lineView[idx] != '=' && !typeTable.contains(std::string(name))) {
            throw UndefinedVariableError{};
        }
        std::shared_ptr<Type> currentType = typeTable[std::string(name)];
        std::string_view next_token;
        size_t currentArgN = 0;
        size_t oldIdx = idx;
        size_t argNo = 0;
        while (IsName(next_token = GetToken({&lineView[0]}, idx)) || next_token[0] == '{') {
            if (next_token == "{}") {
                break;
            }
            auto currentArgRepresentation = std::to_string(currentArgN++) + Def::MANGLING_SYMBOL;
            if (next_token[0] == '{') {
                auto posEq = lineView.find('=');
                if (posEq == std::string_view::npos) {
                    throw UnexpectedSymbol{};
                }
                auto rsymb = lineView.rfind('}', posEq);
                if (rsymb == std::string_view::npos) {
                    throw UnexpectedSymbol{};
                }
                std::string_view subline{&lineView[oldIdx + 1], rsymb - (oldIdx + 1)};
                AnalyzeClassInPlace(currentArgRepresentation, subline, argsMapping,
                                    GetVarByNo(currentType.get(), argNo), typeTable, classTable);
                idx = rsymb + 2;
            } else {
                argsMapping[std::string(next_token)] = currentArgRepresentation;
            }
            oldIdx = idx;
            argNo++;
        }
        if (IsConstant(next_token)) {
            if (desugared) {
                throw DoubleWhere{};
            }
            std::string lineCopy = line;
            lineCopy.replace(line.find(next_token), next_token.size(), Def::GEN_NAME);
            std::string tokenString = std::string(next_token);
            if (next_token == "{}") {
                lineCopy += " where isNull " + Def::GEN_NAME;
            } else {
                lineCopy += " where " + Def::GEN_NAME + " == " + tokenString;
            }
            Analyze(typeTable, defTable, annTable, classTable, colorTable, lineCopy, true, racingFuns);
            return;
        }
        if (next_token != "=") {
            throw UnexpectedSymbol{};
        }
        size_t whereKeyword = lineView.find(Def::WHERE_KEYWORD);
        std::string_view expr(&lineView[idx], whereKeyword == std::string::npos ?
                    lineView.size() - idx :
                    whereKeyword - idx);
        expr = Strip(expr);
        CheckExprSyntax(expr);
        std::unique_ptr<AnnotatedExpression> typedExpr;
        if (expr[0] == '{') {
            typedExpr = GeneratePackedExpr(expr, argsMapping, typeTable, colorTable, color, typeTable.at(funName));
        } else {
            typedExpr = BuildExpression(expr, argsMapping, typeTable, colorTable, color);
        }
        if (!racingFuns.contains(std::string(name))) {
            DataRace::CheckForColors(colorTable, *typedExpr);
        }
        // support syntactic sugar : x = 5 (no type presented)
        if ((*typedExpr)[(size_t)-1].back()->type == ExpressionType::LITERAL &&
        !typeTable.contains(std::string(name))) {
            if (defTable.contains(std::string(name)) ||
            BaseLib::CheckTypeExistence(std::string(name), classTable)) {
                throw DoubleDefinition{};
            }
            auto* constant = dynamic_cast<ConstExpression*>((*typedExpr)[(size_t)-1].back().get());
            typeTable[std::string(name)] = constant->correspondingType;
        } else {
            CheckAnnotatedType(typeTable, std::string(name), currentArgN,
                      typeTable[std::string(name)].get(), *typedExpr);
        }
        std::unique_ptr<AnnotatedExpression> cond;
        if (whereKeyword != std::string::npos) {
            cond = BuildExpression({&lineView[whereKeyword + Def::WHERE_KEYWORD.size()]}, argsMapping, typeTable,
                                   colorTable, color);
            Def::ResolveTable* tablePtr = nullptr;
            if (*GetTermType(typeTable, *(*cond)[(size_t)-1].back(), *cond, &tablePtr) != Pod("Bool")) {
                throw TypeMismatchError{};
            }
            if (!racingFuns.contains(std::string(name))) {
                DataRace::CheckForColors(colorTable, *cond);
            }
        }
        if (!defTable[std::string(name)].empty() && defTable[std::string(name)].back().cond == nullptr) {
            throw DefaultAlreadyDeclared{};
        }
        defTable[std::string(name)].push_back({std::move(cond), std::move(typedExpr), currentArgN,
                                               racingFuns.contains(std::string(name))});
        if (!colorTable.contains(std::string(name)) || colorTable[std::string(name)] == Color::BLUE) {
            colorTable[std::string(name)] = color;
        }
        ClearLocalVars(typeTable);
        return;
    }
    throw UnexpectedSymbol{};
}

void CheckTypeSyntax(const std::string_view view) {
    enum class State {
        INITIAL,
        TYPE,
        AFTER_TYPE,
        ARROW,
        AFTER_ARROW
    };
    State st{State::INITIAL};
    size_t balance = 0;
    for (char c : view) {
        if (!std::isalpha(c) && !std::isspace(c) &&
            Def::TYPE_DELIMETER.find(c) == std::string::npos && c != '(' && c != ')') {
            throw UnexpectedSymbol{};
        }
        // BRACKETS
        if ((st == State::INITIAL || st == State::AFTER_ARROW) && c == '(') {
            ++balance;
            continue;
        }
        if ((st == State::TYPE || st == State::AFTER_TYPE) && c == ')') {
            if (balance == 0) {
                throw UnexpectedSymbol{};
            }
            --balance;
            st = State::AFTER_TYPE;
            continue;
        }
        // TYPE
        if ((st == State::INITIAL || st == State::TYPE ||
        st == State::AFTER_ARROW) && std::isalpha(c)) {
            st = State::TYPE;
            continue;
        }
        if (st == State::TYPE && std::isspace(c)) {
            st = State::AFTER_TYPE;
            continue;
        }
        // ARROW
        if ((st == State::TYPE || st == State::AFTER_TYPE) && c == Def::TYPE_DELIMETER.front()) {
            st = State::ARROW;
            continue;
        }
        if (st == State::ARROW && c == Def::TYPE_DELIMETER.back()) {
            st = State::AFTER_ARROW;
            continue;
        }
        if (st == State::AFTER_ARROW || st == State::AFTER_TYPE && std::isspace(c)) {
            continue;
        }
        throw UnexpectedSymbol{};
    }
    if (balance != 0) {
        throw UnexpectedEndOfString{};
    }
}


void CheckExprSyntax(const std::string_view view) {
    size_t balance = 0;
    for (char c : view) {
        if (!std::isspace(c) && !std::isalpha(c) && !std::isdigit(c) &&
        Def::EXPR_SPEC_SYMB.find(c) == std::string::npos) {
            throw UnexpectedSymbol{};
        }
        if (c == '(') {
            ++balance;
        }
        if (c == ')') {
            if (balance == 0) {
                throw UnexpectedSymbol{};
            }
            --balance;
        }
    }
    if (balance != 0) {
        throw UnexpectedEndOfString{};
    }
}


std::string_view GetToken(const std::string_view view, size_t& pos) {
    size_t prev_pos = pos;
    pos = view.find(' ', prev_pos) + 1;
    if (pos == 0) {
        pos = view.size();
        return {&view[prev_pos], view.size() - prev_pos};
    }
    return {&view[prev_pos], pos - prev_pos - 1};
}


void AddAnnotation(Def::AnnTable& annTable, const std::string_view src) {
    size_t idx = 0;
    size_t prev_idx = idx;
    std::vector<std::string> annotations;
    std::string annotName;
    while ((idx = src.find(' ', idx)) != std::string_view::npos) {
        std::string_view token(&src[prev_idx], idx - prev_idx);
        if (annotName.empty()) {
            annotName = std::string(token);
        } else {
            std::string_view token_stripped(&token[0], token.size() - 1);
            if (!IsPositiveNum(token_stripped)) {
                throw UnexpectedSymbol{};
            }
            annotations.emplace_back(token_stripped);
        }
        idx++;
        prev_idx = idx;
    }
    annotations.emplace_back(&src[prev_idx]);
    annotations.back().pop_back();
    if (annotName.empty() || annotations.empty()) {
        throw UnexpectedEndOfString{};
    }
    annTable[annotName] = {annotations};
}

void CheckNamingSyntax(const std::string& className) {
    if (!std::all_of(className.begin(), className.end(), [](char c){ return std::isalnum(c) || c == '_'; })) {
        throw UnexpectedSymbol{};
    }
}

void AnalyzeClassInPlace(const std::string& baseValue, std::string_view line,
std::unordered_map<std::string, std::string>& argsMapping, const std::string& currentType,
Def::TypeTable& typeTable, const Def::ClassTable& clTable) {
    std::string_view next_token;
    size_t idx = 0, i = 0;
    while (IsName(next_token = GetToken({&line[0]}, idx)) || next_token[0] == '{' || next_token.back() == '}') {
        std::string curType = FindClass(currentType, clTable)->values[i].first;
        if (next_token.back() == '}') {
            next_token = std::string_view(&next_token[0], next_token.size() - 1);
            if (!IsName(next_token)) {
                break;
            }
        }
        auto currentArgRepresentation = baseValue + Def::UNPACKING_SYMBOL + std::to_string(i);
        if (next_token[0] == '{') {
            auto rsymb = line.rfind('}');
            if (rsymb == std::string_view::npos) {
                throw UnexpectedSymbol{};
            }
            std::string_view subline{&line[idx + 1], rsymb - (idx + 1)};
            AnalyzeClassInPlace(currentArgRepresentation, subline, argsMapping, curType, typeTable, clTable);
            idx = rsymb + 1;
        } else {
            argsMapping[std::string(next_token)] = currentArgRepresentation;
            typeTable[currentArgRepresentation] = std::make_shared<Pod>(curType);
        }
        i++;
    }
}

void ClearLocalVars(Def::TypeTable& typeTable) {
    auto typeTableClone = typeTable;
    for (const auto& [name, type] : typeTableClone) {
        if (name.find('%') != std::string::npos) {
            typeTable.erase(name);
        }
    }
}

static std::shared_ptr<Type> GetLastFunType(std::shared_ptr<Type> funType) {
    while (funType->type == TermType::ARROW) {
        auto* arrow = dynamic_cast<Arrow*>(funType.get());
        funType = arrow->right;
    }
    return funType;
}

std::unique_ptr<AnnotatedExpression> GeneratePackedExpr(const std::string_view line,
                                               const std::unordered_map<std::string, std::string>& varPos,
                                               Def::TypeTable& typeTable,
                                               Def::ColorTable& colorTable,
                                               Color& color,
                                               std::shared_ptr<Type>& funType) {
    std::string_view next_token;
    size_t prev_pos = 1;
    size_t pos;
    auto* expr = new PackedExpression();
    expr->correspondingType = GetLastFunType(funType);
    if (line != "{}") {
        for (;;) {
            pos = line.find(';', prev_pos);
            if (pos == -1) {
                pos = line.size() - 1;
            }
            next_token = line.substr(prev_pos, pos - prev_pos);
            prev_pos = pos + 1;

            expr->exprs.push_back(BuildExpression(next_token, varPos, typeTable, colorTable, color));
            if (pos == line.size() - 1) {
                break;
            }
        }
    }
    auto result = std::make_unique<AnnotatedExpression>();
    (*result)[(size_t)-1].push_back(std::unique_ptr<PackedExpression>(expr));
    return result;
}
