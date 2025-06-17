#pragma once

#include "TestGameplayPiles.h"
#include "TestGameMode.h"
#include <vector>

struct CardMovement {
  unsigned int cardId;
  Pile from;
  Pile to;
};

struct UpdateInfo {
  std::vector<CardMovement> movements;  
  bool phaseChange;
  GameMode phase;
  Pile openView; 
  std::vector<unsigned int> openViewCards;
  std::vector<unsigned int> selectableCards;
};

