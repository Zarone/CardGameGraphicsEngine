#pragma once

#include <vector>

enum GameMode {
  MY_TURN, 
  OPPONENT_TURN,
  SELECTING_CARDS
};

class TestGameplayPhase {
private:
  GameMode mode;
  // game IDs the user can play
  std::vector<unsigned int> playableCardIds;

public:
  bool IsPlayableCard(unsigned int id);
  TestGameplayPhase(bool goingFirst);
};
