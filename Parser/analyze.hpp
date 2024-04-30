#pragma once

#include <string>
#include <memory>

#include "../TypeSystem/types.hpp"


std::unique_ptr<Expression> ParseExpression(std::string line);