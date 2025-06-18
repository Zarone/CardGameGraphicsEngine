#pragma once

#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include "../external/json.hpp"
#include "../TestUpdateInfo.h"

using json = nlohmann::json;

struct SetupData {
  std::vector<unsigned int> correspondingGameIds;
  std::vector<unsigned int> oppCorrespondingGameIds;
  UpdateInfo info;
};

class ServerManager {
private:
	int sock;
	const int PORT = 3000;
	const char* HOST = "localhost";
	const char* PATH = "/socket?room=3";

	int SendSetupMessage(const std::vector<unsigned int>& deck, unsigned int messageTypeSetup);

public:
	ServerManager() : sock(0) {}

	json ReceiveMessage(std::string& response, bool jsonMessage = false);
	json ReceiveMessageNonBlocking(std::string& response, bool jsonMessage = false);

	int SendMessage(const std::string& message);

	int SendMessage(const json& message, unsigned int type);

	bool ConnectToServer();

  SetupData Initialize(
    const std::vector<unsigned int>& deck,
    unsigned int messageTypeSetup,
    unsigned int headsOrTailsChoice, 
    unsigned int firstOrSecondChoice
  );

	~ServerManager();
};
