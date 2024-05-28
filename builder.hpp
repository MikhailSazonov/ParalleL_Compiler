#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "expression.hpp"
#include "type.hpp"
#include "definitions.hpp"

void SyntaxError();

std::unique_ptr<AnnotatedExpression> BuildExpression(const std::string_view,
                                   const std::unordered_map<std::string, std::string>&,
                                   Def::TypeTable&, Def::ColorTable&, Color&);

std::unique_ptr<Expression> BuildExpressionImpl(const std::string_view,
                                       const std::unordered_map<std::string, std::string>&,
                                       AnnotatedExpression&,
                                       Def::TypeTable&, Def::ColorTable&,
                                       const std::string_view,
                                       Color&);

std::unique_ptr<Type> BuildType(const std::string_view, const Def::ClassTable&);

std::shared_ptr<Type> BuildTypeForTemplate(const std::string_view, const Def::TypeTable&,
                                           const std::unordered_map<std::string, std::string>&,
                                           std::shared_ptr<Type>, Def::ResolveTable&);

Type* LoadArgs(Def::TypeTable& typesTable, size_t, Type*);

//std::pair<size_t, size_t> GetNextTokenPos(const std::string_view);
//
//std::pair<size_t, size_t> GetLastTokenPos(const std::string_view);
