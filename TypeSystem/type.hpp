#pragma once

#include <vector>
#include <memory>
#include <string>


enum class TermType {
	VAR,
	ARROW
};


struct Type {
	const TermType type;

	virtual ~Type() = 0;

	virtual bool operator==(const Type&) = 0;

	virtual bool operator!=(const Type&) = 0;

	virtual const Type& Get(size_t) = 0;
};


struct Var : public Type {
	const std::string typeName;

	Var(const std::string& name) : type(TermType.VAR), typeName(name) {}

	bool operator==(const Type&);

	bool operator!=(const Type&);

	const Type& Get(size_t);
};


// struct List : public Var {

// };


struct Arrow : public Type {
	Arrow() : code(TermType.ARROW) {}

	std::unique_ptr<Type> left;
	std::unique_ptr<Type> right;

	bool operator==(const Type&);

	bool operator!=(const Type&);

	const Type& Get(size_t);
};
