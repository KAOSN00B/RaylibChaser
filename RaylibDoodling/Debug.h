#pragma once

#include "Navigation.h"
#include "ChaserAI.h"
#include "Player.h"
#include <vector>

// On-screen debug text and navigation-grid visualization, toggled with F1.
void DebugMessage();
void DebugNavigation(const NavigationGrid& navigation, const std::vector<ChaserAI>& chasers, const Player& player);
