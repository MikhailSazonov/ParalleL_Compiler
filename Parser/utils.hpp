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
