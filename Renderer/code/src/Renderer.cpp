#include "Renderer.h"
#include <iostream>
#include <stdexcept>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);

}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGl Error] (" << error << ") " << function << " "
            << " " << line << " " << file << " " << std::endl;
        return false;
    }

    return true;
}

Renderer::Renderer(unsigned int height, unsigned int width, const std::string& name)
: m_height(height), m_width(width), m_name(name)
{
    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
       

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create the GLFW window");
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);

    //enables vsync
    glfwSwapInterval(1);

    // GLEW is the loader used by the existing Windows build. Linux's GLVND
    // exports the OpenGL 3.3 entry points directly for both GLX and EGL.
#if !defined(__linux__)
    glewExperimental = GL_TRUE;
    const GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error(
            reinterpret_cast<const char*>(glewGetErrorString(glewStatus)));
    }

    // GLEW can leave a benign GL_INVALID_ENUM behind on core profiles.
    GLClearError();
#endif
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    Clear();
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
    glfwSwapBuffers(m_window);
}
