#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/CardGroup.h"

const unsigned int CardGroup::estimatedMax = 60;

CardGroup::CardGroup(
  bool zFlipped
) 
: 
dirtyDisplay(true), 
dirtyPosition(true), 
zFlipped(zFlipped)
{
}

void CardGroup::LoadPositions(
  float* buffer,
  Shader* cardShader,
  int offset,
  int numPositions,
  int size,
  bool reverse
) {
  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);
  if (reverse) {
    int i = offset*transformVertexSize;
    int cardIndex = size-1;
    for (; i < (offset+numPositions)*transformVertexSize; i+=transformVertexSize) {
      for (; cardIndex>0 && cards[cardIndex].renderData.shader != cardShader; --cardIndex);
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;

      cardIndex++;
    }
  } else {
    int i = offset*transformVertexSize;
    int cardIndex = 0;
    for (; i < (offset+numPositions)*transformVertexSize; i+=transformVertexSize) {
      for (; cardIndex<size && cards[cardIndex].renderData.shader != cardShader; ++cardIndex);
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;

      cardIndex++;
    }
  }
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

void CardGroup::UpdateTick(double deltaTime) {
  bool anythingUpdated = false;
  for (auto& card : this->cards) {
    anythingUpdated = card.renderData.UpdateDisplayed(deltaTime) || anythingUpdated;
  }
  this->dirtyDisplay = true;
}

void CardGroup::SetDirtyPosition(bool dirty) {
  this->dirtyPosition = dirty;
}

void CardGroup::AddCard(unsigned int id) {
  this->cards.push_back({
    .card = Card(id),
    .renderData = CardRenderingData()
  });
}

void CardGroup::AddCard(CardItem card) {
  this->cards.push_back(card);
}

Card CardGroup::GetCard(unsigned int index) {
  return this->cards[index].card;
}

std::vector<CardItem>* CardGroup::GetCards() {
  return &this->cards;
}
