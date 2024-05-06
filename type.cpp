#include "type.hpp"

#include <stdexcept>


bool Pod::operator==(const Type& other) const {
    return type == other.type && typeName == dynamic_cast<const Pod*>(&other)->typeName;
}


bool Pod::operator!=(const Type& other) const {
    return !(*this == other);
}


bool Arrow::operator==(const Type& other) const {
    if (type != other.type) {
        return false;
    }
    const Arrow* otherArrow = dynamic_cast<const Arrow*>(&other);
    return (*left) == (*otherArrow->left) && (*right) == (*otherArrow->right);
}


bool Arrow::operator!=(const Type& other) const {
    return !(*this == other);
}
