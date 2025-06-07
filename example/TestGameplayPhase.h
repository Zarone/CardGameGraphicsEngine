#pragma once

#include <vector>

enum GameMode {
  MY_TURN, 
  OPPONENT_TURN,
  SELECTING_CARDS,
  SELECTING_TEMPORARY_CARDS
};

class TestGameplayPhase {
private:
  GameMode mode;
  // game IDs the user can play
  std::vector<unsigned int> playableCardIds;
  int selectionMax;
  int selectionMin;

public:
  GameMode GetMode();
  void SetMode(GameMode mode);
  bool IsPlayableCard(unsigned int id);
  void SetPlayableCards(std::vector<unsigned int> cards);

  /*
  * Sets the possible range for the number of cards selected by the user
  *
  * @param min Minimum number of possible cards to select (inclusive)
  * @param max Maximum number of possible cards to select (inclusive)
  */
  void SetSelectionRange(int min, int max);

  bool InSelectionRange(int value);
  TestGameplayPhase(bool goingFirst);
};
