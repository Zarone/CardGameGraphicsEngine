#include "TestGameplayManager.h"
#include "TestGameState.h"
#include <vector>

TestGameplayManager::TestGameplayManager() 
: phase(true)
{
}

bool TestGameplayManager::IsPlayableCard(unsigned int id) {
  return this->phase.IsPlayableCard(id);
}

UpdateInfo TestGameplayManager::RequestUpdate(GameAction action) {
  std::vector<CardMovement> movements = {{
    .cardId = action.selectedCard,
    .from = action.from,
    .to = action.from == DISCARD ? DECK : DISCARD,
  }};

  return {
    .movements = movements
  };
}
