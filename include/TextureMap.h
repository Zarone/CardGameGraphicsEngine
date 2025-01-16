#pragma once

#include "Texture.h"
#include <unordered_map>

class TextureMap
{
private:
  static const std::string pathPrefix;
  static const std::unordered_map<unsigned int, std::string> IDToMap;
  std::unordered_map<std::string, Texture> map;
  void Setup(const std::string& path);
public:
  TextureMap();
  void SetupCard(unsigned int id);
  void SetupBack();
};
