#include "system.hpp"

void Context::AddEntity(const std::string& name, EntityP& entity) {
	if (entity_table.contains(name)) {
		throw NameDuplicatingError().SetNameAndStr(name, entity_table[name]->str);
	}
	entity_table[name] = entity;
}
