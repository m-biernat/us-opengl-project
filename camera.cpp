#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#define CAMERA_POSITION vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_FRONT vec3(0.0f, 0.0f, -1.0f)
#define CAMERA_UP vec3(0.0f, 1.0f, 0.0f)

#define CAMERA_SPEED 2.5f
#define CAMERA_FOV 45.0f

#define CAMERA_YAW -90.0f
#define CAMERA_PITCH 0.0f

class Camera
{
public:
	vec3 position;

	enum Direction { Front, Back, Left, Right };

	Camera(float aspectRatio)
	{
		position = CAMERA_POSITION;
		front = CAMERA_FRONT;
		up = CAMERA_UP;
		
		speed = CAMERA_SPEED;
		fov = CAMERA_FOV;

		yaw = CAMERA_YAW;
		pitch = CAMERA_PITCH;

		this->aspectRatio = aspectRatio;
	}

	mat4 lookAt()
	{
		return glm::lookAt(position, position + front, up);
	}

	mat4 perspective()
	{
		return glm::perspective(radians(fov), aspectRatio, 0.1f, 100.0f);
	}

	void calculateAspectRatio(int width, int height)
	{
		aspectRatio = static_cast<float>(width) / ((height == 0) ? 1 : height);
	}

	void move(Direction direction, double deltaTime)
	{
		float speed = this->speed * (float)deltaTime;

		switch (direction)
		{
		case Front:
			position += speed * front;
			break;
		case Back:
			position -= speed * front;
			break;
		case Left:
			position -= normalize(cross(front, up)) * speed;
			break;
		case Right:
			position += normalize(cross(front, up)) * speed;
			break;
		}
	}

	void rotate(float xOffset, float yOffset)
	{
		float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		vec3 front;
		front.x = cos(radians(yaw)) * cos(radians(pitch));
		front.y = sin(radians(pitch));
		front.z = sin(radians(yaw)) * cos(radians(pitch));
		this->front = normalize(front);
	}

private:
	vec3 front;
	vec3 up;

	float speed;
	float fov;

	float yaw;
	float pitch;

	float aspectRatio;
};

class MousePosition 
{
public:
	float xOffset;
	float yOffset;

	MousePosition(float width, float height)
	{
		firstCapture = true;

		lastX = width / 2.0f;
		lastY = height / 2.0f;

		xOffset = 0.0f;
		yOffset = 0.0f;
	}

	void Capture(float x, float y)
	{
		if (firstCapture)
		{
			lastX = x;
			lastY = y;
			
			firstCapture = false;
		}

		xOffset = x - lastX;
		yOffset = lastY - y;
		
		lastX = x;
		lastY = y;
	}

private:
	bool firstCapture;
	
	float lastX;
	float lastY;
};

