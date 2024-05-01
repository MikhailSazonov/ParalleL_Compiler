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
