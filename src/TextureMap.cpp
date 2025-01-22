#include <iostream>
#include "../include/TextureMap.h"
#include "../include/ErrorHandling.h"

TextureMap::TextureMap()
:currentlyBound(0), lru(), ID_Database(nullptr)
{}

const std::string TextureMap::pathPrefix = "../assets/";

void TextureMap::SetupCardDataBase(CardDatabaseSingleton* database) {
  this->ID_Database = database;
}

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
  if (this->ID_Database == nullptr) {
    std::cout << "No card database defined" << id << std::endl;
    exit(EXIT_FAILURE);
  }

  CardInfo* path = this->ID_Database->GetInfo(id);
  if (path != nullptr) {
    this->SetupTexturePath(path->imageName);
  } else {
    std::cout << "Could not find card of id " << id << std::endl;
    exit(EXIT_FAILURE);
  }
}

int TextureMap::RequestBind(unsigned int maxBindableTextures, const std::string& path) {
  auto texture = this->map.find(path);
  if (texture != this->map.end()) {
    // if it's already bound, do nothing
    if (texture->second.GetIsBound()) {
      // update lru list by moving this element to front
      this->lru.Access(&(texture->second));
      return texture->second.GetBoundSlot();
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
    return nextSlot;
  } else {
    std::cout << "Could not find texture of path " << path << std::endl;
    exit(EXIT_FAILURE);
  }
}

int TextureMap::RequestBind(unsigned int maxBindableTextures, unsigned int id) {
  if (this->ID_Database == nullptr) {
    std::cout << "No card database defined" << id << std::endl;
    exit(EXIT_FAILURE);
  }

  CardInfo* path = this->ID_Database->GetInfo(id);
  if (path != nullptr) {
    return TextureMap::RequestBind(maxBindableTextures, path->imageName);
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

int TextureMap::CurrentlyBound() {
  return this->currentlyBound;
}
