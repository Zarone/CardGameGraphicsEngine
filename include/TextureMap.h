#pragma once

#include <unordered_map>

#include "Texture.h"
#include "TextureLRU.h"
#include "CardDatabaseSingleton.h"

class TextureMap
{
private:
  static const std::string pathPrefix;
  //static const std::unordered_map<unsigned int, std::string> IDToPath;
  std::unordered_map<std::string, Texture> map;
  TextureLRU lru;
  unsigned int currentlyBound;
  CardDatabaseSingleton* ID_Database;
public:
  TextureMap();
  void SetupTexturePath(const std::string& path);
  void SetupCardDataBase(CardDatabaseSingleton* database);

  void SetupCard(unsigned int id);
  void SetupBack();

  /*
  *
  * @brief Gets least recently used, currently active texture
  *
  */
  Texture* GetLeastRecentTexture();

  int RequestBind(unsigned int maxMindableTextures, unsigned int id);
  int RequestBind(unsigned int maxMindableTextures, const std::string& path);

  friend std::ostream& operator<<(std::ostream& os, const TextureMap& t);
  int Size();
  int CurrentlyBound();
};
