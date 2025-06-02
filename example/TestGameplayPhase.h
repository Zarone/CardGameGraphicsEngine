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
  GameMode GetMode();
  void SetMode(GameMode mode);
  bool IsPlayableCard(unsigned int id);
  void SetPlayableCards(std::vector<unsigned int> cards);
  TestGameplayPhase(bool goingFirst);
};
