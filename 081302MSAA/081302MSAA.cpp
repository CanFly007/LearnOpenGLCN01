#include <iostream>
#include "../learnopenglInclude/glfwgladInit.h"
#include "../learnopenglInclude/shader.h"
#include "../learnopenglInclude/camera.h"
#include <stb_image.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//#include <map>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float deltaTime = 0.0f;//��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;//��һ֡��ʱ��
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Position += cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Position -= cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
}

int LoadTexture(const char* texturePath)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//U����repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//V����repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//������Сʱ����ֱ�������Ӧ���ڽ�Զ��С�����ϣ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//����Ŵ�ʱ��С�ֱ�������Ӧ���ڴ������ϣ���mipmap��
	//stb_image��Ӵ��̼�����ͼ��������سɹ�����glTexImage2D������ݣ�glGenerateMipmap����Ҫ�ֹ�Ϊ������mipmap
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);//Ϊ��ǰ�󶨵������Զ�����������Ҫ��mipmap
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

unsigned int LoadCubemap(std::vector<std::string> textures_face)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures_face.size(); i++)//����6��ͼ
	{
		unsigned char* data = stbi_load(textures_face[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << textures_face[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//U����repeat or clamp
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//Cubemap���еģ�������ά��
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//����Ŵ�ʱ��С�ֱ�������Ӧ���ڴ������ϣ���mipmap��
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//������Сʱ����ֱ�������Ӧ���ڽ�Զ��С�����ϣ�
	return texture;
}

void SetViewProjectionMat4(Shader& shader);
void TransformLightObj(Shader& shader)
{
	glm::mat4 model;
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	SetViewProjectionMat4(shader);
}
void TransformObj(Shader& shader)
{
	glm::mat4 model;
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	int modelLoc = glGetUniformLocation(shader.program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	SetViewProjectionMat4(shader);
}
void SetViewProjectionMat4(Shader& shader)
{
	glm::mat4 view;
	view = camera.World2ViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective<float>(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

	int viewLoc = glGetUniformLocation(shader.program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(shader.program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void SetUniformValue(unsigned int program, glm::vec3* pointLightPositions)
{
	glUniform3fv(glGetUniformLocation(program, "pointLights[0].position"), 1, glm::value_ptr(pointLightPositions[0]));
	glUniform3fv(glGetUniformLocation(program, "pointLights[1].position"), 1, glm::value_ptr(pointLightPositions[1]));
	glUniform3fv(glGetUniformLocation(program, "pointLights[2].position"), 1, glm::value_ptr(pointLightPositions[0]));
	glUniform3fv(glGetUniformLocation(program, "pointLights[3].position"), 1, glm::value_ptr(pointLightPositions[0]));
	glUniform3f(glGetUniformLocation(program, "pointLights[0].color"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(program, "pointLights[1].color"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(program, "pointLights[2].color"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(program, "pointLights[3].color"), 1.0f, 1.0f, 1.0f);

	glUniform3fv(glGetUniformLocation(program, "spotLight.position"), 1, glm::value_ptr(camera.Position));
	glUniform3fv(glGetUniformLocation(program, "spotLight.direction"), 1, glm::value_ptr(camera.Front));
	glUniform3f(glGetUniformLocation(program, "spotLight.color"), 1.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(program, "spotLight.range"), glm::cos(glm::radians(30.0f)));
}

void Mouse_Callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;//���λ�ƣ�������������������
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}
void Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

int main()
{
	GLFWwindow* window = Init();

	//ע��ص����������һ�ƶ�Mouse_Callback�����ͻᱻ����
	glfwSetCursorPosCallback(window, Mouse_Callback);
	//ע�������ֵĻص�����
	glfwSetScrollCallback(window, Scroll_Callback);

	//Shader
	Shader shader("Cube.vs", "Cube.fs");

	GLfloat cubeVertices[] = {
		// Positions       
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);//����VAO���������о͹��������VAO��

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//�����޸Ķ����С
	glEnable(GL_PROGRAM_POINT_SIZE);

	//��Ȳ���
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//���޳�
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);//��ʱ��Ϊ����Ĭ�ϣ� GL_CW��ʾ˳ʱ��
	//glCullFace(GL_BACK);//�޳����棨Ĭ�ϣ� GL_FRONT��GL_FRONT_AND_BACK

	glEnable(GL_MULTISAMPLE);//���ö��ز�����Ĭ���ǿ�����

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//����
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//���������Ļ����ɫ ���Ǹ�״̬
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//���������õ�״̬��ʹ��״̬����ColorBuffer
		glEnable(GL_DEPTH_TEST);

		glUseProgram(shader.program);
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		SetViewProjectionMat4(shader);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//����˫���棬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();//����ѭ���Ƿ񴥷�����/�����¼�
	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glfwTerminate();
	return 0;
}