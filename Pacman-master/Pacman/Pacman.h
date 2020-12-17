#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.

#define MUNCHIECOUNT 50
#define CHERRYCOUNT 4
#define GHOSTCOUNT 4

struct Player 
{
	bool _dead;
	Vector2* _position;
	Rect* _sourceRect;
	Texture2D* _texture;
	int _direction;
	int _frameCount;
	int _frame;
	int _currentFrameTime;
	float _speedMultiplier;
	float _speed;
};

struct Enemy
{
	Rect* _rect;
	Vector2* _position;
	Texture2D* _texture;
	int _frame;
	int _currentFrameTime;
};

struct MovingEnemy
{
	Vector2* _position;
	Texture2D* _texture;
	Rect* _sourceRect;
	int _direction;
	float _speed;
};


struct Pellet 
{
	
};

class Pacman : public Game
{
private:
	// Constansts 
	const int _cPacmanFrameTime;
	const int _cMunchieFrameTime;
	const int _cCherryFrameTime;
	
	// last movement key pressed
	char* _keyPressed;

	Player* _pacman;

	Enemy* _munchie[MUNCHIECOUNT];

	Enemy* _cherry[CHERRYCOUNT];

	MovingEnemy* _ghost[GHOSTCOUNT];
	
	Pellet* _pellet;

	bool _movementKeyPreseed;


	// Position for String
	Vector2* _stringPosition;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;

	//data for startmenu
	bool _startMenu;
	bool _esckeydown;

	
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	void AnimatePacman(int elapsedTime);

	void AnimateMunchie(int elapsedTime,int i);

	void AnimateCherry(int elapsedTime,int i);

	void AnimateGhost(int elapsedTime, int i);
	
	void StartMenu();

	/// <summary> called every frame the game is paused </summary>
	void Paused();

	/// <summary> This handles pacmans position when going offscreen and draw another pacman on the other screen </summary>
	void ScreenWrapping();

	/// <summary> This handles anything that may required the use of a keyboard </summary>
	void HandleInput(int elapsedTime, Input::KeyboardState* _keyboardState, char* _keyPressed, Input::MouseState* _mouseState);

	/// <summary> This runs every update for Ghost related code </summary>
	void UpdateGhost(MovingEnemy* _ghost, int elapsedTime);

	void CheckGhostCollisions();

	/// <summary> This runs every update for Munchie related code </summary>
	void UpdateMunchie(Enemy* _munchie, int elapsedTime);
};