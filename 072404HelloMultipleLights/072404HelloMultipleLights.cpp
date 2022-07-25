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
float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//U方向repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//V方向repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//纹理缩小时（大分辨率纹理应用在较远的小物体上）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理放大时（小分辨率纹理应用在大物体上，无mipmap）
	//stb_image库从磁盘加载贴图，如果加载成功，则glTexImage2D填充数据，glGenerateMipmap不需要手工为纹理创建mipmap
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
		glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的mipmap
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
	float yoffset = lastY - ypos;//鼠标位移：向上是正，向右是正
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

	//注册回调方法后，鼠标一移动Mouse_Callback函数就会被调用
	glfwSetCursorPosCallback(window, Mouse_Callback);
	//注册鼠标滚轮的回调函数
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

	//VAO：之所以要VAO是因为下面Draw的时候，不然要BindBuffer各种和Element
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//绑定了VAO，下面两行就关联到这个VAO上
	//第一个参数对应shader中：location = 0 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//解析VBO顶点数据，通过之前绑定到GL_ARRAY_BUFFER的VBO决定
	glEnableVertexAttribArray(0);//启用0号顶点属性，默认是禁用的
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

	//绑定完成，可以解绑 VBO VAO
	//因为EBO也可以存储在VAO中，所以EBO要在VAO之后解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Texture
	unsigned int diffuseTexture = LoadTexture("../Texture/container2.png");
	//stbi_set_flip_vertically_on_load(true);//Y轴翻转 stb_image库中方法
	unsigned int specMaskTexture = LoadTexture("../Texture/container2_specular.png");

	//每个sampler属于哪个纹理单元
	glUseProgram(modelShader.program);
	glUniform1i(glGetUniformLocation(modelShader.program, "material.diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(modelShader.program, "material.specularMaskTex"), 1);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//输入
		processInput(window);

		//渲染指令
		glClearColor(0.0, 0.0, 0.0, 1.0);//设置清空屏幕的颜色 这是个状态
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//用上面设置的状态，使用状态清理ColorBuffer

		//激活纹理位置，接下来bindTexture会绑定该texture到当前激活的纹理位置上
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specMaskTexture);

		//每次Drawcall都会使用这个着色器程序对象
		glUseProgram(modelShader.program);

		//让点光源动起来，pointLightPositions的y轴不变，xz平面绕着半径为3,5,12,3的大小圆旋转
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

		//灯可视化到场景中
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

		//交换双缓存，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();//本次循环是否触发键盘/鼠标等事件
	}
	glfwTerminate();
	return 0;
}