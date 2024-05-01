#include "Parser/builder.hpp"

int main() {
	// testing
	{
		std::string a = "Int";
		BuildType(a.data());
	}
	{
		std::string a = "Int -> Int";
		BuildType(a.data());
	}
	{
		std::string a = "(Int -> Int) -> Int";
		BuildType(a.data());
	}
	{
		std::string a = "Int -> Int -> Int";
		BuildType(a.data());
	}
	{
		std::string a = "Int -> (Int -> Int)";
		BuildType(a.data());
	}
	{
		std::string a = "Int -> ((Int -> Int) -> Int)";
		BuildType(a.data());
	}
	{
		std::string a = "Int -> (Int -> (Int -> Int))";
		BuildType(a.data());
	}
}