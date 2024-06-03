#include <cstdlib>
#include <stdexcept>

#include "app.h"
#include "log.h"

int main(int argc, char** argv) {
	VkDraw::App app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		LOG("Exception: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}