#pragma once

#include <vector>
#include "TestGameplayPhase.h"
#include "TestGameplayPiles.h"

struct CardMovement {
  unsigned int cardId;
  Pile from;
  Pile to;
};

struct UpdateInfo {
  std::vector<CardMovement> movements;  
};

enum GameActionType {
  END_TURN,
  SELECT_CARD
};

struct GameAction {
  GameActionType type;
  unsigned int selectedCard;
  Pile from;
};

class TestGameplayManager {
private:
  std::vector<unsigned int> selectedCards;
  TestGameplayPhase phase;
public:
  TestGameplayManager();
  bool IsPlayableCard(unsigned int id);
  UpdateInfo RequestUpdate(GameAction action);
};
