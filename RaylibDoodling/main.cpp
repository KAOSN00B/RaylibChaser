#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

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

Node WorldToGrid(const std::vector<std::vector<Node>>& grid, const Vector3& worldPosition);
std::vector<Node> FindPath(const std::vector<std::vector<Node>>& grid, Node start, Node goal);


struct PlatformCube
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 size = { 0.0f, 0.0f, 0.0f };
	Color color = GRAY;
};

struct PlayerCube
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 size = { .50f, .50f, .50f };
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	float gravity = 9.81f;
	float jumpForce = 5.0f;
	float moveSpeed = 5.0f;

	bool canJump = true;
	bool isColliding = false;
	Color color = DARKGREEN;
};

struct ChaserAI
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	float radius;
	float moveSpeed;

	Color color;


	//Path Navigation
	std::vector<Node> path;
	size_t waypointIndex = 0;
	float repathTimer = 0.0f;


	void ChaserPhysics()
	{
		if (position.y <= 0.0f)
		{
			position.y = radius;
		}
	}
	//prototype function just to get AI to move
	void ChasePlayer(const PlayerCube& player, float deltaTime)
	{
		Vector3 chaseDirection = Vector3Subtract(player.position, position);
		chaseDirection.y = 0.0f;
		Vector3 toPlayer = Vector3Normalize(chaseDirection);

		position.x += toPlayer.x * moveSpeed * deltaTime;
		position.z += toPlayer.z * moveSpeed * deltaTime;
	}

	void UpdatePath(const PlayerCube& player, const std::vector<std::vector<Node>>& grid, float deltaTime)
	{
		// Count down until we're allowed to calculate another path.
		repathTimer -= deltaTime;

		if (repathTimer > 0.0f)
		{
			return;
		}

		// Recalculate every half-second.
		repathTimer = 0.5f;

		// Convert world positions to grid coordinates.
		Node start = WorldToGrid(grid, position);
		Node goal = WorldToGrid(grid, player.position);

		// Make sure both cells are walkable.
		if (!start.walkable || !goal.walkable)
		{
			path.clear();
			waypointIndex = 0;
			return;
		}

		// Calculate the path using A*.
		path = FindPath(grid, start, goal);

		// Skip the starting node.
		waypointIndex = path.size() > 1 ? 1 : 0;
	}

	void FollowPath(float deltaTime)
	{
		// Make sure there is another waypoint to follow.
		if (waypointIndex >= path.size())
		{
			return;
		}

		// Get the node we're currently travelling toward.
		const Node& targetNode = path[waypointIndex];

		// Convert the grid coordinates into world coordinates.
		Vector3 targetPosition =
		{
			GRID_ORIGIN_X + (targetNode.x + 0.5f) * CELL_SIZE,
			position.y,
			GRID_ORIGIN_Z + (targetNode.z + 0.5f) * CELL_SIZE
		};

		// Calculate the direction toward the waypoint.
		Vector3 direction = Vector3Subtract(targetPosition, position);
		direction.y = 0.0f;

		// Determine how far away the waypoint is.
		float distance = Vector3Length(direction);

		// Determine how far the chaser can move this frame.
		float moveDistance = moveSpeed * deltaTime;

		// If we can reach the waypoint this frame, snap to it.
		if (distance <= moveDistance)
		{
			position.x = targetPosition.x;
			position.z = targetPosition.z;

			// Move to the next waypoint.
			waypointIndex++;
			return;
		}

		// Otherwise, move toward the current waypoint.
		Vector3 normalizedDirection = Vector3Normalize(direction);

		position.x += normalizedDirection.x * moveDistance;
		position.z += normalizedDirection.z * moveDistance;
	}


};


bool IsColliding(PlayerCube& playerCube, PlatformCube& platformCube);
void ResolveXCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax);

void ResolveYCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax);

void ResolveZCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax);
void ResolveCollision(PlayerCube& playerCube, const PlatformCube& platform, const Vector3 previousPos);

void HandleInput(PlayerCube& playerCube, const Camera3D& camera, float deltaTime);

void UpdatePhysics(PlayerCube& playerCube, float deltaTime);

void Update(PlayerCube& playerCube, std::vector<ChaserAI>& chasers, std::vector<std::vector<Node>>& grid,
	const Camera3D& camera, std::vector<PlatformCube>& obstacles);

void DrawWorld(const PlayerCube& playerCube, const std::vector<ChaserAI>& chasers, const std::vector<PlatformCube>& obstacles);

void DebugMessage();

void DebugNavigation(const std::vector<std::vector<Node>>& grid, const std::vector<ChaserAI>& chasers, const PlayerCube& player);

int main()
{
	constexpr int Window_Width = 1200;
	constexpr int Window_Length = 800;


	InitWindow(Window_Width, Window_Length, "Raylib Test");
	SetTargetFPS(60);

	// Create a 3D camera
	Camera3D camera = { 0 };

	camera.position = { -12.0f, 14.0f, 12.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	std::vector<std::vector<Node>> grid(100, std::vector<Node>(100));

	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			grid[i][j].x = static_cast<int>(i);
			grid[i][j].z = static_cast<int>(j);
		}
	}


	PlayerCube playerCube;
	std::vector<ChaserAI> chasers = { { {-1.0f, 0.0f, -3.0f}, 0.50f, 3.0f, BLUE },
		{{ 3.0f, 0.0f, 1.0f }, 0.50f, 3.0f, DARKBLUE },
		{{ 7.0f, 0.0f, 2.0f }, 0.50f, 3.0f, DARKBLUE } };

	std::vector<PlatformCube> obstacles =
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


	while (!WindowShouldClose())
	{
		Update(playerCube, chasers, grid, camera, obstacles);

	}

	CloseWindow();
	return 0;
}

void Update(PlayerCube& playerCube, std::vector<ChaserAI>& chasers, std::vector<std::vector<Node>>& grid,
	const Camera3D& camera, std::vector<PlatformCube>& obstacles)
{
	Vector3 previousPos = playerCube.position;

	float deltaTime = GetFrameTime();

	static bool showNavigation = false;

	if (IsKeyPressed(KEY_F1))
	{
		showNavigation = !showNavigation;
	}


	HandleInput(playerCube, camera, deltaTime);

	UpdatePhysics(playerCube, deltaTime);

	for (PlatformCube& obstacle : obstacles)
	{
		if (IsColliding(playerCube, obstacle))
		{
			ResolveCollision(playerCube, obstacle, previousPos);
		}
	}


	// Update every enemy independently before drawing the scene.
	for (ChaserAI& chaser : chasers)
	{
		chaser.UpdatePath(playerCube, grid, deltaTime);
		chaser.FollowPath(deltaTime);
		chaser.ChaserPhysics();
	}

	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode3D(camera);

	if (showNavigation)
	{
		DebugNavigation(grid, chasers, playerCube);
	}

	DrawWorld(playerCube, chasers, obstacles);

	DrawPlane(Vector3{ 0.0f, -0.02f, 0.0f },
		Vector2{ 200.0f, 200.0f }, DARKGRAY);
	DrawGrid(200, 1.0f);

	EndMode3D();

	DebugMessage();

	EndDrawing();
}

void DrawWorld(const PlayerCube& playerCube, const std::vector<ChaserAI>& chasers, const std::vector<PlatformCube>& obstacles)
{
	DrawCube(playerCube.position, playerCube.size.x, playerCube.size.y, playerCube.size.z, playerCube.color);

	// Each enemy has its own position, route, and waypoint index.
	for (const ChaserAI& chaser : chasers)
	{
		DrawSphere(chaser.position, chaser.radius, chaser.color);
	}

	for (const PlatformCube& obstacle : obstacles)
	{
		DrawCube(obstacle.position, obstacle.size.x, obstacle.size.y, obstacle.size.z, obstacle.color);
	}
}

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

void DebugNavigation(const std::vector<std::vector<Node>>& grid, const std::vector<ChaserAI>& chasers, const PlayerCube& player)
{
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

void UpdatePhysics(PlayerCube& playerCube, float deltaTime)
{
	float currentGravity = playerCube.gravity;

	// Apply gravity
	playerCube.velocity.y -= currentGravity * deltaTime;

	// Update position
	playerCube.position.y += playerCube.velocity.y * deltaTime;

	// Landing
	if (playerCube.position.y <= 0.0f)
	{
		playerCube.position.y = 0.0f;
		playerCube.velocity.y = 0.0f;
		playerCube.canJump = true;
	}

	// Shorten jump when Space isn't held
	if (IsKeyUp(KEY_SPACE) && playerCube.velocity.y > 0.0f)
	{
		currentGravity *= 2.0f;
	}
}


void HandleInput(PlayerCube& playerCube, const Camera3D& camera, float deltaTime)
{
	Vector3 forward = Vector3Subtract(camera.target, camera.position);
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);

	Vector3 right = Vector3CrossProduct(forward, camera.up);
	right = Vector3Normalize(right);

	//Camera is offset so having the input math normalized to the way the camera faces

	if (IsKeyDown(KEY_W))
	{
		playerCube.position.x += forward.x * playerCube.moveSpeed * deltaTime;
		playerCube.position.z += forward.z * playerCube.moveSpeed * deltaTime;
	}
	else if (IsKeyDown(KEY_S))
	{
		playerCube.position.x -= forward.x * playerCube.moveSpeed * deltaTime;
		playerCube.position.z -= forward.z * playerCube.moveSpeed * deltaTime;
	}
	else if (IsKeyDown(KEY_A))
	{
		playerCube.position.x -= right.x * playerCube.moveSpeed * deltaTime;
		playerCube.position.z -= right.z * playerCube.moveSpeed * deltaTime;
	}
	if (IsKeyDown(KEY_D))
	{
		playerCube.position.x += right.x * playerCube.moveSpeed * deltaTime;
		playerCube.position.z += right.z * playerCube.moveSpeed * deltaTime;
	}

	// Start jump
	if (IsKeyPressed(KEY_SPACE) && playerCube.canJump)
	{
		playerCube.velocity.y = playerCube.jumpForce;
		playerCube.canJump = false;
	}


}

void ResolveZCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax)
{
	if (playerPreviousMax.z <= platformMin.z)
	{
		playerCube.position.z = platformMin.z - playerCube.size.z / 2.0f;
		playerCube.velocity.z = 0;
		std::cout << "Colliding from the Left" << std::endl;
	}
	else if (playerPreviousMin.z >= platformMax.z)
	{
		playerCube.position.z = platformMax.z + playerCube.size.z / 2.0f;
		playerCube.velocity.z = 0.0f;
		std::cout << "Colliding from the Right" << std::endl;
	}
}

void ResolveYCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax)
{
	if (playerPreviousMax.y <= platformMin.y)
	{
		playerCube.position.y = platformMin.y - playerCube.size.y / 2.0f;
		playerCube.velocity.y = 0.0f;
		std::cout << "Colliding from below" << std::endl;

	}
	else if (playerPreviousMin.y >= platformMax.y)
	{
		playerCube.position.y = platformMax.y + playerCube.size.y / 2.0f;
		playerCube.velocity.y = 0.0f;
		playerCube.canJump = true;
		std::cout << "Colliding from above" << std::endl;
	}
}

void ResolveXCollision(Vector3& playerPreviousMax, Vector3& platformMin, PlayerCube& playerCube, Vector3& playerPreviousMin, Vector3& platformMax)
{
	if (playerPreviousMax.x <= platformMin.x)
	{
		playerCube.position.x = platformMin.x - playerCube.size.x / 2.0f;
		playerCube.velocity.x = 0.0f;
		std::cout << "Colliding from Back" << std::endl;
	}
	else if (playerPreviousMin.x >= platformMax.x)
	{
		playerCube.position.x = platformMax.x + playerCube.size.x / 2.0f;
		playerCube.velocity.x = 0.0f;
		std::cout << "Colliding from Front" << std::endl;
	}
}

bool IsColliding(PlayerCube& playerCube, PlatformCube& platformCube)
{
	if (CheckCollisionBoxes(BoundingBox{
		Vector3 {
		playerCube.position.x - playerCube.size.x / 2,
			playerCube.position.y - playerCube.size.y / 2,
			playerCube.position.z - playerCube.size.z / 2
	},
			Vector3 {
			playerCube.position.x + playerCube.size.x / 2,
				playerCube.position.y + playerCube.size.y / 2,
				playerCube.position.z + playerCube.size.z / 2
		}
		},
		BoundingBox{
		Vector3{
		platformCube.position.x - platformCube.size.x / 2,
			platformCube.position.y - platformCube.size.y / 2,
			platformCube.position.z - platformCube.size.z / 2
	},
			Vector3{
			platformCube.position.x + platformCube.size.x / 2,
				platformCube.position.y + platformCube.size.y / 2,
				platformCube.position.z + platformCube.size.z / 2
		}
		}
	))
	{
		playerCube.isColliding = true;
		return true;
	}
	playerCube.isColliding = false;
	return false;

}

void ResolveCollision(PlayerCube& playerCube, const PlatformCube& platform, const Vector3 previousPos)
{
	Vector3 playerPreviousMin =
	{
		previousPos.x - playerCube.size.x / 2.0f,
		previousPos.y - playerCube.size.y / 2.0f,
		previousPos.z - playerCube.size.z / 2.0f
	};

	Vector3 playerPreviousMax =
	{
		previousPos.x + playerCube.size.x / 2.0f,
		previousPos.y + playerCube.size.y / 2.0f,
		previousPos.z + playerCube.size.z / 2.0f
	};
	Vector3 platformMin =
	{
		platform.position.x - platform.size.x / 2.0f,
		platform.position.y - platform.size.y / 2.0f,
		platform.position.z - platform.size.z / 2.0f
	};

	Vector3 platformMax =
	{
		platform.position.x + platform.size.x / 2.0f,
		platform.position.y + platform.size.y / 2.0f,
		platform.position.z + platform.size.z / 2.0f
	};

	ResolveXCollision(playerPreviousMax, platformMin, playerCube, playerPreviousMin, platformMax);
	ResolveYCollision(playerPreviousMax, platformMin, playerCube, playerPreviousMin, platformMax);
	ResolveZCollision(playerPreviousMax, platformMin, playerCube, playerPreviousMin, platformMax);

}

std::vector<Node> FindPath(const std::vector<std::vector<Node>>& grid, Node start, Node goal)
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

Node WorldToGrid(const std::vector<std::vector<Node>>& grid, const Vector3& worldPosition)
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



//// ignore this has was for my learning
//// Example grid coordinates
//int gridX = 3;
//int gridZ = 5;

//// Example chaser world position
//float chaserWorldX = 2.0f;
//float chaserWorldZ = -2.4f;

//// --------------------------------
//// GRID → WORLD
//// --------------------------------

//float worldX = GRID_ORIGIN_X + (gridX * CELL_SIZE);
//float worldZ = GRID_ORIGIN_Z + (gridZ * CELL_SIZE);

//// --------------------------------
//// WORLD → GRID
//// --------------------------------

//int chaserGridX = static_cast<int>(
//	std::floor((chaserWorldX - GRID_ORIGIN_X) / CELL_SIZE)
//	);

//int chaserGridZ = static_cast<int>(
//	std::floor((chaserWorldZ - GRID_ORIGIN_Z) / CELL_SIZE)
//	);
