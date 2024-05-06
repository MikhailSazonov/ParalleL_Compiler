#pragma once

#include <vector>
#include <memory>
#include <string>


enum class TermType {
    POD,
    ARROW
};


struct Type {
    const TermType type;

    Type(TermType type) : type(type) {}

    virtual ~Type() = default;

    virtual bool operator==(const Type&) const = 0;

    virtual bool operator!=(const Type&) const = 0;
};


struct Pod : public Type {
    const std::string typeName;

    Pod(const std::string& name) : Type(TermType::POD), typeName(name) {}

    bool operator==(const Type&) const;

    bool operator!=(const Type&) const;
};


struct Arrow : public Type {
    Arrow() : Type(TermType::ARROW) {}

    Arrow(Type* left, Type* right)
            :
            Type(TermType::ARROW)
            , left(left)
            , right(right)
    {}

    std::shared_ptr<Type> left;
    std::shared_ptr<Type> right;

    bool operator==(const Type&) const;

    bool operator!=(const Type&) const;
};
