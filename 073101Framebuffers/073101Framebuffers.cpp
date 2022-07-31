#include <iostream>
#include "../learnopenglInclude/glfwgladInit.h"
#include "../learnopenglInclude/shader.h"
#include "../learnopenglInclude/camera.h"
#include <stb_image.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

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
	Shader modelShader = Shader("ModelObj.vs", "ModelObj.fs");
	Shader screenShader = Shader("ScreenFramebuffer.vs", "ScreenFramebuffer.fs");

	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		-5.0f, -0.5f, 5.0f,  0.0f, 0.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f
	};
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);//����VAO���������о͹��������VAO��

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	//Texture
	unsigned int cubeTex = LoadTexture("../Texture/marble.jpg");
	//stbi_set_flip_vertically_on_load(true);//Y�ᷭת stb_image���з���
	unsigned int floorTex = LoadTexture("../Texture/metal.png");
	
	//ÿ��sampler�����ĸ�����Ԫ
	glUseProgram(modelShader.program);
	glUniform1i(glGetUniformLocation(modelShader.program, "_Diffuse"), 0);

	glUseProgram(screenShader.program);
	glUniform1i(glGetUniformLocation(screenShader.program, "screenTexture"), 0);

	//��Ȳ���
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//���޳�
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);//��ʱ��Ϊ����Ĭ�ϣ� GL_CW��ʾ˳ʱ��
	glCullFace(GL_BACK);//�޳����棨Ĭ�ϣ� GL_FRONT��GL_FRONT_AND_BACK

	//Framebuffers
	//��Ҫ��������һ�Ű���color��Ϣ��һ�Ű�����ȣ����ڳ������������Ȳ��ԣ����û�������framebuffer�޷�����Ȳ��Թ��ܣ�
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	//colorBuffer��һ������
	unsigned int colorTex;
	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	//��Ȼ������һ�������У�(������Renderbuffer���������Ż��� �����ʽҪ�ã�GL_DEPTH_COMPONENT
	unsigned int depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//����framebuffer�Ϳ��Լ�¼(attach����color��depth��Ϣ

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//����
		processInput(window);

		//��һ�׶Σ����������Զ����framebuffer��
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//���������Ļ����ɫ ���Ǹ�״̬
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//���������õ�״̬��ʹ��״̬����ColorBuffer
		glEnable(GL_DEPTH_TEST);
		glUseProgram(modelShader.program);

		//��������λ�ã�������bindTexture��󶨸�texture����ǰ���������λ����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTex);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, floorTex);
		SetViewProjectionMat4(modelShader);//VP���󴫵�modelShader����

		//������������
		glBindVertexArray(cubeVAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//���ذ�
		glBindTexture(GL_TEXTURE_2D, floorTex);
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(modelShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//�ڶ��׶Σ���framebufferͼ�����ݣ�������������Ƭ��
		glBindFramebuffer(GL_FRAMEBUFFER, 0);//Ĭ��֡����
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(screenShader.program);
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);//�ر���Ȳ���
		glBindTexture(GL_TEXTURE_2D, colorTex);//colorTex��ľ���framebuffer�е�color attach��Ϣ
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//����˫���棬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();//����ѭ���Ƿ񴥷�����/�����¼�
	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &quadVBO);

	glfwTerminate();
	return 0;
}