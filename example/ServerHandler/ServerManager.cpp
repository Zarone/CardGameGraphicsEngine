#include <iostream>
#include "ServerManager.h"
#include "WebSocketFrame.h"

int ServerManager::SendSetupMessage(const std::vector<unsigned int>& deck) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", MessageTypeSetup},
    {
      "content", {
        { "deck", deck }
      }
    },
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  auto frame = WebSocketFrame::createFrame(jsonString);
  if (send(sock, frame.data(), frame.size(), 0) < 0) {
    std::cerr << "Failed to send message" << std::endl;
    return -1;
  }
  return 0;
}

json ServerManager::ReceiveMessage(std::string& response, bool jsonMessage) {
  std::vector<uint8_t> frameBuffer;
  frameBuffer.resize(1024);
  int valread = read(sock, frameBuffer.data(), frameBuffer.size());
  if (valread < 0 || valread > 1024) {
    std::cerr << "Failed to read response" << std::endl;
    return -1;
  }

  frameBuffer.resize(valread);
  response = WebSocketFrame::parseFrame(frameBuffer);
  if (jsonMessage) {
    try {
      json jsonResponse = json::parse(response);
      std::cout << "Server response (JSON): " << jsonResponse.dump(2) << std::endl;
      return std::move(jsonResponse);
    } catch (const json::parse_error& e) {
      std::cout << "Server response (raw): " << response << std::endl;
    }
  } else {
    std::cout << "Server response: " << response << std::endl;
  }

  return 0;
}

int ServerManager::SendMessage(const std::string& message) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", "Message"},
    {"content", message},
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  auto frame = WebSocketFrame::createFrame(jsonString);
  if (send(sock, frame.data(), frame.size(), 0) < 0) {
    std::cerr << "Failed to send message" << std::endl;
    return -1;
  }
  return 0;
}

int ServerManager::SendMessage(const json& message, const std::string& type) {
  time_t currentTime = std::time(nullptr);

  json jsonMessage = {
    {"type", type},
    {"content", message},
    {"timestamp", asctime(localtime(&currentTime))}
  };

  std::string jsonString = jsonMessage.dump();
  auto frame = WebSocketFrame::createFrame(jsonString);
  if (send(sock, frame.data(), frame.size(), 0) < 0) {
    std::cerr << "Failed to send message" << std::endl;
    return -1;
  }
  return 0;
}

bool ServerManager::ConnectToServer() {
  struct sockaddr_in serv_addr;
    
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return false;
  }

  struct hostent* host = gethostbyname(HOST);
  if (host == nullptr) {
    std::cerr << "Failed to get host by name" << std::endl;
    return false;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  memcpy(&serv_addr.sin_addr.s_addr, host->h_addr, host->h_length);

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection failed" << std::endl;
    return false;
  }

  std::cout << "Connected to server at " << HOST << ":" << PORT << std::endl;

  std::string request = "GET " + std::string(PATH) + " HTTP/1.1\r\n"
                        "Host: " + std::string(HOST) + ":" + std::to_string(PORT) + "\r\n"
                        "Connection: Upgrade\r\n"
                        "Upgrade: websocket\r\n"
                        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                        "Sec-WebSocket-Version: 13\r\n\r\n";

  if (send(sock, request.c_str(), request.length(), 0) < 0) {
    std::cerr << "Failed to send request" << std::endl;
    return false;
  }

  return true;
}


std::vector<unsigned int> ServerManager::Initialize(const std::vector<unsigned int>& deck) {
  std::string _;
  ReceiveMessage(_); // Initializer message
  ReceiveMessage(_); // 'Hi Client!' Message
  SendSetupMessage(deck);
  json setupResponse = ReceiveMessage(_, true); // Response to Setup Message

  json response = ReceiveMessage(_, true); // Receive either coin flip or wait
  std::cout << "response.content.isChoosingFlip: " << response["content"]["isChoosingFlip"] << std::endl;

  bool isChoosingFlip = response["content"]["isChoosingFlip"];

  if (isChoosingFlip) {
    // send heads or tails
    SendMessage({
      {"heads", true}
    }, MessageTypeHeadsOrTailsChoice);
    json response = ReceiveMessage(_, true); // after selecting
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, MessageTypeFirstOrSecondChoice);
    }
  } else {
    // wait
    json response = ReceiveMessage(_, true); // after selecting
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, MessageTypeFirstOrSecondChoice);
    }
  }

  response = ReceiveMessage(_, true); // after turn order selected by server
  std::cout << "Is Going First: " << response["content"]["myTurn"] << std::endl;

  return setupResponse;
}

ServerManager::~ServerManager() {
  if (sock > 0) {
    close(sock);
  }
}
