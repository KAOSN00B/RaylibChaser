#include "Score.h"
#include <iostream>

void Score::AddScore(int score)
{
	totalScore += score;
	std::cout << "Score is: " << totalScore;
}

void Score::Draw() const
{

	if (state == Available && !isCollected)
	{
		DrawCube(position, size.x, size.y, size.z, color);
	}
}

void Score::Respawner(float deltaTime)
{

	respawnDelay -= deltaTime;

	position.x = static_cast<float>(GetRandomValue(-15, 15));
	position.y = size.y / 2.0f;
	position.z = static_cast<float>(GetRandomValue(-15, 15));

	isCollected = false;
	state = Available;
}

void Score::Update(float deltaTime)
{

	if (state == Respawn)
	{
		Respawner(deltaTime);
	}
	

}

