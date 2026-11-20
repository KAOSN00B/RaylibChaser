#include "Debug.h"
#include <algorithm>
#include <cmath>

void DebugMessage()
{
	const char* message = "Press F1 to toggle navigation grid";

	int x = 10;
	int y = GetScreenHeight() - 30;
	int fontSize = 20;

	// Black outline (1 pixel in each direction)
	DrawText(message, x - 1, y, fontSize, BLACK);
	DrawText(message, x + 1, y, fontSize, BLACK);
	DrawText(message, x, y - 1, fontSize, BLACK);
	DrawText(message, x, y + 1, fontSize, BLACK);

	// White text on top
	DrawText(message, x, y, fontSize, WHITE);
}

void DrawScoreHUD(int totalScore)
{
	const char* scoreText = TextFormat("Score: %d", totalScore);

	int x = 10;
	int y = 10;
	int fontSize = 28;

	// Black outline (1 pixel in each direction)
	DrawText(scoreText, x - 1, y, fontSize, BLACK);
	DrawText(scoreText, x + 1, y, fontSize, BLACK);
	DrawText(scoreText, x, y - 1, fontSize, BLACK);
	DrawText(scoreText, x, y + 1, fontSize, BLACK);

	// White text on top
	DrawText(scoreText, x, y, fontSize, WHITE);
}

void DebugNavigation(const NavigationGrid& navigation, const std::vector<ChaserAI>& chasers, const Player& player)
{
	const std::vector<std::vector<Node>>& grid = navigation.GetGrid();

	// Drawing only nearby cells keeps the debug overlay responsive.
	const int playerGridX = static_cast<int>(std::floor((player.position.x - GRID_ORIGIN_X) / CELL_SIZE));
	const int playerGridZ = static_cast<int>(std::floor((player.position.z - GRID_ORIGIN_Z) / CELL_SIZE));
	constexpr int RENDER_RADIUS = 15;

	const int minX = std::max(0, playerGridX - RENDER_RADIUS);
	const int maxX = std::min(static_cast<int>(grid.size()), playerGridX + RENDER_RADIUS + 1);
	const int minZ = std::max(0, playerGridZ - RENDER_RADIUS);
	const int maxZ = std::min(static_cast<int>(grid[0].size()), playerGridZ + RENDER_RADIUS + 1);

	for (int i = minX; i < maxX; i++)
	{
		for (int j = minZ; j < maxZ; j++)
		{
			const float worldX = GRID_ORIGIN_X + grid[i][j].x * CELL_SIZE;
			const float worldZ = GRID_ORIGIN_Z + grid[i][j].z * CELL_SIZE;
			DrawCube(Vector3{ worldX + CELL_SIZE / 2.0f, 0.02f, worldZ + CELL_SIZE / 2.0f },
				0.8f, 0.04f, 0.8f, grid[i][j].walkable ? GREEN : RED);
		}
	}

	// Draw each enemy's current A* path only once, outside the grid loops.
	for (const ChaserAI& chaser : chasers)
	{
		for (const Node& node : chaser.path)
		{
			const float worldX = GRID_ORIGIN_X + node.x * CELL_SIZE;
			const float worldZ = GRID_ORIGIN_Z + node.z * CELL_SIZE;
			DrawCube(Vector3{ worldX + CELL_SIZE / 2.0f, 0.07f, worldZ + CELL_SIZE / 2.0f },
				0.8f, 0.04f, 0.8f, BLUE);
		}
	}
}
