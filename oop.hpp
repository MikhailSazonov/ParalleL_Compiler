#pragma once

#include <string>
#include <vector>
#include <utility>
#include <set>

struct Class {
    std::string name;
    std::vector<std::pair<std::string, std::string>> values;
    bool opened{true};

    Class(const std::string& name) : name(name) {}
};

namespace Def {
    typedef std::vector<Class> ClassTable;
}

bool ClassIsPresent(const std::string&, const Def::ClassTable&);

const Class* FindClass(const std::string& name, const Def::ClassTable& classTable);