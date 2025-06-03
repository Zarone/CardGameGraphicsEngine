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

bool TestGameplayManager::IsSelectedCard(unsigned int id) {
  return this->selectedCards.find(id) != this->selectedCards.end();
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
      return {
        .movements = movements,
        .phaseChange = true 
      };
    } 
    return {
      .movements = movements,
      .phaseChange = false
    };
  } else {
    std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCard);
    if (!res.second) {
      this->selectedCards.erase(action.selectedCard);
    }
    PrintSet(std::cout, this->selectedCards);

    std::vector<CardMovement> movements = {};
    for (unsigned int el : this->selectedCards) {
      CardMovement movement = {
        .cardId = el,
        .from = HAND,
        .to = DISCARD
      };
      movements.push_back(movement);
    }

    if (this->selectedCards.size() == 2) {
      this->phase.SetMode(MY_TURN);
      return {
        .movements = movements,
        .phaseChange = true
      };
    } else {
      return {
        .movements = {},
        .phaseChange = false
      };
    }
  }
}

GameMode TestGameplayManager::GetPhase() {
  return this->phase.GetMode();
}
