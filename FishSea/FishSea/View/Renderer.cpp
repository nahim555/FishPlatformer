
#include "../View/Headers/Renderer.h"

Renderer::Renderer(GLfloat width, GLfloat height) : targetWidth(width), targetHeight(height)
{
	
}

void Renderer::display(Player* p, std::vector<Entity> entities)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	drawBackground(texture.textures["background"]);

	drawEntity(p);

	for (Entity e : entities)
	{
		drawEntity(e);
	}
	
	freetype::print(our_font, 25.0f, heightInfo - 40.0f, "Score: %i", p->GetScore());
	freetype::print(our_font, 250.0f, heightInfo - 40.0f, "Lives: %i", p->GetLives());

	if(p->GetLives() < 1) freetype::print(our_font, widthInfo / 2.0f, heightInfo / 2.0f, "YOU DIED");

	glFlush();

}

void Renderer::displayMenu(GLuint currentWidth, GLuint currentHeight, std::vector<Entity> entities, bool beatGame, Player* p)
{
	GLfloat w = (GLfloat)currentWidth / targetWidth;
	GLfloat h = (GLfloat)currentHeight / targetHeight;
	const float c = 200.0f;
	GLdouble camX = 0;
	GLdouble camXWidth = 0 + c * w;
	GLdouble camY = 0;
	GLdouble camYHeight = 0 + c *h;


	glViewport(0, 0, currentWidth, currentHeight);						// Reset the current viewport

	glMatrixMode(GL_PROJECTION);						// select the projection matrix stack
	glLoadIdentity();									// reset the top of the projection matrix to an identity matrix

	gluOrtho2D(0, currentWidth, 0, currentHeight);           // set the coordinate system for the window

	glMatrixMode(GL_MODELVIEW);							// Select the modelview matrix stack
	glLoadIdentity();									// Reset the top of the modelview matrix to an identity matrix

	

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture.textures["menubg"]);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glPushMatrix();
	
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); 													glVertex2f(0, 0);
	glTexCoord2f(0.0, (GLfloat)targetHeight / 512);								glVertex2f(0, currentHeight);
	glTexCoord2f((GLfloat)targetWidth / 512, (GLfloat)targetHeight / 512);		glVertex2f(currentWidth, currentHeight);
	glTexCoord2f((GLfloat)targetWidth / 512, 0.0);								glVertex2f(currentWidth, 0);


	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glColor3f(0.0, 0.0, 0.0);

	if (beatGame) freetype::print(our_font, 0.0f, currentHeight - 50.0f, "Congratulations! Your score is %i", p->GetScore());

	freetype::print(our_font, 150.0f, 200.0f, "Quit");
	freetype::print(our_font, 150.0f, 50.0f, "Play");

	for (Entity e : entities)
	{
		drawEntity(e);
	}



	glFlush();
}

std::pair<Renderer::X, Renderer::Y> Renderer::reshape(GLuint currentWidth, GLuint currentHeight, Player* p)
{

	GLfloat w = (GLfloat)currentWidth / targetWidth;
	GLfloat h = (GLfloat) currentHeight/ targetHeight;
	const float c = 200.0f;
	GLdouble camX = p->GetCoordinate().x - c * w;
	GLdouble camXWidth = p->GetCoordinate().x + c * w;
	GLdouble camY = p->GetCoordinate().y - c * h;
	GLdouble camYHeight = p->GetCoordinate().y + c *h;

	heightInfo = currentHeight; widthInfo = currentWidth;

	std::pair<X, Y> cam; cam.first.first = camX; cam.first.second = camXWidth; cam.second.first = camY; cam.second.second = camYHeight;


	glViewport(0, 0, currentWidth, currentHeight);						// Reset the current viewport

	glMatrixMode(GL_PROJECTION);						// select the projection matrix stack
	glLoadIdentity();									// reset the top of the projection matrix to an identity matrix

	gluOrtho2D(camX, camXWidth, camY, camYHeight);           // set the coordinate system for the window

	glMatrixMode(GL_MODELVIEW);							// Select the modelview matrix stack
	glLoadIdentity();									// Reset the top of the modelview matrix to an identity matrix

	return cam;
}

void Renderer::init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);						//sets the clear colour to black
															//glClear(GL_COLOR_BUFFER_BIT) in the display function
															//will clear the buffer to this colour.
}

void Renderer::drawBackground(GLuint bkgr)
{
	float edge = 500.0f;

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, bkgr);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glPushMatrix();
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); 													glVertex2f(-edge, -edge);
	glTexCoord2f(0.0,(GLfloat)targetHeight/512);								glVertex2f(-edge, edge + targetHeight);
	glTexCoord2f((GLfloat)targetWidth/512, (GLfloat)targetHeight / 512);		glVertex2f(-edge + targetWidth, edge + targetHeight);
	glTexCoord2f((GLfloat)targetWidth/512, 0.0);								glVertex2f(edge + targetWidth, -edge);


	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Renderer::drawEntity(Entity* entity)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, entity->GetTextureID());
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glTranslatef(entity->GetCoordinate().x, entity->GetCoordinate().y, 0.0);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0, 1.0); glVertex2f(0.0f, entity->GetLength().y);
	glTexCoord2f(1.0, 1.0); glVertex2f(entity->GetLength().x, entity->GetLength().y);
	glTexCoord2f(1.0, 0.0); glVertex2f(entity->GetLength().x, 0.0f);


	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void Renderer::drawEntity(Entity entity)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, entity.GetTextureID());
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
		glTranslatef(entity.GetCoordinate().x, entity.GetCoordinate().y, 0.0);
		glBegin(GL_QUADS);

		glTexCoord2f(0.0, 0.0); glVertex2f(0.0f, 0.0f);
		glTexCoord2f(0.0, 1.0); glVertex2f(0.0f, entity.GetLength().y);
		glTexCoord2f(1.0, 1.0); glVertex2f(entity.GetLength().x, entity.GetLength().y);
		glTexCoord2f(1.0, 0.0); glVertex2f(entity.GetLength().x, 0.0f);


		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	glPopMatrix();
}


