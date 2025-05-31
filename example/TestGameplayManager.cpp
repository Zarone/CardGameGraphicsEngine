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
  if (this->phase.GetMode() == MY_TURN) {
    std::vector<CardMovement> movements = {{
      .cardId = action.selectedCard,
      .from = action.from,
      .to = action.from == DISCARD ? DECK : DISCARD,
    }};
    if (action.selectedCard == 12) {
      this->phase.SetMode(SELECTING_CARDS);
      this->phase.SetPlayableCards({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
      this->selectedCards = {};
    } 
    return {
      .movements = movements
    };
  } else {
    std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCard);
    if (!res.second) {
      this->selectedCards.erase(action.selectedCard);
    }
    PrintSet(std::cout, this->selectedCards);
    return {
      .movements = {}
    };
  }
}
