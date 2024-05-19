#include "oop.hpp"

bool ClassIsPresent(const std::string& name, const Def::ClassTable& classTable) {
    return std::any_of(classTable.begin(), classTable.end(), [&name](const Class& cl) {
        return cl.name == name;
    });
}

const Class* FindClass(const std::string& name, const Def::ClassTable& classTable) {
    for (const auto& nextClass : classTable) {
        if (nextClass.name == name) {
            return &nextClass;
        }
    }
    return nullptr;
}
