#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../include/CardGroup.h"
#include "../include/ErrorHandling.h"
#include "../include/shaders/allShaders.h"

const unsigned int CardGroup::estimatedMax = 60;

CardGroup::CardGroup(
  glm::vec3 position, 
  float rotationX, 
  float width, 
  bool zFlipped,
  bool isHand
) 
: 
zFlipped(zFlipped),
isHand(isHand),
dirtyDisplay(true), 
dirtyPosition(true), 
cardShader(myShaders::cardVertex, myShaders::cardFragment),
width(width),
lastCursorX(0),
lastCursorY(0),
wasInsideBoundary(false),
lastClosestIndex(-1),
strictBackingPlaneShader(myShaders::basicVertex, myShaders::basicFragment),
#ifdef DEBUG
strictBackingPlane(
  SimplePlane(
    glm::identity<glm::mat4>(), 
    Material(
      {
        .hasTexture=false,
        .shader=&this->strictBackingPlaneShader,
        .color=glm::vec4(0.5f, 0.0f, 0.5f, 0.5f)
      }
    )
  )
),
extendedBackingPlane(
  SimplePlane(
    glm::identity<glm::mat4>(), 
    Material(
      {
        .hasTexture=false,
        .shader=&this->strictBackingPlaneShader,
        .color=glm::vec4(0.0f, 0.5f, 0.0f, 0.5f)
      }
    )
  )
),
#endif
fullBackingPlane(
  SimplePlane(
    glm::identity<glm::mat4>(), 
    Material(
      {
        .hasTexture=false,
        .shader=&this->strictBackingPlaneShader,
        .color=glm::vec4(0.5f,0.5f,0.5f,0.5f)
      }
    )
  )
)
{
  ASSERT(width>3.0f);
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::translate(this->transform, position);
  this->transform = glm::translate(this->transform, glm::vec3(-width/2.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::scale(this->transform, glm::vec3(1.0f, 1.0f, zFlipped ? -1.0f : 1.0f));

  this->groupVao = VertexArray();
  this->groupVao.Bind();

  this->staticBuffer = VertexBuffer(CardRenderingData::cardPositions, 4*sizeof(CardVertex));
  this->staticBufferLayout = MemoryLayout();
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 3); // position
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 2); // texture uv coordinates
  this->groupVao.AddBuffer(this->staticBuffer, this->staticBufferLayout);

  this->transformBuffer = VertexBuffer(NULL, estimatedMax*sizeof(CardTransformVertex), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 3, true); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // rotation
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, estimatedMax*sizeof(TextureVertex), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
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

void CardGroup::DrawElements(int size) {
  GLCall(glDrawElementsInstanced(
    GL_TRIANGLES, 
    6, 
    GL_UNSIGNED_INT, 
    nullptr,
    size 
  ));
}

void CardGroup::GroupPositionToScreen(
  Renderer* renderer, 
  glm::vec4& src, 
  glm::vec2& dest
) const {
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  glm::vec4 screenSpace = projMatrix * camMatrix * this->transform * src;
  dest = renderer->GetScreenPositionFromCamera(screenSpace);
}

void CardGroup::GroupPositionTo3DScreen(
  Renderer* renderer, 
  glm::vec4& src, 
  glm::vec3& dest
) const {
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  glm::vec4 screenSpace = projMatrix * camMatrix * this->transform * src;
  dest = renderer->Get3DScreenPositionFromCamera(screenSpace);
}

bool CardGroup::GetInsideHandBoundary(
  Renderer* renderer,
  const CursorData& renderData,
  double horizontalOffset,
  double verticalOffset,
  bool& mouseMovedInBoundary,
  double& xScale,
  double& projectedLeftBoundary
) {
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;

  // if the cursor moved
  if (
    renderer->InsideWindowBounds(renderData.cursorX, renderData.cursorY) &&
    (
      (int)renderData.cursorY != lastCursorY || 
      (int)renderData.cursorX != lastCursorX
    )
  ) {
    // project top bound to screen
    glm::vec4 topLeft = glm::vec4(
      horizontalOffset, 
      0.5f*CardRenderingData::cardHeightRatio-verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec2 projectedTopLeft;
    this->GroupPositionToScreen(renderer, topLeft, projectedTopLeft);
    
    // project bottom bound to screen
    glm::vec4 bottomRight = glm::vec4(
      width-horizontalOffset, 
      -0.5f*CardRenderingData::cardHeightRatio+verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec2 projectedBottomRight;
    this->GroupPositionToScreen(renderer, bottomRight, projectedBottomRight);
    
    xScale = (projectedBottomRight.x - projectedTopLeft.x)/(this->width+2*(-horizontalOffset));
    projectedLeftBoundary = projectedTopLeft.x+(-horizontalOffset)*xScale;

    // see if cursor is below top left boundary and 
    // above bottom right boundary
    if (
      renderData.cursorX > projectedTopLeft.x && renderData.cursorY > projectedTopLeft.y &&
      renderData.cursorX < projectedBottomRight.x && renderData.cursorY < projectedBottomRight.y 
    ) {
      mouseMovedInBoundary = true;
      return true;
    } else {
      return false;
    }
  } else {
    return this->wasInsideBoundary;
  }
}

int CardGroup::GetClosestCardIndex(
  double projectedLeftBoundary,
  double margin,
  double xGap,
  double xScale,
  const CursorData& renderData,
  int size
) {
  int closestIndex = 0;

  double distanceToLastClosest = 1.0f;
  if (this->wasInsideBoundary) {
    ASSERT(this->lastClosestIndex != -1);
    distanceToLastClosest = abs(projectedLeftBoundary + (0.5f+margin+xGap*this->lastClosestIndex)*xScale-renderData.cursorX)/xScale;
    closestIndex = this->lastClosestIndex;
  }

  if (!this->wasInsideBoundary || distanceToLastClosest > 0.4f) {
    double thisX = projectedLeftBoundary + (0.5f+margin)*xScale;
    double smallestDistance = abs(thisX-renderData.cursorX);
    closestIndex = 0;
    for (int i = 1; i < size; ++i) {
      thisX += xGap*xScale;
      double distance = abs(thisX - renderData.cursorX);
      if (distance < smallestDistance) {
        smallestDistance = distance;
        closestIndex = i;
      }
    }

    // move 1 in the direction of the new closest index
    if (this->wasInsideBoundary) {
      closestIndex = this->lastClosestIndex + (closestIndex-this->lastClosestIndex)/abs(closestIndex-this->lastClosestIndex);
    }
  }

  return closestIndex;
}

void CardGroup::UpdateHandPosition(
  const CursorData& renderData,
  bool insideHandBoundary,
  double xGap,
  double margin,
  int size,
  double whitespace,
  double zGap
) {
  if (size == 0) return;

  float rotationPerCard = 0;//this->isHand ? glm::radians(10.0f) : 0;

  // if we need to display the cards
  // dynamically based on cursor position
  if (this->isHand && insideHandBoundary) {

    double centerX = this->lastClosestIndex*xGap+0.5f+margin;

    int leftSize = this->lastClosestIndex;
    int rightSize = size-this->lastClosestIndex-1;
    double leftWidth = centerX-whitespace-0.5f;
    double rightWidth = this->width-centerX-0.5f-whitespace;
    double leftGap = leftSize == 1 ? 0 : fmin(xGap, (leftWidth-1.0f)/(leftSize-1.0f));
    double rightGap = rightSize == 1 ? 0 : fmin(xGap, (rightWidth-1.0f)/(rightSize-1.0f));

    // ensures z is always positive,
    // which matters for rendering over 
    // backing
    int zOffset = fmax(1, rightSize - leftSize + 1); 

    // setup buffer info that depends
    // on left/right divide
    for (int i = 0; i < this->lastClosestIndex; i++) {
      int cardIndex = i;
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      thisCard.SetActualTransform(
        glm::vec3(
          //(float)cardIndex*leftGap+0.5f,
          (float)(centerX - fmax(0.5f + whitespace + 0.5f, xGap) - (this->lastClosestIndex-cardIndex-1)*leftGap),
          0.0f,
          (float)(cardIndex+zOffset)*zGap
        ),
        (float)(cardIndex-(float)this->lastClosestIndex)/size*rotationPerCard
      );
    }

    CardRenderingData& closestCard = cards[this->lastClosestIndex].renderData;
    closestCard.SetActualTransform(
      glm::vec3(centerX, 0.0f, (this->lastClosestIndex+zOffset)*zGap),
      0.0f
    );

    for (int i = (this->lastClosestIndex+1); i < size; i++) {
      int cardIndex = i;
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      thisCard.SetActualTransform(
        glm::vec3(
          (float)(centerX + fmax(0.5f + whitespace + 0.5f, xGap) + (cardIndex-this->lastClosestIndex-1)*rightGap),
          0.0f,
          (float)(this->lastClosestIndex - (cardIndex-this->lastClosestIndex) + zOffset) *zGap
        ),
        (float)(cardIndex-(float)(this->lastClosestIndex))/size*rotationPerCard
      );
    }
  } else {
    for (int i = 0; i < size; i++) {
      int cardIndex = i;

      CardRenderingData& thisCard = cards[cardIndex].renderData;

      thisCard.SetActualTransform(
        glm::vec3(
          (float)cardIndex*xGap+0.5f+margin,
          0.0f,
          (float)cardIndex*zGap+0.01f
        ),
        (float)(cardIndex-(float)(size-1)/2.0f)/size*rotationPerCard
      );
    }
  }
}

void CardGroup::BindAndDrawAllFrontFaces(
  Renderer* renderer,
  int maxBindableTextures,
  int size
) {
  int buffer[sizeof(TextureVertex)/sizeof(unsigned int)*size];

  int i = 0;
  while (i < size) {
    int batchSize = fmin(maxBindableTextures, size-i);

    // bind all textures about to be used
    for (int j = i; j < i+batchSize; ++j) {
      unsigned int cardID = this->cards[j].card.GetID();

      // update texture buffer using newly bound addresses
      buffer[j] = renderer->textureMap.RequestBind(maxBindableTextures, cardID);
    }

    // write data from buffer to gpu
    this->textureIDBuffer.OverwriteData(
      buffer+i*(sizeof(TextureVertex)/sizeof(unsigned int)), 
      i*sizeof(TextureVertex), 
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
      (const void*)(i*sizeof(TextureVertex))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID-1,
      3,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)(i*sizeof(CardTransformVertex))
    );

    this->transformBuffer.OverwriteAttrib(
      this->transformEndAttribID,
      1,
      GL_FLOAT,
      sizeof(CardTransformVertex), 
      (const void*)(i*sizeof(CardTransformVertex)+offsetof(CardTransformVertex, rotationZ))
    );

    this->DrawElements(batchSize);

    i += maxBindableTextures;
  }
}


void CardGroup::Render(
  Renderer* renderer
) {
  CursorData cursor;
  renderer->GetCursorPosition(&cursor);

  bool insideHandBoundary = false;
  bool mouseMovedInBoundary = false;

  int size = this->cards.size();

  // I store this because I use it in
  // determining closest card
  double projectedLeftBoundary = -1.0f;
  double xScale = -1.0f;

  // this is the area we reserve for white space
  // to the left and right of the card the user is
  // hovering over
  const double whitespace = 0.05f;

  // this makes sure the default card packing allows
  // room for whitespace
  double margin = 0.0f;

  if (size == 1) {
    // just enough to make sure the
    // single card is centered
    margin = (this->width-1.0f)/2.0f;
  } else if (size == 2) {
    // makes sure the 2 cards take up
    // only the required space
    margin = (width - 2.0f - 2*whitespace)/2.0f;
  } else if (this->isHand) {
    if (size > 3) {
      // I got this equation by solving:
      //    xGap = (width-1-2*margin)/(size-1)
      //    margin = whitespace + 1 - xGap
      margin = (size + size*whitespace - whitespace - width)/(size-3);
    } else {
      if (width > 3.0f + 2*whitespace) {
        margin = (width - 3.0f - 2*whitespace)/2.0f;
      } else {
        margin = 0.0f;
      }
    }
  } else {
    if (width > (float)size + 2*whitespace) {
      margin = (width - size - 2*whitespace)/2.0f;
    } else {
      margin = 0.0f;
    }
  }

  const float verticalMargin = 0.15f;
  const float horizontalMargin = 0.15f;

  // if we need to know boundaries
  if (this->isHand) {

    // check if cursor is near the card group
    insideHandBoundary = this->GetInsideHandBoundary(renderer, cursor, margin-horizontalMargin, -verticalMargin, mouseMovedInBoundary, xScale, projectedLeftBoundary);
  }

  if (this->wasInsideBoundary != insideHandBoundary) this->dirtyPosition = true;

  // prevent division by 0 error
  float xGap = size == 1 ? 0 : (this->width-1-2*margin)/(size-1);
  float zGap = 1.0f/200.0f;

  // check is the closest card changed.
  // if it didn't, we don't need to rerender
  if (mouseMovedInBoundary && size != 0) {
    // find closest card
    
    int closestIndex = this->GetClosestCardIndex(
      projectedLeftBoundary,
      margin,
      xGap,
      xScale,
      cursor,
      size
    );

    if (this->wasInsideBoundary && closestIndex != this->lastClosestIndex) {
      this->dirtyPosition = true;
    }

    this->lastClosestIndex = closestIndex;
  }

  // If we need to update the actual
  // card positions
  if (this->dirtyPosition) {
    this->UpdateHandPosition(
      cursor,
      insideHandBoundary,
      xGap,
      margin,
      size,
      whitespace,
      zGap
    );
  }

  // 3 for positions plus 1 for rotation
  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);

  // if we need to update the
  // displayed position
  if (this->dirtyDisplay) {

    // update buffer for relative position and rotation
    float buffer[transformVertexSize*size];

    // load display info from cards into buffer
    for (int i = 0; i < size*transformVertexSize; i+=transformVertexSize) {
      int cardIndex = i/transformVertexSize;
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;
    }


    this->transformBuffer.OverwriteData(buffer, 0, size*sizeof(CardTransformVertex));
  }

  this->groupVao.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();
  this->cardShader.Bind();
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  cardShader.SetUniform4fv("projMatrix", false, glm::value_ptr(projMatrix));
  cardShader.SetUniform4fv("cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader.SetUniform4fv("modelMatrix", false, glm::value_ptr(this->transform));

  this->PrepareTextures(&renderer->textureMap);

  // Pass texture units as an array to the shader.
  int maxBindableTextures = renderer->maxBindableTextures;
  cardShader.SetInstancedTextures(maxBindableTextures, &renderer->textureMap);

  // bind textures and shift buffer for
  // rendering
  if (size != 0) {
    if (this->zFlipped) {
      int boundTo = renderer->textureMap.RequestBind(maxBindableTextures, "back");
      int boundToArray[size];
      for (int i = 0; i < size; ++i) {
        boundToArray[i] = boundTo;
      }
      this->textureIDBuffer.OverwriteData(
        boundToArray, 
        0, 
        sizeof(GLint)*size
      );

      this->transformBuffer.OverwriteAttrib(
        this->transformEndAttribID-1,
        3,
        GL_FLOAT,
        sizeof(CardTransformVertex), 
        0
      );

      this->transformBuffer.OverwriteAttrib(
        this->transformEndAttribID,
        1,
        GL_FLOAT,
        sizeof(CardTransformVertex), 
        (const void*) (offsetof(CardTransformVertex, rotationZ))
      );

      this->DrawElements(size);
    } else {
      this->BindAndDrawAllFrontFaces(
        renderer,
        maxBindableTextures,
        size
      );
    }
  }

  if (this->dirtyPosition) {
    #ifdef DEBUG
    this->strictBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.01f));
    this->extendedBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.02f));
    this->strictBackingPlaneTransform = glm::scale(this->strictBackingPlaneTransform, glm::vec3(width-2*margin, CardRenderingData::cardHeightRatio, 1.0f));
    this->extendedBackingPlaneTransform = glm::scale(this->extendedBackingPlaneTransform, glm::vec3(width-2*margin+2*horizontalMargin, CardRenderingData::cardHeightRatio+2*verticalMargin, 1.0f));
    #endif
    if (!this->isHand) {
      this->fullBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.03f));
      this->fullBackingPlaneTransform = glm::scale(this->fullBackingPlaneTransform, glm::vec3(width, CardRenderingData::cardHeightRatio, 1.0f));
    }
  }
  #ifdef DEBUG
  this->strictBackingPlane.SetTransform(&this->strictBackingPlaneTransform);
  this->strictBackingPlane.Render(renderer);
  this->extendedBackingPlane.SetTransform(&this->extendedBackingPlaneTransform);
  this->extendedBackingPlane.Render(renderer);
  #endif
  if (!this->isHand) {
    this->fullBackingPlane.SetTransform(&this->fullBackingPlaneTransform);
    this->fullBackingPlane.Render(renderer);
  }

  this->dirtyPosition = false;
  this->dirtyDisplay = false;

  if (this->isHand) {
    this->lastCursorX = (int)cursor.cursorX;
    this->lastCursorY = (int)cursor.cursorY;
    this->wasInsideBoundary = insideHandBoundary;
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

void CardGroup::UpdateTick(double deltaTime) {
  bool anythingUpdated = false;
  for (auto& card : this->cards) {
    anythingUpdated = card.renderData.UpdateDisplayed(deltaTime) || anythingUpdated;
  }
  this->dirtyDisplay = true;
}

bool CardGroup::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* collisionInfo
) const {
  int size = this->cards.size();
  if (size == 0) return false;

  // This function makes the assumption that
  // all cardgroups are projected onto the screen
  // as isosceles trapezoids, which is actually
  // enforced by the fact that the transform in the
  // constructor only accepts x rotation
  
  // In addition, we make the assumption that cards
  // themselves are trapezoids with constant topHeight
  // and bottomHeight, as a result of the fact that rotationZ
  // is 0 when you hover over the card

  glm::vec4 bottomRight = glm::vec4(
    width, 
    -0.5f*CardRenderingData::cardHeightRatio, 
    0.0f, 
    1.0f
  );
  glm::vec3 projectedBottomRight;
  this->GroupPositionTo3DScreen(renderer, bottomRight, projectedBottomRight);

  glm::vec4 topRight = glm::vec4(
    width, 
    0.5f*CardRenderingData::cardHeightRatio, 
    0.0f, 
    1.0f
  );
  glm::vec3 projectedTopRight;
  this->GroupPositionTo3DScreen(renderer, topRight, projectedTopRight);

  // if y inside inside the box, then you can short circuit
  // because the cursor definitely isn't in the group
  if (y > projectedBottomRight.y || y < projectedTopRight.y) return false;

  glm::vec4 bottomLeft = glm::vec4(
    0, 
    -0.5f*CardRenderingData::cardHeightRatio, 
    0.0f, 
    1.0f
  );
  glm::vec2 projectedBottomLeft;
  this->GroupPositionToScreen(renderer, bottomLeft, projectedBottomLeft);

  glm::vec4 topLeft = glm::vec4(
    0, 
    0.5f*CardRenderingData::cardHeightRatio, 
    0.0f, 
    1.0f
  );
  glm::vec2 projectedTopLeft;
  this->GroupPositionToScreen(renderer, topLeft, projectedTopLeft);

  double t = (y-projectedTopLeft.y)/(projectedBottomLeft.y-projectedTopLeft.y);

  // linear interpolate to find boundaries
  double leftBoundary = t*projectedBottomLeft.x + (1-t)*(projectedTopLeft.x);
  double rightBoundary = t*projectedBottomRight.x + (1-t)*(projectedTopRight.x);
  double zAtCursor = t*projectedBottomRight.z + (1-t)*projectedTopRight.z;

  bool inBounds = (x >= leftBoundary && x <= rightBoundary);

  if (!inBounds) return false;

  double xGap = (rightBoundary - leftBoundary)/this->width;
  
  // iterate through all the cards
  for (int i = 0; i < size; ++i) {
    double currentCardLeftBound = leftBoundary + xGap*(this->cards[i].renderData.displayedPosition.x - 0.5f);
    double currentCardRightBound = currentCardLeftBound + xGap;
    double rightBoundary = (i+1 < size) ? 
       fmin(
         leftBoundary + xGap*(this->cards[i+1].renderData.displayedPosition.x - 0.5f), 
         currentCardRightBound
       ) :
       currentCardRightBound;
    if (x >= currentCardLeftBound && x <= rightBoundary) {
      *collisionZ = zAtCursor;
      collisionInfo->collisionIndex = i;
      return true;
    }
  }

  return false;
}

void CardGroup::MoveToGroup(int index, CardGroup* to) {
  this->dirtyPosition = true;
  to->dirtyPosition = true;

  // this just makes it so that
  // in the next render we reset
  // the selected index
  this->wasInsideBoundary = false;

  glm::vec4 v = glm::vec4(
    this->cards[index].renderData.displayedPosition.x,
    this->cards[index].renderData.displayedPosition.y,
    this->cards[index].renderData.displayedPosition.z,
    1
  );

  CardItem cardCopy = {
    .card = this->cards[index].card,
  };
  cardCopy.renderData.displayedPosition = glm::inverse(to->transform)*this->transform*v;

  to->cards.push_back(cardCopy);
  this->cards.erase(this->cards.begin() + index);
}

