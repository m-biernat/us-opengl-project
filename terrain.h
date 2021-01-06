#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#include "noise.h"

using namespace std;
using namespace glm;

class Terrain
{
public:
	mat4 modelMatrix;

	Terrain(int size = 16, float sizeScale = 1.0f, float heightScale = 1.0f, int octaves = 4);
	
	~Terrain();

	void draw();

private:
	int size;

	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> textureUVs;
	vector<int> indices;

	GLuint vao;
	GLuint buffers[4];

	int endPrimitive;
	GLuint indicesNumber;

	PerlinNoise* pn;
	int octaves;

	void generateData();
	void clearData();
	void setupBuffers();
	
	float getHeight(vec2 p);
};
