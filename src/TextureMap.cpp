#include <iostream>
#include "../include/TextureMap.h"

TextureMap::TextureMap(){}

const std::string pathPrefix = "../assets/";
const std::unordered_map<unsigned int, std::string> TextureMap::IDToMap = {
  {0, "card1"},
  {1, "card2"},
  {2, "card3"},
};

void TextureMap::Setup(const std::string& path) {
  // if there's no such texture in map
  if (this->map.count(path) == 0) {
    this->map[path] = Texture(path);
  }
}

void TextureMap::SetupCard(unsigned int id) {
  if (this->IDToMap.count(id) == 0) {
    std::cout << "Could not find card of id " << id << std::endl;
    exit(EXIT_FAILURE);
  } else {
    this->Setup(this->pathPrefix + this->IDToMap.at(id));
  }
}
