#include "TestGameplayManager.h"
#include "TestGameMode.h"
#include "TestGameState.h"
#include "ServerHandler/ServerManager.h"
#include "../src/Helper/ErrorHandling.h"
#include "../src/pch.h"

TestGameplayManager::TestGameplayManager() 
: phase(true), shouldStop(false)
{
  if (!server.ConnectToServer()) {
    throw std::runtime_error("Failed to connect to server!");
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

const size_t MAX_QUEUE_SIZE = 100; // Prevent memory issues
void TestGameplayManager::MessageReceiverLoop() {
  while (!shouldStop) {
    try {
      std::string response;
      json update = server.ReceiveMessage(response, true, true); // Non-blocking receive
      
      if (!response.empty()) {
        std::cout << "Background thread received message: " << response << std::endl;
        
        UpdateInfo info = ProcessReceivedMessage(response);
        
        AddToQueue(info);
        
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

void TestGameplayManager::AddToQueue(const UpdateInfo& info) {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (messageQueue.size() < MAX_QUEUE_SIZE) {
    messageQueue.push_back(info);
  } else {
    std::cerr << "Warning: Message queue full, dropping message" << std::endl;
  }
}

UpdateInfo TestGameplayManager::ProcessReceivedMessage(const std::string& response) {
  json update = json::parse(response);
  
  std::cout << "Getting card movements" << std::endl;
  std::vector<CardMovement> movements = {};
  for (json jsonMovement : update["content"]["movements"]) {
    std::cout << "Pushing movement" << std::endl;
    movements.push_back({
      .cardID = jsonMovement["cardId"],
      .gameID = jsonMovement["gameId"],
      .from = Pile(jsonMovement["from"]),
      .to = Pile(jsonMovement["to"])
    });
  }

  UpdateInfo info = {
    .movements = movements,
    .phase = update["content"]["phase"],
    .openView = Pile(update["content"]["pile"]),
    .openViewCards = update["content"]["openViewCards"],
    .selectableCards = update["content"]["selectableCards"],
    .selectedCardsChanged = false,
    .selectableCardsChanged = true,
  };

  if (update["content"]["count"].contains("atMost")) {
    info.selectionMax = update["content"]["count"]["atMost"];
  }
  if (update["content"]["count"].contains("atLeast")) {
    info.selectionMin = update["content"]["count"]["atLeast"];
  }

  return info;
}

void TestGameplayManager::ChangePhaseForUpdate(const UpdateInfo& info) {
  this->phase.SetMode(info.phase);
  if (info.selectableCardsChanged) {
    this->phase.SetPlayableCards(info.selectableCards);
    this->phase.SetSelectionRange(info.selectionMin, info.selectionMax);
  }
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

SetupData TestGameplayManager::Setup(const std::vector<unsigned int>& deck, std::atomic<bool>* stopFlag) {
  std::cout << "inside Setup" << std::endl;
  SetupData setupData = server.Initialize(
    deck,
    MessageType::SETUP,
    MessageType::COIN_CHOICE,
    MessageType::FIRST_OR_SECOND_CHOICE,
    stopFlag
  );

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

void TestGameplayManager::SendAction(json action) {
  server.SendMessage(action, MessageType::GAMEPLAY);
}

void TestGameplayManager::PostAction(GameAction action) {
  if (this->phase.GetMode() == MY_TURN) {
    this->SendAction({
      { "type", action.type },
      { "selectedCards", action.selectedCards },
      { "from", action.from.GetInString() }
    });
  } else if (this->phase.GetMode() == SELECTING_CARDS) {
    if (action.type == FINISH_SELECTION) {
      this->SendAction({
        { "type", FINISH_SELECTION },
        { "selectedCards", this->selectedCards },
        { "from", action.from.GetInString() }
      });
      this->selectedCards = {};
    } else {
      std::pair<std::set<unsigned int>::iterator, bool> res = this->selectedCards.insert(action.selectedCards[0]);
      if (!res.second) {
        this->selectedCards.erase(action.selectedCards[0]);
      }
      this->AddToQueue({
        .movements = {},
        .phase = SELECTING_CARDS,
        .openView = Pile("HAND"),
        .selectableCards = {},
        .selectedCardsChanged = true,
        .selectableCardsChanged = false
      });
    }
  }
}

bool TestGameplayManager::SelectionPossiblyDone() {
  return this->phase.InSelectionRange(this->selectedCards.size());
}

GameMode TestGameplayManager::GetPhase() {
  return this->phase.GetMode();
}
