#include "builder.hpp"

#include "utils.hpp"

#include <cctype>
#include <algorithm>


// Expression BuildExpression(const std::string& str) {
// 	std::vector<std::string> tokens = ParseExpression(str);
// }


std::unique_ptr<Type> BuildType(const std::string_view view) {
	std::string_view space_stripped = Strip(view);

	if (IsName(stripped)) {
		return std::make_unique<Var>(next_token);
	}

	auto next_token_pos = GetNextTokenPos(stripped);
	std::string_view next_token(&stripped[next_token_pos.first], next_token_pos.second);

	size_t delimeter_pos = view.find(next_token.first + next_token_pos.second + 1, TYPE_DELIMETER);

	if (delimeter_pos == std::string_view::npos) {
		return BuildType(next_token);
	}

	return std::make_unique<Arrow>(*BuildType(next_token), 
		*BuildType(std::string_view(&stripped[delimeter_pos + TYPE_DELIMETER.size()])));
}


std::pair<size_t, size_t> GetNextTokenPos(const std::string_view view) {
	if (view[0] == '(') {
		size_t i = 1;
		for (; i < view.size() && view[i] != ')'; ++i) {}
		if (i == view.size()) {
			SyntaxError();
		}
		return std::make_pair(1, i - 1);
	}
	return std::make_pair(0, std::min(view.find(' '), view.size()));
}


bool IsName(const std::string_view src) {
	return std::all_of(src.begin(), src.end(), [](char c){ return std::isalpha(c) });
}
