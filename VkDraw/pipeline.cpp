#include <fstream>
#include <stdexcept>

#include "pipeline.h"
#include "log.h"

namespace VkDraw {
	Pipeline::Pipeline(std::string_view vert_shader_path, std::string_view frag_shader_path) {
		__create(vert_shader_path, frag_shader_path);
	}

	std::vector<char> Pipeline::__read_file(std::string_view path) {
		std::ifstream file(path.data(), std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open shader file");
		}

		size_t size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(size);

		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();
		return buffer;
	}

	void Pipeline::__create(std::string_view vert_shader_path, std::string_view frag_shader_path) {
		auto vert_code = __read_file(vert_shader_path);
		auto frag_code = __read_file(frag_shader_path);

		LOG("Shader Code Size {{\n\tvert: {}\n\tfrag: {}\n}}", vert_code.size(), frag_code.size());
	}
}