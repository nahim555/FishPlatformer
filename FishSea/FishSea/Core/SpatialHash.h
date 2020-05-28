#if !defined(_SPATIALHASH_H)
#define _SPATIALHASH_H

#include <unordered_map>
#include <map>
#include "../Object/Headers/Entity.h"
#include <vector>
#include <algorithm>
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include <memory>


#define GET(Type, MemberName, FaceName) \
          Type Get##FaceName() const { \
             return MemberName; \
          }
#define SET(Type, MemberName, FaceName) \
            void Set##FaceName(const Type &value) { \
                MemberName = value; \
            }

struct SimpleHash {
	size_t operator()(const glm::vec2& p) const {
		return (unsigned int)p.x ^ (unsigned int)p.y; 
	}
};

class SpatialHash {
public:
	typedef glm::vec2 GridCoordinates;
	typedef std::vector<Entity> Entities;
	typedef std::unordered_map<GridCoordinates, Entities, SimpleHash> Grid;
	//CONSTRUCTORS
	SpatialHash();
	SpatialHash(float wWidth, float wHeight, int cellSize);


	void add(Entity &e), remove(Entity e), clear();
	std::vector<Entity> collect(float camX, float camY, float camXWidth, float camYHeigh);
private:


	Grid grid;
	const int CELL_SIZE;
	float camX, camY, camXWidth, camYHeight, worldWidth, worldHeight;
};



#endif  //_SPATIALHASH_H