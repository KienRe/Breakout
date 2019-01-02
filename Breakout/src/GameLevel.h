#pragma once

#include <vector>
#include <GL/glew.h>

#include "GameObject.h"

class GameLevel
{
public:
	std::vector<GameObject> Bricks;

	void Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
	void Draw(SpriteRenderer& renderer);
	GLboolean IsCompleted();

private:
	void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};