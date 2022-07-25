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

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f
	};
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,  0.2f,  0.0f),
	glm::vec3(0.0f, -3.3f, 0.0f),
	glm::vec3(0.0f,  2.0f, 0.0f),
	glm::vec3(0.0f,  0.0f, 0.0f)
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Shader
	Shader modelShader = Shader("ModelObj.vs", "ModelObj.fs");
	Shader lightShader = Shader("LightObj.vs", "LightObj.fs");

	//VAO��֮����ҪVAO����Ϊ����Draw��ʱ�򣬲�ȻҪBindBuffer���ֺ�Element
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//����VAO���������о͹��������VAO��
	//��һ��������Ӧshader�У�location = 0 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//����VBO�������ݣ�ͨ��֮ǰ�󶨵�GL_ARRAY_BUFFER��VBO����
	glEnableVertexAttribArray(0);//����0�Ŷ������ԣ�Ĭ���ǽ��õ�
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//lightVAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//EBO
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//����ɣ����Խ�� VBO VAO
	//��ΪEBOҲ���Դ洢��VAO�У�����EBOҪ��VAO֮����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Texture
	unsigned int diffuseTexture = LoadTexture("../Texture/container2.png");
	//stbi_set_flip_vertically_on_load(true);//Y�ᷭת stb_image���з���
	unsigned int specMaskTexture = LoadTexture("../Texture/container2_specular.png");

	//ÿ��sampler�����ĸ�����Ԫ
	glUseProgram(modelShader.program);
	glUniform1i(glGetUniformLocation(modelShader.program, "material.diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(modelShader.program, "material.specularMaskTex"), 1);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//����
		processInput(window);

		//��Ⱦָ��
		glClearColor(0.0, 0.0, 0.0, 1.0);//���������Ļ����ɫ ���Ǹ�״̬
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//���������õ�״̬��ʹ��״̬����ColorBuffer

		//��������λ�ã�������bindTexture��󶨸�texture����ǰ���������λ����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specMaskTexture);

		//ÿ��Drawcall����ʹ�������ɫ���������
		glUseProgram(modelShader.program);

		//�õ��Դ��������pointLightPositions��y�᲻�䣬xzƽ�����Ű뾶Ϊ3,5,12,3�Ĵ�СԲ��ת
		for (int i = 0; i < 4; i++)
		{
			pointLightPositions[i].x += cos(currentFrame) / 10 * (i * 0.1);
			pointLightPositions[i].z += sin(currentFrame) / 10 * (i * 0.1);
		}

		SetUniformValue(modelShader.program, pointLightPositions);

		glUniform3f(glGetUniformLocation(modelShader.program, "material.specularColor"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(modelShader.program, "material.gloss"), 256.0f);

		glUniform3f(glGetUniformLocation(modelShader.program, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3fv(glGetUniformLocation(modelShader.program, "worldlightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(modelShader.program, "worldViewPos"), 1, glm::value_ptr(camera.Position));

		glBindVertexArray(VAO);

		SetViewProjectionMat4(modelShader);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//�ƿ��ӻ���������
		glUseProgram(lightShader.program);
		glBindVertexArray(lightVAO);
		SetViewProjectionMat4(lightShader);
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//����˫���棬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();//����ѭ���Ƿ񴥷�����/�����¼�
	}
	glfwTerminate();
	return 0;
}