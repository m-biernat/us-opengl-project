#pragma once

class Time
{
public:
	double deltaTime;
	double lastFrame;

	Time()
	{
		deltaTime = 0.0f;
		lastFrame = 0.0f;
	}

	void update(double currentFrame)
	{
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
};

