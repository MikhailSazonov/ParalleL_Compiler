#include "type.hpp"

#include <stdexcept>

#include "errors.hpp"

bool Type::operator!=(const Type& other) const {
    return !(*this == other);
}

bool Pod::operator==(const Type& other) const {
    return type == other.type && typeName == dynamic_cast<const Pod*>(&other)->typeName;
}

bool Abstract::operator==(const Type& other) const {
    return type == other.type && abstractName == dynamic_cast<const Abstract*>(&other)->abstractName;
}

bool Arrow::operator==(const Type& other) const {
    if (type != other.type) {
        return false;
    }
    const Arrow* otherArrow = dynamic_cast<const Arrow*>(&other);
    return (*left) == (*otherArrow->left) && (*right) == (*otherArrow->right);
}

void GatherAbstractTypes(Type* type, std::unordered_set<std::string>& absTypeNames) {
    if (type->type == TermType::ABSTRACT) {
        auto* abs = dynamic_cast<Abstract*>(type);
        absTypeNames.insert(abs->abstractName);
        return;
    } else if (type->type == TermType::POD) {
        return;
    }
    auto arrow = dynamic_cast<Arrow*>(type);
    GatherAbstractTypes(arrow->left.get(), absTypeNames);
    GatherAbstractTypes(arrow->right.get(), absTypeNames);
}

bool HasAbststractTypes(Type* type) {
    if (type->type == TermType::ABSTRACT) {
        return true;
    } else if (type->type == TermType::POD) {
        return false;
    }
    auto arrow = dynamic_cast<Arrow*>(type);
    return HasAbststractTypes(arrow->left.get()) || HasAbststractTypes(arrow->right.get());
}

std::shared_ptr<Type> ResolveAbstractTypes(std::shared_ptr<Type> absType, Def::ResolveTable& resTable) {
    if (absType->type == TermType::POD) {
        return absType;
    }
    if (absType->type == TermType::ABSTRACT) {
        auto* abs = dynamic_cast<Abstract*>(absType.get());
        if (!resTable.contains(abs->abstractName)) {
            throw AbstractTypeUnresolved{};
        }
        return resTable[abs->abstractName];
    }
    auto* arr = dynamic_cast<Arrow*>(absType.get());
    return std::make_shared<Arrow>(ResolveAbstractTypes(arr->left, resTable),
                                   ResolveAbstractTypes(arr->right, resTable));
}
