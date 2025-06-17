#pragma once

#include <set>
#include <unordered_map>
#include "TestGameplayPhase.h"
#include "TestGameplayPiles.h"
#include "ServerHandler/ServerManager.h"
#include "TestUpdateInfo.h"

enum GameActionType {
  END_TURN,
  SELECT_CARD,
  FINISH_SELECTION
};

enum MessageType {
  SETUP,
  HEADS_OR_TAILS,
  COIN_CHOICE,
  FIRST_OR_SECOND,
  FIRST_OR_SECOND_CHOICE,
  GAMEPLAY,
};

struct GameAction {
  GameActionType type;
  std::vector<unsigned int> selectedCards;
  Pile from;
};

class TestGameplayManager {
private:
  std::set<unsigned int> selectedCards;
  TestGameplayPhase phase;
  ServerManager server;
  UpdateInfo SendAction(json action);
public:
  TestGameplayManager();
  SetupData Setup(const std::vector<unsigned int>& deck);
  bool IsPlayableCard(unsigned int id);
  bool IsSelectedCard(unsigned int id);
  UpdateInfo RequestUpdate(GameAction action);
  bool SelectionPossiblyDone();
  GameMode GetPhase();
  std::unordered_map<unsigned int, unsigned int> gameIDToID;
};
