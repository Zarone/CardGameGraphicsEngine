#include "TestGameplayManager.h"
#include "TestGameState.h"
#include "ServerHandler/ServerManager.h"
#include "../src/Helper/ErrorHandling.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

TestGameplayManager::TestGameplayManager() 
: phase(true), shouldStop(false)
{
  if (!server.ConnectToServer()) {
    throw std::runtime_error("Failed to connect to server");
    return;
  }

  std::cout << "Starting Server Initialization" << std::endl;
}

TestGameplayManager::~TestGameplayManager() {
  // Signal the thread to stop
  shouldStop = true;
  queueCondition.notify_all();
  
  // Wait for the thread to finish
  if (messageReceiverThread.joinable()) {
    messageReceiverThread.join();
  }
}

void TestGameplayManager::MessageReceiverLoop() {
  const size_t MAX_QUEUE_SIZE = 100; // Prevent memory issues
  
  while (!shouldStop) {
    try {
      std::string response;
      json update = server.ReceiveMessageNonBlocking(response, true); // Non-blocking receive
      
      if (!response.empty()) {
        std::cout << "Background thread received message: " << response << std::endl;
        
        UpdateInfo info = ProcessReceivedMessage(response);
        
        // Add to queue with size limit
        {
          std::lock_guard<std::mutex> lock(queueMutex);
          if (messageQueue.size() < MAX_QUEUE_SIZE) {
            messageQueue.push_back(info);
          } else {
            std::cerr << "Warning: Message queue full, dropping message" << std::endl;
          }
        }
        
        // Notify waiting threads
        queueCondition.notify_one();
      }
      
      // Small sleep to prevent busy waiting
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      
    } catch (const std::exception& e) {
      std::cerr << "Error in message receiver loop: " << e.what() << std::endl;
      // Continue running unless we're supposed to stop
      if (shouldStop) break;
    }
  }
}

UpdateInfo TestGameplayManager::ProcessReceivedMessage(const std::string& response) {
  json update = json::parse(response);
  
  std::cout << "Getting card movements" << std::endl;
  std::vector<CardMovement> movements = {};
  for (json jsonMovement : update["content"]["movements"]) {
    std::cout << "Pushing movement" << std::endl;
    movements.push_back({
      .gameID = jsonMovement["gameId"],
      .cardID = jsonMovement["cardId"],
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

  // info.phaseChange = this->phase.GetMode() != info.phase;
  this->phase.SetMode(info.phase);
  this->phase.SetPlayableCards(info.selectableCards);
  // std::cout << "Updated SetPlayableCards" << std::endl;
  // PrintVector(std::cout, info.selectableCards);
  this->phase.SetSelectionRange(2, 2);

  return info;
}

void TestGameplayManager::ChangePhaseForUpdate(const UpdateInfo& info) {
  this->phase.SetMode(info.phase);
  this->phase.SetPlayableCards(info.selectableCards);
  this->phase.SetSelectionRange(2, 2);
}

bool TestGameplayManager::HasPendingMessages() {
  std::lock_guard<std::mutex> lock(queueMutex);
  return !messageQueue.empty();
}

std::vector<UpdateInfo> TestGameplayManager::GetAllMessages() {
  std::lock_guard<std::mutex> lock(queueMutex);
  std::vector<UpdateInfo> messages = messageQueue;
  messageQueue.clear();
  return messages;
}

size_t TestGameplayManager::GetQueueSize() {
  std::lock_guard<std::mutex> lock(queueMutex);
  return messageQueue.size();
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
  
  std::cout << "Starting receiver thread" << std::endl;
  // Start the message receiver thread
  messageReceiverThread = std::thread(&TestGameplayManager::MessageReceiverLoop, this);

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

  return ProcessReceivedMessage(response);
}

UpdateInfo TestGameplayManager::RequestUpdate(GameAction action) {
  if (this->phase.GetMode() == MY_TURN) {
    UpdateInfo info = this->SendAction({
      { "type", action.type },
      { "selectedCards", action.selectedCards },
      { "from", action.from }
    });
    this->ChangePhaseForUpdate(info);
    return info;
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
      this->ChangePhaseForUpdate(info);
      return info;
    } else {
      std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCards[0]);
      if (!res.second) {
        this->selectedCards.erase(action.selectedCards[0]);
      }

      return {
        .movements = {},
        .phaseChange = true,
        .openView = HAND,
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
