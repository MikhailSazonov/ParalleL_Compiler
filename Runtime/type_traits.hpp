#include "entity.hpp"


class IArithmetical : public Entity {
	virtual IArithmetical& operator+=();

	virtual IArithmetical& operator-=();

	virtual IArithmetical& operator*=();

	virtual IArithmetical& operator/=();
};

class ISumType : public Entity {

};

class Indexable : public Entity {

};