#pragma once

#include "window.h"

namespace VkDraw {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run(void);
	private:
		Window _window{ WIDTH, HEIGHT, "VkDraw" };
	};
}