#include <glm/gtc/type_ptr.hpp>
#include "../../include/CardGroup/FannedCardGroupRenderer.h"

FannedCardGroupRenderer::FannedCardGroupRenderer(
  Renderer* renderer,
  glm::vec3 position, 
  float rotationX, 
  float width, 
  bool zFlipped,
  bool isHand
) : 
  isHand(isHand),
  width(width),
  lastCursorX(0),
  lastCursorY(0),
  wasInsideBoundary(false),
  lastClosestIndex(-1),
#ifdef DEBUG
  strictBackingPlane(
    SimplePlane(
      glm::identity<glm::mat4>(), 
      Material(
        {
          .hasTexture=false,
          .shader=renderer->GetShader("basicShader"),
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
          .shader=renderer->GetShader("basicShader"),
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
          .shader=renderer->GetShader("basicShader"),
          .color=glm::vec4(0.5f,0.5f,0.5f,0.5f)
        }
      )
    )
  ),
  CardGroupRenderer(zFlipped) 
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

void FannedCardGroupRenderer::UpdateHandPosition(
  const CursorData& renderData,
  bool insideHandBoundary,
  double xGap,
  double margin,
  int size,
  double whitespace,
  double zGap
) {
  if (size == 0) return;

  float rotationPerCard = this->isHand ? glm::radians(10.0f) : 0;

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
      CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

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

    CardRenderingData& closestCard = (*cardsPointer)[this->lastClosestIndex].renderData;
    closestCard.SetActualTransform(
      glm::vec3(centerX, 0.1f, (this->lastClosestIndex+zOffset)*zGap+0.1f),
      0.0f
    );

    for (int i = (this->lastClosestIndex+1); i < size; i++) {
      int cardIndex = i;
      CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

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

      CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

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

bool FannedCardGroupRenderer::GetInsideHandBoundary(
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
    glm::vec4 bottomRight = glm::vec4(
      width-horizontalOffset, 
      -0.5f*CardRenderingData::cardHeightRatio+verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec3 projectedBottomRight;
    this->GroupPositionTo3DScreen(renderer, bottomRight, projectedBottomRight);

    glm::vec4 topRight = glm::vec4(
      width-horizontalOffset, 
      0.5f*CardRenderingData::cardHeightRatio-verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec3 projectedTopRight;
    this->GroupPositionTo3DScreen(renderer, topRight, projectedTopRight);

    glm::vec4 bottomLeft = glm::vec4(
      horizontalOffset, 
      -0.5f*CardRenderingData::cardHeightRatio+verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec2 projectedBottomLeft;
    this->GroupPositionToScreen(renderer, bottomLeft, projectedBottomLeft);

    glm::vec4 topLeft = glm::vec4(
      horizontalOffset, 
      0.5f*CardRenderingData::cardHeightRatio-verticalOffset, 
      0.0f, 
      1.0f
    );
    glm::vec2 projectedTopLeft;
    this->GroupPositionToScreen(renderer, topLeft, projectedTopLeft);

    double t = (renderData.cursorY-projectedTopLeft.y)/(projectedBottomLeft.y-projectedTopLeft.y);

    // linear interpolate to find boundaries
    double leftBoundary = t*projectedBottomLeft.x + (1-t)*(projectedTopLeft.x);
    double halfWayLeftBoundary = 0.5f*projectedBottomLeft.x + (0.5f)*(projectedTopLeft.x);
    double rightBoundary = t*projectedBottomRight.x + (1-t)*(projectedTopRight.x);
    double zAtCursor = t*projectedBottomRight.z + (1-t)*projectedTopRight.z;

    xScale = (rightBoundary - leftBoundary)/(this->width+2*(-horizontalOffset));
    
    projectedLeftBoundary = halfWayLeftBoundary+(-horizontalOffset)*xScale;

    // see if cursor is below top left boundary and 
    // above bottom right boundary
    if (
      renderData.cursorX > leftBoundary && renderData.cursorY > projectedTopLeft.y &&
      renderData.cursorX < rightBoundary && renderData.cursorY < projectedBottomRight.y 
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

int FannedCardGroupRenderer::GetClosestCardIndex(
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

bool FannedCardGroupRenderer::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* collisionInfo
) const {
  int size = cardsPointer->size();
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

  double leftBoundary;
  double zAtCursor;
  double xScale;

  if (!this->IsInsideScreenRectangle(
    renderer,
    x, y,
    glm::vec4( // top left
      0, 
      0.5f*CardRenderingData::cardHeightRatio, 
      0.0f, 
      1.0f
    ),
    glm::vec4( // top right
      width, 
      0.5f*CardRenderingData::cardHeightRatio, 
      0.0f, 
      1.0f
    ),
    glm::vec4( // bottom left 
      0, 
      -0.5f*CardRenderingData::cardHeightRatio, 
      0.0f, 
      1.0f
    ),
    glm::vec4( // bottom right
      width, 
      -0.5f*CardRenderingData::cardHeightRatio, 
      0.0f, 
      1.0f
    ),
    this->width,
    &leftBoundary,
    &zAtCursor,
    &xScale
  )) return false;
  
  // iterate through all the cards
  for (int i = 0; i < size; ++i) {
    double currentCardLeftBound = leftBoundary + xScale*((*cardsPointer)[i].renderData.displayedPosition.x - 0.5f);
    double currentCardRightBound = currentCardLeftBound + xScale;
    double rightBoundary = (i+1 < size) ? 
       fmin(
         leftBoundary + xScale*((*cardsPointer)[i+1].renderData.displayedPosition.x - 0.5f), 
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

void FannedCardGroupRenderer::Render(
  Renderer* renderer
) {
  CursorData cursor;
  renderer->GetCursorPosition(&cursor);

  bool insideHandBoundary = false;
  bool mouseMovedInBoundary = false;

  int size = cardsPointer->size();

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

  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);

  // if we need to update the
  // displayed position
  if (this->dirtyDisplay) {

    // update buffer for relative position and rotation
    float buffer[transformVertexSize*size];

    // load display info from cards into buffer
    Shader* cardShader = renderer->GetShader("cardShader");
    this->LoadPositions(buffer, cardShader, 0, size-this->highlightedCards, size);
    cardShader = renderer->GetShader("highlightCardShader");
    this->LoadPositions(buffer, cardShader, size-this->highlightedCards, this->highlightedCards, size);

    this->transformBuffer.OverwriteData(buffer, 0, size*sizeof(CardTransformVertex));
  }

  int maxBindableTextures = renderer->maxBindableTextures;
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;

  this->groupVao.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();

  Shader* cardShader = renderer->GetShader("cardShader");
  cardShader->Bind();
  cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
  cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
  cardShader->SetInstancedTextures(maxBindableTextures, &renderer->textureMap);

  this->PrepareTextures(&renderer->textureMap, 0, size);

  // Pass texture units as an array to the shader.

  // bind textures and shift buffer for
  // rendering
  if (size != 0) {
    this->BindAndDrawAllFrontFaces(
      renderer,
      cardShader,
      maxBindableTextures,
      0,
      size-this->highlightedCards,
      size, zFlipped
    );
    if (this->highlightedCards != 0) {
      cardShader = renderer->GetShader("highlightCardShader");
      cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
      cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
      cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
      cardShader->SetInstancedTextures(maxBindableTextures, &renderer->textureMap);
      this->BindAndDrawAllFrontFaces(
        renderer,
        cardShader,
        maxBindableTextures,
        size-this->highlightedCards,
        this->highlightedCards,
        size, zFlipped
      );
    }
  }

  if (this->dirtyPosition) {
    #ifdef DEBUG
    this->strictBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.001f));
    this->extendedBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.002f));
    this->strictBackingPlaneTransform = glm::scale(this->strictBackingPlaneTransform, glm::vec3(width-2*margin, CardRenderingData::cardHeightRatio, 1.0f));
    this->extendedBackingPlaneTransform = glm::scale(this->extendedBackingPlaneTransform, glm::vec3(width-2*margin+2*horizontalMargin, CardRenderingData::cardHeightRatio+2*verticalMargin, 1.0f));
    #endif
    if (!this->isHand) {
      this->fullBackingPlaneTransform = glm::translate(this->transform, glm::vec3(width/2.0f, 0, -0.003f));
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

const glm::mat4 FannedCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform);
}
