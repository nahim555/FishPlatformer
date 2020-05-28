#include <windows.h>		
#include <iostream>
#include <memory>
#include "../glew/include/GL/glew.h"			
#include "../View/Headers/Renderer.h"
#include "../Object/Headers/NPC.h"
#include "../Object/Headers/Player.h"
//#include "../Object/Headers/World.h" //Deprecated. Using a Spatial Grid to collect world objects now.
#include "../View/Headers/Texture.h"
#include "tgaload.h"
#include "SpatialHash.h"
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include <experimental\filesystem>
#include "../View/Headers/FreeType.h"
#include <irrklang/irrKlang.h>
using namespace irrklang;

#define GAME_NULL_VELOCITY glm::vec2(0.0f, 0.0f)
#define GAME_SQUARE_SIZE glm::vec2(25.0f, 25.0f)
#define GAME_WORLD_WIDTH 2500
#define GAME_WORLD_HEIGHT 2500
#define GAME_JUMP_HEIGHT 70.0f
#define GAME_INITIAL_VELOCITY glm::vec2(15.0f, 18.0f)
#define GAME_PLAYER_SIZE GAME_SQUARE_SIZE
#define GAME_GRID_CELL_SIZE 200
#define GAME_TARGET_WIDTH GLuint(1024)
#define GAME_TARGET_HEIGHT GLuint(1024)
#define GAME_PLAYER_MAX_LIFE 3
#define GAME_STARTING_SCORE 0

//List of available levels in the directory of the game (.amap extension)
std::vector<std::string> levels;
//List of accepted tiles that level reading function will convert into tiles
std::vector<int> acceptTiles;
//Set current level to NULL (1 element of array)
int thisLevel = NULL;
//Set current score to NULL 
int currentScore = NULL;
//Set current life as maximum available during initialisation
int currentLife = GAME_PLAYER_MAX_LIFE;
//If the last level was finished
bool beatGame = false;
//Specifies if the game is Active, displaying Menu, Setting up or Quitting
enum GameState
{
	MENU,
	ACTIVE,
	SETUP,
	QUIT,
};
ISoundEngine *SoundEngine = createIrrKlangDevice();

//Font that is used to text in the game
freetype::font_data font;
//Mouse coordinates
int mouse_x, mouse_y;
//If left mouse button is pressed in menu
bool mouseKeyPressed = false;
//Is the world set up
bool worldSetUp = false;
//Current resolution
GLuint currentWidth = 1024, currentHeight = 768; 
//Frequency for delta time
double timerFrequencyRecip = 0.000003;
//Delta time between each update cycle
float deltaT; 
//Previous time used to calculate delta time
__int64 prevTime;
//Starting coordinate for player
glm::vec2 startingCoord;
//List of textures
Texture t;
//Initial player object
Player* player = new Player(); GLuint plChar;
//Rendering functions
Renderer renderer(GAME_TARGET_WIDTH, GAME_TARGET_HEIGHT);
//Spatial Grid to collect world objects
SpatialHash grid(GAME_WORLD_WIDTH, GAME_WORLD_HEIGHT, GAME_GRID_CELL_SIZE);
//Entities on screen that are going to be drawn and checked for collision
std::vector<Entity> collected;
//Player perspective to get Entities on screen
std::pair<Renderer::X, Renderer::Y> cam;
//Whether the user is playing, in Menu or wants to Quit
GameState stateOfThisGame;
//World world; //Deprecated. 

/*--------------------------Game functions------------------------------------------------------*/

//Processes player, setting proper life and score after an event trigger
void processPlayer();
//Checks if current tile code is an acceptable tile
bool checkTileCode(int tileCode);
//Switch to next level when portal is entered
void nextLevel();
//Gets all ".amap" extension files in current directory of the game executable
void getMaps();
//Loads Entity textures
void loadTextures();		
//Checks if the game is in Menu, if user wants to Quit or is the game Active
void checkGameState();	
//Not used in code. Generates a map template.
void generateMapFile();										
void setCollisionFlags(int things[99][99], Entity &e, int i, int j);
//Simulates the delta of time for each update cycle
double timeSimulation();	
//Checks for collisions of Entities that are displayed on the screen
void doCollisions();
//External keyboard processing (non-player movement, e.g. reset game)
void processKeys_external();	
//Game update cycle to process objects on screen, keyboard, resolve collision, check jumping
void update();				
//Renders the objects on screen
void render(int width, int height);		
//Function to populate the game world, adding objects to grid
void populateWorld(int selectedLevel);		
// Declaration For WndProc
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	
// releases and destroys the window
void KillGLWindow();					
//creates the window
bool CreateGLWindow(char* title, int width, int height);	
// Win32 main function
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);	
/*------------------------------------------------------------------------------------------*/

/*Win32 global variables:
-----------------------------*/
// Private GDI Device Context
HDC			hDC = NULL;	
// Permanent Rendering Context
HGLRC		hRC = NULL;		
// Holds Our Window Handle
HWND		hWnd = NULL;	
// Holds The Instance Of The Application
HINSTANCE	hInstance;	
/*--------------------------*/

void checkGameState()
{
	if (stateOfThisGame == GameState::SETUP)
	{
		thisLevel = 0;
		currentScore = GAME_STARTING_SCORE;
		currentLife = GAME_PLAYER_MAX_LIFE;
		grid.clear();
		delete(player);
		player = new Player(glm::vec2(0.0f, 80.0f), GAME_PLAYER_SIZE, GAME_INITIAL_VELOCITY, plChar, GAME_JUMP_HEIGHT, NULL, GAME_PLAYER_MAX_LIFE, SoundEngine);
		std::cout << "Player created." << std::endl;

		player->SetTextures(t);
		std::cout << "Player texture list set." << std::endl;
		
		std::cout << "Populating world." << std::endl; 
		populateWorld(thisLevel);

		stateOfThisGame = GameState::ACTIVE;
		worldSetUp = true;


	}	
	else if (stateOfThisGame == GameState::ACTIVE)
	{
		if (SoundEngine->isCurrentlyPlaying("audio/gw2.mp3"))
		{
			SoundEngine->stopAllSounds();
		}
		if (!SoundEngine->isCurrentlyPlaying("audio/game.mp3")) SoundEngine->play2D("audio/game.mp3");
		update();
	}
	else
	{
		if (SoundEngine->isCurrentlyPlaying("audio/game.mp3")) SoundEngine->stopAllSounds();
		Entity start = Entity(glm::vec2(25.0f, 25.0f), glm::vec2(100.0f, 100.0f), GAME_NULL_VELOCITY, t.textures["start"]);
		Entity quit = Entity(glm::vec2(25.0f, 160.0f), glm::vec2(100.0f, 100.0f), GAME_NULL_VELOCITY, t.textures["quit"]);
		Entity cursor = Entity(glm::vec2(mouse_x, mouse_y), glm::vec2(5.0f, 5.0f), GAME_NULL_VELOCITY, t.textures["cursor"]);


		std::vector<Entity> buttonslist;

		buttonslist.push_back(start);
		buttonslist.push_back(quit);
		buttonslist.push_back(cursor);

		std::vector<std::pair<Entity, int>> buttons;
		
		buttons.push_back({ start,0 });
		buttons.push_back({ quit, 1 });

		for (auto e : buttons)
		{
			bool on_x = cursor.GetCoordinate().x + cursor.GetLength().x >= e.first.GetCoordinate().x && e.first.GetCoordinate().x + e.first.GetLength().x >= cursor.GetCoordinate().x;
			bool on_y = cursor.GetCoordinate().y + cursor.GetLength().y >= e.first.GetCoordinate().y && e.first.GetCoordinate().y + e.first.GetLength().y >= cursor.GetCoordinate().y;

			if (on_x && on_y)
			{
				if (mouseKeyPressed)
				{
					if (e.second == 0)
					{
						if (!worldSetUp)
						{
							stateOfThisGame = GameState::SETUP;
							beatGame = false;
						}
						else stateOfThisGame = GameState::ACTIVE;
					}
					if (e.second == 1) stateOfThisGame = GameState::QUIT;
				}

				break;
			}
		}

		renderer.displayMenu(currentWidth, currentHeight, buttonslist, beatGame, player);
	}
}

void processPlayer()
{
	currentScore = player->GetScore();
	currentLife = player->GetLives() - 1;
	delete(player);
	player = new Player(startingCoord, GAME_PLAYER_SIZE, GAME_INITIAL_VELOCITY, plChar, GAME_JUMP_HEIGHT, currentScore, currentLife, SoundEngine);
	player->SetTextures(t);
}

void update()
{		
	collected = grid.collect(cam.first.first, cam.first.second, cam.second.first, cam.second.second);

	double dt = timeSimulation();

	if (currentLife > 0)
	{
		processKeys_external();
		doCollisions();									//Collision detection
		player->processKeys();							//Process keyboard
		player->checkJumpState(dt);						//Check if player is jumping/falling/on-ground
	}

	//Sets Perspective GL Screen in respect to player coordinates
	cam = renderer.reshape(currentWidth, currentHeight, player);	
	renderer.display(player, collected);			// Draw the scene of objects near player
	const float padding = 100.0f;
	if (player->GetCoordinate().x < -padding 
			|| player->GetCoordinate().y < -padding 
			|| player->GetCoordinate().x > GAME_WORLD_WIDTH + padding 
			|| player->GetCoordinate().y  > GAME_WORLD_HEIGHT + padding)
	{
		processPlayer();
		std::cout << "Player fell." << std::endl;
	}
}

void loadTextures()
{
	std::cout << "Loading textures." << std::endl;

	t.textures["pChar"] = tgaLoadAndBind("player.tga", TGA_ALPHA); plChar = tgaLoadAndBind("player.tga", TGA_ALPHA);
	t.textures["pCharL"] = tgaLoadAndBind("player_left.tga", TGA_ALPHA);
	t.textures["pCharLU"] = tgaLoadAndBind("player_leftUp.tga", TGA_ALPHA);
	t.textures["pCharLD"] = tgaLoadAndBind("player_leftDown.tga", TGA_ALPHA);
	t.textures["pCharR"] = tgaLoadAndBind("player_right.tga", TGA_ALPHA);
	t.textures["pCharRU"] = tgaLoadAndBind("player_rightUp.tga", TGA_ALPHA);
	t.textures["pCharRD"] = tgaLoadAndBind("player_rightDown.tga", TGA_ALPHA);
	t.textures["pCharU"] = tgaLoadAndBind("player_up.tga", TGA_ALPHA);
	t.textures["pCharD"] = tgaLoadAndBind("player_down.tga", TGA_ALPHA);
	t.textures["grass"] = tgaLoadAndBind("grass.tga", TGA_ALPHA);
	t.textures["dirt"] = tgaLoadAndBind("dirt.tga", TGA_ALPHA);
	t.textures["start"] = tgaLoadAndBind("start.tga", TGA_ALPHA);
	t.textures["quit"] = tgaLoadAndBind("quit.tga", TGA_ALPHA);
	t.textures["pea"] = tgaLoadAndBind("pea.tga", TGA_ALPHA);
	t.textures["background"] = tgaLoadAndBind("background.tga", TGA_ALPHA);
	t.textures["menubg"] = tgaLoadAndBind("menubg.tga", TGA_ALPHA);
	t.textures["portal"] = tgaLoadAndBind("portal.tga", TGA_ALPHA);
	t.textures["cursor"] = tgaLoadAndBind("cursor.tga", TGA_ALPHA);

	std::cout << "Textures loaded." << std::endl;
	
	renderer.SetTextureList(t);
}

void processKeys_external()
{
	if (player->keys[0x52])
	{
		delete(player);
		player = new Player(startingCoord, GAME_PLAYER_SIZE, GAME_INITIAL_VELOCITY, plChar, GAME_JUMP_HEIGHT, currentScore, currentLife, SoundEngine);
		grid.clear();
		populateWorld(thisLevel);
		player->SetTextures(t);
		std::cout << "Player reset." << std::endl;
	}
}

void render(int width, int height)
{
	cam = renderer.reshape(width, height, player);	// Set Up Our Perspective GL Screen
	renderer.init();
}

void setCollisionFlags(int things[99][99], Entity& e, int i, int j )
{
	if (things[i + 1][j] == 1 || things[i + 1][j] == 2)
		e.SetN_up(true);
	if (things[i - 1][j] == 1 || things[i - 1][j] == 2)
		e.SetN_down(true);
	if (things[i][j + 1] == 1 || things[i][j + 1] == 2)
		e.SetN_right(true);
	if (things[i][j - 1] == 1 || things[i][j - 1] == 2)
		e.SetN_left(true);
}

void doCollisions()
{	
	player->resetCollisions(); //Each update cycle resets collision to none

	for(Entity& e : collected) //For each entity on screen
	{
		if (player->checkCollision(e)) //AABB collision detection
		{
			if (e.GetDestructible() == true)
			{
				player->SetScore(player->GetScore() + 1);
				grid.remove(e); break;
			}			
			if (e.GetTextureID() == t.textures["portal"])
			{
				SoundEngine->play2D("audio/portal.wav");
				thisLevel++;
				nextLevel();				
				break;
			}
			player->collisionSide(e); //Check for the side of collision 
		}
	}
}

void nextLevel()
{
	if (thisLevel < levels.size())
	{
		currentScore = player->GetScore();
		grid.clear();
		populateWorld(thisLevel);
	}
	else
	{
		worldSetUp = false;
		beatGame = true;
		stateOfThisGame = GameState::MENU;
		grid.clear();
	}
}

void generateMapFile()
{
	using namespace std;
	ofstream out;

	out.open("level_template.amap", fstream::out);

	for (int i = 0; i <= GAME_WORLD_HEIGHT/25; i++)
	{
		for (int j = 0; j <= GAME_WORLD_WIDTH/25; j++)
		{
			out << "0 ";
		}
		out << endl;
	}
}

bool checkTileCode(int tileCode)
{
	bool accept = false;

	for (auto i : acceptTiles)
	{
		if (tileCode == i)
		{
			accept = true;
			break;
		}
	}
	return accept;
}


void getMaps()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	auto thisdir = std::string(buffer).substr(0, pos);

	for (const auto& p : std::experimental::filesystem::directory_iterator(thisdir))
	{
		if (p.path().filename().extension().string() == ".amap")
		{
			auto s = thisdir + "\\" + p.path().filename().string();
			levels.push_back(s);
		}
	}
}

void populateWorld(int selectedLevel)
{
	int row = 0, col = 0;
	int tileCode;
	std::string line;
	std::string levelName;
	levelName = levels[selectedLevel];
	std::cout << "Loading " << levelName << "." << std::endl;
	std::ifstream fstream(levelName);
	std::vector<std::vector<GLuint>> tileData;
	acceptTiles = { 1,2,7,5,9 };

	int things[99][99];
	if (fstream)
	{
		while (std::getline(fstream, line)) // Read each line from level
		{
			col = 0;
			std::istringstream sstream(line);
			while (sstream >> tileCode) 
			{
				if (checkTileCode(tileCode))
				{
					things[100-row][col] = tileCode;
				}
				col++;
			}
		row++;
		}
	}

	for (int i = 0; i < 99; i++)
	{
		for (int j = 0; j < 99; j++)
		{
			if (things[i][j] == 1)
			{
				Entity e(glm::vec2(j*25.0f, i*25.0f), glm::vec2(25.0f, 25.0f), 
					glm::vec2(0.08f, 0.033f), t.textures["dirt"]);
				setCollisionFlags(things, e, i, j);
				grid.add(e);
			}
			if (things[i][j] == 2)
			{
				Entity e(glm::vec2(j*25.0f, i*25.0f), glm::vec2(25.0f, 25.0f), 
					glm::vec2(0.08f, 0.033f), t.textures["grass"]);
				setCollisionFlags(things, e, i, j);
				grid.add(e);
			}
			if (things[i][j] == 9)
			{
				Entity e(glm::vec2((j*25.0f)+5.5f, (i*25.0f)+5.5f), glm::vec2(17.0f, 17.0f),
					glm::vec2(0.08f, 0.033f), t.textures["portal"]);
				setCollisionFlags(things, e, i, j);
				grid.add(e);
			}
			if (things[i][j] == 7)
			{
				Entity e(glm::vec2((j*25.0f)+12.5f, (i*25.0f)+12.5f), glm::vec2(10.0f, 10.0f),
					glm::vec2(0.08f, 0.033f), t.textures["pea"]);
				setCollisionFlags(things, e, i, j);
				e.SetDestructible(true);
				grid.add(e);
			}
			if (things[i][j] == 5)
			{
				player->SetCoordinate(glm::vec2(j*25.0f, i*25.0f));
				startingCoord = glm::vec2(j*25.0f, i*25.0f);
				std::cout << "Starting player coordinate set." << std::endl;
			}
				
		}
	}
}

double timeSimulation()
{
	// Get the current time
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	__int64 currentTime = t.QuadPart;

	__int64 ticksElapsed = currentTime - prevTime;					// Ticks elapsed since the previous time step
	double deltaT = double(ticksElapsed) * timerFrequencyRecip;		// Convert to second
																	//cout << ticksElapsed << " " << deltaT << endl;
	
	// Advance timer
	prevTime = currentTime;					// use the current time as the previous time in the next step
	return deltaT;
}
							/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done = false;								// Bool Variable To Exit Loop


	AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	// Create Our OpenGL Window
	if (!CreateGLWindow("Peas Out! A Legend of Lost Peas", currentWidth, currentHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	prevTime = 0;

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if (player->keys[VK_ESCAPE]) stateOfThisGame = GameState::MENU;
			if (stateOfThisGame == GameState::QUIT) done = true;

				checkGameState();
				SwapBuffers(hDC);							// Swap Buffers (Double Buffering)
			
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,								// Message For This Window
	WPARAM	wParam,								// Additional Message Information
	LPARAM	lParam)								// Additional Message Information
{
	switch (uMsg)								// Check For Windows Messages
	{
	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(404);						// Send A Quit Message
		return 0;								// Jump Back
	}
	break;

	case WM_SIZE:								// Resize The OpenGL Window
	{
		renderer.reshape(LOWORD(lParam), HIWORD(lParam), player);  // LoWord=Width, HiWord=Height
		currentWidth = LOWORD(lParam);
	    currentHeight = HIWORD(lParam);
		
		return 0;								// Jump Back
	}
	break;

	case WM_LBUTTONDOWN:
	{
		mouseKeyPressed = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		mouseKeyPressed = false;
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (stateOfThisGame == MENU)
		{
			mouse_x = LOWORD(lParam);
			mouse_y = currentHeight - HIWORD(lParam);
		}

		//LeftPressed = true;
	}
	break;
	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		player->keys[wParam] = true;					// If So, Mark It As TRUE
		if (player->GetLives() < 1 && stateOfThisGame == GameState::ACTIVE) 
			stateOfThisGame = GameState::SETUP;

		if (player->keys[VK_F11])
		{

		}
		return 0;								// Jump Back
	}
	break;
	case WM_KEYUP:								// Has A Key Been Released?
	{
		player->keys[wParam] = false;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}

	font.clean();
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*/

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window

	stateOfThisGame = GameState::MENU;

	std::cout << "Initial screen rendering." << std::endl;
	render(width, height);
	
	loadTextures();
	font.init("arialbd.ttf", 36);
	renderer.SetFont(font);
	getMaps();
	SoundEngine->setSoundVolume(0.5f);
	SoundEngine->play2D("audio/gw2.mp3", GL_TRUE);
	//SoundEngine->play2D("audio/gw2.mp3", GL_TRUE);

	return true;									// Success
}



