#include "app.h"

namespace VkDraw {
	void App::run(void) {
		while (!_window.should_close()) {
			glfwPollEvents();
		}
	}
}