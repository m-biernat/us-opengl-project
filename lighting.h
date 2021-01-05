#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct DirectLight
{
	vec3 direction;
	vec3 color;

	DirectLight(vec3 direction, vec3 color)
	{
		this->direction = direction;
		this->color = color;
	}
};

struct PointLight
{
	vec3 position;
	vec3 color;

	PointLight(vec3 position, vec3 color)
	{
		this->position = position;
		this->color = color;
	}
};
