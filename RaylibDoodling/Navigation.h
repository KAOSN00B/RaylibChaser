#pragma once

#include "raylib.h"
#include "World.h"
#include <vector>

// Shared navigation grid settings
constexpr float GRID_ORIGIN_X = -50.0f;
constexpr float GRID_ORIGIN_Z = -50.0f;
constexpr float CELL_SIZE = 1.0f;

struct Node
{
	int x = 0;
	int z = 0;
	bool walkable = true;
};

// Owns the walkability grid and the A* search that runs on it.
class NavigationGrid
{
public:
	NavigationGrid(int width, int depth);

	// Marks grid cells as unwalkable wherever an obstacle overlaps them.
	void MarkObstacles(const std::vector<PlatformCube>& obstacles);

	Node WorldToGrid(const Vector3& worldPosition) const;
	std::vector<Node> FindPath(Node start, Node goal) const;

	const std::vector<std::vector<Node>>& GetGrid() const { return grid; }

private:
	std::vector<std::vector<Node>> grid;
};
