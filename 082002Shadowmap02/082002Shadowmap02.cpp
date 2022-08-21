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
float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
bool gammaEnabled = false;
bool gammaKeyPressed = false;

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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !gammaKeyPressed)
	{
		gammaEnabled = !gammaEnabled;
		gammaKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}
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

unsigned int LoadCubemap(std::vector<std::string> textures_face)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures_face.size(); i++)//加载6张图
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//U方向repeat or clamp
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//Cubemap才有的，第三个维度
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理放大时（小分辨率纹理应用在大物体上，无mipmap）
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//纹理缩小时（大分辨率纹理应用在较远的小物体上）
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
	float yoffset = lastY - ypos;//鼠标位移：向上是正，向右是正
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}
void Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void RenderScene(const Shader& shader);
void RenderCube();
unsigned int planeVAO;
void RenderScene(const Shader& shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void RenderCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

int main()
{
	GLFWwindow* window = Init();

	//注册回调方法后，鼠标一移动Mouse_Callback函数就会被调用
	glfwSetCursorPosCallback(window, Mouse_Callback);
	//注册鼠标滚轮的回调函数
	glfwSetScrollCallback(window, Scroll_Callback);

	//Shader
	Shader shadowMapShader("Shadowmap.vs", "Shadowmap.fs");
	Shader modelShader("Scene.vs", "Scene.fs");

	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f
	};
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glBindVertexArray(planeVAO);//绑定了VAO，下面两行就关联到这个VAO上

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)6);
	glBindVertexArray(0);

	//允许修改顶点大小
	glEnable(GL_PROGRAM_POINT_SIZE);

	//深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//面剔除
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);//逆时针为正向（默认） GL_CW表示顺时针
	//glCullFace(GL_BACK);//剔除背面（默认） GL_FRONT、GL_FRONT_AND_BACK

	glEnable(GL_MULTISAMPLE);//启用多重采样，默认是开启的

	//Texture
	unsigned int floorTexture = LoadTexture("../Texture/wood.png");
	glUseProgram(modelShader.program);
	glUniform1i(glGetUniformLocation(modelShader.program, "_Diffuse"), 0);//shader多贴图时候，从0开始
	glUniform1i(glGetUniformLocation(modelShader.program, "shadowMap"), 1);//shader多贴图时候，从0开始

	//Shadowmap
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);//第一步生成一张shadowmap，通过场景渲染一张帧缓存赋值给shadowmap

	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);//shadowmap贴图
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);//shadowmap贴图绑定到帧缓存的深度缓存attach上
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);//不包含颜色缓冲的帧缓冲对象是不完整的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "GL_FRAMEBUFFER Success" << std::endl;
	}

	glUseProgram(modelShader.program);
	glUniform1i(glGetUniformLocation(modelShader.program, "shadowMap"), 0);

	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//输入
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//设置清空屏幕的颜色 这是个状态
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//用上面设置的状态，使用状态清理ColorBuffer
		glEnable(GL_DEPTH_TEST);

		//ShadowMapping
		//设置物体到光源空间的View和Projection矩阵
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float left = -10.0f, right = 10.0f, bottom = -10.0f, top = 10.0f;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(left, right, bottom, top, near_plane, far_plane);//这个平行光的范围
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		glUseProgram(shadowMapShader.program);
		glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.program, "VPMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		//绘制场景到framebuffer上，framebuffer的depthattach了shadowmap贴图，所以填充到了shadowmap贴图上
		//一定要记得调用glViewport。因为shadowmap经常和我们原来渲染的场景有着不同的分辨率
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(shadowMapShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//画正常场景，用上一步绘制得到的shadowmap图作为参数传递到这一步的绘制中，还要传物体到光源空间的矩阵
		glViewport(0, 0, 800, 600);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(modelShader.program);
		SetViewProjectionMat4(modelShader);
		glUniform3fv(glGetUniformLocation(modelShader.program, "viewPos"), 1, glm::value_ptr(camera.Position));
		glUniform3fv(glGetUniformLocation(modelShader.program, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		RenderScene(modelShader);

		//交换双缓存，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();//本次循环是否触发键盘/鼠标等事件
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}