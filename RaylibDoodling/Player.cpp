#include "Player.h"
#include "raymath.h"

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
		velocity.y = jumpForce;
		canJump = false;
	}
}

void Player::UpdatePhysics(float deltaTime)
{
	float currentGravity = gravity;


	// Landing
	if (position.y <= 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;
		canJump = true;
	}

	// Shorten jump when Space isn't held
	if (IsKeyUp(KEY_SPACE) && velocity.y > 0.0f)
	{
		currentGravity *= 2.0f;

		// Apply gravity
		velocity.y -= currentGravity * deltaTime;

		// Update position
		position.y += velocity.y * deltaTime;
	}
} 

void Player::Draw() const
{
	DrawCube(position, size.x, size.y, size.z, color);
}
