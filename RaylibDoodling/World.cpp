#include "World.h"

std::vector<PlatformCube> CreateObstacles()
{
	return
	{
		// Large structures
		{{ -8.0f, 2.0f, -7.0f }, { 4.0f, 4.0f, 4.0f }, DARKGRAY},
		{{  9.0f, 1.5f,  8.0f }, { 5.0f, 3.0f, 3.0f }, GRAY},
		{{  6.0f, 2.5f, -9.0f }, { 3.0f, 5.0f, 3.0f }, DARKGRAY},
		{{-10.0f, 1.5f,  9.0f }, { 5.0f, 3.0f, 2.0f }, GRAY},

		// Long walls scattered around the arena
		{{ -5.0f, 1.5f,  3.0f }, { 1.0f, 3.0f, 7.0f }, BROWN},
		{{  8.0f, 1.0f,  1.0f }, { 6.0f, 2.0f, 1.0f }, BROWN},
		{{  0.0f, 1.5f, -8.0f }, { 7.0f, 3.0f, 1.0f }, DARKBROWN},
		{{ -9.0f, 1.0f, -1.0f }, { 4.0f, 2.0f, 1.0f }, BROWN},

		// Tall pillars
		{{  3.0f, 3.0f,  6.0f }, { 1.5f, 6.0f, 1.5f }, MAROON},
		{{-12.0f, 2.5f,  1.0f }, { 2.0f, 5.0f, 2.0f }, MAROON},
		{{ 11.0f, 2.0f, -5.0f }, { 1.5f, 4.0f, 1.5f }, MAROON},

		// Small blocks you can jump onto
		{{ -3.0f, 0.25f, -4.0f }, { 2.0f, 0.5f, 2.0f }, BEIGE},
		{{  4.0f, 0.25f, -3.0f }, { 2.0f, 0.5f, 2.0f }, BEIGE},
		{{  0.0f, 0.5f,  9.0f }, { 3.0f, 1.0f, 2.0f }, BEIGE},
		{{ -6.0f, 0.5f, 11.0f }, { 2.0f, 1.0f, 3.0f }, BEIGE},

		// Medium obstacles
		{{  2.0f, 0.75f,  2.0f }, { 2.0f, 1.5f, 2.0f }, ORANGE},
		{{-11.0f, 0.75f, -11.0f}, { 2.5f, 1.5f, 2.5f }, ORANGE},
		{{ 12.0f, 1.0f, 11.0f }, { 2.0f, 2.0f, 2.0f }, ORANGE},
		{{ -1.0f, 1.0f, 12.0f }, { 3.0f, 2.0f, 2.0f }, ORANGE},
		// Distant obstacles
		{{  5.0f, 1.5f, -16.0f }, { 4.0f, 3.0f, 2.0f }, GRAY},
		{{ 11.0f, 2.0f, -15.0f }, { 2.0f, 4.0f, 3.0f }, DARKGRAY},
		{{ 17.0f, 1.0f,  -8.0f }, { 5.0f, 2.0f, 1.5f }, BROWN},

		// Scattered pillars
		{{  8.0f, 2.5f, -19.0f }, { 1.5f, 5.0f, 1.5f }, MAROON},
		{{ 19.0f, 1.5f, -14.0f }, { 2.0f, 3.0f, 2.0f }, GRAY},

		// Smaller obstacles
		{{ 14.0f, 0.5f,  -5.0f }, { 2.0f, 1.0f, 2.0f }, BEIGE},
		{{  3.0f, 0.75f,-20.0f }, { 3.0f, 1.5f, 2.0f }, ORANGE}
	};
}

void DrawObstacles(const std::vector<PlatformCube>& obstacles)
{
	for (const PlatformCube& obstacle : obstacles)
	{
		DrawCube(obstacle.position, obstacle.size.x, obstacle.size.y, obstacle.size.z, obstacle.color);
	}
}

void DrawGround()
{
	DrawPlane(Vector3{ 0.0f, -0.02f, 0.0f },
		Vector2{ 200.0f, 200.0f }, DARKGRAY);
	DrawGrid(200, 1.0f);
}
