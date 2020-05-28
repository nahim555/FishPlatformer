#if !defined(_RENDERER_H)
#define _RENDERER_H

#include "../Object/Headers/Entity.h"
#include "../Object/Headers/Player.h"
#include "../Object/Headers/World.h"
#include "../Core/SpatialHash.h"
#include "Texture.h"
#include <windows.h>		// Header file for Windows
//#include <gl\gl.h>			// Header file for the OpenGL32 Library
//#include <gl\glu.h>			// Header file for the GLu32 Library
#include "../glew/include/GL/glew.h"

#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include "FreeType.h"

#pragma once

#define GET(Type, MemberName, FaceName) \
          Type Get##FaceName() const { \
             return MemberName; \
          }
#define SET(Type, MemberName, FaceName) \
            void Set##FaceName(const Type &value) { \
                MemberName = value; \
            }

class Renderer
{	
private:
	GLfloat targetWidth, targetHeight;
	GLfloat heightInfo, widthInfo;
	freetype::font_data our_font;
	Texture texture;
public:
	typedef std::pair<GLdouble, GLdouble> X;
	typedef std::pair<GLdouble, GLdouble> Y;

	GET(Texture, texture, TextureList)
	SET(Texture, texture, TextureList)
		GET(GLfloat, targetWidth, TargetWidth)
		SET(GLfloat, targetWidth, TargetWidth)
		GET(GLfloat, targetHeight, TargetHeight)
		SET(GLfloat, targetHeight, TargetHeight)
		SET(freetype::font_data, our_font, Font)

	Renderer(GLfloat targetW, GLfloat targetH);


	void display(Player*, std::vector<Entity>);						//called in winmain to draw everything to the screen	
	std::pair<X,Y> reshape(GLuint width, GLuint height, Player* p); //called when the window is resized
	void init();										//called in winmain when the program starts.									//called in winmain to update variables
	void drawEntity(Entity* entity);
	void drawEntity(Entity entity);			//draws an Entity (Player, NPC)
	void displayMenu(GLuint width, GLuint height, std::vector<Entity> entities, bool beatGame, Player* p);
	void drawBackground(GLuint bkgr);
};
#endif
