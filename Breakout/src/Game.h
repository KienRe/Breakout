#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <tuple>

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

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

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

private:
	void DoCollisions();
	GLboolean CheckCollision(GameObject& one, GameObject& two);
	Collision CheckCollision(BallObject& one, GameObject& two);
	Direction VectorDirection(glm::vec2 target);
	// Reset
	void ResetLevel();
	void ResetPlayer();
};