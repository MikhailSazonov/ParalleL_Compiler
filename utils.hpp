#pragma once

#include <vector>
#include <string>
#include <optional>

void SyntaxError();

std::vector<std::string> Split(const std::string&, const std::string&);

std::vector<std::string> Tokenize(const std::string&, const std::string&);

std::optional<std::pair<size_t, size_t>> FindBrackets(const std::string&, size_t);

void Strip(std::string&, char symb = ' ');

void LeftStrip(std::string&, char symb = ' ');

void RightStrip(std::string&, char symb = ' ');

std::string_view Strip(const std::string_view);

std::string_view LeftStrip(const std::string_view, char symb = ' ');

std::string_view RightStrip(const std::string_view, char symb = ' ');

std::string_view RemoveBrackets(const std::string_view);

bool IsName(const std::string_view);

bool IsNum(const std::string_view);

bool IsPositiveNum(const std::string_view, bool allowZero = false);

bool IsBool(const std::string_view);

bool IsChar(const std::string_view);

bool IsFloat(const std::string_view);

bool IsString(const std::string_view);

bool IsVoid(const std::string_view);

bool IsNull(const std::string_view src);

bool IsMangledName(const std::string_view);

bool IsAnnotatedName(const std::string_view src);

bool IsOperator(const std::string_view);

std::pair<size_t, size_t> GetNextTokenPos(const std::string_view);

std::pair<size_t, size_t> GetLastTokenPos(const std::string_view);

std::string GenerateAnnotatedName(size_t, size_t);
