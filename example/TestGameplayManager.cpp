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
    if (action.from == HAND && action.selectedCard == 12) {
      this->phase.SetMode(SELECTING_CARDS);
      this->phase.SetPlayableCards({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
      this->phase.SetSelectionRange(2, 2);
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

    if (action.type == FINISH_SELECTION) {
      std::vector<CardMovement> movements = {};
      for (unsigned int el : this->selectedCards) {
        CardMovement movement = {
          .cardId = el,
          .from = HAND,
          .to = DISCARD
        };
        movements.push_back(movement);
      }

      this->phase.SetMode(MY_TURN);
      this->phase.SetPlayableCards({});

      return {
        .movements = movements,
        .phaseChange = true
      };
    } else {
      std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCard);
      if (!res.second) {
        this->selectedCards.erase(action.selectedCard);
      }

      return {
        .movements = {},
        .phaseChange = true
      };
    }

  }
}

bool TestGameplayManager::SelectionPossiblyDone() {
  std::cout << "Here" << std::endl;
  return this->phase.InSelectionRange(this->selectedCards.size());
}

GameMode TestGameplayManager::GetPhase() {
  return this->phase.GetMode();
}
