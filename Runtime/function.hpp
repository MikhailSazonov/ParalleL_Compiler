#include "entity.hpp"

#include <vector>

struct Signature {
	std::vector<EntityP> types;
};

class Function : public Entity {
	std::vector<EntityP> args;

	EntityP compute();
};
