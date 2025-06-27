#include "ServerManager.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>

using easywsclient::WebSocket;

int ServerManager::SendSetupMessage(const std::vector<unsigned int>& deck, unsigned int messageTypeSetup) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", messageTypeSetup},
    {
      "content", {
        { "deck", deck }
      }
    },
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  if (ws) {
    ws->send(jsonString);
  }
  return 0;
}

json ServerManager::ReceiveMessage(std::string& response, bool jsonMessage, bool nonBlocking) {
  if (!this->HasPendingMessages()) {
    if (!ws) {
      std::cout << "No websocket for some reason" << std::endl;
      return json();
    }

    if (nonBlocking) {
      ws->poll(0); // non-blocking
      ws->dispatch([&](const std::string &message) {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push_back(message);
        queueCondition.notify_one();
      });
    } else {
      while (!this->HasPendingMessages() && ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
        std::cout << "Started blocking for message" << std::endl;
        ws->poll(-1); // Block until a message is received
        std::cout << "Finished blocking for message" << std::endl;
        ws->dispatch([&](const std::string &message) {
          std::lock_guard<std::mutex> lock(queueMutex);
          messageQueue.push_back(message);
          queueCondition.notify_one();
          std::cout << "Receive Message Direct: " << message << std::endl;
        });
        if (!this->HasPendingMessages() && ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
          std::cout << "Poll returned without a full message. Polling again." << std::endl;
        }
      }
      std::cout << "Finished dispatch" << std::endl;
    }
  }

  response = this->PopMessage();

  if (response.empty()) {
    return json();
  }

  if (jsonMessage) {
    try {
      json jsonResponse = json::parse(response);
      std::cout << "Server response (JSON): " << jsonResponse.dump(2) << std::endl;
      return jsonResponse;
    } catch (const json::parse_error& e) {
      std::cout << "Server response (raw): " << response << std::endl;
    }
  } else {
    std::cout << "Server response: " << response << std::endl;
  }

  return json();
}

int ServerManager::SendMessage(const std::string& message) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", "Message"},
    {"content", message},
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  if (ws) {
    ws->send(jsonString);
  }
  return 0;
}

int ServerManager::SendMessage(const json& message, unsigned int type) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", type},
    {"content", message},
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  if (ws) {
    ws->send(jsonString);
  }
  return 0;
}

bool ServerManager::ConnectToServer() {
  std::string url = "ws://" + std::string(HOST) + ":" + std::to_string(PORT) + std::string(PATH);
  ws.reset(WebSocket::from_url(url));
  return ws != nullptr;
}

// Helper for non-blocking receive with stop flag
static json WaitForMessageOrStop(std::function<json()> receiveFunc, std::atomic<bool>* stopFlag) {
  while (true) {
    if (stopFlag && *stopFlag) return {};
    json result = receiveFunc();
    if (!result.is_null()) return result;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

SetupData ServerManager::Initialize(
  const std::vector<unsigned int>& deck, 
  unsigned int messageTypeSetup,
  unsigned int headsOrTailsChoice, 
  unsigned int firstOrSecondChoice,
  std::atomic<bool>* stopFlag
) {
  std::string _;
  std::cout << "before hi client message" << std::endl;
  // Non-blocking receive loop for 'Hi Client!' message
  while (!this->HasPendingMessages()) {
    if (stopFlag && *stopFlag) return {};
    ws->poll(10); // 10ms timeout
    ws->dispatch([&](const std::string &message) {
      std::lock_guard<std::mutex> lock(queueMutex);
      messageQueue.push_back(message);
      queueCondition.notify_one();
    });
  }
  ReceiveMessage(_, false, true); // Non-blocking
  if (stopFlag && *stopFlag) return {};

  std::cout << "before send setup message" << std::endl;
  SendSetupMessage(deck, messageTypeSetup);

  // Wait for setup response
  json setupResponse = WaitForMessageOrStop([&]() { return ReceiveMessage(_, true, true); }, stopFlag);
  if (stopFlag && *stopFlag) return {};

  // Wait for coin flip or wait response
  json response = WaitForMessageOrStop([&]() { return ReceiveMessage(_, true, true); }, stopFlag);
  if (stopFlag && *stopFlag) return {};
  std::cout << "response.content.isChoosingFlip: " << response["content"]["isChoosingFlip"] << std::endl;

  bool isChoosingFlip = response["content"]["isChoosingFlip"];

  if (isChoosingFlip) {
    // send heads or tails
    SendMessage({
      {"heads", true}
    }, headsOrTailsChoice);
    // Wait for response after selecting
    response = WaitForMessageOrStop([&]() { return ReceiveMessage(_, true, true); }, stopFlag);
    if (stopFlag && *stopFlag) return {};
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, firstOrSecondChoice);
    }
  } else {
    // wait
    response = WaitForMessageOrStop([&]() { return ReceiveMessage(_, true, true); }, stopFlag);
    if (stopFlag && *stopFlag) return {};
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, firstOrSecondChoice);
    }
  }

  // Wait for final response after turn order selected by server
  response = WaitForMessageOrStop([&]() { return ReceiveMessage(_, true, true); }, stopFlag);
  if (stopFlag && *stopFlag) return {};

  std::vector<CardMovement> movements = {};
  for (json jsonCardMovement : response["content"]["movements"]) {
    movements.push_back({
      .gameID = jsonCardMovement["gameId"],
      .from = Pile(jsonCardMovement["from"]),
      .to = Pile(jsonCardMovement["to"])
    });
  }

  return {
    .correspondingGameIds = setupResponse["content"]["myDeck"],
    .oppCorrespondingGameIds = setupResponse["content"]["oppDeck"],
    .info = {
      .movements = movements,
      .phase = response["content"]["phase"],
      .openView = Pile(response["content"]["pile"]),
      .openViewCards = response["content"]["openViewCards"],
      .selectableCards = response["content"]["selectableCards"],
    }
  };
}

ServerManager::ServerManager() : ws(nullptr) {}

ServerManager::~ServerManager() {
  std::cout << "Close Server" << std::endl;
  if (ws) {
    ws->close();
  }
}

// Message queue accessors
bool ServerManager::HasPendingMessages() {
  std::lock_guard<std::mutex> lock(queueMutex);
  return !messageQueue.empty();
}

std::string ServerManager::PopMessage() {
  std::unique_lock<std::mutex> lock(queueMutex);
  if (messageQueue.empty()) return std::string();
  std::string msg = messageQueue.front();
  messageQueue.pop_front();
  return msg;
}

size_t ServerManager::GetQueueSize() {
  std::lock_guard<std::mutex> lock(queueMutex);
  return messageQueue.size();
}

void ServerManager::WaitForMessage() {
  std::unique_lock<std::mutex> lock(queueMutex);
  queueCondition.wait(lock, [&]{ return !messageQueue.empty(); });
}
