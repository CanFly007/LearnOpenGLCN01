#ifndef CAMERA_H
#define CAMERA_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<vector>

const float YAW = -90.0f;
const float PITCH = 0.0f;

const float ZOOM = 45.0f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw = -90.0f;
	float Pitch = 0.0f;
	// Camera options
	float Zoom = 45.0f;

	Camera(glm::vec3 cameraPos) :Position(cameraPos) 
	{
		Front = glm::vec3(0, 0, -1);
		WorldUp = glm::vec3(0, 1, 0);
		UpdateCameraVectors();
	}
	Camera(glm::vec3 cameraPos, glm::vec3 lookAtPos, glm::vec3 upDir)
		:Position(cameraPos), Front(lookAtPos - cameraPos), Up(upDir) 
	{
		UpdateCameraVectors();
	}

	glm::mat4 World2ViewMatrix();

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
	void ProcessMouseScroll(float yoffset);

private:

	void UpdateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

glm::mat4 Camera::World2ViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
	float sensity = 0.1f;
	xoffset *= sensity;
	yoffset *= sensity;
	Yaw += xoffset;
	Pitch += yoffset;
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	UpdateCameraVectors();
}
void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}
#endif