#pragma once

#include <string_view>
#include <vector>

namespace VkDraw {
	class Pipeline {
	public:
		Pipeline(std::string_view vert_shader_path, std::string_view frag_shader_path);

	private:
		static std::vector<char> __read_file(std::string_view path);
		void __create(std::string_view vert_shader_path, std::string_view frag_shader_path);
	};
}