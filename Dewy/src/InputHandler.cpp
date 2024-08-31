#include "InputHandler.h"
#include "iostream"

int InputHandler::m_width = 1280;
int InputHandler::m_height = 720;
double InputHandler::mouseXPos = 0;
double InputHandler::mouseYPos = 0;
float InputHandler::m_zoomLevel = 1;
glm::mat4  InputHandler::m_proj;

InputEvents InputHandler::currentInputEvent = InputEvents::NONE;
InputEvents InputHandler::previousInputEvent = InputEvents::NONE;

bool InputHandler::m_inputReceived = false;
bool InputHandler::m_zoomUsed = false;

InputHandler::InputHandler(GLFWwindow* window)
{
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_wheel_callback);
	glfwGetWindowSize(window, &m_width, &m_height);
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	m_inputReceived = true;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		currentInputEvent = InputEvents::LEFT_MOUSE_CLICKED;
		std::cout << mouseXPos << " " << mouseYPos << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		currentInputEvent = InputEvents::MOUSE_RELEASE;
}

void InputHandler::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Location of cursor before modification by imGui
	double originalXPos = xpos;
	double originalYPos = ypos;
	//std::cout << "test" << std::endl;
	mouseXPos = originalXPos;
	mouseYPos = originalYPos;

	glm::mat4 invProj = glm::inverse(m_proj);

	float ndcX = (2.0f * (mouseXPos / m_width)) - 1.0f;
	float ndcY = 1.0f - (2.0f * (mouseYPos / m_height));

	glm::vec4 clipSpaceCoords(ndcX, ndcY, 0.0f, 1.0f);
	glm::vec4 worldCoords = invProj * clipSpaceCoords;

	mouseXPos = worldCoords.x;
	mouseYPos = worldCoords.y;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
	{
		m_inputReceived = true;
		currentInputEvent = InputEvents::MOUSE_DRAG;
	}
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	m_inputReceived = true;
	if (action == GLFW_PRESS && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_C )
	{
		currentInputEvent = InputEvents::COPYING;
		std::cout << "Copying\n";
	}
	else if (action == GLFW_PRESS && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_V)
	{
		currentInputEvent = InputEvents::PASTING;
		std::cout << "Pasting\n";
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_BACKSPACE)
	{
		currentInputEvent = InputEvents::DELETING;
		std::cout << "Deleting\n";
	}
}

void InputHandler::scroll_wheel_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float zoomAmount = 0.5f;

	if (yoffset < 0 && (m_zoomLevel + zoomAmount < 2))
		m_zoomLevel += zoomAmount;

	else if (m_zoomLevel - zoomAmount >= 0.2 && yoffset > 0)
		m_zoomLevel -= zoomAmount;

	m_zoomUsed = true;
	
	std::cout << m_zoomLevel << std::endl;
}

void InputHandler::UpdateInput(glm::mat4 proj)
{
	m_proj = proj;

	if (m_zoomUsed)
		m_zoomUsed = false;

	if (currentInputEvent == InputEvents::MOUSE_DRAG)
		return;

	else if (!m_inputReceived)
		currentInputEvent = InputEvents::NONE;

	previousInputEvent = currentInputEvent;
}
