#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	static void loadTexture(GLuint tex, const wchar_t* filename);

private:
	Texture() {}
};
