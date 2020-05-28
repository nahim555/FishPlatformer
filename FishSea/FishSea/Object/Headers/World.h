#if !defined(_WORLD_H)
#define _WORLD_H

#include "../Object/Headers/Entity.h"
#include <vector>

class World {
private:
    int numberOfEntities = 0;
	std::vector<Entity> entities;
public:
	World();
	inline std::vector<Entity> getEntities();
	inline void addEntity(Entity entity);
};

inline std::vector<Entity> World::getEntities()
{
	return entities;
}
inline void World::addEntity(Entity entity)
{
	entities.push_back(entity);
}



#endif  //_WORLD_H
