#include <iostream>
#include "../Helper/ErrorHandling.h"
#include "TextureLRU.h"

TextureLRU::TextureLRU() {
  this->data = std::list<Texture*>();
  this->itemToIndex = std::unordered_map<Texture*, std::list<Texture*>::iterator>();
}

void TextureLRU::Access(Texture* tex) {
  // get index in lru (use hashmap)
  auto indexIter = this->itemToIndex.find(tex);
  if (indexIter == this->itemToIndex.end()) {
    std::cout << "Couldn't access texture " << *tex << std::endl;
    exit(EXIT_FAILURE);
  }

  // If the texture is already at the front, no need to move it
  if (indexIter->second == this->data.begin()) {
    return;
  }

  // Store the iterator to the element being moved
  auto movedIter = indexIter->second;
  
  // Perform the splice operation
  this->data.splice(this->data.begin(), this->data, movedIter);
  
  // Update all iterators in the map
  auto it = this->data.begin();
  while (it != this->data.end()) {
    itemToIndex[*it] = it;
    ++it;
  }
}

Texture* TextureLRU::PopLRU() {
  Texture* tex = this->data.back();
  this->data.pop_back();
  itemToIndex.erase(tex);
  return tex;
}

void TextureLRU::Push(Texture* tex) {
  this->data.push_front(tex);
  this->itemToIndex[tex] = this->data.begin();
}

void TextureLRU::PrintData() {
  PrintVector<Texture*>(std::cout, this->data);
  PrintMapToStream(std::cout, this->itemToIndex);
}

Texture* TextureLRU::GetIndex(unsigned int index) {
  std::list<Texture*>::iterator it = this->data.begin();
  for (unsigned int i = 0; i < index; i++) {
    it++;
  }
  return *it;
}
