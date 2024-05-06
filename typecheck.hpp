#pragma once

#include <memory>
#include <unordered_map>

#include "type.hpp"
#include "expression.hpp"
#include "definitions.hpp"

std::shared_ptr<Type> GetTermType(const Def::TypeTable&,
                                  const Expression&);


void CheckType(Def::TypeTable&, const std::string&,
               size_t, Type*, const Expression&);

