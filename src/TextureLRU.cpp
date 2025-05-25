#include <iostream>
#include "ErrorHandling.h"
#include "TextureLRU.h"

TextureLRU::TextureLRU() {
  this->data = std::list<Texture*>();
  this->itemToIndex = std::unordered_map<Texture*, std::list<Texture*>::iterator>();
}

void TextureLRU::Access(Texture* tex) {
  // get index in lru (use hashmap)
  auto indexIter = this->itemToIndex.find(tex);
  if (indexIter == this->itemToIndex.end()) {
    std::cout << "Couldn't access texture" << std::endl;
    exit(EXIT_FAILURE);
  }

  Texture* currentFront = *this->data.begin();
  this->data.splice(this->data.begin(), this->data, indexIter->second);
  itemToIndex[tex] = this->data.begin();
  itemToIndex[currentFront] = ++this->data.begin();
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
