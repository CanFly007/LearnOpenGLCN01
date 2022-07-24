#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void CheckShaderSuccess(unsigned int shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}
void CheckProgramLinkSuccess(unsigned int shaderProgram)
{
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//OpenGL�汾��ʹ��Coreģʽ
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpengl", NULL, NULL);//���ڶ���* window
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//GLAD��������OpenGL����ָ��
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//�����ӿ�ʱ��Ļص�����

	//����
	float vertices[] = {
	0.5f, 0.5f, 0.0f,   // ���Ͻ�
	0.5f, -0.5f, 0.0f,  // ���½�
	-0.5f, -0.5f, 0.0f, // ���½�
	-0.5f, 0.5f, 0.0f   // ���Ͻ�
	};
	unsigned int indices[] = { // ע��������0��ʼ! 
	0, 1, 3, // ��һ��������
	1, 2, 3  // �ڶ���������
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Shader
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	CheckShaderSuccess(vertexShader);

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";
	unsigned int fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);
	CheckShaderSuccess(fragShader);

	//��������ɫ�����ӵ�һ����ɫ��������󣬰�ÿ����ɫ����������ӵ��¸���ɫ��������
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	CheckProgramLinkSuccess(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	//VAO��֮����ҪVAO����Ϊ����Draw��ʱ�򣬲�ȻҪBindBuffer���ֺ�Element
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//����VAO���������о͹��������VAO��
	//��һ��������Ӧshader�У�location = 0 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//����VBO�������ݣ�ͨ��֮ǰ�󶨵�GL_ARRAY_BUFFER��VBO����
	glEnableVertexAttribArray(0);//����0�Ŷ������ԣ�Ĭ���ǽ��õ�

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


	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window);

		//��Ⱦָ��
		glClearColor(0.2, 0.3, 0.3, 1.0);//���������Ļ����ɫ ���Ǹ�״̬
		glClear(GL_COLOR_BUFFER_BIT);//���������õ�״̬��ʹ��״̬����ColorBuffer

		//ÿ��Drawcall����ʹ�������ɫ���������
		glUseProgram(shaderProgram);

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