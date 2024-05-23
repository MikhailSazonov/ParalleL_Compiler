#pragma once

#include <memory>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

enum class TermType {
    POD,
    ARROW,
    ABSTRACT
};

struct Type;

namespace Def {
    typedef std::unordered_map<std::string, std::shared_ptr<Type>> ResolveTable;
}

struct Type {
    const TermType type;

    Type(TermType type) : type(type) {}

    virtual ~Type() = default;

    virtual bool operator==(const Type&) const = 0;

    virtual bool operator!=(const Type&) const;
};

struct Pod : public Type {
    std::string typeName;

    Pod(const std::string& name) : Type(TermType::POD), typeName(name) {}

    bool operator==(const Type&) const;
};


struct Abstract : public Type {
    const std::string abstractName;

    Abstract(const std::string& name) : Type(TermType::ABSTRACT), abstractName(name) {}

    bool operator==(const Type&) const;
};


struct Arrow : public Type {
    Arrow() : Type(TermType::ARROW) {}

    Arrow(Type* left, Type* right)
            :
            Type(TermType::ARROW)
            , left(left)
            , right(right)
    {}

    Arrow(const std::shared_ptr<Type>& left, const std::shared_ptr<Type>& right)
            :
            Type(TermType::ARROW)
            , left(left)
            , right(right)
    {}

    std::shared_ptr<Type> left;
    std::shared_ptr<Type> right;

    bool operator==(const Type&) const;
};

std::shared_ptr<Type> ResolveAbstractTypes(std::shared_ptr<Type>, Def::ResolveTable&);

void GatherAbstractTypes(Type*, std::unordered_set<std::string>&);

bool HasAbststractTypes(Type*);

std::string GetVarByNo(Type*, size_t);