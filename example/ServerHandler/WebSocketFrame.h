#pragma once

#include <random>

class WebSocketFrame {
private:
	static std::array<uint8_t, 4> generateMaskingKey();
public:
	static std::vector<uint8_t> createFrame(const std::string& message);
	static std::string parseFrame(const std::vector<uint8_t>& frame);
};

