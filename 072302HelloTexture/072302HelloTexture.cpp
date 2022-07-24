#include <iostream>
#include <learnopengl/glfwgladInit.h>
#include <learnopengl/shader.h>
#include <stb_image.h>

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	GLFWwindow* window = Init();

	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // 左上
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
	//Shader shader = Shader("../vertex.glsl", "../fragment.glsl");
	Shader shader = Shader("helloTex.vs", "helloTex.fs");

	//VAO：之所以要VAO是因为下面Draw的时候，不然要BindBuffer各种和Element
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//绑定了VAO，下面两行就关联到这个VAO上
	//第一个参数对应shader中：location = 0 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//解析VBO顶点数据，通过之前绑定到GL_ARRAY_BUFFER的VBO决定
	glEnableVertexAttribArray(0);//启用0号顶点属性，默认是禁用的
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//绑定完成，可以解绑 VBO VAO
	//因为EBO也可以存储在VAO中，所以EBO要在VAO之后解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//U方向repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//V方向repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//纹理缩小时（大分辨率纹理应用在较远的小物体上）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理放大时（小分辨率纹理应用在大物体上，无mipmap）
	//stb_image库从磁盘加载贴图，如果加载成功，则glTexImage2D填充数据，glGenerateMipmap不需要手工为纹理创建mipmap
	int width, height, nrChannels;
	unsigned char* data = stbi_load("../Texture/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的mipmap
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//第二张笑脸图
	stbi_set_flip_vertically_on_load(true);//Y轴翻转 stb_image库中方法
	unsigned int textureSmile;
	glGenTextures(1, &textureSmile);
	glBindTexture(GL_TEXTURE_2D, textureSmile);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../Texture/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	

	//每个sampler属于哪个纹理单元
	glUseProgram(shader.program);
	glUniform1i(glGetUniformLocation(shader.program, "containerTex"), 1);
	glUniform1i(glGetUniformLocation(shader.program, "smileTex"), 0);

	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window);

		//渲染指令
		glClearColor(0.2, 0.3, 0.3, 1.0);//设置清空屏幕的颜色 这是个状态
		glClear(GL_COLOR_BUFFER_BIT);//用上面设置的状态，使用状态清理ColorBuffer

		//激活纹理位置，接下来bindTexture会绑定该texture到当前激活的纹理位置上
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSmile);

		//每次Drawcall都会使用这个着色器程序对象
		glUseProgram(shader.program);

		glBindVertexArray(VAO);
		//使用当前激活的着色器，之前定义的顶点属性配置（glVertexAttribPointer），和VBO的顶点数据（通过VAO间接绑定）来绘制图元（该例图元为GL_TRIANGLES）
		//glDrawArrays(GL_TRIANGLES, 0, 3);//画三角形
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//交换双缓存，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();//本次循环是否触发键盘/鼠标等事件
	}
	glfwTerminate();
	return 0;
}