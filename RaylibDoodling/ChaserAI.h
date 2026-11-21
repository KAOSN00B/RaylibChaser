#pragma once

#include "raylib.h"
#include "Navigation.h"
#include "Player.h"
#include <vector>

// An enemy that paths toward the player over the navigation grid.
class ChaserAI
{
public:
	ChaserAI(Vector3 position, float radius, float moveSpeed, Color color);

	void ChaserPhysics();

	//prototype function just to get AI to move
	void ChasePlayer(const Player& player, float deltaTime);

	void UpdatePath(const Player& player, const NavigationGrid& navigation, float deltaTime);
	void FollowPath(float deltaTime);

	void Draw() const;

	void Restart();

	Vector3 position;
	float radius;
	float moveSpeed;
	Color color;

	//Path Navigation
	std::vector<Node> path;
	size_t waypointIndex = 0;
	float repathTimer = 0.0f;
};
