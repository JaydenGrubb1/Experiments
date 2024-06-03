#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace VkDraw {
	class Window {
	public:
		Window(int width, int hgiht, std::string name);
		~Window(void);

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool should_close(void) const { return glfwWindowShouldClose(_window); }

	private:
		GLFWwindow* _window;
		const int _width;
		const int _height;
		std::string _name;

		void __init(void);
	};
}