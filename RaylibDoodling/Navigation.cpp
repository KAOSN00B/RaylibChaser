#include "Navigation.h"

#include <algorithm>
#include <cmath>

NavigationGrid::NavigationGrid(int width, int depth)
	: grid(width, std::vector<Node>(depth))
{
	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			grid[i][j].x = static_cast<int>(i);
			grid[i][j].z = static_cast<int>(j);
		}
	}
}

void NavigationGrid::MarkObstacles(const std::vector<PlatformCube>& obstacles)
{
	// Identify which ground cells are occupied by obstacles (once, since walls are static).
	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			const float cellMinX = GRID_ORIGIN_X + grid[i][j].x * CELL_SIZE;
			const float cellMinZ = GRID_ORIGIN_Z + grid[i][j].z * CELL_SIZE;
			const float cellMaxX = cellMinX + CELL_SIZE;
			const float cellMaxZ = cellMinZ + CELL_SIZE;

			for (const PlatformCube& obstacle : obstacles)
			{
				const float wallMinX = obstacle.position.x - obstacle.size.x / 2.0f;
				const float wallMaxX = obstacle.position.x + obstacle.size.x / 2.0f;
				const float wallMinZ = obstacle.position.z - obstacle.size.z / 2.0f;
				const float wallMaxZ = obstacle.position.z + obstacle.size.z / 2.0f;

				// AABB overlap in the X/Z ground plane; Y is irrelevant for this grid.
				if (cellMinX < wallMaxX && cellMaxX > wallMinX &&
					cellMinZ < wallMaxZ && cellMaxZ > wallMinZ)
				{
					grid[i][j].walkable = false;
					break;
				}
			}
		}
	}
}

Node NavigationGrid::WorldToGrid(const Vector3& worldPosition) const
{
	// Convert world coordinates into grid indices.
	int gridX = static_cast<int>(std::floor((worldPosition.x - GRID_ORIGIN_X) / CELL_SIZE));

	int gridZ = static_cast<int>(std::floor((worldPosition.z - GRID_ORIGIN_Z) / CELL_SIZE));

	// Keep the indices within the grid boundaries.
	gridX = std::clamp(gridX, 0, static_cast<int>(grid.size()) - 1);
	gridZ = std::clamp(gridZ, 0, static_cast<int>(grid[gridX].size()) - 1);

	// Return the node at those coordinates.
	return grid[gridX][gridZ];
}

std::vector<Node> NavigationGrid::FindPath(Node start, Node goal) const
{
	// A* Algorithm for AI to find the player while avoiding walls.

	std::vector<Node> path;
	std::vector<Node> openSet;

	//g cost grid for fgh
	std::vector<std::vector<int>> gCost(grid.size(), std::vector<int>(grid[0].size(), 1000000));
	gCost[start.x][start.z] = 0;

	// F being what G + H make up together.
	std::vector<std::vector<int>> fCost(grid.size(), std::vector<int>(grid[0].size(), 1000000));
	fCost[start.x][start.z] = std::abs(goal.x - start.x) + std::abs(goal.z - start.z);

	// Remember which node we came from to reach each position.
	std::vector<std::vector<Node>> cameFrom(grid.size(), std::vector<Node>(grid[0].size()));

	std::vector<std::vector<bool>> hasParent(grid.size(), std::vector<bool>(grid[0].size(), false));

	// Track which nodes we've already explored.
	std::vector<std::vector<bool>> closedSet(grid.size(), std::vector<bool>(grid[0].size(), false));

	// Start searching from the chaser's position.
	openSet.push_back(start);

	// Possible movement directions.
	int directionX[4] = { -1, 1, 0, 0 };
	int directionZ[4] = { 0, 0, -1, 1 };

	// Keep exploring while there are nodes available.
	while (!openSet.empty())
	{

		// STEP 1: Find the node with the lowest F cost
		size_t bestIndex = 0;

		for (size_t i = 1; i < openSet.size(); i++)
		{
			if (fCost[openSet[i].x][openSet[i].z] < fCost[openSet[bestIndex].x][openSet[bestIndex].z])
			{
				bestIndex = i;
			}
		}

		Node current = openSet[bestIndex];
		openSet.erase(openSet.begin() + bestIndex);

		// If we've already explored this node, skip it.
		if (closedSet[current.x][current.z])
		{
			continue;
		}

		// Mark the current node as explored.
		closedSet[current.x][current.z] = true;

		if (current.x == goal.x && current.z == goal.z)
		{
			Node step = current;

			// Follow the parents backwards.
			while (step.x != start.x || step.z != start.z)
			{
				path.push_back(step);

				if (!hasParent[step.x][step.z])
				{
					return {};
				}

				step = cameFrom[step.x][step.z];
			}

			// Include the starting node.
			path.push_back(start);

			// Reverse the route so it runs START -> GOAL.
			std::reverse(path.begin(), path.end());

			return path;
		}
		// Check all four neighbours.
		for (int i = 0; i < 4; i++)
		{
			int neighbourX = current.x + directionX[i];
			int neighbourZ = current.z + directionZ[i];


			// Check grid boundaries.
			if (neighbourX < 0 || neighbourZ < 0 ||
				neighbourX >= static_cast<int>(grid.size()) ||
				neighbourZ >= static_cast<int>(grid[neighbourX].size()))
			{
				continue;
			}

			// Check if the neighbour is blocked by a wall.
			if (!grid[neighbourX][neighbourZ].walkable)
			{
				continue;
			}

			// Check if we've already explored this neighbour.
			if (closedSet[neighbourX][neighbourZ])
			{
				continue;
			}

			int tentativeG = gCost[current.x][current.z] + 1;

			if (tentativeG < gCost[neighbourX][neighbourZ])
			{
				// Record the cheaper route.
				gCost[neighbourX][neighbourZ] = tentativeG;

				// Estimate the remaining distance.
				int hCost = std::abs(goal.x - neighbourX) + std::abs(goal.z - neighbourZ);

				fCost[neighbourX][neighbourZ] = tentativeG + hCost;


				cameFrom[neighbourX][neighbourZ] = current;
				hasParent[neighbourX][neighbourZ] = true;


				// Add the valid neighbour to the open set.
				openSet.push_back(grid[neighbourX][neighbourZ]);

			}


		}
	}

	return path;
}
