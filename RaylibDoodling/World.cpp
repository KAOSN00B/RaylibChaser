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
		{{ -3.0f, 0.25f, -4.0f }, { 2.0f, 0.5f, 2.0f }, BLUE},
		{{  4.0f, 0.25f, -3.0f }, { 2.0f, 0.5f, 2.0f }, BLUE},
		{{  0.0f, 0.5f,  9.0f }, { 3.0f, 1.0f, 2.0f }, BLUE},
		{{ -6.0f, 0.5f, 11.0f }, { 2.0f, 1.0f, 3.0f }, BLUE},

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
		{{ 14.0f, 0.5f,  -5.0f }, { 2.0f, 1.0f, 2.0f }, GREEN},
		{{  3.0f, 0.75f,-20.0f }, { 3.0f, 1.5f, 2.0f }, GREEN}
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

ArenaBounds GetArenaBounds(const std::vector<PlatformCube>& obstacles, float padding)
{
	ArenaBounds bounds;

	// Start "biggest/smallest so far" at the very first obstacle's edges,
	// so the first comparison has something real to compare against.
	bounds.minX = obstacles[0].position.x - obstacles[0].size.x / 2.0f;
	bounds.maxX = obstacles[0].position.x + obstacles[0].size.x / 2.0f;
	bounds.minZ = obstacles[0].position.z - obstacles[0].size.z / 2.0f;
	bounds.maxZ = obstacles[0].position.z + obstacles[0].size.z / 2.0f;

	for (const PlatformCube& obstacle : obstacles)
	{
		float leftEdge = obstacle.position.x - obstacle.size.x / 2.0f;
		float rightEdge = obstacle.position.x + obstacle.size.x / 2.0f;
		float backEdge = obstacle.position.z - obstacle.size.z / 2.0f;
		float frontEdge = obstacle.position.z + obstacle.size.z / 2.0f;

		if (leftEdge < bounds.minX) bounds.minX = leftEdge;
		if (rightEdge > bounds.maxX) bounds.maxX = rightEdge;
		if (backEdge < bounds.minZ) bounds.minZ = backEdge;
		if (frontEdge > bounds.maxZ) bounds.maxZ = frontEdge;
	}

	// Pull the wall in a little so the player stops at the obstacle edge
	// instead of standing with their body halfway inside the outer wall.
	bounds.minX += padding;
	bounds.maxX -= padding;
	bounds.minZ += padding;
	bounds.maxZ -= padding;

	return bounds;
}

void DrawArenaWalls(const ArenaBounds& bounds, float wallHeight)
{
	float wallThickness = 0.5f;

	// Centers and full lengths, so each wall cube can be placed and
	// sized to run along one whole edge of the boundary box.
	float centerX = (bounds.minX + bounds.maxX) / 2.0f;
	float centerZ = (bounds.minZ + bounds.maxZ) / 2.0f;
	float width = bounds.maxX - bounds.minX;
	float depth = bounds.maxZ - bounds.minZ;
	float wallY = wallHeight / 2.0f;

	// Back wall (along minZ) and front wall (along maxZ)
	DrawCube(Vector3{ centerX, wallY, bounds.minZ }, width, wallHeight, wallThickness, DARKBLUE);
	DrawCube(Vector3{ centerX, wallY, bounds.maxZ }, width, wallHeight, wallThickness, DARKBLUE);

	// Left wall (along minX) and right wall (along maxX)
	DrawCube(Vector3{ bounds.minX, wallY, centerZ }, wallThickness, wallHeight, depth, DARKBLUE);
	DrawCube(Vector3{ bounds.maxX, wallY, centerZ }, wallThickness, wallHeight, depth, DARKBLUE);
}
