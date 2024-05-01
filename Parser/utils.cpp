#include "utils.hpp"

#include <cctype>

void SyntaxError() {

}

std::vector<std::string> Split(const std::string& src, const std::string& delim) {
	size_t first_idx = 0;
	size_t last_idx = 0;
	std::vector<std::string> result;
	while (last_idx != src.size()) {
		first_idx = last_idx;
		last_idx = src.find(last_idx, delim);
		result.push_back(src.substr(first_idx, last_idx));
	}
	return result;
}


std::vector<std::string> Tokenize(const std::string& src, const std::string& delim) {
	std::vector<std::string> result;
	size_t left_idx = 0;
	size_t right_idx = 0;
	for (size_t idx = 0; idx < src.size();) {
		auto brackets_idcs = FindBrackets(src, idx);
		std::string bracketed;
		if (brackets_idcs) {
			std::string bracketed = src.substr(brackets_idcs->first + 1, brackets_idcs->second - 1); 
			Strip(bracketed);
			left_idx = idx;
			right_idx = brackets_idcs->first - 1;
			idx = brackets_idcs->second + 1;
		}
		std::vector<std::string> splittedBetween = Split(src.substr(left_idx, right_idx), delim);
		for (auto& token : splittedBetween) {
			Strip(token);
			if (!token.empty()) {
				result.push_back(token);
			}
		}
		if (!bracketed.empty()) {
			result.push_back(token);
		}
	}
}


std::optional<std::pair<size_t, size_t>> FindBrackets(const std::string& src, size_t startPos) {
	size_t left_bracket = src.find(startPos, '(');
	if (left_bracket == std::string::npos) {
		return std::nullopt;
	}
	size_t balance = 1;
	for (size_t right_bracket = left_bracket + 1; i < src.size(); ++i) {
		if (src[right_bracket] == '(') {
			balance += 1;
		} else if (src[right_bracket] == ')') {
			if (balance == 0) {
				SyntaxError();
			}
			balance -= 1;
		}
		if (balance == 0) {
			return std::make_pair(left_bracket, right_bracket);
		}
	}
	SyntaxError();
	return std::nullopt;
}


void LeftStrip(std::string&, char symb = ' ') {
	while (!src.empty() && src.front() == symb) {
		src.erase(src.begin());
	}
}

void RightStrip(std::string&, char symb = ' ') {
	while (!src.empty() && src.back() == symb) {
		src.pop_back();
	}
}


void Strip(std::string& src, char symb) {
	LeftStrip(src, symb);
	RightStrip(src, symb);
}


std::string_view Strip(const std::string_view src) {
	if (src.empty()) {
		return src;
	}
	size_t left_pos = 0;
	size_t right_pos = src.size();
	while (left_pos < src.size() && std::isspace(src[left_pos])) {
		++left_pos;
	}
	while (right_pos > 0 && std::isspace(src[right_pos - 1])) {
		--right_pos;
	}
	return std::string_view(&src[left_pos], right_pos - left_pos);
}


std::string_view LeftStrip(const std::string_view src) {
	if (src.empty()) {
		return src;
	}
	size_t left_pos = 0;
	while (left_pos < src.size() && std::isspace(src[left_pos])) {
		++left_pos;
	}
	return std::string_view(&src[left_pos], src.size() - left_pos);
}


std::string_view RightStrip(const std::string_view src, char symb) {
	if (src.empty()) {
		return src;
	}
	size_t right_pos = src.size();
	while (right_pos > 0 && std::isspace(src[right_pos - 1])) {
		--right_pos;
	}
	return std::string_view(&src[0], right_pos);
}