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
	this->DoCollisions();

	if(Ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
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

void Game::DoCollisions()
{
	//Check all Bricks against Ball
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;

				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT)
				{
					Ball->Velocity.x = -Ball->Velocity.x;

					GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);

					if (dir == LEFT)
						Ball->Position.x += penetration;	//Move ball to right
					else
						Ball->Position.x -= penetration;	//Move ball to left
				}
				else
				{
					Ball->Velocity.y = -Ball->Velocity.y;

					GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);

					if (dir == UP)
						Ball->Position.y -= penetration;	//Move ball up
					else
						Ball->Position.y += penetration;	//Move ball down
				}
			}
		}
	}

	Collision result = CheckCollision(*Ball, *Player);
	if(!Ball->Stuck && std::get<0>(result))
	{
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);

		GLfloat strenght = 2.0f;

		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strenght;
		Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);

		//Sticky paddle bug fix
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;

	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& ball, GameObject& two)
{
	glm::vec2 ballCenter(ball.Position + ball.Radius);

	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

	glm::vec2 difference = ballCenter - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	glm::vec2 closest = aabb_center + clamped;

	difference = closest - ballCenter;

	if (glm::length(difference) <= ball.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] =
	{
		glm::vec2(0.0f,1.0f),	//up
		glm::vec2(1.0f,0.0f),	//right
		glm::vec2(0.0f,-1.0f),	//down
		glm::vec2(-1.0f,0.0f)	//left
	};

	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("res/levels/01.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("res/levels/02.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("res/levels/03.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("res/levels/04.txt", this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
	// Reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}
