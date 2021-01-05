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
#include "terrain.h"
#include "lighting.h"
#include "material.h"

using namespace std;
using namespace glm;

#define WIDTH 800
#define HEIGHT 600
#define ANTI_ALIASING_SAMPLES 4
#define TITLE "pwwg-terrain M. Biernat, A. Hosumbek"

#define NUM_OF_TEX_OBJ 2

//******************************************************************************************

Time frameTime;
Camera camera(static_cast<float>(WIDTH) / HEIGHT);
MousePosition mousePosition(WIDTH, HEIGHT);

bool wireframe = false;

Shader* defaultShader;
Shader* terrainShader;

Terrain* terrain;
Model* model;

GLuint texObj[NUM_OF_TEX_OBJ];

mat4 projMatrix;
mat4 viewMatrix;

//DirectLight directLight;
PointLight pointLight(vec3(0.0f, 0.0f, 10.0f), vec3(1.0f));
vec3 ambientLight = vec3(0.2f);

Material objectMaterial(vec3(1.0f), vec3(1.0f), 83.2f);
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

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glfwSwapInterval(1);	// v-sync on
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


	defaultShader = new Shader("shaders/default.vert", "shaders/default.frag");
	terrainShader = new Shader("shaders/terrain.vert", "shaders/terrain.frag");

	setupTextures();
	setupObjects();


	while (!glfwWindowShouldClose(window))
	{
		frameTime.update(glfwGetTime());

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

	cleanup();

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
		wireframe = !wireframe;
}

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

	Texture::loadTexture(texObj[0], L"textures/diffuse.png");
	Texture::loadTexture(texObj[1], L"textures/specular.png");
}

void setupObjects()
{
	terrain = new Terrain(128, 50.0f, 0.5f);

	model = new Model("models/monkey.obj");

	vec3 extent = abs(model->getBBmax() - model->getBBmin());
	float maxExtent = glm::max(glm::max(extent.x, extent.y), extent.z);
	modelScale = vec3(7.0 / maxExtent);
}

void cleanup()
{
	glDeleteProgram(defaultShader->ID);
	glDeleteProgram(terrainShader->ID);
	delete(defaultShader);
	delete(terrainShader);
	
	glDeleteTextures(NUM_OF_TEX_OBJ, texObj);

	delete(terrain);
	delete(model);
}

void render()
{
	defaultShader->use();

	defaultShader->setMat4("projectionMatrix", projMatrix);
	defaultShader->setMat4("viewMatrix", viewMatrix);
	defaultShader->setVec3("viewPos", camera.position);

	float rot = 0.5f;
	mat4 modelMatrix = mat4(1.0f);

	modelMatrix = scale(modelMatrix, modelScale);
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.z), vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.y += rot), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotationAngles.x), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = translate(modelMatrix, -model->getCentroid());
	modelMatrix = translate(modelMatrix, vec3(0.0f, 2.5f, 0.0f));

	vec4 lightPos = modelMatrix * vec4(pointLight.position, 1.0f);
	defaultShader->setMat4("modelMatrix", modelMatrix);

	defaultShader->setVec3("pointLight.position", vec3(lightPos));
	defaultShader->setVec3("pointLight.color", pointLight.color);
	defaultShader->setVec3("ambientLight", ambientLight);

	defaultShader->setVec3("material.diffuse", objectMaterial.diffuse);
	defaultShader->setVec3("material.specular", objectMaterial.specular);
	defaultShader->setFloat("material.shininess", objectMaterial.shininess);

	defaultShader->setInt("diffuseTex", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texObj[0]);

	defaultShader->setInt("specularTex", 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texObj[1]);

	defaultShader->setBool("texturing", (model->hasTextureCoords()) ? 1 : 0);

	model->draw();


	terrainShader->use();

	terrainShader->setMat4("projectionMatrix", projMatrix);
	terrainShader->setMat4("viewMatrix", viewMatrix);
	terrainShader->setVec3("viewPos", camera.position);
	
	terrainShader->setMat4("modelMatrix", terrain->modelMatrix);

	terrainShader->setVec3("pointLight.position", vec3(lightPos));
	terrainShader->setVec3("pointLight.color", pointLight.color);
	terrainShader->setVec3("ambientLight", ambientLight);

	terrainShader->setVec3("material.diffuse", terrainMaterial.diffuse);
	terrainShader->setVec3("material.specular", terrainMaterial.specular);
	terrainShader->setFloat("material.shininess", terrainMaterial.shininess);

	terrainShader->setBool("texturing", true);
	terrainShader->setVec2("tiling", vec2(8, 8));

	terrainShader->setInt("diffuseTex", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texObj[0]);

	terrainShader->setInt("specularTex", 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texObj[1]);

	terrain->draw();
}
