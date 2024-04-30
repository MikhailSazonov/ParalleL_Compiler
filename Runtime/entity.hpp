#include <string>
#include <vector>
#include <memory>


class Entity;


using std::unique_ptr<Entity> = EntityP;


class Entity {
	std::string str;

	virtual EntityP compute() = 0;
};


class Variable : public Entity {

};