#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "type.hpp"
#include "funcdef.hpp"
#include "definitions.hpp"

void Analyze(Def::TypeTable&, Def::FuncTable&,
             Def::AnnTable&, Def::ClassTable&,
             const std::string&, bool,
             std::unordered_set<std::string>&);

int Parse(Def::TypeTable&, Def::FuncTable&, Def::AnnTable&, Def::ClassTable&, const std::string&);

void AddAnnotation(Def::AnnTable&, const std::string_view);

void CheckTypeSyntax(const std::string_view);

void CheckExprSyntax(const std::string_view);

void CheckNamingSyntax(const std::string& className);

void AnalyzeClassInPlace(const std::string&, std::string_view, std::unordered_map<std::string, std::string>&,
        const std::string&, Def::TypeTable&, const Def::ClassTable&);

std::string_view GetToken(const std::string_view, size_t&);

void ClearLocalVars(Def::TypeTable&);

std::unique_ptr<AnnotatedExpression> GeneratePackedExpr(const std::string_view,
                                               const std::unordered_map<std::string, std::string>&,
                                               Def::TypeTable&,
                                               std::shared_ptr<Type>&);