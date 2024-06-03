#include "window.h"

namespace VkDraw {
	Window::Window(int width, int height, std::string name) : _width(width), _height(height), _name(name) {
		__init();
	}

	Window::~Window(void) {
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void Window::__init(void) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		_window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
	}
}