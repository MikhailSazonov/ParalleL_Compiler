#pragma once

#include "definitions.hpp"
#include "base_lib/base.hpp"
#include "expression.hpp"

#include <unordered_set>
#include <vector>
#include <deque>

namespace DataRace {
//    enum class ResourceType {
//        INPUT,
//        CONST
//    };
//
//    struct Resource {
//        Resource(ResourceType type) : type(type) {}
//
//        const ResourceType type;
//    };
//
//    struct InputResource : public Resource {
//        InputResource() : Resource(ResourceType::INPUT) {}
//    };
//
//    struct ConstResource : public Resource {
//        const std::string value;
//
//        ConstResource(const std::string& value) : Resource(ResourceType::CONST),
//            value(value)
//        {}
//    };

    const std::string INPUT_PREFIX = "INPUT#";
    const std::string CONST_PREFIX = "CONST#";

    typedef std::unordered_map<std::string, BaseLib::Resources> ResourcesContainer;

    struct ResourcesGroups {
        std::unordered_set<std::string> unsortedResources;
        std::vector<std::unordered_set<std::string>> sortedGroups;
    };

    void DataRaceCheck(const Def::FuncTable&, const Def::TypeTable&, const Def::ClassTable&);

    BaseLib::Resources CheckFunc(const std::string&, const std::vector<FuncDef>&,
                   const Def::FuncTable&, const Def::TypeTable&, const Def::ClassTable&,
                    const std::deque<std::shared_ptr<Expression>>& args);

    BaseLib::Resources CheckAnnExpr(const AnnotatedExpression&,
                      const Def::FuncTable&, const Def::TypeTable&, const Def::ClassTable&,
                      const std::string&, bool, const std::deque<std::shared_ptr<Expression>>&);

    BaseLib::Resources CheckExpr(const Expression*,
                                 const Def::FuncTable&, const Def::TypeTable&, const Def::ClassTable&,
                                 const std::deque<std::shared_ptr<Expression>>&);

    void FillExpr(const Expression*,
               const Def::FuncTable&, const Def::TypeTable&,
               const Def::ClassTable&,
               std::string&,
               std::deque<std::string>&);

    BaseLib::Resources ReplaceResources(const BaseLib::Resources&, const std::deque<std::string>&);

    BaseLib::Resources CompareResources(const BaseLib::Resources&, const BaseLib::Resources&,
                    bool, const std::string&);

    std::string GetFunName(const Expression*);


    // COLORS
    void CheckForColors(const Def::ColorTable&, const AnnotatedExpression&);
}
