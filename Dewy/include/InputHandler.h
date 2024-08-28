#pragma once
#include <GLFW/glfw3.h>

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
private:
	GLFWwindow* m_window;

public:
	InputHandler(GLFWwindow* window);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void UpdateInput();

public:
	static InputEvents currentInputEvent;
	static InputEvents previousInputEvent;
	static bool isLeftMouseButtonHeld;
	static bool m_inputReceived;
	static double mouseXPos;
	static double mouseYPos;
};

