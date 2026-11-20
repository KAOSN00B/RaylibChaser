#pragma once
#include "raylib.h"

enum ScoreState
{
	Available,
	Respawn

};

class Score
{
public:
	void AddScore(int score);
	void Draw() const;
	void Respawner(float deltaTime);
	void Update(float deltaTime);
	void DrawHUD();

	bool isColliding = false;
	bool isCollected = false;
	float randomXValue = GetRandomValue(0, 100);
	float randomYValue = GetRandomValue(0, 100);

	Vector3 position = { 0, 0, 0 };
	Vector3 size = { 1.0f, 1.0f, 1.0f };
	Color color = YELLOW;

	float respawnDelay = 1.0f;
	int scoreAmount = 100;
	int totalScore = 0;
	ScoreState state =  ScoreState::Available;


};
