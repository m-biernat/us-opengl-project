#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct Material
{
	vec3 diffuse;
	vec3 specular;

	float shininess;

	Material(vec3 diffuse = vec3(1), vec3 specular = vec3(1), float shininess = 32.0f)
	{
		this->diffuse = diffuse;
		this->specular = specular;

		this->shininess = shininess;
	}
};
