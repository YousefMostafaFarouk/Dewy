#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
protected:
    int m_height;
    int m_width;

    GLFWwindow* m_window;
    std::string m_name;


public:
    Renderer(unsigned int height, unsigned int width, const std::string& name);
    ~Renderer();

    void Clear() const;


    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

    inline GLFWwindow* getWindowPointer() { return m_window; };
    inline int WindowShouldClose() { return glfwWindowShouldClose(m_window); };
    inline void PollEvents() const { glfwPollEvents(); };

};

