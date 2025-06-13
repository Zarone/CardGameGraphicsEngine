#pragma once

#include <set>
#include "TestGameplayPhase.h"
#include "TestGameplayPiles.h"
#include "ServerHandler/ServerManager.h"

struct CardMovement {
  unsigned int cardId;
  Pile from;
  Pile to;
};

struct UpdateInfo {
  std::vector<CardMovement> movements;  
  bool phaseChange;
  Pile openView; 
  std::vector<unsigned int> openViewCards;
};

enum GameActionType {
  END_TURN,
  SELECT_CARD,
  FINISH_SELECTION
};

struct GameAction {
  GameActionType type;
  unsigned int selectedCard;
  Pile from;
};

class TestGameplayManager {
private:
  std::set<unsigned int> selectedCards;
  TestGameplayPhase phase;
  ServerManager server;
public:
  TestGameplayManager();
  bool IsPlayableCard(unsigned int id);
  bool IsSelectedCard(unsigned int id);
  UpdateInfo RequestUpdate(GameAction action);
  bool SelectionPossiblyDone();
  GameMode GetPhase();
};
