#pragma once

#include <GL/glew.h>
#include <SDL.h>

#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "GameLevel.h"
#include "BallObject.h"

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	GameState State;
	const Uint8* Keys;
	GLuint Width, Height;

	std::vector<GameLevel> Levels;
	GLuint Level;

	Game(GLuint width, GLuint height);
	~Game();

	void Init();

	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
};