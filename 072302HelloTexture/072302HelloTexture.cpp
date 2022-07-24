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
		//     ---- λ�� ----       ---- ��ɫ ----     - �������� -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // ����
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // ����
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // ����
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

	//VAO��֮����ҪVAO����Ϊ����Draw��ʱ�򣬲�ȻҪBindBuffer���ֺ�Element
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//����VAO���������о͹��������VAO��
	//��һ��������Ӧshader�У�location = 0 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//����VBO�������ݣ�ͨ��֮ǰ�󶨵�GL_ARRAY_BUFFER��VBO����
	glEnableVertexAttribArray(0);//����0�Ŷ������ԣ�Ĭ���ǽ��õ�
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//����ɣ����Խ�� VBO VAO
	//��ΪEBOҲ���Դ洢��VAO�У�����EBOҪ��VAO֮����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//U����repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//V����repeat or clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//������Сʱ����ֱ�������Ӧ���ڽ�Զ��С�����ϣ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//����Ŵ�ʱ��С�ֱ�������Ӧ���ڴ������ϣ���mipmap��
	//stb_image��Ӵ��̼�����ͼ��������سɹ�����glTexImage2D������ݣ�glGenerateMipmap����Ҫ�ֹ�Ϊ������mipmap
	int width, height, nrChannels;
	unsigned char* data = stbi_load("../Texture/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);//Ϊ��ǰ�󶨵������Զ�����������Ҫ��mipmap
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//�ڶ���Ц��ͼ
	stbi_set_flip_vertically_on_load(true);//Y�ᷭת stb_image���з���
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
	

	//ÿ��sampler�����ĸ�����Ԫ
	glUseProgram(shader.program);
	glUniform1i(glGetUniformLocation(shader.program, "containerTex"), 1);
	glUniform1i(glGetUniformLocation(shader.program, "smileTex"), 0);

	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window);

		//��Ⱦָ��
		glClearColor(0.2, 0.3, 0.3, 1.0);//���������Ļ����ɫ ���Ǹ�״̬
		glClear(GL_COLOR_BUFFER_BIT);//���������õ�״̬��ʹ��״̬����ColorBuffer

		//��������λ�ã�������bindTexture��󶨸�texture����ǰ���������λ����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSmile);

		//ÿ��Drawcall����ʹ�������ɫ���������
		glUseProgram(shader.program);

		glBindVertexArray(VAO);
		//ʹ�õ�ǰ�������ɫ����֮ǰ����Ķ����������ã�glVertexAttribPointer������VBO�Ķ������ݣ�ͨ��VAO��Ӱ󶨣�������ͼԪ������ͼԪΪGL_TRIANGLES��
		//glDrawArrays(GL_TRIANGLES, 0, 3);//��������
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//����˫���棬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();//����ѭ���Ƿ񴥷�����/�����¼�
	}
	glfwTerminate();
	return 0;
}