#include "TestGameplayManager.h"
#include "TestGameState.h"
#include "ServerHandler/ServerManager.h"
#include <stdexcept>
#include <vector>

TestGameplayManager::TestGameplayManager() 
: phase(true)
{
	if (!server.ConnectToServer()) {
    throw std::runtime_error("Failed to connect to server");
		return;
	}

  std::cout << "Starting Server Initialization" << std::endl;

  std::vector<unsigned int> deck = {
    1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,
    51,52,53,54,55,56,57,58,59,60,
  };

  std::vector<unsigned int> correspondingGameIds = server.Initialize(deck);
  
  PrintVector(std::cout, correspondingGameIds);
  for (int i = 0; i < correspondingGameIds.size(); ++i) {
    this->gameIDToID[correspondingGameIds[i]] = deck[i]; 
  }

  std::cout << "Finish Server Initialization" << std::endl;
}

bool TestGameplayManager::IsPlayableCard(unsigned int id) {
  return this->phase.IsPlayableCard(id);
}

bool TestGameplayManager::IsSelectedCard(unsigned int id) {
  return 
    (this->GetPhase()==SELECTING_CARDS && this->selectedCards.find(id) != this->selectedCards.end())
    || (this->GetPhase()==SELECTING_TEMPORARY_CARDS && id == -1);
}

UpdateInfo TestGameplayManager::RequestUpdate(GameAction action) {
  std::unordered_map<GameActionType, std::string> GameActionTypeToString = {
    {GameActionType::END_TURN, "END_TURN"},
    {GameActionType::FINISH_SELECTION, "FINISH_SELECTION"},
    {GameActionType::SELECT_CARD, "SELECT_CARD"}
  };

  std::unordered_map<Pile, std::string> PileToString = {
    {Pile::DECK, "DECK"},
    {Pile::HAND, "HAND"},
  };

  server.SendMessage({
    {
      { "type", GameActionTypeToString[action.type] },
      { "selectedCard", action.selectedCard },
      { "from", PileToString[action.from] }
    }
  }, ServerManager::MessageTypeGamePlay);

  return {
    .movements = {},
    .phaseChange = true,
    .openView = HAND
  };
  //if (this->phase.GetMode() == MY_TURN) {
    //std::vector<CardMovement> movements = {{
      //.cardId = action.selectedCard,
      //.from = action.from,
      //.to = action.from == DISCARD ? DECK : DISCARD,
    //}};
    //if (action.from == HAND && action.selectedCard == 12) {
      //this->phase.SetMode(SELECTING_CARDS);
      //this->phase.SetPlayableCards({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
      //this->phase.SetSelectionRange(2, 2);
      //this->selectedCards = {};
      //return {
        //.movements = movements,
        //.phaseChange = true,
        //.openView = HAND
      //};
    //} 
    //return {
      //.movements = movements,
      //.phaseChange = false
    //};
  //} else {
    //if (action.type == FINISH_SELECTION) {
      //std::vector<CardMovement> movements = {};
      //for (unsigned int el : this->selectedCards) {
        //CardMovement movement = {
          //.cardId = el,
          //.from = HAND,
          //.to = DISCARD
        //};
        //movements.push_back(movement);
      //}

      //this->phase.SetMode(SELECTING_TEMPORARY_CARDS);
      //this->phase.SetPlayableCards({});

      //return {
        //.movements = movements,
        //.phaseChange = true,
        //.openView = TEMPORARY,
        //.openViewCards = { 6, 6, 6 }
      //};
    //} else {
      //std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCard);
      //if (!res.second) {
        //this->selectedCards.erase(action.selectedCard);
      //}

      //return {
        //.movements = {},
        //.phaseChange = true,
        //.openView = HAND
      //};
    //}

  //}
}

bool TestGameplayManager::SelectionPossiblyDone() {
  std::cout << "Here" << std::endl;
  return this->phase.InSelectionRange(this->selectedCards.size());
}

GameMode TestGameplayManager::GetPhase() {
  return this->phase.GetMode();
}
