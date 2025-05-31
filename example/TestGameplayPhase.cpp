#include "TestGameplayPhase.h"

GameMode TestGameplayPhase::GetMode() {
  return this->mode;
}

void TestGameplayPhase::SetMode(GameMode mode) {
  this->mode = mode;
}

bool TestGameplayPhase::IsPlayableCard(unsigned int id) {
  auto index = std::find(this->playableCardIds.begin(), this->playableCardIds.end(), id);
  return index != this->playableCardIds.end();
}

void TestGameplayPhase::SetPlayableCards(std::vector<unsigned int> cards) {
  this->playableCardIds = cards;
}

TestGameplayPhase::TestGameplayPhase(bool goingFirst) {
  this->mode = goingFirst ? MY_TURN : OPPONENT_TURN;

  this->playableCardIds.push_back(12);
}
