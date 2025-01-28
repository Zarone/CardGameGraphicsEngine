#include "../../include/CardGroup/CardGroupRenderer.h"

const unsigned int CardGroupRenderer::estimatedMax = 60;

CardGroupRenderer::CardGroupRenderer(
  bool zFlipped
) 
: 
dirtyDisplay(true), 
dirtyPosition(true), 
zFlipped(zFlipped)
{
}

void CardGroupRenderer::LoadPositions(
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
      for (; cardIndex>0 && (*cardsPointer)[cardIndex].renderData.shader != cardShader; --cardIndex);
      CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;
      buffer[i+4] = thisCard.displayedScaleXY;

      cardIndex--;
    }
  } else {
    int i = offset*transformVertexSize;
    int cardIndex = 0;
    for (; i < (offset+numPositions)*transformVertexSize; i+=transformVertexSize) {
      for (; cardIndex<size && (*cardsPointer)[cardIndex].renderData.shader != cardShader; ++cardIndex);
      CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;
      buffer[i+4] = thisCard.displayedScaleXY;

      cardIndex++;
    }
  }
}

void CardGroupRenderer::GroupPositionToScreen(
  Renderer* renderer, 
  glm::vec4& src, 
  glm::vec2& dest
) const {
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  glm::vec4 screenSpace = projMatrix * camMatrix * this->transform * src;
  dest = renderer->GetScreenPositionFromCamera(screenSpace);
}

void CardGroupRenderer::GroupPositionTo3DScreen(
  Renderer* renderer, 
  glm::vec4& src, 
  glm::vec3& dest
) const {
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  glm::vec4 screenSpace = projMatrix * camMatrix * this->transform * src;
  dest = renderer->Get3DScreenPositionFromCamera(screenSpace);
}

void CardGroupRenderer::SetCardsPointer(std::vector<CardItem>* ptr) {
  this->cardsPointer = ptr;
}

void CardGroupRenderer::BindAndDrawAllFrontFaces(
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
      for (; cardIndex < totalSize && (*cardsPointer)[cardIndex].renderData.shader != shader; cardIndex++);

      unsigned int cardID = (*cardsPointer)[cardIndex].card.GetID();

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
      this->transformEndAttribID-2,
      3,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)((i+offset)*sizeof(CardTransformVertex))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID-1,
      1,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)((offset+i)*sizeof(CardTransformVertex)+offsetof(CardTransformVertex, rotationZ))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID,
      1,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)((offset+i)*sizeof(CardTransformVertex)+offsetof(CardTransformVertex, scaleXY))
    );

    this->DrawElements(batchSize);

    i += maxBindableTextures;
  }
}

void CardGroupRenderer::DrawElements(int size) {
  GLCall(glDrawElementsInstanced(
    GL_TRIANGLES, 
    6, 
    GL_UNSIGNED_INT, 
    nullptr,
    size 
  ));
}

void CardGroupRenderer::PrepareTextures(TextureMap* textureMap, int start, int end) {
  int size = (*cardsPointer).size();

  if (zFlipped) {
    textureMap->SetupBack();
  } else {

    auto startIter = (*cardsPointer).begin();
    for (auto iter = startIter+start; iter < startIter+end; ++iter) {
      textureMap->SetupCard((*iter).card.GetID());
    }
  }
}

void CardGroupRenderer::SetDirtyPosition(bool dirty) {
  this->dirtyPosition = dirty;
}

void CardGroupRenderer::UpdateTick(double deltaTime) {
  bool anythingUpdated = false;
  for (auto& card : *cardsPointer) {
    anythingUpdated = card.renderData.UpdateDisplayed(deltaTime) || anythingUpdated;
  }
  if (anythingUpdated) {
    this->dirtyDisplay = true;
  }
}

bool CardGroupRenderer::IsInsideScreenRectangle(
  Renderer* renderer,
  double x, 
  double y, 
  glm::vec4 topLeft,
  glm::vec4 topRight,
  glm::vec4 bottomLeft,
  glm::vec4 bottomRight,
  double width,
  double* leftBoundary,
  double* zAtCursor,
  double* xScale
) const {
  glm::vec3 projectedBottomRight;
  this->GroupPositionTo3DScreen(renderer, bottomRight, projectedBottomRight);

  glm::vec3 projectedTopRight;
  this->GroupPositionTo3DScreen(renderer, topRight, projectedTopRight);

  // if y inside inside the box, then you can short circuit
  // because the cursor definitely isn't in the group
  if (y > projectedBottomRight.y || y < projectedTopRight.y) return false;

  glm::vec2 projectedBottomLeft;
  this->GroupPositionToScreen(renderer, bottomLeft, projectedBottomLeft);

  glm::vec2 projectedTopLeft;
  this->GroupPositionToScreen(renderer, topLeft, projectedTopLeft);

  double t = (y-projectedTopLeft.y)/(projectedBottomLeft.y-projectedTopLeft.y);

  // linear interpolate to find boundaries
  *leftBoundary = t*projectedBottomLeft.x + (1-t)*(projectedTopLeft.x);
  double rightBoundary = t*projectedBottomRight.x + (1-t)*(projectedTopRight.x);
  *zAtCursor = t*projectedBottomRight.z + (1-t)*projectedTopRight.z;

  bool inBounds = (x >= *leftBoundary && x <= rightBoundary);

  if (!inBounds) return false;

  *xScale = (rightBoundary - *leftBoundary)/width;

  return true;
}
