#if !defined(_NPC_H)
#define _NPC_H

#include "../Object/Headers/Entity.h"

class NPC : public Entity {
private:
	bool isAlly;
public:
	NPC();
	NPC(glm::vec2 coords, glm::vec2 len, glm::vec2 vel, GLuint texID, bool ally);
};

#endif  //_NPC_H
