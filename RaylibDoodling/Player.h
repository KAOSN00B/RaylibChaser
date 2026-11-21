#pragma once

#include "raylib.h"
#include "World.h"
#include "Score.h"

// The player-controlled cube: holds its own state and knows how to move,
// fall, jump, and draw itself. Collision response lives in Collision.h/.cpp
// since it is a relationship between the player and a platform, not purely
// player behaviour.
class Player
{
public:
	void HandleInput(const Camera3D& camera, float deltaTime, const ArenaBounds& bounds);
	void UpdatePhysics(float deltaTime);
	void Draw() const;
	BoundingBox GetBounds() const;
	void RestartGame(Score& score);

	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 size = { 1.0f, 1.0f, 1.0f };
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	float gravity = 9.81f;
	float jumpForce = 5.0f;
	float moveSpeed = 5.0f;

	bool canJump = true;
	bool isColliding = false;
	Color color = DARKGREEN;
};
