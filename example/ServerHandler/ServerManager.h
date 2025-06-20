#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../external/json.hpp"
#include "../TestUpdateInfo.h"
#include "../external/easywsclient.hpp"
#include "../../src/Card/CardInfo.h"
#include "../../src/Scene/SceneData.h"
#include <deque>
#include <mutex>
#include <condition_variable>

using json = nlohmann::json;

struct SetupData {
  std::vector<unsigned int> correspondingGameIds;
  std::vector<unsigned int> oppCorrespondingGameIds;
  UpdateInfo info;
};

class ServerManager {
private:
	const int PORT = 3000;
	const char* HOST = "localhost";
	const char* PATH = "/socket?room=3";
	std::unique_ptr<easywsclient::WebSocket> ws;
	std::deque<std::string> messageQueue;
	std::mutex queueMutex;
	std::condition_variable queueCondition;

	int SendSetupMessage(const std::vector<unsigned int>& deck, unsigned int messageTypeSetup);

public:
	ServerManager();

	json ReceiveMessage(std::string& response, bool jsonMessage = false, bool nonBlocking = false);

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

	// Message queue accessors
	bool HasPendingMessages();
	std::string PopMessage();
	size_t GetQueueSize();
	void WaitForMessage();
};
