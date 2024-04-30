#include "analyze.hpp"

std::unique_ptr<Expression> ParseExpression(std::string line) {
	int next_pos = 0;
	int prev_pos = 0;
	std::string syntax_element = "";
	while (next_pos != line.size()) {
		prev_pos = next_pos;
		next_pos = line.find(prev_pos, ' ');
		syntax_element = line.substr(prev_pos, next_pos);
		if (syntax_element.empty()) {
			continue;
		}
	}
}