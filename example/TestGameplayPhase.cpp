#include "TestGameplayPhase.h"

bool TestGameplayPhase::IsPlayableCard(unsigned int id) {
  auto index = std::find(this->playableCardIds.begin(), this->playableCardIds.end(), id);
  return index != this->playableCardIds.end();
}

TestGameplayPhase::TestGameplayPhase(bool goingFirst) {
  this->mode = goingFirst ? MY_TURN : OPPONENT_TURN;

  this->playableCardIds.push_back(5);
  this->playableCardIds.push_back(2);
  this->playableCardIds.push_back(1);
  this->playableCardIds.push_back(6);
}
