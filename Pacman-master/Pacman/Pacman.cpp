#include "Pacman.h"

#include <time.h>
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250), _cMunchieFrameTime(500), _cCherryFrameTime(500)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Enemy();
		_munchie[i]->_currentFrameTime = 0;
		_munchie[i]->_currentFrameTime = rand() % 500 + 50;
	}

	//pacman varibles
	_pacman = new Player();
	_pacman->_currentFrameTime = 0;
	_pacman->_direction = 0;
	_pacman->_frame = 0;
	_pacman->_frameCount = 0;
	_pacman->_speedMultiplier = 1.0f;
	_pacman->_speed = 0.1f;

	//input varibles
	_movementKeyPreseed = false;
	_keyPressed = new char('X');
	
	//collectable varibles
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherry[i] = new Enemy();
		_cherry[i]->_currentFrameTime = 0;
		_cherry[i]->_currentFrameTime = rand() % 500 + 50;
	}

	//setting the game to unpaused from the start
	_paused = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->_texture;
	delete _pacman->_sourceRect;
	delete _pacman->_position;
	delete _pacman;

	delete _munchie[0]->_texture;

	for (int nCount = 0; nCount < MUNCHIECOUNT; nCount++)
	{
		delete _munchie[nCount]->_position;
		delete _munchie[nCount]->_rect;
		delete _munchie[nCount];
	}
	//Order of operations is important, array deleted last
	delete[] _munchie;

	delete _cherry;

	delete _cherry[0]->_texture;

	for (int nCount = 0; nCount < CHERRYCOUNT; nCount++)
	{
		delete _cherry[nCount]->_position;
		delete _cherry[nCount]->_rect;
		delete _cherry[nCount];
	}
	//Order of operations is important, array deleted last
	delete[] _cherry;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->_texture = new Texture2D();
	_pacman->_texture->Load("Textures/Pacman.tga", false);
	_pacman->_position = new Vector2(350.0f, 350.0f);
	_pacman->_sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	
	// Load Munchie
	Texture2D* _munchieTexture = new Texture2D();
	_munchieTexture->Load("Textures/Munchie.png", true);

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->_texture = _munchieTexture;
		_munchie[i]->_rect = new Rect(0, 0, 12, 12);
		_munchie[i]->_position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}



	// Load cherry
	Texture2D* _cherryTexture = new Texture2D();
	_cherryTexture->Load("Textures/CherrySpriteSheet.png", true);

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherry[i]->_texture = _cherryTexture;
		_cherry[i]->_rect = new Rect(0, 0, 21, 30);
		_cherry[i]->_position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Load all menu related variables 
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(),
		Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f,
		Graphics::GetViewportHeight() / 2.0f);
	_pKeyDown = false;

	// Load all start menu related varibles
	_startMenu = true;
	_esckeydown = false;
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* _keyboardState = Input::Keyboard::GetState();

	//Gets the current state of the mouse
	Input::MouseState* _mouseState = Input::Mouse::GetState();


	// handles input of the keyboard and runes code accrodingly
	HandleInput( elapsedTime, _keyboardState, _keyPressed, _mouseState);
	
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		UpdateMunchie(*_munchie[i], elapsedTime);
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	stream << "Pacman X: " << _pacman->_position->X << " Y: " << _pacman->_position->Y<< *_keyPressed;
	
	SpriteBatch::BeginDraw(); // Starts Drawing
	
	if (_startMenu)
	{
		StartMenu();
	}
	if (!_startMenu) 
	{
		if (_paused)
		{
			Paused();	
		}
		if (!_paused)
		{
			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				AnimateMunchie(elapsedTime,i);
			}
			for (int i = 0; i < CHERRYCOUNT; i++)
			{
				AnimateCherry(elapsedTime, i);
			}
			AnimatePacman(elapsedTime);
		}
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	
	SpriteBatch::EndDraw(); // Ends Drawing
}

//------------------------------------------------------------------------------------------------------------------

// deals with pacmnas animations
void Pacman::AnimatePacman(int elapsedTime) 
{
	_pacman->_currentFrameTime += elapsedTime;

	if (_pacman->_currentFrameTime > _cPacmanFrameTime)
	{
		if (*_keyPressed != 'X')
		{
			_pacman->_frame++;
		}
		else
		{
			_pacman->_frame = 0;
		}
		if (_pacman->_frame >= 2)
		{
			_pacman->_frame = 0;
		}

		_pacman->_currentFrameTime = 0;
	}

	// Sets the right sprite direction and then draws Pacman
	_pacman->_sourceRect->X = _pacman->_sourceRect->Width * _pacman->_frame;
	_pacman->_sourceRect->Y = _pacman->_sourceRect->Height * _pacman->_direction;
	SpriteBatch::Draw(_pacman->_texture, _pacman->_position, _pacman->_sourceRect);
	
	//draws and changes pacmans position once he goes off the screen
	ScreenWrapping();
}

// deals with munchies animations
void Pacman::AnimateMunchie(int elapsedTime,int i)
{
	//all the code makes it so it checks if a second has passed and then adds one to [_munchieFrame] and the resets to 0 when it hits 2
	_munchie[i]->_currentFrameTime += elapsedTime;

	if (_munchie[i]->_currentFrameTime > _cMunchieFrameTime)
	{
		_munchie[i]->_frame++;
		if (_munchie[i]->_frame >= 2)
		{
			_munchie[i]->_frame = 0;
		}
		_munchie[i]->_currentFrameTime = 0;
	}

	_munchie[i]->_rect->X = _munchie[i]->_rect->Width * _munchie[i]->_frame;

	SpriteBatch::Draw(_munchie[i]->_texture,_munchie[i]->_position, _munchie[i]->_rect);
}

// deals with cherries animations 
void Pacman::AnimateCherry(int elapsedTime,int i)
{
	//all the code makes it so it checks if a second has passed and then adds one to [_munchieFrame] and the resets to 0 when it hits 2
	_cherry[i]->_currentFrameTime += elapsedTime;

	if (_cherry[i]->_currentFrameTime > _cCherryFrameTime)
	{
		_cherry[i]->_frame++;
		if (_cherry[i]->_frame >= 2)
		{
			_cherry[i]->_frame = 0;
		}
		_cherry[i]->_currentFrameTime = 0;
	}

	_cherry[i]->_rect->X = _cherry[i]->_rect->Width * _cherry[i]->_frame;

	SpriteBatch::Draw(_cherry[i]->_texture, _cherry[i]->_position, _cherry[i]->_rect);
}

// deals with pacman going of screen
void Pacman::ScreenWrapping()
{
	/// <summary>
	/// first each if statement checks checks pacmans position compared to each edge and if he over laps
	/// in the nested if statement it checks if pacman has gone fully past the view port edge and then moves him to the other side
	/// during this another pacman is being draw as to create the illusion of pacman is wraped around it
	/// </summary>
	if (_pacman->_position->X + _pacman->_sourceRect->Width > Graphics::GetViewportWidth())
	{
		if (_pacman->_position->X > Graphics::GetViewportWidth())
		{
			_pacman->_position->X = 0;
		}
		SpriteBatch::Draw(_pacman->_texture, new Vector2(_pacman->_position->X - Graphics::GetViewportWidth(), _pacman->_position->Y), _pacman->_sourceRect);
	}
	if (_pacman->_position->X - _pacman->_sourceRect->Width < 0)
	{
		SpriteBatch::Draw(_pacman->_texture, new Vector2(_pacman->_position->X + Graphics::GetViewportWidth(), _pacman->_position->Y), _pacman->_sourceRect);
		if (_pacman->_position->X + _pacman->_sourceRect->Width < 0)
		{
			_pacman->_position->X = Graphics::GetViewportWidth() - _pacman->_sourceRect->Width;
		}
	}
	if (_pacman->_position->Y + _pacman->_sourceRect->Height > Graphics::GetViewportHeight())
	{
		SpriteBatch::Draw(_pacman->_texture, new Vector2(_pacman->_position->X, _pacman->_position->Y - Graphics::GetViewportHeight()), _pacman->_sourceRect);
		if (_pacman->_position->Y > Graphics::GetViewportHeight())
		{
			_pacman->_position->Y = 0;
		}
	}
	if (_pacman->_position->Y - _pacman->_sourceRect->Height < 0)
	{
		SpriteBatch::Draw(_pacman->_texture, new Vector2(_pacman->_position->X, _pacman->_position->Y + Graphics::GetViewportHeight()), _pacman->_sourceRect);
		if (_pacman->_position->Y + _pacman->_sourceRect->Height < 0)
		{
			_pacman->_position->Y = Graphics::GetViewportHeight() - _pacman->_sourceRect->Height;
		}
	}
}

// draws the start menu
void Pacman::StartMenu()
{
	std::stringstream menuStream;
	menuStream << "Press space to start!";
	SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
	SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
}

// draws the paused menu
void Pacman::Paused()
{

	std::stringstream menuStream;
	menuStream << "PAUSED!";
	SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
	SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);

}

//------------------------------------------------------------------------------------------------------------------

// checks key presses and runs code according the the button and game state
void Pacman::HandleInput(int elapsedTime, Input::KeyboardState* _keyboardState, char* _keyPressed, Input::MouseState* _mouseState)
{
	if (_startMenu)
	{

		if (_keyboardState->IsKeyDown(Input::Keys::SPACE))
		{
			_startMenu = !_startMenu;
		}

		if (_keyboardState->IsKeyDown(Input::Keys::ESCAPE) && _esckeydown == false)
		{
			Audio::Destroy();
			Input::Destroy();
			Graphics::Destroy();
		}

		if (_keyboardState->IsKeyUp(Input::Keys::ESCAPE))
		{
			_esckeydown = false;
		}
	}

	if (!_startMenu)
	{
		//changes (_paused) to its reverse and the only allows it to run once
		if (_keyboardState->IsKeyDown(Input::Keys::ESCAPE))  
		{
			_startMenu = !_startMenu;
			_esckeydown = true;
		}

		//changes (_paused) to its reverse and the only allows it to run once
		if (_keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)  
		{
			_pKeyDown = true;
			_paused = !_paused;
		}

		// if the p key is not pressed it sets the varible to false
		if (_keyboardState->IsKeyUp(Input::Keys::P))
		{
			_pKeyDown = false;
		}

		// only runs when the game is paused
		if (_paused) 
		{
		
		}

		// only runs when the game is not paused 
		if (!_paused)//only runs when not paused 
		{
			// Speed Multiplier
			if (_keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT) && _keyboardState->IsKeyUp(Input::Keys::LEFTCONTROL))
			{
				// Apply multiplier
				_pacman->_speedMultiplier = 2.0f;
			}
			else if (_keyboardState->IsKeyDown(Input::Keys::LEFTCONTROL) && _keyboardState->IsKeyUp(Input::Keys::LEFTSHIFT))
			{
				// Apply multiplier
				_pacman->_speedMultiplier = 3.0f;
			}
			else
			{
				// Reset multiplier
				_pacman->_speedMultiplier = 1.0f;
			}


			/// <summary>
			/// checks which direction key is pressed and changes pacmans postion accoridingly based on the speed set
			/// </summary>
			
			//if no key is pressed check for a key being pressed
			{
				if (*_keyPressed != 'A' && *_keyPressed != 'D' && *_keyPressed != 'W' && *_keyPressed != 'S')
				{
					if (_keyboardState->IsKeyDown(Input::Keys::A))
					{
						*_keyPressed = 'A';
						_pacman->_direction = 2;
					}
					if (_keyboardState->IsKeyDown(Input::Keys::D))
					{
						*_keyPressed = 'D';
						_pacman->_direction = 0;
					}
					if (_keyboardState->IsKeyDown(Input::Keys::W))
					{
						*_keyPressed = 'W';
						_pacman->_direction = 3;
					}
					if (_keyboardState->IsKeyDown(Input::Keys::S))
					{
						*_keyPressed = 'S';
						_pacman->_direction = 1;
					}
				}
			}

			//if a key was pressed checks for it being released
			//commenting this out allows you to make it move constaly in one direction
			{
				if (*_keyPressed == 'A' && _keyboardState->IsKeyDown(Input::Keys::A) == false)
				{
					*_keyPressed = 'X';
				}
				if (*_keyPressed == 'D' && _keyboardState->IsKeyDown(Input::Keys::D) == false)
				{
					*_keyPressed = 'X';
				}
				if (*_keyPressed == 'S' && _keyboardState->IsKeyDown(Input::Keys::S) == false)
				{
					*_keyPressed = 'X';
				}
				if (*_keyPressed == 'W' && _keyboardState->IsKeyDown(Input::Keys::W) == false)
				{
					*_keyPressed = 'X';
				}
			}


			//if a key is pressed apply force releated to key
			{
				if (*_keyPressed == 'A')
				{
					_pacman->_position->X -= _pacman->_speed * elapsedTime * _pacman->_speedMultiplier;
					_pacman->_frameCount++;
				}
				if (*_keyPressed == 'D')
				{
					_pacman->_position->X += _pacman->_speed * elapsedTime * _pacman->_speedMultiplier;
					_pacman->_frameCount++;
				}
				if (*_keyPressed == 'W')
				{
					_pacman->_position->Y -= _pacman->_speed * elapsedTime * _pacman->_speedMultiplier;
					_pacman->_frameCount++;
				}
				if (*_keyPressed == 'S')
				{
					_pacman->_position->Y += _pacman->_speed * elapsedTime * _pacman->_speedMultiplier;
					_pacman->_frameCount++;
				}
			}
		}
	}
};

// Munchie code run ever update
void Pacman::UpdateMunchie(Enemy _munchie,int elapsedTime)
{

}