#pragma once

#include "raylib.h"
#include "Player.h"
#include "World.h"
#include "Score.h"

// Collision detection and resolution between the player and static platforms.
namespace Collision
{
	bool IsColliding(Player& player, const PlatformCube& platform);
	bool IsColliding(Player& player, Score& score);
	void ResolveCollision(Player& player, const PlatformCube& platform,
		const Vector3 previousPos, Score& score);

}
