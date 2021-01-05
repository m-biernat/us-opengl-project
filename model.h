#ifndef __MODEL_H__
#define __MODEL_H__

#include <glm\glm.hpp>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>

#include "mesh.h"

class Model
{
	std::vector<Mesh*> meshes;
	glm::vec3 bbMin;
	glm::vec3 bbMax;
	glm::vec3 centroid;
	bool hasTexCoords;

public:
	Model();
	Model(const std::string& filename, GLint vertexLoc = 0, GLint normalLoc = 1, GLint texLoc = 2);
	Model(const aiScene *scene, GLint vertexLoc, GLint normalLoc, GLint texLoc);
	~Model();

	void draw() const;
	Mesh* getMesh(unsigned int n) const;
	unsigned int getNumberOfMeshes() const;
	glm::vec3 getBBmin() const;
	glm::vec3 getBBmax() const;
	glm::vec3 getCentroid() const;
	bool hasTextureCoords() const;

private:
	bool importModelFromFile( const std::string& filename, Assimp::Importer& importer, const aiScene **scene );
	void modelFromScene(const aiScene *scene, GLint vertexLoc, GLint normalLoc, GLint texLoc);
	void computeBoundingBox();
	void computeCentroid(const aiScene *scene);
};

#endif /*__MODEL_H__*/