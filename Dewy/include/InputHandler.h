#pragma once
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

enum class InputEvents
{
	LEFT_MOUSE_CLICKED,
	NONE,
	MOUSE_DRAG,
	MOUSE_RELEASE,
	COPYING,
	PASTING,
	DELETING
};

class InputHandler
{
public:
	static InputEvents m_currentInputEvent;
	static InputEvents m_previousInputEvent;
	static bool isLeftMouseButtonHeld;
	static bool m_inputReceived;
	static bool m_zoomUsed;
	static double mouseXPos;
	static double mouseYPos;
	static int m_width;
	static int m_height;
	static float m_zoomLevel;
	static float m_prevZoomLevel;
	static glm::mat4 m_proj;

public:
	InputHandler(GLFWwindow* window);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_wheel_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void UpdateInput(glm::mat4 proj);
};

