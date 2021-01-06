#include "Skybox.h"

Skybox::Skybox()
{
	setupBuffers();
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, buffers);
}

void Skybox::draw()
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(endPrimitive);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, indicesNumber, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);

	glDisable(GL_PRIMITIVE_RESTART);
}

void Skybox::setupBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, buffers);

	float t = 250.0f; // wielkosc szescianu

	GLfloat v[] =
	{
		t, -t, -t,
		t, -t, t,
		t, t, -t,
		t, t, t,
		-t, -t, -t,
		-t, t, -t,
		-t, -t, t,
		-t, t, t
	};

	endPrimitive = sizeof(v) / (sizeof(GLfloat) * 3);

	GLint faces[] =
	{
		0, 1, 2, 3, endPrimitive,
		4, 5, 6, 7, endPrimitive,
		5, 2, 7, 3, endPrimitive,
		4, 6, 0, 1, endPrimitive,
		6, 7, 1, 3, endPrimitive,
		4, 0, 5, 2
	};

	indicesNumber = sizeof(faces) / sizeof(GLint);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	glBindVertexArray(0);
}
