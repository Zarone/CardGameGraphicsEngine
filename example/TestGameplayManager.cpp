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
}

SetupData TestGameplayManager::Setup(const std::vector<unsigned int>& deck) {
  std::cout << "inside Setup" << std::endl;
  SetupData setupData = server.Initialize(
    deck,
    MessageType::SETUP,
    MessageType::COIN_CHOICE,
    MessageType::FIRST_OR_SECOND_CHOICE
  );

  setupData.info.phaseChange = this->phase.GetMode() != setupData.info.phase;
  this->phase.SetMode(setupData.info.phase);
  std::cout << "playable on setup:";
  PrintVector(std::cout, setupData.info.selectableCards);
  this->phase.SetPlayableCards(setupData.info.selectableCards);
  
  PrintVector(std::cout, setupData.correspondingGameIds);
  for (int i = 0; i < setupData.correspondingGameIds.size(); ++i) {
    this->gameIDToID[setupData.correspondingGameIds[i]] = deck[i]; 
  }

  std::cout << "Finish Server Initialization" << std::endl;

  return setupData;
}

bool TestGameplayManager::IsPlayableCard(unsigned int id) {
  return this->phase.IsPlayableCard(id);
}

bool TestGameplayManager::IsSelectedCard(unsigned int id) {
  return 
    (this->GetPhase()==SELECTING_CARDS && this->selectedCards.find(id) != this->selectedCards.end())
    || (this->GetPhase()==SELECTING_TEMPORARY_CARDS && id == -1);
}

UpdateInfo TestGameplayManager::SendAction(json action) {
  server.SendMessage(action, MessageType::GAMEPLAY);

  std::string response;
  json update = server.ReceiveMessage(response, true);
  std::cout << "Game Action RESPONSE: " << response << std::endl;

  std::cout << "Getting card movements" << std::endl;
  std::vector<CardMovement> movements = {};
  for (json jsonMovement : update["content"]["movements"]) {
    std::cout << "Pushing movement" << std::endl;
    movements.push_back({
      .cardId = jsonMovement["cardId"],
      .from = jsonMovement["from"],
      .to = jsonMovement["to"]
    });
  }

  UpdateInfo info = {
    .movements = movements,
    .phaseChange = true,
    .phase = update["content"]["phase"],
    .openView = update["content"]["pile"],
    .openViewCards = update["content"]["openViewCards"],
    .selectableCards = update["content"]["selectableCards"]
  };

  info.phaseChange = this->phase.GetMode() != info.phase;
  this->phase.SetMode(info.phase);
  this->phase.SetPlayableCards(info.selectableCards);
  std::cout << "Updated SetPlayableCards" << std::endl;
  PrintVector(std::cout, info.selectableCards);
  this->phase.SetSelectionRange(2, 2);

  return info;
}

UpdateInfo TestGameplayManager::RequestUpdate(GameAction action) {

  if (this->phase.GetMode() == MY_TURN) {
    return this->SendAction({
      { "type", action.type },
      { "selectedCards", action.selectedCards },
      { "from", action.from }
    });
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
  } else {
    if (action.type == FINISH_SELECTION) {
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
      UpdateInfo info = this->SendAction({
        { "type", FINISH_SELECTION },
        { "selectedCards", this->selectedCards },
        { "from", action.from }
      });
      this->selectedCards = {};
      return info;
    } else {
      std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCards[0]);
      if (!res.second) {
        this->selectedCards.erase(action.selectedCards[0]);
      }

      return {
        .movements = {},
        .phaseChange = true,
        .openView = HAND
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
