#include <iostream>
#include <stdexcept>
#include "../Helper/ErrorHandling.h"
#include "TextureLRU.h"

void TextureLRU::Access(Texture* tex) {
  if (!tex) {
    throw std::invalid_argument("Cannot access null texture");
  }

  auto it = itemToIndex.find(tex);
  if (it == itemToIndex.end()) {
    throw std::runtime_error("Texture not found in LRU cache");
  }

  // If already at front, no need to move
  if (it->second == data.begin()) {
    return;
  }

  // Move to front using splice (O(1) operation)
  data.splice(data.begin(), data, it->second);
  
  // Update the iterator in the map
  itemToIndex[tex] = data.begin();
}

Texture* TextureLRU::PopLRU() {
  if (data.empty()) {
    return nullptr;
  }

  Texture* tex = data.back();
  data.pop_back();
  itemToIndex.erase(tex);
  return tex;
}

void TextureLRU::Push(Texture* tex) {
  if (!tex) {
    throw std::invalid_argument("Cannot push null texture");
  }

  // Check if already exists
  if (itemToIndex.find(tex) != itemToIndex.end()) {
    throw std::runtime_error("Texture already exists in LRU cache");
  }

  data.push_front(tex);
  itemToIndex[tex] = data.begin();
}

void TextureLRU::PrintData() {
  std::cout << "LRU Cache contents:" << std::endl;
  std::cout << "List: ";
  for (const auto& tex : data) {
    std::cout << tex << " ";
  }
  std::cout << std::endl;
  
  std::cout << "Map size: " << itemToIndex.size() << std::endl;
}

Texture* TextureLRU::GetIndex(unsigned int index) {
  if (index >= data.size()) {
    return nullptr;
  }

  auto it = data.begin();
  std::advance(it, index);
  return *it;
}
