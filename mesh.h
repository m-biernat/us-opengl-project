#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <GL/glcorearb.h>
#include <assimp/Scene.h>

class Mesh
{
	static constexpr int VBOS = 4;

	GLuint vao;
	GLuint vbo[VBOS];
	unsigned int numTriangles;
	glm::vec3 bbMin;
	glm::vec3 bbMax;
	bool hasTexCoords;

public:
	Mesh();
	Mesh(const aiMesh *mesh, GLint vertexLoc = 0, GLint normalLoc = 1, GLint texCoordLoc = 2);
	~Mesh();

	void draw() const;
	glm::vec3 getBBmin() const;
	glm::vec3 getBBmax() const;
	bool hasTextureCoords() const;
};

#endif /*__MESH_H__*/