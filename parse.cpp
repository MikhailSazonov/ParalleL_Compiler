#include "parse.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "builder.hpp"
#include "typecheck.hpp"
#include "definitions.hpp"

#include <cctype>
#include <fstream>
#include <iostream>

int Parse(Def::TypeTable & typeTable,
          Def::FuncTable& defTable,
          Def::AnnTable& annTable,
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
    while (std::getline(file, line)) {
        try {
            Analyze(typeTable, defTable, annTable, line);
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
             const std::string& line) {
    if (line.empty() || line.starts_with("%%")) {
        return;
    }
    size_t idx = 0;
    auto name = Strip(GetToken(line, idx));
    if (!IsName(name)) {
        throw ImproperName{};
    }
    if (name == Def::ANNOTATE_KEYWORD) {
        AddAnnotation(annTable, {&line[idx]});
        return;
    }
    for (; idx < line.size() && std::isspace(line[idx]); ++idx) {}
    if (idx == line.size()) {
        throw UnexpectedEndOfString{};
    }
    if (line[idx] == ':') {
        if (typeTable.contains(std::string(name))) {
            throw DoubleDefinition{};
        }
        std::string_view typeDef(&line[idx + 1]);
        CheckTypeSyntax(Strip(typeDef));
        typeTable[std::string(name)] = BuildType(typeDef);
        return;
    } else if (std::isalpha(line[idx]) || line[idx] == '=') {
        std::unordered_map<std::string, size_t> argsMapping;
        if (line[idx] != '=' && !typeTable.contains(std::string(name))) {
            throw UndefinedVariableError{};
        }
        std::shared_ptr<Type> currentType = typeTable[std::string(name)];
        std::string_view next_token;
        size_t currentArgN = 0;
        while (IsName(next_token = GetToken({&line[0]}, idx))) {
            argsMapping[std::string(next_token)] = currentArgN++;
        }
        if (next_token != "=") {
            throw UnexpectedSymbol{};
        }
        size_t whereKeyword = line.find(Def::WHERE_KEYWORD);
        std::string_view expr(&line[idx], whereKeyword == std::string::npos ?
                    line.size() - idx :
                    whereKeyword - idx);
        CheckExprSyntax(Strip(expr));
        auto typedExpr = BuildExpression(expr, argsMapping);
        // support syntactic sugar : x = 5 (no type presented)
        if (typedExpr->type == ExpressionType::LITERAL && !typeTable.contains(std::string(name))) {
            if (defTable.contains(std::string(name))) {
                throw DoubleDefinition{};
            }
            auto* constant = dynamic_cast<ConstExpression*>(typedExpr.get());
            typeTable[std::string(name)] = constant->correspondingType;
        } else {
            CheckType(typeTable, std::string(name), currentArgN, typeTable[std::string(name)].get(), *typedExpr);
        }
        std::unique_ptr<Expression> cond;
        if (whereKeyword != std::string::npos) {
            cond = BuildExpression({&line[whereKeyword + Def::WHERE_KEYWORD.size()]}, argsMapping);
            if (*GetTermType(typeTable, *cond) != Pod("Bool")) {
                throw TypeMismatchError{};
            }
        }
        if (!defTable[std::string(name)].empty() && defTable[std::string(name)].back().cond == nullptr) {
            throw DefaultAlreadyDeclared{};
        }
        defTable[std::string(name)].push_back({std::move(cond), std::move(typedExpr), currentArgN});
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
