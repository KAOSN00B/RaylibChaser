#include "raylib.h"
#include "rcamera.h"
#include <vector>

#include "Navigation.h"
#include "World.h"
#include "Player.h"
#include "ChaserAI.h"
#include "Collision.h"
#include "Debug.h"

void Update(Player& player, std::vector<ChaserAI>& chasers, const NavigationGrid& navigation,
	const Camera3D& camera, const std::vector<PlatformCube>& obstacles);

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

	NavigationGrid navigation(100, 100);

	Player player;
	std::vector<ChaserAI> chasers = { ChaserAI({-1.0f, 0.0f, -3.0f}, 0.50f, 3.0f, BLUE),
		ChaserAI({ 3.0f, 0.0f, 1.0f }, 0.50f, 3.0f, DARKBLUE),
		ChaserAI({ 7.0f, 0.0f, 2.0f }, 0.50f, 3.0f, DARKBLUE) };

	std::vector<PlatformCube> obstacles = CreateObstacles();

	navigation.MarkObstacles(obstacles);

	while (!WindowShouldClose())
	{
		Update(player, chasers, navigation, camera, obstacles);
	}

	CloseWindow();
	return 0;
}

void Update(Player& player, std::vector<ChaserAI>& chasers, const NavigationGrid& navigation,
	const Camera3D& camera, const std::vector<PlatformCube>& obstacles)
{
	Vector3 previousPos = player.position;

	float deltaTime = GetFrameTime();

	static bool showNavigation = false;

	if (IsKeyPressed(KEY_F1))
	{
		showNavigation = !showNavigation;
	}


	player.HandleInput(camera, deltaTime);

	player.UpdatePhysics(deltaTime);

	for (const PlatformCube& obstacle : obstacles)
	{
		if (Collision::IsColliding(player, obstacle))
		{
			Collision::ResolveCollision(player, obstacle, previousPos);
		}
	}


	// Update every enemy independently before drawing the scene.
	for (ChaserAI& chaser : chasers)
	{
		chaser.UpdatePath(player, navigation, deltaTime);
		chaser.FollowPath(deltaTime);
		chaser.ChaserPhysics();
	}

	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode3D(camera);

	if (showNavigation)
	{
		DebugNavigation(navigation, chasers, player);
	}

	player.Draw();

	for (const ChaserAI& chaser : chasers)
	{
		chaser.Draw();
	}

	DrawObstacles(obstacles);
	DrawGround();

	EndMode3D();

	DebugMessage();

	EndDrawing();
}
