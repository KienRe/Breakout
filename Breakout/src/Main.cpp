#pragma once

#include <iostream>

#include <SDL.h>
#include <GL/glew.h>

#include "Game.h"
#include "ResourceManager.h"
#include "Debug.h"

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_HEIGHT, SCREEN_WIDTH);

bool isRunning = true;

int main(int argc, char* argv[])
{
	//SDL Init
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << ("Unable to initalize SDL: %s \n", SDL_GetError());
	}
	else
	{
		std::cout << "SDL initalized succesfully! \n";
	}

	SDL_Window* window = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	//OpenGL Init
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	//Glew Init
	const GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		std::cout << "Glew Init failed! >>" << glewGetErrorString(err);
	}
	else
	{
		std::cout << "Glew initalized succesfully! \nVideo Driver Version: " << glGetString(GL_VERSION) << "\n";
	}

	//OpenGL Setup
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();

	//Game Init
	Breakout.Init();

	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	Breakout.State = GAME_ACTIVE;

	while(isRunning)
	{
		SDL_PumpEvents();
		Breakout.Keys = SDL_GetKeyboardState(NULL);

		//Calculate delta time
		GLfloat currentFrame = SDL_GetTicks();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Manage Input
		Breakout.ProcessInput(deltaTime);

		//Render
		glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Breakout.Render();

		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	ResourceManager::Clear();

	return 0;
}