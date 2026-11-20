#include "raylib.h"
#include "rcamera.h"
#include <vector>

#include "Navigation.h"
#include "World.h"
#include "Player.h"
#include "ChaserAI.h"
#include "Collision.h"
#include "Score.h"
#include "Debug.h"

void Update(Player& player, std::vector<ChaserAI>& chasers, const NavigationGrid& navigation,
	const Camera3D& camera, const std::vector<PlatformCube>& obstacles, Score& score,
	const ArenaBounds& arenaBounds, float deltaTime);

int main()
{
	constexpr int Window_Width = 1200;
	constexpr int Window_Length = 800;


	InitWindow(Window_Width, Window_Length, "Raylib Test");
	SetTargetFPS(60);

	// Create a 3D camera
	Camera3D camera = { 0 };

	camera.position = { -10.0f, 14.0f, 12.0f };
	camera.target = { 0.0f, 4.0f, 0.0f };
	camera.up = { 0.0f, 3.0f, 0.0f };
	camera.fovy = 35.0f;
	camera.projection = CAMERA_ORTHOGRAPHIC;

	NavigationGrid navigation(100, 100);

	Player player;
	std::vector<ChaserAI> chasers = { ChaserAI({-1.0f, 0.0f, -3.0f}, 1.0f, 3.0f, DARKBLUE) };


	std::vector<PlatformCube> obstacles = CreateObstacles();
	ArenaBounds arenaBounds = GetArenaBounds(obstacles, -3.0f);
	Score score;

		
	
	navigation.MarkObstacles(obstacles);

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		score.Update(deltaTime);
		Update(player, chasers, navigation, camera, obstacles, score, arenaBounds, deltaTime);
		
	}

	CloseWindow();
	return 0;
}

void Update(Player& player, std::vector<ChaserAI>& chasers, const NavigationGrid& navigation,
	const Camera3D& camera, const std::vector<PlatformCube>& obstacles, Score& score,
	const ArenaBounds& arenaBounds, float deltaTime)
{
	Vector3 previousPos = player.position;



	static bool showNavigation = false;

	if (IsKeyPressed(KEY_F1))
	{
		showNavigation = !showNavigation;
	}


	player.HandleInput(camera, deltaTime, arenaBounds);

	player.UpdatePhysics(deltaTime);



	for (const PlatformCube& obstacle : obstacles)
	{
		if (Collision::IsColliding(player, obstacle))
		{
			Collision::ResolveCollision(player, obstacle, previousPos, score);
		}	

		
	}

	static bool wasColliding = false; // persists between Update calls

	bool isColliding = Collision::IsColliding(player, score);

	if (isColliding && !wasColliding && score.state == Available)
	{
		score.AddScore(score.scoreAmount); // just entered
		score.state = Respawn;
		score.isCollected = true;
	}

	if (!isColliding && wasColliding && score.state == Respawn)
	{
		// just exited
	}

	wasColliding = isColliding;



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
	score.Draw();


	for (const ChaserAI& chaser : chasers)
	{
		chaser.Draw();
	}


	DrawObstacles(obstacles);
	DrawArenaWalls(arenaBounds, 4.0f);
	DrawGround();

	EndMode3D();

	DebugMessage();
	DrawScoreHUD(score.totalScore);

	EndDrawing();
}
