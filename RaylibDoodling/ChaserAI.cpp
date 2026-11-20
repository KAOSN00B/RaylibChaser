#include "ChaserAI.h"
#include "raymath.h"
#include "Score.h"

ChaserAI::ChaserAI(Vector3 position, float radius, float moveSpeed, Color color)
	: position(position), radius(radius), moveSpeed(moveSpeed), color(color)
{
}

void ChaserAI::ChaserPhysics()
{
	if (position.y <= 0.0f)
	{
		position.y = radius;
	}
}

void ChaserAI::ChasePlayer(const Player& player, float deltaTime)
{
	Vector3 chaseDirection = Vector3Subtract(player.position, position);
	chaseDirection.y = 0.0f;
	Vector3 toPlayer = Vector3Normalize(chaseDirection);

	position.x += toPlayer.x * moveSpeed * deltaTime;
	position.z += toPlayer.z * moveSpeed * deltaTime;
}

void ChaserAI::UpdatePath(const Player& player, const NavigationGrid& navigation, float deltaTime)
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
	Node start = navigation.WorldToGrid(position);
	Node goal = navigation.WorldToGrid(player.position);

	// Make sure both cells are walkable.
	if (!start.walkable || !goal.walkable)
	{
		path.clear();
		waypointIndex = 0;
		return;
	}

	// Calculate the path using A*.
	path = navigation.FindPath(start, goal);

	// Skip the starting node.
	waypointIndex = path.size() > 1 ? 1 : 0;
}

void ChaserAI::FollowPath(float deltaTime)
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

void ChaserAI::Draw() const
{
	DrawSphere(position, radius, color);
}
