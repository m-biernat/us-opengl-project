#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Skybox
{
public:
	Skybox();
	~Skybox();

	void draw();

private:
	GLuint vao;
	GLuint buffers[2];

	int endPrimitive;
	GLuint indicesNumber;

	void setupBuffers();
};
