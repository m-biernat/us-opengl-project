#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "terrain.h"

using namespace std;
using namespace glm;

Terrain::Terrain(int size, float sizeScale, float heightScale)
{
	this->size = size;

	generateData();
	setupBuffers();

	clearData();

	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = scale(modelMatrix, vec3(sizeScale, sizeScale, heightScale));
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(4, buffers);
}

void Terrain::draw()
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(endPrimitive);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, indicesNumber, GL_UNSIGNED_INT, 0);

	glDisable(GL_PRIMITIVE_RESTART);

	glBindVertexArray(0);
}

void Terrain::generateData()
{
	#pragma region Generowanie siatki wierzchołków

	float step = 2.0f / (size - 1);
	float x = -1.0f, y = 1.0f;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			vertices.push_back(vec3(x, y, 0.0f));
			//vertices.push_back(pixels[size * i + j] * heightScale);
			x += step;
		}
		x = -1.0f;
		y -= step;
	}

	#pragma endregion

	#pragma region Generowanie normali

	int lastRow = size * (size - 1);
	int size_d = size - 1;
	int lastCol = size_d;

	normals.push_back(normalize((vec3(vertices[0].z, vertices[size].z, 2.0f))));

	for (int i = 1; i < vertices.size(); i++)
	{
		float hL = -1.0f, hR = -1.0f, hU = -1.0f, hD = -1.0f;

		if (i % size != 0)				// Bez pierwszej kolumny
			hL = vertices[i - 1].z;
		if (i % lastCol != 0)			// Bez ostatniej kolumny
			hR = vertices[i + 1].z;
		else
			lastCol += size;
		if (i > size_d)					// Bez pierzwszego rzędu
			hU = vertices[i - size].z;
		if (i < lastRow)				// Bez ostatniego rzędu
			hD = vertices[i + size].z;

		vec3 norm = vec3(hL - hR, hD - hU, 2.0f);

		if (hL < 0)
			norm.x = hR;
		if (hR < 0)
			norm.x = hL;
		if (hU < 0)
			norm.y = hD;
		if (hD < 0)
			norm.y = hU;

		normals.push_back(normalize(norm));
	}

	#pragma endregion

	#pragma region Generowanie koordynatów tekstur

	step = 1.0f / (size - 1);
	x = 0.0f, y = 1.0f;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			textureUVs.push_back(vec2(x, y));
			x += step;
		}
		x = -1.0f;
		y -= step;
	}

	#pragma endregion

	#pragma region Generowanie indeksów

	int verts = (size - 1) * size;
	endPrimitive = size * size;

	int n = size - 1;

	for (int i = 0; i < verts; i++)
	{
		int j = i + 1;
		int k = i + size;

		indices.push_back(i);
		indices.push_back(k);

		if (i != 0 && i % n == 0)
		{
			indices.push_back(endPrimitive);
			n += size;
		}
	}

	indicesNumber = (GLuint)indices.size();

	#pragma endregion
}

void Terrain::clearData()
{
	// Czyszczenie pamięci
	vector<vec3>().swap(vertices);
	vector<vec3>().swap(normals);
	vector<vec2>().swap(textureUVs);
	vector<int>().swap(indices);
}

void Terrain::setupBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, buffers);

	// VBO dla wierzchołków
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), reinterpret_cast<GLfloat*>(&vertices[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VBO dla normali
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), reinterpret_cast<GLfloat*>(&normals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VBO dla koordynatów tekstur
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, textureUVs.size() * sizeof(vec2), reinterpret_cast<GLfloat*>(&textureUVs[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VBO dla indeksów
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), reinterpret_cast<GLuint*>(&indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
}
