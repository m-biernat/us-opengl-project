﻿// Projekt na zaliczenie przedmiotu Programowanie warstwy wizualnej gry
// Wykonanie: Michał Biernat, Anna Hosumbek INŻ IV PGK 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <IL/il.h>
#include "texture.h"

#include <iostream>

#include <shader.h>
#include "model.h"

#include "time.cpp"
#include "camera.cpp"
#include "lighting.h"
#include "material.h"
#include "terrain.h"
#include "skybox.h"

using namespace std;
using namespace glm;

#define WIDTH 800
#define HEIGHT 600
#define ANTI_ALIASING_SAMPLES 4
#define TITLE "pwwg-terrain M. Biernat, A. Hosumbek"

#define NUM_OF_TEX_OBJ 6

//******************************************************************************************

Time frameTime;
Camera camera(static_cast<float>(WIDTH) / HEIGHT);
MousePosition mousePosition(WIDTH, HEIGHT);

bool wireframe = false;
bool normalDebug = false;

Shader* defaultShader;
Shader* terrainShader;
Shader* skyboxShader;
Shader* debugShader;

Model* model;
Terrain* terrain;
Skybox* skybox;

GLuint texObj[NUM_OF_TEX_OBJ];
GLuint texCube;

mat4 projMatrix;
mat4 viewMatrix;

DirectLight directLight(vec3(0.0f, -0.5f, -1.0f), vec3(0.2f), vec3(0.8f), vec3(0.3f));
PointLight pointLight(vec3(0.0f, 0.0f, 10.0f), vec3(0.1f), vec3(1.0f) * 2.0f, vec3(0.7f), 1.0f, 0.09f, 0.032f);
vec3 pointLightPosition = pointLight.position;

Material objectMaterial(vec3(0.6266f, 0.4537f, 0.0481f), vec3(1.0f), 250.0f);
Material terrainMaterial;

glm::vec3 rotationAngles = glm::vec3(0.0, -45.0, 0.0); // katy rotacji wokol poszczegolnych osi
glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f);

//******************************************************************************************

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void glShowInfo();
void processInput(GLFWwindow* window);
void setupTextures();
void setupObjects();
void cleanup();
void render();

void setMaterials(Shader* shader, Material& material);

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, ANTI_ALIASING_SAMPLES);

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Blad: " << glewGetErrorString(err) << endl;
		return 1;
	}

	if (!GLEW_VERSION_3_3)
	{
		cout << "Brak obslugi OpenGL 3.3" << endl;
		return 1;
	}

	glShowInfo();

	glEnable(GL_MULTISAMPLE); // Włącza antyaliasing 
	glEnable(GL_DEPTH_TEST);  // Włącza test głębi
	glEnable(GL_CULL_FACE);	  // Rysuje wyłącznie widoczne elementy
	glfwSwapInterval(1);	  // Włącza synchronizacje pionową
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


	defaultShader = new Shader("shaders/default.vert", "shaders/default.frag");
	terrainShader = new Shader("shaders/terrain.vert", "shaders/terrain.frag");
	skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");

	debugShader = new Shader("shaders/debug.vert", "shaders/debug.frag", "shaders/debug.geom");

	setupTextures();
	setupObjects();


	while (!glfwWindowShouldClose(window))
	{
		frameTime.update(glfwGetTime()); // Przechowuje informacje o czasie między klatkami

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		projMatrix = camera.perspective();
		viewMatrix = camera.lookAt();

		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanup(); // Czyści pamięć i bufory po zakończeniu działania programu

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void errorCallback(int error, const char* description)
{
	cout << "Error: " << description << endl;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.calculateAspectRatio(width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS && key == GLFW_KEY_F1)
		wireframe = !wireframe;		// Ustawienie renderowania w trybie siatki

	if (action == GLFW_PRESS && key == GLFW_KEY_F2)
		normalDebug = !normalDebug;	// Włączenie shadera geometrii rysującego normale

	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT_SHIFT)
		camera.toggleSpeed();		// Zmienia tryb prędkości ruchu kamery
}

// Wykorzystujemy fubkcje zwrotną myszy do obrotu kamerą po przytrzymaniu PPM
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	mousePosition.Capture((float)xpos, (float)ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera.rotate(mousePosition.xOffset, mousePosition.yOffset);
}

void glShowInfo()
{
	cout << "GLEW = " << glewGetString(GLEW_VERSION) << endl;
	cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << endl;
	cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << endl;
	cout << "GL_VERSION = " << glGetString(GL_VERSION) << endl;
	cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

// Funkcja sterująca pozycją kamery, działająca w pętli głównej
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.move(Camera::Front, frameTime.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.move(Camera::Back, frameTime.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.move(Camera::Left, frameTime.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.move(Camera::Right, frameTime.deltaTime);
}

void setupTextures()
{
	glGenTextures(NUM_OF_TEX_OBJ, texObj);

	Texture::loadTexture(texObj[0], L"textures/fabricDiff.jpg");
	Texture::loadTexture(texObj[1], L"textures/fabricSpec.jpg");
	Texture::loadTexture(texObj[2], L"textures/fabricNorm.jpg");
	Texture::loadTexture(texObj[3], L"textures/sandDiff.jpg");
	Texture::loadTexture(texObj[4], L"textures/sandSpec.jpg");
	Texture::loadTexture(texObj[5], L"textures/sandNorm.jpg");

	glGenTextures(1, &texCube);

	const wchar_t* cubemapFilename[6] = { 
		L"textures/skybox/right.jpg", L"textures/skybox/left.jpg",
		L"textures/skybox/top.jpg", L"textures/skybox/bottom.jpg",
		L"textures/skybox/front.jpg", L"textures/skybox/back.jpg" 
	};

	Texture::loadCubemap(texCube, cubemapFilename);
}

void setupObjects()
{
	model = new Model("models/monkey.obj");

	vec3 extent = abs(model->getBBmax() - model->getBBmin());
	float maxExtent = glm::max(glm::max(extent.x, extent.y), extent.z);
	modelScale = vec3(7.0 / maxExtent);

	terrain = new Terrain(256, 180.0f, 0.9f, 4);

	skybox = new Skybox();
}

void cleanup()
{
	glDeleteProgram(defaultShader->ID);
	glDeleteProgram(terrainShader->ID);
	glDeleteProgram(skyboxShader->ID);
	glDeleteProgram(debugShader->ID);
	
	delete(defaultShader);
	delete(terrainShader);
	delete(skyboxShader);
	delete(debugShader);
	
	glDeleteTextures(NUM_OF_TEX_OBJ, texObj);

	delete(model);
	delete(terrain);
	delete(skybox);
}

void render()
{
	// Renderowanie obiektu
	defaultShader->use();

	defaultShader->setMat4("projectionMatrix", projMatrix);
	defaultShader->setMat4("viewMatrix", viewMatrix);
	defaultShader->setVec3("viewPos", camera.position);

	float rot = 0.5f;
	mat4 modelMatrix = mat4(1.0f);

	modelMatrix = translate(modelMatrix, vec3(0.0f, 5.0f, -15.0f));
	modelMatrix = scale(modelMatrix, modelScale);
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.z), vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.y += rot), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.x), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = translate(modelMatrix, -model->getCentroid());

	defaultShader->setMat4("modelMatrix", modelMatrix);

	pointLight.position = modelMatrix * vec4(pointLightPosition, 1.0f);

	setMaterials(defaultShader, objectMaterial);

	defaultShader->setBool("texturing", true);
	defaultShader->setVec2("tiling", vec2(2));

	defaultShader->setInt("diffuseTex", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texObj[0]);

	defaultShader->setInt("specularTex", 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texObj[1]);

	defaultShader->setInt("normalTex", 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texObj[2]);

	model->draw();

	if (normalDebug)
	{
		debugShader->use();
		debugShader->setMat4("projMatrix", projMatrix);
		debugShader->setMat4("viewMatrix", viewMatrix);
		debugShader->setMat4("modelMatrix", modelMatrix);

		model->draw();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Renderowanie terenu
	terrainShader->use();

	terrainShader->setMat4("projectionMatrix", projMatrix);
	terrainShader->setMat4("viewMatrix", viewMatrix);
	terrainShader->setVec3("viewPos", camera.position);
	
	terrainShader->setMat4("modelMatrix", terrain->modelMatrix);

	setMaterials(terrainShader, terrainMaterial);

	terrainShader->setBool("texturing", true);
	terrainShader->setVec2("tiling", vec2(0.1));

	terrainShader->setInt("diffuseTex", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texObj[3]);

	terrainShader->setInt("specularTex", 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texObj[4]);

	terrainShader->setInt("normalTex", 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texObj[5]);

	terrain->draw();

	if (normalDebug)
	{
		debugShader->use();
		debugShader->setMat4("projMatrix", projMatrix);
		debugShader->setMat4("viewMatrix", viewMatrix);
		debugShader->setMat4("modelMatrix", terrain->modelMatrix);

		terrain->draw();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Renderowanie skyboxa
	skyboxShader->use();

	skyboxShader->setMat4("projMatrix", projMatrix);
	skyboxShader->setMat4("viewMatrix", viewMatrix);

	//skyboxShader->setInt("skyboxTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);

	skybox->draw();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void setMaterials(Shader* shader, Material& material)
{
	shader->setVec3("directLight.direction", directLight.direction);
	shader->setVec3("directLight.ambient", directLight.ambient);
	shader->setVec3("directLight.diffuse", directLight.diffuse);
	shader->setVec3("directLight.specular", directLight.specular);

	shader->setVec3("pointLight.position", pointLight.position);
	shader->setVec3("pointLight.ambient", pointLight.ambient);
	shader->setVec3("pointLight.diffuse", pointLight.diffuse);
	shader->setVec3("pointLight.specular", pointLight.specular);
	shader->setFloat("pointLight.constant", pointLight.constant);
	shader->setFloat("pointLight.linear", pointLight.linear);
	shader->setFloat("pointLight.quadratic", pointLight.quadratic);

	shader->setVec3("material.diffuse", material.diffuse);
	shader->setVec3("material.specular", material.specular);
	shader->setFloat("material.shininess", material.shininess);
}
