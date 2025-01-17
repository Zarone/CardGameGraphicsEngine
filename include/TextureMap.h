#pragma once

#include "Texture.h"
#include <unordered_map>

class TextureMap
{
private:
  static const std::string pathPrefix;
  static const std::unordered_map<unsigned int, std::string> IDToPath;
  std::unordered_map<std::string, Texture> map;
  unsigned int currentlyBound;
  unsigned int nextFreeIndex;
  void SetupTexturePath(const std::string& path);
  int GetUnusedTextureSlot();
public:
  TextureMap();
  void SetupCard(unsigned int id);
  void SetupBack();
  void RequestBind(unsigned int maxMindableTextures, unsigned int id);
  void ResetIndexing();
  int GetSlotOf(unsigned int id);
  friend std::ostream& operator<<(std::ostream& os, const TextureMap& t);
};
