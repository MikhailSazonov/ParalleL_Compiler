#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "type.hpp"
#include "funcdef.hpp"
#include "definitions.hpp"

void Analyze(Def::TypeTable&, Def::FuncTable&, Def::AnnTable&, const std::string&);

int Parse(Def::TypeTable&, Def::FuncTable&, Def::AnnTable&, const std::string&);

void AddAnnotation(Def::AnnTable&, const std::string_view);

void CheckTypeSyntax(const std::string_view);

void CheckExprSyntax(const std::string_view);

std::string_view GetToken(const std::string_view, size_t&);