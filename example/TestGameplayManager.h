#pragma once

#include <set>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "TestGameplayPhase.h"
#include "TestGameplayPiles.h"
#include "ServerHandler/ServerManager.h"
#include "TestUpdateInfo.h"

enum GameActionType {
  END_TURN,
  SELECT_CARD,
  FINISH_SELECTION
};

enum MessageType {
  SETUP,
  HEADS_OR_TAILS,
  COIN_CHOICE,
  FIRST_OR_SECOND,
  FIRST_OR_SECOND_CHOICE,
  GAMEPLAY,
};

struct GameAction {
  GameActionType type;
  std::vector<unsigned int> selectedCards;
  Pile from;
};

class TestGameplayManager {
private:
  std::set<unsigned int> selectedCards;
  TestGameplayPhase phase;
  ServerManager server;
  
  // Message queue components
  std::vector<UpdateInfo> messageQueue;
  std::mutex queueMutex;
  std::condition_variable queueCondition;
  std::thread messageReceiverThread;
  std::atomic<bool> shouldStop;
  
  UpdateInfo SendAction(json action);
  void MessageReceiverLoop();
  UpdateInfo ProcessReceivedMessage(const std::string& response);


public:
  TestGameplayManager();
  ~TestGameplayManager();
  SetupData Setup(const std::vector<unsigned int>& deck);
  bool IsPlayableCard(unsigned int id);
  bool IsSelectedCard(unsigned int id);
  UpdateInfo RequestUpdate(GameAction action);
  bool SelectionPossiblyDone();
  GameMode GetPhase();
  void ChangePhaseForUpdate(const UpdateInfo &info);
  
  // Message queue access methods
  bool HasPendingMessages();
  std::vector<UpdateInfo> GetAllMessages();
  size_t GetQueueSize();
  
  std::unordered_map<unsigned int, unsigned int> gameIDToID;
};
