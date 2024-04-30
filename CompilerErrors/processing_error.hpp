#pragma once

#include "base_error.hpp"

class ProcessingError : public BaseError {

};


class NameDuplicatingError : public ProcessingError {
	private:
		std::string name;
		std::string line;

	public:
		const NameDuplicatingError& SetNameAndStr(const std::string& newName, const std::string& newLine) {
			name = newName;
			line = newLine;
			return *this;
		}

		const std::string& reason() {
			return "Name duplicated: " + name + " already declared at line " + line;
		}
};
