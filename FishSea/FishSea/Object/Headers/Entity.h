#if !defined(_ENTITY_H)
#define _ENTITY_H

#include "../View/Headers/Texture.h"
#include <vector>
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include <iostream>
#include "../../glew/include/GL/glew.h"


#define GET(Type, MemberName, FaceName) \
          Type Get##FaceName() const { \
             return MemberName; \
          }
#define SET(Type, MemberName, FaceName) \
            void Set##FaceName(const Type &value) { \
                MemberName = value; \
            }

class Entity {
public:
	//CONSTRUCTORS
	Entity();
	Entity(glm::vec2 coords, glm::vec2 len, glm::vec2 vel, GLuint texID);

	typedef glm::vec2 GridCoordinates;

	//Getters
	GET(glm::vec2, coordinate, Coordinate)
	GET(glm::vec2, length, Length)
	GET(std::vector<GridCoordinates>, grids, Grids)
	GET(bool, nU, N_up)
	GET(bool, nD, N_down)
	GET(bool, nL, N_left)
	GET(bool, nR, N_right)
	GET(GLuint, textureID, TextureID)
	GET(bool, destructible, Destructible)
		GET(int, _id, ObjID)

	SET(glm::vec2, coordinate, Coordinate)
	SET(glm::vec2, length, Length)
	SET(std::vector<GridCoordinates>, grids, Grids)
	SET(bool, nU, N_up)
	SET(bool, nD, N_down)
	SET(bool, nL, N_left)
	SET(bool, nR, N_right)
	SET(GLuint, textureID, TextureID)
	SET(bool, destructible, Destructible)

	friend bool operator==(const Entity &e1, const Entity &e2);

protected:
	int _id;
	float dt;
	bool destructible;
	bool nU, nD, nR, nL;
	glm::vec2 coordinate, length, velocity;
	GLuint textureID;
	std::vector<GridCoordinates> grids;
};



#endif  //_ENTITY_H
