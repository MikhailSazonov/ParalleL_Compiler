#include "builder.hpp"

#include "utils.hpp"

Expression BuildExpression(const std::string& str) {
	std::vector<std::string> tokens = ParseExpression(str);
}

std::unique_ptr<Type> BuildType(const std::string& str) {
	std::vector<std::string> tokens = ParseType(str);
	if (tokens.empty()) {
		SyntaxError();
	}
	if (tokens.size() == 1) {
		StripLeft(tokens[0], ')');
		StripRight(tokens[0], '(');
		return std::make_unique<Var>(tokens[0]);
	}
	std::unique_ptr<Type> result = std::make_unique<Arrow>();
	result->left = BuildType();
	return result;
}

std::vector<std::string> ParseExpression(const std::string& str) {
	return Tokenize(str, EXPR_DELIMETER);
}

std::vector<std::string> ParseType(const std::string& str) {
	return Tokenize(str, TYPE_DELIMETER);
}