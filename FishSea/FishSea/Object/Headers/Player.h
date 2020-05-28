#if !defined(_PLAYER_H)
#define _PLAYER_H

#include "../Object/Headers/Entity.h"
#include <windows.h>
#include <irrklang/irrKlang.h>
using namespace irrklang;

class Player : public Entity {
public:
	Player();
	Player(glm::vec2 coords, glm::vec2 len, glm::vec2 vel, GLuint texID, float jumpH, int sc, int life, ISoundEngine* sound);
	//~Player();

	enum JumpState {
		ON_GROUND,
		JUMPING,
		FALLING
	};

	enum Direction {
		UP,
		RIGHT,
		DOWN,
		LEFT,
		NONE
	};

	enum Moving {
		MLEFT,
		MRIGHT,
		OTHER
	};

	typedef Direction Horizontal;//For horizontal collision (Up or Down)
	typedef Direction Vertical;//For vertical collision (Left or Right)

	typedef std::pair<Horizontal, Vertical> CollisionSides;

	GET(Texture, t, Textures)
	SET(Texture, t, Textures)
	GET(int, score, Score)
	SET(int, score, Score)
		GET(int, lives, Lives)
		SET(int, lives, Lives)

	bool keys[256];

	bool checkCollision(Entity &second);
	void collisionSide(Entity &e);
	void resetCollisions(), processKeys(), moveRight(), moveLeft(), jump(), checkJumpState(float dt);
private:
	ISoundEngine* soundEng;
	Texture t;
	CollisionSides collision;
	JumpState jstate;
	Moving moving;
	float initialCoordY, jumpHeight, seconds_on_ground;
	int score, lives;
	glm::vec2 initialVelocity;
};



#endif  //_PLAYER_H


