#include <iostream>
#include "../include/TextureMap.h"
#include "../include/ErrorHandling.h"

TextureMap::TextureMap()
:currentlyBound(0), lru()
{}

const std::string TextureMap::pathPrefix = "../assets/";
const std::unordered_map<unsigned int, std::string> TextureMap::IDToPath = {
  {0, "card1"},
  {1, "card2"},
  {2, "card3"},
  {3, "card4"},
};

void TextureMap::SetupTexturePath(const std::string& path) {
  auto texture = this->map.find(path);
  if (texture == this->map.end()) {
    this->map.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(path),
      std::forward_as_tuple(TextureMap::pathPrefix + path + ".png"));
  }
}

void TextureMap::SetupBack() {
  this->SetupTexturePath("back");
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

void TextureMap::RequestBind(unsigned int maxBindableTextures, const std::string& path) {
  auto texture = this->map.find(path);
  if (texture != this->map.end()) {
    // if it's already bound, do nothing
    if (texture->second.GetIsBound()) {
      // update lru list by moving this element to front
      this->lru.Access(&(texture->second));
      return;
    }

    // the default is just the currentlyBound because
    // we can just fill linearly at the start
    unsigned int nextSlot = this->currentlyBound;

    if (this->currentlyBound == maxBindableTextures) {
      Texture* tex = this->lru.PopLRU();
      ASSERT(tex->GetIsBound());
      nextSlot = tex->GetBoundSlot();

      tex->Unbind();
      this->currentlyBound--;
    }

    this->lru.Push(&(texture->second));
    texture->second.Bind(nextSlot);
    this->currentlyBound++;
  } else {
    std::cout << "Could not find texture of path " << path << std::endl;
    exit(EXIT_FAILURE);
  }
}

void TextureMap::RequestBind(unsigned int maxBindableTextures, unsigned int id) {
  auto path = this->IDToPath.find(id);
  if (path != this->IDToPath.end()) {
    TextureMap::RequestBind(maxBindableTextures, path->second);
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

std::ostream& operator<<(std::ostream& os, const TextureMap& t) {
  os << "Map: ";
  PrintMapToStream(os, t.map);
  return os;
}

int TextureMap::Size() {
  return this->map.size();
}
