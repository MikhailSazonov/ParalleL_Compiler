#pragma once

#include "definitions.hpp"
#include "base_lib/base.hpp"

#include <unordered_set>
#include <vector>

typedef std::unordered_map<std::string, BaseLib::Resources> ResourcesContainer;

struct ResourcesGroups {
    std::unordered_set<std::string> unsortedResources;
    std::vector<std::unordered_set<std::string>> sortedGroups;
};

void DataRaceCheck(const Def::FuncTable&);

void DataRaceCheckFunc(const std::string&, const Def::FuncTable&, ResourcesContainer&);

BaseLib::Resources GetResources(const std::string&, const Def::FuncTable&);
