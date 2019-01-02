#include "Game.h"

SpriteRenderer* Renderer;

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

	// Load shaders
	ResourceManager::LoadShader("res//shaders//Sprite.vs", "res//shaders//Sprite.fs", nullptr, "sprite");
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// Load textures
	ResourceManager::LoadTexture("res//textures//test.png", GL_TRUE, "face");
}

void Game::ProcessInput(GLfloat dt)
{
}

void Game::Update(GLfloat dt)
{
}

void Game::Render()
{
	Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
