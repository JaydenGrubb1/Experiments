#pragma once

#include <vector>

#include <SDL.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;

typedef enum {
	SDL_CULL_NONE = 0,
	SDL_CULL_BACKFACE = 1,
	SDL_CULL_FRONTFACE = 2
} SDL_CullType;

struct SDL_Mesh {
	std::vector<vec3> vertices;
	std::vector<int> indices;
};

struct SDL_Camera {
	int width;
	int height;
	float fov;
};

void SDL_RenderDrawDottedLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1);

void SDL_RenderDrawMesh(SDL_Renderer* renderer, SDL_Camera* cam, SDL_Mesh* mesh, vec3 pos, vec3 rot, SDL_CullType cull = SDL_CULL_BACKFACE, bool backedges = false);