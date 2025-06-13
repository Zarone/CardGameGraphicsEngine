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

  const std::string MessageTypeSetup = "SETUP_MESSAGE";
  const std::string MessageTypeHeadsOrTailsChoice = "COIN_CHOICE";
  const std::string MessageTypeFirstOrSecondChoice = "FIRST_OR_SECOND_CHOICE";

	int SendSetupMessage(const std::vector<unsigned int>& deck);

public:
	ServerManager() : sock(0) {}

	json ReceiveMessage(std::string& response, bool jsonMessage = false);

	int SendMessage(const std::string& message);

	int SendMessage(const json& message, const std::string& type);

	bool ConnectToServer();

	void Initialize(const std::vector<unsigned int>& deck);

	~ServerManager();
};
