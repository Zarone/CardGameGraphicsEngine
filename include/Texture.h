#pragma once

#include <string>

class Texture
{
private:
  unsigned int textureID;
  unsigned char* buffer;
  int height;
  int width;
  int bitsPerPixel;
  bool isBound;
  unsigned int boundTo;
public:
  Texture(const std::string& path);
  ~Texture();
  void Bind(unsigned int textureSlot);
  void Unbind();
  bool GetIsBound();
};
