#include <iostream>
#include <errno.h>
#include "ServerManager.h"
#include "WebSocketFrame.h"
#include <fcntl.h>

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
  std::cout << "before read" << std::endl;
  int valread = read(sock, frameBuffer.data(), frameBuffer.size());
  std::cout << "after read" << std::endl;
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
      return jsonResponse;
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

int ServerManager::SendMessage(const json& message, unsigned int type) {
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

  std::cout << "Finished connecting to server" << std::endl;

  // Setup web socket instead of http
  std::string _;
  ReceiveMessage(_); // Initializer message
  
  return true;
}

SetupData ServerManager::Initialize(
  const std::vector<unsigned int>& deck, 
  unsigned int messageTypeSetup,
  unsigned int headsOrTailsChoice, 
  unsigned int firstOrSecondChoice
) {
  std::string _;
  std::cout << "before hi client message" << std::endl;
  ReceiveMessage(_); // 'Hi Client!' Message
  std::cout << "before send setup message" << std::endl;
  SendSetupMessage(deck, messageTypeSetup);
  json setupResponse = ReceiveMessage(_, true); // Response to Setup Message

  json response = ReceiveMessage(_, true); // Receive either coin flip or wait
  std::cout << "response.content.isChoosingFlip: " << response["content"]["isChoosingFlip"] << std::endl;

  bool isChoosingFlip = response["content"]["isChoosingFlip"];

  if (isChoosingFlip) {
    // send heads or tails
    SendMessage({
      {"heads", true}
    }, headsOrTailsChoice);
    json response = ReceiveMessage(_, true); // after selecting
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, firstOrSecondChoice);
    }
  } else {
    // wait
    json response = ReceiveMessage(_, true); // after selecting
    std::cout << "isChoosingTurnOrder: " << response["content"] << std::endl;
    bool isChoosingTurnOrder = response["content"]["isChoosingTurnOrder"];
    if (isChoosingTurnOrder) {
      SendMessage({
        {"first", true}
      }, firstOrSecondChoice);
    }
  }

  response = ReceiveMessage(_, true); // after turn order selected by server
  std::cout << "Is Going First: " << response["content"]["myTurn"] << std::endl;

  std::vector<CardMovement> movements = {};
  for (json jsonCardMovement : response["content"]["movements"]) {
    movements.push_back({
      .gameID = jsonCardMovement["gameId"],
      .from = jsonCardMovement["from"],
      .to = jsonCardMovement["to"]
    });
  }

  return {
    .correspondingGameIds = setupResponse["content"]["myDeck"],
    .oppCorrespondingGameIds = setupResponse["content"]["oppDeck"],
    .info = {
      .movements = movements,
      .phase = response["content"]["phase"],
      .openView = response["content"]["pile"],
      .openViewCards = response["content"]["openViewCards"],
      .selectableCards = response["content"]["selectableCards"],
    }
  };
}

json ServerManager::ReceiveMessageNonBlocking(std::string& response, bool jsonMessage) {
  // Set socket to non-blocking mode
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  
  std::vector<uint8_t> frameBuffer;
  frameBuffer.resize(1024);
  
  int valread = read(sock, frameBuffer.data(), frameBuffer.size());
  
  // Restore blocking mode
  fcntl(sock, F_SETFL, flags);
  
  if (valread < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No data available
      response = "";
      return json();
    }
    std::cerr << "Failed to read response" << std::endl;
    return json();
  }
  
  if (valread == 0 || valread > 1024) {
    std::cerr << "Invalid read size: " << valread << std::endl;
    return json();
  }

  frameBuffer.resize(valread);
  response = WebSocketFrame::parseFrame(frameBuffer);
  
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

ServerManager::~ServerManager() {
  std::cout << "Close Server" << std::endl;
  if (sock > 0) {
    close(sock);
  }
}
