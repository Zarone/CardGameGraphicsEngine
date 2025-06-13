#include "WebSocketFrame.h"

std::array<uint8_t, 4> WebSocketFrame::generateMaskingKey() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);
  std::array<uint8_t, 4> key;
  for (int i = 0; i < 4; i++) {
    key[i] = static_cast<uint8_t>(dis(gen));
  }
  return key;
}

std::vector<uint8_t> WebSocketFrame::createFrame(const std::string& message) {
  std::vector<uint8_t> frame;
  
  // First byte: FIN bit set (1), opcode for text frame (0x1)
  frame.push_back(0x81);
  
  // Second byte: MASK bit set (1), payload length
  size_t payloadLength = message.length();
  if (payloadLength <= 125) {
    frame.push_back(0x80 | payloadLength);
  } else if (payloadLength <= 65535) {
    frame.push_back(0x80 | 126);
    frame.push_back((payloadLength >> 8) & 0xFF);
    frame.push_back(payloadLength & 0xFF);
  } else {
    frame.push_back(0x80 | 127);
    for (int i = 7; i >= 0; i--) {
      frame.push_back((payloadLength >> (i * 8)) & 0xFF);
    }
  }
  
  // Generate and add masking key
  auto maskingKey = generateMaskingKey();
  for (uint8_t key : maskingKey) {
    frame.push_back(key);
  }
  
  // Add masked payload
  for (size_t i = 0; i < message.length(); i++) {
    frame.push_back(message[i] ^ maskingKey[i % 4]);
  }
  
  return frame;
}

std::string WebSocketFrame::parseFrame(const std::vector<uint8_t>& frame) {
  if (frame.size() < 2) {
    return "Invalid frame: too short";
  }

  // Get first byte (FIN, RSV1-3, opcode)
  uint8_t firstByte = frame[0];
  bool fin = (firstByte & 0x80) != 0;
  uint8_t opcode = firstByte & 0x0F;

  // Get second byte (MASK, payload length)
  uint8_t secondByte = frame[1];
  bool masked = (secondByte & 0x80) != 0;
  uint8_t payloadLength = secondByte & 0x7F;

  // Calculate header size and get extended length if needed
  size_t headerSize = 2;
  size_t actualLength = payloadLength;
  
  if (payloadLength == 126) {
    if (frame.size() < 4) return "Invalid frame: missing extended length";
    actualLength = (frame[2] << 8) | frame[3];
    headerSize = 4;
  } else if (payloadLength == 127) {
    if (frame.size() < 10) return "Invalid frame: missing extended length";
    actualLength = 0;
    for (int i = 0; i < 8; i++) {
      actualLength = (actualLength << 8) | frame[2 + i];
    }
    headerSize = 10;
  }

  // Get masking key if present
  std::array<uint8_t, 4> maskingKey;
  if (masked) {
    if (frame.size() < headerSize + 4) return "Invalid frame: missing masking key";
    for (int i = 0; i < 4; i++) {
      maskingKey[i] = frame[headerSize + i];
    }
    headerSize += 4;
  }

  // Check if we have enough data
  if (frame.size() < headerSize + actualLength) {
    return "Invalid frame: incomplete payload";
  }

  // Handle different opcodes
  switch (opcode) {
    case 0x1: // Text frame
    case 0x2: // Binary frame
      break;
    case 0x8: // Close frame
      return "Received close frame";
    case 0x9: // Ping frame
      return "Received ping frame";
    case 0xA: // Pong frame
      return "Received pong frame";
    default:
      return "Unknown opcode: " + std::to_string(opcode);
  }

  // Extract and unmask payload
  std::string payload;
  payload.reserve(actualLength);
  for (size_t i = 0; i < actualLength; i++) {
    uint8_t byte = frame[headerSize + i];
    if (masked) {
      byte ^= maskingKey[i % 4];
    }
    payload.push_back(byte);
  }

  return payload;
}

