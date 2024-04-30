#include "../CompilerErrors/base_error.hpp"
#include "../System/system.hpp"

#include <fstream>

Context Parse(const std::string& filename) {
	std::ifstream fstr(filename);
	if (!fstr) {
		throw FileNotFoundError().SetFilename(filename);
	}
	std::string next_line;
	Context ctx;
	while (std::getline(fstr, next_line)) {
		Analyze(fstr, ctx, next_line);
	}
	return ctx;
}

void Analyze(Context& ctx, std::ifstream& fstr, const std::string& line) {
	
}