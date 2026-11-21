#include "Player.h"
#include "raymath.h"
#include <iostream>

void Player::HandleInput(const Camera3D& camera, float deltaTime, const ArenaBounds& bounds)
{
	Vector3 forward = Vector3Subtract(camera.target, camera.position);
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);

	Vector3 right = Vector3CrossProduct(forward, camera.up);
	right = Vector3Normalize(right);

	//Camera is offset so having the input math normalized to the way the camera faces

	if (IsKeyDown(KEY_W))
	{
		position.x += forward.x * moveSpeed * deltaTime;
		position.z += forward.z * moveSpeed * deltaTime;
	}
	else if (IsKeyDown(KEY_S))
	{
		position.x -= forward.x * moveSpeed * deltaTime;
		position.z -= forward.z * moveSpeed * deltaTime;
	}
	else if (IsKeyDown(KEY_A))
	{
		position.x -= right.x * moveSpeed * deltaTime;
		position.z -= right.z * moveSpeed * deltaTime;
	}
	else if (IsKeyDown(KEY_D))
	{
		position.x += right.x * moveSpeed * deltaTime;
		position.z += right.z * moveSpeed * deltaTime;
	}

	// Keep the player inside the wall that wraps the obstacle layout.
	if (position.x < bounds.minX) position.x = bounds.minX;
	if (position.x > bounds.maxX) position.x = bounds.maxX;
	if (position.z < bounds.minZ) position.z = bounds.minZ;
	if (position.z > bounds.maxZ) position.z = bounds.maxZ;

	// Start jump
	if (IsKeyPressed(KEY_SPACE) && canJump)
	{
		std::cout << "Player is Jumping";
		velocity.y = jumpForce;
		canJump = false;
	}
}

void Player::UpdatePhysics(float deltaTime)
{
	float groundHeight = size.y / 2.0f;

	// Apply gravity while airborne
	if (!canJump)
	{
		float currentGravity = gravity;

		// Release Space early for a shorter jump
		if (IsKeyUp(KEY_SPACE) && velocity.y > 0.0f)
		{
			currentGravity *= 2.0f;
		}

		// Apply gravity
		velocity.y -= currentGravity * deltaTime;

		// Move player vertically
		position.y += velocity.y * deltaTime;
	}

	// Landing
	if (position.y <= groundHeight && velocity.y <= 0.0f)
	{
		position.y = groundHeight;
		velocity.y = 0.0f;
		canJump = true;
	}
}

void Player::Draw() const
{
	DrawCube(position, size.x, size.y, size.z, color);
}

BoundingBox Player::GetBounds() const
{
	BoundingBox box;

	box.min = {
		position.x - size.x / 2.0f,
		position.y - size.y / 2.0f,
		position.z - size.z / 2.0f
	};

	box.max = {
		position.x + size.x / 2.0f,
		position.y + size.y / 2.0f,
		position.z + size.z / 2.0f
	};

	return box;
}

void Player::RestartGame(Score& score)
{
	position = { 0.0f, 0.0f, 0.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	canJump = true;

	score.totalScore = 0;
}
