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

      cardIndex--;
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

void CardGroup::BindAndDrawAllFrontFaces(
  Renderer* renderer,
  Shader* shader,
  int maxBindableTextures,
  int offset,
  int groupSize,
  int totalSize,
  bool zFlipped
) {
  int buffer[sizeof(TextureVertex)/sizeof(unsigned int)*groupSize];

  int i = 0;
  while (i < groupSize) {
    int batchSize = fmin(maxBindableTextures, groupSize-i);

    int cardIndex = i;

    // bind all textures about to be used
    for (int j = i; j < i+batchSize; ++j) {
      // find the next card with the type of
      // shader we're trying to render
      for (; cardIndex < totalSize && this->cards[cardIndex].renderData.shader != shader; cardIndex++);

      unsigned int cardID = this->cards[cardIndex].card.GetID();

      // update texture buffer using newly bound addresses
      if (zFlipped) {
        buffer[j] = renderer->textureMap.RequestBind(maxBindableTextures, "back");
      } else {
        buffer[j] = renderer->textureMap.RequestBind(maxBindableTextures, cardID);
      }
      cardIndex++;
    }

    // write data from buffer to gpu
    this->textureIDBuffer.OverwriteData(
      buffer+i*(sizeof(TextureVertex)/sizeof(unsigned int)), 
      (offset+i)*sizeof(TextureVertex), 
      batchSize*sizeof(TextureVertex)
    );

    // this changes the offset within the
    // instanced buffers so that they are read
    // from the right offset.
    this->textureIDBuffer.OverwriteAttrib(
      this->textureEndAttribID, 
      1, 
      GL_UNSIGNED_INT, 
      sizeof(TextureVertex), 
      (const void*)((offset+i)*sizeof(TextureVertex))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID-1,
      3,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)((i+offset)*sizeof(CardTransformVertex))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID,
      1,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)((offset+i)*sizeof(CardTransformVertex)+offsetof(CardTransformVertex, rotationZ))
    );

    this->DrawElements(batchSize);

    i += maxBindableTextures;
  }
}

void CardGroup::DrawElements(int size) {
  GLCall(glDrawElementsInstanced(
    GL_TRIANGLES, 
    6, 
    GL_UNSIGNED_INT, 
    nullptr,
    size 
  ));
}

void CardGroup::PrepareTextures(TextureMap* textureMap, int start, int end) {
  int size = this->cards.size();

  if (zFlipped) {
    textureMap->SetupBack();
  } else {

    auto startIter = this->cards.begin();
    for (auto iter = startIter+start; iter < startIter+end; ++iter) {
      textureMap->SetupCard((*iter).card.GetID());
    }
    //for (CardItem& cardItem : this->cards) {
      //textureMap->SetupCard(cardItem.card.GetID());
    //}
  }
}

void CardGroup::UpdateTick(double deltaTime) {
  bool anythingUpdated = false;
  for (auto& card : this->cards) {
    anythingUpdated = card.renderData.UpdateDisplayed(deltaTime) || anythingUpdated;
  }
  if (anythingUpdated) {
    this->dirtyDisplay = true;
  }
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
