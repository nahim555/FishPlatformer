#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#include "../../glew/include/GL/glew.h"
#include <unordered_map>
//#include "tgaload.h"

#define GET(Type, MemberName, FaceName) \
          Type Get##FaceName() const { \
             return MemberName; \
          }
#define SET(Type, MemberName, FaceName) \
            void Set##FaceName(const Type &value) { \
                MemberName = value; \
            }

class Texture {
private:
public:
	Texture();
	std::unordered_map<std::string, GLuint> textures;
};

#endif  //_TEXTURE_H
