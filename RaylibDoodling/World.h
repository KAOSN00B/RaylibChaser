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
