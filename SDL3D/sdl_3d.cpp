#include "SDL_3D.h"

#include <glm/mat3x3.hpp>

vec3 rotate(vec3 vec, quat rot, vec3 pivot = { 0, 0, 0 }) {
	vec4 temp = rot * vec4(vec - pivot, 0) * glm::conjugate(rot);
	return vec3(temp.x, temp.y, temp.z) + pivot;
}

vec2 __project(vec3 vec, vec2 screen, float fov) {
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

void SDL_RenderDrawMesh(SDL_Renderer* renderer, SDL_Camera* cam, SDL_Mesh* mesh, SDL_Transform* transform, SDL_CullType cull, bool backedges) {
	for (int i = 0; i < mesh->indices.size(); i += 3) {
		// calculate global vertex positions
		vec3 v0 = rotate(mesh->vertices[mesh->indices[i + 0]] * transform->scale, transform->rotation) + transform->position;
		vec3 v1 = rotate(mesh->vertices[mesh->indices[i + 1]] * transform->scale, transform->rotation) + transform->position;
		vec3 v2 = rotate(mesh->vertices[mesh->indices[i + 2]] * transform->scale, transform->rotation) + transform->position;

		// project vertices to screen space
		vec2 p0 = __project(v0, vec2{ cam->width, cam->height }, cam->fov);
		vec2 p1 = __project(v1, vec2{ cam->width, cam->height }, cam->fov);
		vec2 p2 = __project(v2, vec2{ cam->width, cam->height }, cam->fov);

		// backface culling
		vec2 edge1 = p1 - p0;
		vec2 edge2 = p2 - p0;
		float cross = edge1.x * edge2.y - edge1.y * edge2.x;
		bool dotted = false;
		if (cull == SDL_CULL_BACKFACE && cross > 0) {
			if (!backedges) {
				continue;
			}
			dotted = true;
		}
		else if (cull == SDL_CULL_FRONTFACE && cross < 0) {
			if (!backedges) {
				continue;
			}
			dotted = true;
		}

		SDL_Color color;
		SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;

		// draw triangle face
		if (!backedges) {
			SDL_Vertex verts[3];
			verts[0] = { {p0.x, p0.y}, color, {0.0f, 0.0f} };
			verts[1] = { {p1.x, p1.y}, color, {0.0f, 0.0f} };
			verts[2] = { {p2.x, p2.y}, color, {0.0f, 0.0f} };
			int indix[3] = { 0, 1, 2 };
			SDL_RenderGeometry(renderer, nullptr, verts, 3, indix, 3);
		}

		// draw triangle outline
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