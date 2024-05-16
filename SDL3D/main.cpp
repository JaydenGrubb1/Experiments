#include <chrono>
#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <SDL.h>

#define LOG(FMT, ...) std::cout << std::format(FMT, __VA_ARGS__) << std::endl

constexpr auto width = 1280;
constexpr auto height = 720;
constexpr auto fov = 47.0;

using vec2 = glm::vec2;
using vec3 = glm::vec3;

enum class culling {
	none,
	back,
	front
};

struct mesh {
	std::vector<vec3> vertices;
	std::vector<int> indices;
};

vec3 rotate(vec3 vec, vec3 angles, vec3 pivot = { 0, 0, 0 }) {
	glm::mat3x3 rot{
		{
			cos(angles.y) * cos(angles.z),
			cos(angles.z) * sin(angles.x) * sin(angles.y) - cos(angles.x) * sin(angles.z),
			cos(angles.x) * cos(angles.z) * sin(angles.y) + sin(angles.x) * sin(angles.z)
		},
		{
			cos(angles.y) * sin(angles.z),
			cos(angles.x) * cos(angles.z) + sin(angles.x) * sin(angles.y) * sin(angles.z),
			-cos(angles.z) * sin(angles.x) + cos(angles.x) * sin(angles.y) * sin(angles.z)
		},
		{
			-sin(angles.y),
			cos(angles.y) * sin(angles.x),
			cos(angles.x) * cos(angles.y)
		}
	};
	return (rot * (vec - pivot)) + pivot;
}

vec2 project(vec3 vec, vec2 screen, float fov) {
	float aspect = screen.x / screen.y;
	float scale_x = std::tan(fov * 0.5 * (M_PI / 180));
	float scale_y = scale_x * aspect;
	return vec2{
		screen.x / 2.0f + (vec.x * scale_x) / (scale_x + vec.z) * screen.x / 2.0f,
		screen.y / 2.0f - (vec.y * scale_y) / (scale_y + vec.z) * screen.y / 2.0f
	};
}

void SDL_RenderDrawDottedLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	int count = 0;
	while (true) {
		if (count < 10) {
			SDL_RenderDrawPoint(renderer, x0, y0);
		}
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = 2 * err;
		if (e2 > dy) {
			err += dy; x0 += sx;
		}
		if (e2 < dx) {
			err += dx; y0 += sy;
		}
		count = (count + 1) % 20;
	}
}

void SDL_RenderDrawMesh(SDL_Renderer* renderer, mesh* m, vec3 pos, vec3 rot, culling cull = culling::back, bool backedges = false) {
	for (int i = 0; i < m->indices.size(); i += 3) {
		// calculate global vertex positions
		vec3 v0 = rotate(m->vertices[m->indices[i]], rot) + pos;
		vec3 v1 = rotate(m->vertices[m->indices[i + 1]], rot) + pos;
		vec3 v2 = rotate(m->vertices[m->indices[i + 2]], rot) + pos;

		// project vertices to screen space
		vec2 p0 = project(v0, vec2{ width, height }, fov);
		vec2 p1 = project(v1, vec2{ width, height }, fov);
		vec2 p2 = project(v2, vec2{ width, height }, fov);

		// backface culling
		vec2 edge1 = p1 - p0;
		vec2 edge2 = p2 - p0;
		float cross = edge1.x * edge2.y - edge1.y * edge2.x;
		bool dotted = false;
		if (cull == culling::back && cross > 0) {
			if (!backedges) {
				continue;
			}
			dotted = true;
		}
		else if (cull == culling::front && cross < 0) {
			if (!backedges) {
				continue;
			}
			dotted = true;
		}

		// draw triangle
		if (dotted) {
			SDL_RenderDrawDottedLine(renderer, p0.x, p0.y, p1.x, p1.y);
			SDL_RenderDrawDottedLine(renderer, p1.x, p1.y, p2.x, p2.y);
			SDL_RenderDrawDottedLine(renderer, p2.x, p2.y, p0.x, p0.y);
		}
		else {
			SDL_RenderDrawLine(renderer, p0.x, p0.y, p1.x, p1.y);
			SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
			SDL_RenderDrawLine(renderer, p2.x, p2.y, p0.x, p0.y);
		}
	}
}

int main(int argc, char** argv) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	if ((window = SDL_CreateWindow("SDL3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0)) == nullptr) {
		LOG("Failed to create window: {}", SDL_GetError());
		return EXIT_FAILURE;
	}

	if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == nullptr) {
		LOG("Failed to create renderer: {}", SDL_GetError());
		return EXIT_FAILURE;
	}

	mesh cube;
	cube.vertices = {
		vec3{-1.0f, -1.0f, -1.0f},
		vec3{-1.0f, -1.0f, 1.0f},
		vec3{1.0f, -1.0f, -1.0f},
		vec3{1.0f, -1.0f, 1.0f},
		vec3{-1.0f, 1.0f, -1.0f},
		vec3{-1.0f, 1.0f, 1.0f},
		vec3{1.0f, 1.0f, -1.0f},
		vec3{1.0f, 1.0f, 1.0f}
	};
	cube.indices = {
		6, 2, 3,
		6, 3 ,7,
		7, 3, 1,
		7, 1, 5,
		5, 1, 0,
		5, 0, 4,
		4, 0, 2,
		4, 2, 6,
		4, 6, 5,
		5, 6, 7,
		2, 0, 3,
		3, 0, 1
	};

	bool running = true;
	auto last_time = std::chrono::high_resolution_clock::now();

	float rotation = 0.0f;
	bool backedges = false;

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_k:
					backedges = !backedges;
					break;
				}
			}
		}
		if (!running) {
			break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		auto current_time = std::chrono::high_resolution_clock::now();
		double dt = std::chrono::duration<double, std::ratio<1, 1>>(current_time - last_time).count();
		last_time = current_time;

		rotation += 1.0f * dt;
		vec3 rot{ rotation, rotation, rotation };

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawMesh(renderer, &cube, { 0.0, 0.0f, 3.0f }, rot, culling::back, backedges);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}