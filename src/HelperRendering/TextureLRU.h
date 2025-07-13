#pragma once

#include <list>
#include <unordered_map>
#include "Texture.h"

class TextureLRU
{
private:
  std::list<Texture*> data;
  std::unordered_map<Texture*, std::list<Texture*>::iterator> itemToIndex;

public:
  TextureLRU() = default;
  ~TextureLRU() = default;
  
  void Access(Texture* tex);
  Texture* PopLRU();
  void Push(Texture* tex);

  void PrintData();
  Texture* GetIndex(unsigned int index);
};
