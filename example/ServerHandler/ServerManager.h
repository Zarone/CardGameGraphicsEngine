#pragma once

#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "../external/json.hpp"

using json = nlohmann::json;

class ServerManager {
private:
	int sock;
	const int PORT = 3000;
	const char* HOST = "localhost";
	const char* PATH = "/socket?room=3";

	int SendSetupMessage(const std::vector<unsigned int>& deck);

public:
  static constexpr std::string MessageTypeSetup = "SETUP_MESSAGE";
  static constexpr std::string MessageTypeHeadsOrTailsChoice = "COIN_CHOICE";
  static constexpr std::string MessageTypeFirstOrSecondChoice = "FIRST_OR_SECOND_CHOICE";
  static constexpr std::string MessageTypeGamePlay = "GAMEPLAY";

	ServerManager() : sock(0) {}

	json ReceiveMessage(std::string& response, bool jsonMessage = false);

	int SendMessage(const std::string& message);

	int SendMessage(const json& message, const std::string& type);

	bool ConnectToServer();

  std::vector<unsigned int> Initialize(const std::vector<unsigned int>& deck);

	~ServerManager();
};
