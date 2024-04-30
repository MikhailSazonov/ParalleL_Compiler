#pragma once

#include "string"

class BaseError {
	public:
		virtual const std::string& reason();
};

class FileNotFoundError : public BaseError {
	std::string filename;

	public:
		BaseError& SetFilename(std::string newFilename) {
			filename = newFilename;
			return *this;
		}

		const std::string& reason() {
			return "File " + filename + " not found.";
		}
}
