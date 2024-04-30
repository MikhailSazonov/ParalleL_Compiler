#pragma once

#include <string>
#include <vector>

class Context;

Context Parse(std::string& filename);

void Analyze(Context& ctx, std::ifstream& fstr, const std::string& line);

std::vector<std::string> ExtractLines(std::ifstream& stream);

enum class ParserState {
	NONE,
	FUNCTION
};

ParserState parserState;
