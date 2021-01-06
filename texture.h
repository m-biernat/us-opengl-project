#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	static void loadTexture(GLuint tex, const wchar_t* filename);
	static void loadCubemap(GLuint tex, const wchar_t* filename[]);

private:
	Texture() {}

	static void loadFromFile(GLuint tex, const wchar_t* filename);
	static void loadFromFile2(GLuint tex, const wchar_t* filename, int i);
};
