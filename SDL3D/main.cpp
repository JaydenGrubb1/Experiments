#include <chrono>
#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <SDL.h>

#include "SDL_3D.h";

#define LOG(FMT, ...) std::cout << std::format(FMT, __VA_ARGS__) << std::endl

constexpr auto width = 1280;
constexpr auto height = 720;
constexpr auto scale = 1;
constexpr auto fov = 47.0;

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

	SDL_RenderSetScale(renderer, scale, scale);

	SDL_Camera camera{
		width / scale,
		height / scale,
		fov
	};

	SDL_Mesh cube;
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

	SDL_Mesh pyramid;
	pyramid.vertices = {
		vec3{-0.8f, -0.7f, -0.5f},
		vec3{0.8f, -0.7f, -0.5f},
		vec3{0.0f, -0.7f, 1.0f},
		vec3{0.0f, 0.7f, 0.0f}
	};
	pyramid.indices = {
		0, 2, 1,
		3, 0, 1,
		3, 1, 2,
		3, 2, 0
	};

	SDL_Mesh plane;
	plane.vertices = {
		vec3{-1.0f, 0.0f, -1.0f},
		vec3{-1.0f, 0.0f, 1.0f},
		vec3{1.0f, 0.0f, -1.0f},
		vec3{1.0f, 0.0f, 1.0f}
	};
	plane.indices = {
		2, 0, 1,
		2, 1, 3
	};

	SDL_Transform cube_transform{
		vec3{-2.0f, 0.0f, 4.0f},
		vec3{0.0f, 0.0f, 0.0f},
		vec3{1.0f, 1.0f, 1.0f}
	};

	SDL_Transform pyramid_transform{
		vec3{2.0f, 0.0f, 6.0f},
		vec3{0.0f, 0.0f, 0.0f},
		vec3{2.0f, 2.0f, 2.0f}
	};

	SDL_Transform plane_transform{
		vec3{0.0f, -1.5f, 5.0f},
		vec3{0.0f, 0.0f, 0.0f},
		vec3{4.0f, 1.0f, 4.0f}
	};

	bool running = true;
	auto last_time = std::chrono::high_resolution_clock::now();

	float rotation = 0.0f;
	bool backedges = false;
	bool paused = false;

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
				case SDLK_p:
					paused = !paused;
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

		if (!paused) {
			rotation += 1.0f * dt;
			cube_transform.rotation = vec3{ rotation, rotation, rotation };
			pyramid_transform.rotation = vec3{ rotation, rotation, rotation };
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawMesh(renderer, &camera, &plane, &plane_transform, SDL_CULL_NONE, backedges);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawMesh(renderer, &camera, &cube, &cube_transform, SDL_CULL_BACKFACE, backedges);
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawMesh(renderer, &camera, &pyramid, &pyramid_transform, SDL_CULL_BACKFACE, backedges);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}