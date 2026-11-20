#pragma once

#include "raylib.h"
#include <vector>

// Simple static obstacle in the arena. Plain data, so a struct is enough.
struct PlatformCube
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 size = { 0.0f, 0.0f, 0.0f };
	Color color = GRAY;
};

// Builds the arena's static obstacle layout.
std::vector<PlatformCube> CreateObstacles();

void DrawObstacles(const std::vector<PlatformCube>& obstacles);
void DrawGround();

// The outer edges of everything in the obstacle list, so the player
// boundary can wrap the whole layout instead of using guessed numbers.
struct ArenaBounds
{
	float minX = 0.0f;
	float maxX = 0.0f;
	float minZ = 0.0f;
	float maxZ = 0.0f;
};

ArenaBounds GetArenaBounds(const std::vector<PlatformCube>& obstacles, float padding);

// Draws a visible dark blue wall along each of the four boundary edges.
void DrawArenaWalls(const ArenaBounds& bounds, float wallHeight);
