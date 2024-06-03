#pragma once

#include "window.h"
#include "pipeline.h"

namespace VkDraw {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run(void);
	private:
		Window _window{ WIDTH, HEIGHT, "VkDraw" };
		Pipeline _pipeline{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
	};
}