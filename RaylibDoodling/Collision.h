#pragma once

#include "raylib.h"
#include "Player.h"
#include "World.h"

// Collision detection and resolution between the player and static platforms.
namespace Collision
{
	bool IsColliding(Player& player, const PlatformCube& platform);
	void ResolveCollision(Player& player, const PlatformCube& platform, const Vector3 previousPos);
}
