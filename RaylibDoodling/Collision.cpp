#include "Collision.h"
#include "Score.h"
#include <iostream>

namespace
{
	void ResolveXCollision(Vector3& playerPreviousMax, Vector3& platformMin, Player& player, Vector3& playerPreviousMin, Vector3& platformMax)
	{
		if (playerPreviousMax.x <= platformMin.x)
		{
			player.position.x = platformMin.x - player.size.x / 2.0f;
			player.velocity.x = 0.0f;
			std::cout << "Colliding from Back" << std::endl;
		}
		else if (playerPreviousMin.x >= platformMax.x)
		{
			player.position.x = platformMax.x + player.size.x / 2.0f;
			player.velocity.x = 0.0f;
			std::cout << "Colliding from Front" << std::endl;
		}
	}

	void ResolveYCollision(Vector3& playerPreviousMax, Vector3& platformMin, Player& player, Vector3& playerPreviousMin, Vector3& platformMax)
	{
		if (playerPreviousMax.y <= platformMin.y)
		{
			player.position.y = platformMin.y - player.size.y / 2.0f;
			player.velocity.y = 0.0f;
			std::cout << "Colliding from below" << std::endl;

		}
		else if (playerPreviousMin.y >= platformMax.y)
		{
			player.position.y = platformMax.y + player.size.y / 2.0f;
			player.velocity.y = 0.0f;
			player.canJump = true;
			std::cout << "Colliding from above" << std::endl;
		}
	}

	void ResolveZCollision(Vector3& playerPreviousMax, Vector3& platformMin, Player& player, Vector3& playerPreviousMin, Vector3& platformMax)
	{
		if (playerPreviousMax.z <= platformMin.z)
		{
			player.position.z = platformMin.z - player.size.z / 2.0f;
			player.velocity.z = 0;
			std::cout << "Colliding from the Left" << std::endl;
		}
		else if (playerPreviousMin.z >= platformMax.z)
		{
			player.position.z = platformMax.z + player.size.z / 2.0f;
			player.velocity.z = 0.0f;
			std::cout << "Colliding from the Right" << std::endl;
		}
	}
}

namespace Collision
{
	bool IsColliding(Player& player, const PlatformCube& platform)
	{
		if (CheckCollisionBoxes(BoundingBox{
			Vector3 {
			player.position.x - player.size.x / 2,
				player.position.y - player.size.y / 2,
				player.position.z - player.size.z / 2
		},
				Vector3 {
				player.position.x + player.size.x / 2,
					player.position.y + player.size.y / 2,
					player.position.z + player.size.z / 2
			}
			},
			BoundingBox{
			Vector3{
			platform.position.x - platform.size.x / 2,
				platform.position.y - platform.size.y / 2,
				platform.position.z - platform.size.z / 2
		},
				Vector3{
				platform.position.x + platform.size.x / 2,
					platform.position.y + platform.size.y / 2,
					platform.position.z + platform.size.z / 2
			}
			}
		))
		{
			player.isColliding = true;
			return true;
		}
		player.isColliding = false;
		return false;

	}

	void ResolveCollision(Player& player, const PlatformCube& platform, const Vector3 previousPos, Score& score)
	{
		Vector3 playerPreviousMin =
		{
			previousPos.x - player.size.x / 2.0f,
			previousPos.y - player.size.y / 2.0f,
			previousPos.z - player.size.z / 2.0f
		};

		Vector3 playerPreviousMax =
		{
			previousPos.x + player.size.x / 2.0f,
			previousPos.y + player.size.y / 2.0f,
			previousPos.z + player.size.z / 2.0f
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
		Vector3 scoreMin =
		{
			score.position.x - score.size.x / 2.0f,
			score.position.y - score.size.y / 2.0f,
			score.position.z - score.size.z / 2.0f
		};

		Vector3 scoreMax =
		{
			score.position.x + score.size.x / 2.0f,
			score.position.y + score.size.y / 2.0f,
			score.position.z + score.size.z / 2.0f
		};

		ResolveXCollision(playerPreviousMax, platformMin, player, playerPreviousMin, platformMax);
		ResolveYCollision(playerPreviousMax, platformMin, player, playerPreviousMin, platformMax);
		ResolveZCollision(playerPreviousMax, platformMin, player, playerPreviousMin, platformMax);

		
	}

	bool IsColliding(Player& player, Score& score)
	{

		BoundingBox playerBox{
			{
				player.position.x - player.size.x / 2.0f,
				player.position.y - player.size.y / 2.0f,
				player.position.z - player.size.z / 2.0f
			},
			{
				player.position.x + player.size.x / 2.0f,
				player.position.y + player.size.y / 2.0f,
				player.position.z + player.size.z / 2.0f
			}
		};

		BoundingBox scoreBox{
			{
				score.position.x - score.size.x / 2.0f,
				score.position.y - score.size.y / 2.0f,
				score.position.z - score.size.z / 2.0f
			},
			{
				score.position.x + score.size.x / 2.0f,
				score.position.y + score.size.y / 2.0f,
				score.position.z + score.size.z / 2.0f
			}
		};
	

		if (!CheckCollisionBoxes(playerBox, scoreBox))
		{
			return false;
		}
		

		return true;
	}
}
