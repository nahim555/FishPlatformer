#include "../Object/Headers/Entity.h"

Entity::Entity()
	: coordinate(0, 0), length(1, 1), velocity(0.0f),
	textureID(0), dt(0.0f), destructible(false)
{ 
	static int id = 0;
	_id = id++;
}

Entity::Entity(glm::vec2 coords, glm::vec2 len, glm::vec2 vel,
	GLuint texID)
	: coordinate(coords), length(len), velocity(vel), textureID(texID), dt(0.0f) , destructible(false)
{ 
	static int id = 0;
	_id = id++;
}


bool operator==(const Entity &e1, const Entity &e2)
{
	return e1.GetObjID() == e2.GetObjID();
}


