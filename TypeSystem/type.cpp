#include "types.hpp"

#include <stdexcept>


bool Var::operator==(const Type& other) {
	return type == other.type && typeName == dynamic_cast<Var*>(&other)->typeName;
}


bool Var::operator!=(const Type& other) {
	return !(*this == other);
}


const Type& Var::Get(size_t idx) {
	if (idx > 0) {
		throw std::runtime_error("Wrong Get() on var: idx > 0");
	}
	return *this;
}


bool Arrow::operator==(const Type& other) {
	if (type != other.type) {
		return false;
	}
	Arrow* otherArrow = dynamic_cast<Arrow*>(&other);
	return left == otherArrow->left && right == otherArrow->right;
}


bool Arrow::operator!=(const Type& other) {
	return !(*this == other);
}


const Type& Arrow::Get(size_t idx) {
	if (idx == 0) {
		return *left;
	}
	return right->Get(idx - 1);
}
