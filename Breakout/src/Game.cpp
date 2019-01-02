#include "Game.h"

SpriteRenderer* Renderer;

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(1.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(0.1f, -0.35f);
const GLfloat BALL_RADIUS = 12.5f;

GameObject* Player;
BallObject* Ball;

Game::Game(GLuint width, GLuint height)
{
	this->Width = width;
	this->Height = height;
}

Game::~Game()
{
}

void Game::Init()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Load textures
	ResourceManager::LoadTexture("res//textures//background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("res//textures//block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("res//textures//block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("res//textures//paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("res//textures//face.png", GL_TRUE, "face");

	//Load levels
	GameLevel one; one.Load("res//levels//01.txt", this->Width, this->Height * 0.5f);
	GameLevel two; two.Load("res//levels//02.txt", this->Width, this->Height * 0.5f);
	GameLevel three; three.Load("res//levels//03.txt", this->Width, this->Height * 0.5f);
	GameLevel four; four.Load("res//levels//04.txt", this->Width, this->Height * 0.5f);

	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);

	this->Level = 0;

	//Configure player & ball
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	//Load shaders
	ResourceManager::LoadShader("res//shaders//Sprite.vs", "res//shaders//Sprite.fs", nullptr, "sprite");
	//Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (Keys[SDL_SCANCODE_LEFT] || Keys[SDL_SCANCODE_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
			}
		}

		if (Keys[SDL_SCANCODE_RIGHT] || Keys[SDL_SCANCODE_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
		}

		if (Keys[SDL_SCANCODE_SPACE])
		{
			Ball->Stuck = false;
		}
	}
}

void Game::Update(GLfloat dt)
{
	Ball->Move(dt, this->Width);
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

		this->Levels[this->Level].Draw(*Renderer);

		Player->Draw(*Renderer);
		Ball->Draw(*Renderer);
	}
}