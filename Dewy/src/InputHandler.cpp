#include "InputHandler.h"
#include "iostream"
#include "imgui_impl_glfw.h"

double InputHandler::mouseXPos = 0;
double InputHandler::mouseYPos = 0;

InputEvents InputHandler::currentInputEvent = InputEvents::NONE;
InputEvents InputHandler::previousInputEvent = InputEvents::NONE;

bool InputHandler::m_inputReceived = false;

InputHandler::InputHandler(GLFWwindow* window) : m_window(window)
{
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	glfwSetCursorPosCallback(m_window, cursor_position_callback);
	glfwSetKeyCallback(m_window, key_callback);
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	m_inputReceived = true;
	ImGuiIO& ioptr = ImGui::GetIO();

	ioptr.AddMouseButtonEvent(button, action == GLFW_PRESS);

	if (!ioptr.WantCaptureMouse)
	{
		//std::cout << "here\n";
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			currentInputEvent = InputEvents::LEFT_MOUSE_CLICKED;
			std::cout << mouseXPos << " " << mouseYPos << std::endl;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			currentInputEvent = InputEvents::MOUSE_RELEASE;
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT &&  action == GLFW_RELEASE)
		currentInputEvent = InputEvents::MOUSE_RELEASE;
}

void InputHandler::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Location of cursor before modification by imGui
	double originalXPos = xpos;
	double originalYPos = ypos;

	ImGuiIO& ioptr = ImGui::GetIO();
	if (ioptr.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		int window_x, window_y;
		glfwGetWindowPos(window, &window_x, &window_y);
		xpos += window_x;
		ypos += window_y;
	}
	ioptr.AddMousePosEvent((float)xpos, (float)ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	//std::cout << "test" << std::endl;
	mouseXPos = (originalXPos - (width / 2)) / (width / 16);
	mouseYPos = -(originalYPos - (height / 2)) / (width / 16);

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) || (ioptr.WantCaptureMouse && ImGui::IsMouseDragging(0)))
	{
		//std::cout << "dragging" << std::endl;
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

void InputHandler::UpdateInput()
{

	if (currentInputEvent == InputEvents::MOUSE_DRAG)
		return;

	else if (!m_inputReceived)
		currentInputEvent = InputEvents::NONE;

	previousInputEvent = currentInputEvent;
}
