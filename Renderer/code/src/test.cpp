// This file is meant for testing the renderer
// 
// 
// 
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include "Renderer.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "Sprite.h"
//#include "SpriteRenderer.h"
//
//int main(void)
//{
//	//Renderer renderer(720, 1280, "Hello World");
//	float positions[] =
//	{
//	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
//		0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
//		0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
//	   -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
//
//	   1.5f, -0.5f, 0.0f, 0.0f, 1.0f,
//	   0.5f, -0.5f, 1.0f, 0.0f,1.0f,
//	   0.5f,  -1.5f, 1.0f, 1.0f,1.0f,
//	   1.5f, -1.5f, 0.0f, 1.0f,1.0f
//
//	};
//
//	Vertex positions2[12] =
//	{
//	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
//		0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
//		0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
//		0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
//	   -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
//	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
//
//	    0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//		1.5f, 0.5f, 1.0f, 0.0f, 1.0f,
//		1.5f,  1.5f, 1.0f, 1.0f, 1.0f,
//		1.5f,  1.5f, 1.0f, 1.0f, 1.0f,
//	   0.5f, 1.5f, 0.0f, 1.0f, 1.0f,
//	   0.5f, 0.5f, 0.0f, 0.0f, 1.0f
//
//	};
//
//	unsigned int indices[]
//	{
//		0, 1, 2,
//		2, 3, 0,
//
//		4, 5, 6,
//		6, 7, 4
//	};
//
//
//	//VertexArray va;
//	//VertexBuffer vb(positions, 5 * 8 * sizeof(float),8);
//
//	//VertexBufferLayout layout;
//	//layout.Push<float>(2);
//	//layout.Push<float>(2);
//	//layout.Push<float>(1);
//	//va.AddBuffer(vb, layout);
//
//	//IndexBuffer ib(indices, 12);
//
//	//glm::mat4 proj =  glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
//
//	//Shader shader("res/shaders/basic.shader");
//	//shader.Bind();
//	//shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
//
//	float r = 0.0f;
//	float increment = 0.005f;
//
//	int testarray[2] = { 0, 1 };
//
//	//shader.SetUniform1i("u_NumberOfTextures", 2);
//	//shader.SetUniform1iv("u_Texture", 2, testarray);
//	//shader.SetUniformMat4f("u_MVP", proj);
//
//	/*va.UnBind();
//	vb.UnBind();
//	ib.UnBind();*/
//	//shader.UnBind();
//
//	//shader.Bind();
//	glm::mat4 proj = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
//
//	SpriteRender spriteRenderer(720, 1280,
//	"yarab2a", "res/shaders/basic.shader", testarray, 2, proj);
//
//	Texture tex("res/textures/adelshakal.png");
//	tex.Bind(1);
//
//	Texture texture("res/textures/and_gate.png");
//	texture.Bind(0);
//
//	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
//	GLCall(glEnable(GL_BLEND));
//
//
//	/* Loop until the user closes the window */
//	std::vector<Vertex> verticies(12);
//
//	for (int i = 0; i < 12; ++i)
//		verticies.push_back(positions2[i]);
//	std::cout << glGetString(GL_VERSION) << std::endl;
//
//	while (!spriteRenderer.WindowShouldClose())
//	{
//
//		spriteRenderer.DrawSprite(verticies);
//
//		/*shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
//
//		if (r > 1.0f) {
//			increment = -0.05f;
//			r = 0.0f;
//		}
//		else if (r < 0.0f) {
//			increment = 0.05f;
//		}
//
//		r += increment;*/
//		spriteRenderer.PollEvents();
//
//	}
//
//	return 0;
//}