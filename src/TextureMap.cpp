#include <iostream>
#include "../include/TextureMap.h"

TextureMap::TextureMap()
:currentlyBound(0), nextFreeIndex(0)
{}

const std::string TextureMap::pathPrefix = "../assets/";
const std::unordered_map<unsigned int, std::string> TextureMap::IDToPath = {
  {0, "card1"},
  {1, "card2"},
  {2, "card3"},
};

template<typename K, typename V>
void print_map(std::unordered_map<K, V> const &m)
{
  std::cout << "printing map:" << std::endl;
  for (auto const &pair: m) {
    std::cout << "{" << pair.first << ": " << pair.second << "}\n";
  }
}

void TextureMap::SetupTexturePath(const std::string& path) {
  auto texture = this->map.find(path);
  if (texture == this->map.end()) {
    this->map.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(path),
      std::forward_as_tuple(TextureMap::pathPrefix + path + ".png"));
  }

  print_map(this->map);

}

void TextureMap::SetupBack() {
  this->SetupTexturePath(TextureMap::pathPrefix + "back.png");
}

void TextureMap::SetupCard(unsigned int id) {
  auto path = this->IDToPath.find(id);
  if (path != this->IDToPath.end()) {
    this->SetupTexturePath(path->second);
  } else {
    std::cout << "Could not find card of id " << id << std::endl;
    exit(EXIT_FAILURE);
  }
}

void TextureMap::ResetIndexing() {
  this->nextFreeIndex = 0;
  this->currentlyBound = 0;
}

int TextureMap::GetUnusedTextureSlot() {
  std::cout << "UNIMPLEMENTED FreeUnusedTexture" << std::endl;
  exit(EXIT_FAILURE);
}

void TextureMap::RequestBind(unsigned int maxBindableTextures, unsigned int id) {
  unsigned int nextSlot = nextFreeIndex;
  if (this->currentlyBound == maxBindableTextures) {
    this->nextFreeIndex = this->GetUnusedTextureSlot(); 
  }

  auto path = this->IDToPath.find(id);
  if (path != this->IDToPath.end()) {
    auto texture = this->map.find(path->second);
    if (texture != this->map.end()) {
      texture->second.Bind(nextSlot);
    } else {
      std::cout << "Could not find texture of path " << path->second << std::endl;
      exit(EXIT_FAILURE);
    }
  } else {
    std::cout << "Could not find card of id " << id << std::endl;
    exit(EXIT_FAILURE);
  }
}

int TextureMap::GetSlotOf(unsigned int id) {
  auto path = this->IDToPath.find(id);
  if (path != this->IDToPath.end()) {
    return this->map.at(path->second).GetBoundSlot();
  } else {
    std::cout << "Could not find card of id " << id << std::endl;
    exit(EXIT_FAILURE);
  }
}
