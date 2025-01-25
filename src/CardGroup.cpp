#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/CardGroup.h"

const unsigned int CardGroup::estimatedMax = 60;

CardGroup::CardGroup(
  bool zFlipped
) 
: 
zFlipped(zFlipped)
{
}

void CardGroup::PrepareTextures(TextureMap* textureMap) {
  int size = this->cards.size();

  if (zFlipped) {
    textureMap->SetupBack();
  } else {
    for (CardItem& cardItem : this->cards) {
      textureMap->SetupCard(cardItem.card.GetID());
    }
  }
}

void CardGroup::AddCard(unsigned int id) {
  this->cards.push_back({
    .card = Card(id),
    .renderData = CardRenderingData()
  });
}

Card CardGroup::GetCard(unsigned int index) {
  return this->cards[index].card;
}

std::vector<CardItem>* CardGroup::GetCards() {
  return &this->cards;
}
