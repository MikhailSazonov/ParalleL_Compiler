#include <unordered_map>

#include "entity.hpp"
#include "../CompilerErrors/base_error.hpp"

struct Context {
	std::unordered_map<std::string, EntityP> entity_table;

	void AddEntity(const std::string& name, EntityP& entity);
};

