#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <random>

#include "terrain.h"

using namespace std;
using namespace glm;

Terrain::Terrain(int size, float sizeScale, float heightScale, int octaves)
{
	this->size = size;
	this->octaves = octaves;

	default_random_engine generator;
	uniform_int_distribution<int> random(1, INT_MAX);

	pn = new PerlinNoise(random(generator));

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
	glDeleteBuffers(BUFFERS_COUNT, buffers);
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
			vec2 p = vec2(j / (float)size, i / (float)size);
			float height = getHeight(getHeight(p) + p); // domain warping 
			
			vertices.push_back(vec3(x, y, height));
			
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

	#pragma region Generowanie normali

	vector<ivec3> triangles;
	int i = 0, loops = 0;
	int skip = size - 1;

	cout << endPrimitive << endl << indices.size() << endl;

	while (i < indices.size() - size)
	{
		if (indices[i] == endPrimitive)
		{
			//cout << endl << i << endl;
			i++;
		}

		if (loops != skip)
		{
			ivec3 t1 = ivec3(indices[i + 1], indices[i], indices[i + 2]);
			ivec3 t2 = ivec3(indices[i + 2], indices[i + 3], indices[i + 1]);

			triangles.push_back(t1);
			triangles.push_back(t2);

			//cout << "(" << i << ") " << t1.x << " " << t1.y << " " << t1.z << ", " << t2.x << " " << t2.y << " " << t2.z << " ;";
		}
		else
			skip += size;

		loops++;
		i += 2;
	}

	for (int i = 0; i < vertices.size(); i++)
		normals.push_back(vec3(0.0f));

	for (int i = 0; i < triangles.size(); i++)
	{
		const int ia = triangles[i].x;
		const int ib = triangles[i].y;
		const int ic = triangles[i].z;

		const vec3 e1 = vertices[ia] - vertices[ib];
		const vec3 e2 = vertices[ic] - vertices[ib];
		const vec3 no = cross(e1, e2);

		normals[ia] += no;
		normals[ib] += no;
		normals[ic] += no;
	}

	for (int i = 0; i < normals.size(); i++)
		normals[i] = normalize(normals[i]);

	#pragma endregion
}

void Terrain::clearData()
{
	// Czyszczenie pamięci
	vector<vec3>().swap(vertices);
	vector<vec3>().swap(normals);
	vector<int>().swap(indices);
}

void Terrain::setupBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(BUFFERS_COUNT, buffers);

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

	// VBO dla indeksów
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), reinterpret_cast<GLuint*>(&indices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

float Terrain::getHeight(vec2 p)
{
	float frequency = 0.2;
	float amplitude = 3;
	float noise = 0;

	for (int i = 0; i < octaves; i++)
	{
		noise += ((pn->noise(p.x * frequency * 10, p.y * frequency * 10, 1.0) * 2) - 1) * amplitude;
		amplitude *= 0.5;
		frequency *= 2;
	}

	return noise;
}
