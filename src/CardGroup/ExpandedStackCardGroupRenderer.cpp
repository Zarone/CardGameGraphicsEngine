#include <glm/gtc/type_ptr.hpp>
#include "ExpandedStackCardGroupRenderer.h"
#include "../shaders/allShaders.h"

ExpandedStackCardGroupRenderer::ExpandedStackCardGroupRenderer(
  Renderer* renderer,
  std::function<void()> onClose
) : 
  backingPlane(
    SimplePlane(
      glm::identity<glm::mat4>(), 
      Material(
        {
          .hasTexture=false,
          .shader=renderer->GetShader(BasicShader),
          .color=glm::vec4(0.1f,0.1f,0.1f,0.9f)
        }
      )
    )
  ),
  coverOverflowTop(
    SimplePlane(
      glm::identity<glm::mat4>(), 
      Material(
        {
          .hasTexture=false,
          .shader=renderer->GetShader(BasicShader),
          .color=glm::vec4(0.1f,0.1f,0.1f,0.9f)
        }
      )
    )
  ),
  coverOverflowBottom(
    SimplePlane(
      glm::identity<glm::mat4>(), 
      Material(
        {
          .hasTexture=false,
          .shader=renderer->GetShader(BasicShader),
          .color=glm::vec4(0.1f,0.1f,0.1f,0.9f)
        }
      )
    )
  ),
  cardContainerPlane(
    SimplePlane(
      glm::identity<glm::mat4>(), 
      Material(
        {
          .hasTexture=false,
          .shader=renderer->GetShader(BasicShader),
          .color=glm::vec4(0.73f,0.70f,0.66f,0.9f)
        }
      )
    )
  ),
  closeExpandedView(
    renderer,
    glm::identity<glm::mat4>(), 
    Material(
      {
        .hasTexture=false,
        .shader=renderer->GetShader(BasicShader),
        .color=glm::vec4(1.f,0.f,0.f,1.f)
      }
    ),
    onClose
  ),
  CardGroupRenderer(renderer, false) 
{
  this->transform = glm::identity<glm::mat4>();

  // render straight to screen without projection
  this->backingPlane.TogglePerspective(false);
  this->cardContainerPlane.TogglePerspective(false);
  this->closeExpandedView.TogglePerspective(false);
  this->coverOverflowBottom.TogglePerspective(false);
  this->coverOverflowTop.TogglePerspective(false);

  this->backingPlane.MakeClickable();
  this->cardContainerPlane.MakeClickable();

  this->backingPlaneTransform = glm::translate(this->transform, glm::vec3(0.0f, 0.0f, 0.01f));
  this->backingPlaneTransform = glm::scale(this->backingPlaneTransform, glm::vec3(2.0f, 2.0f, 1.0f));
  this->cardContainerTransform = glm::translate(this->transform, glm::vec3(0.0f, (2.0f-this->height)/2.0f-this->yTopPadding, 0.0f));
  this->cardContainerTransform = glm::scale(this->cardContainerTransform, glm::vec3(this->width, this->height, 1.0f));
  float buttonHeight = 0.25f;
  this->closeExpandedTransform = glm::translate(this->transform, glm::vec3(0.0f, 1-this->height-this->yTopPadding-buttonHeight/2, -0.003f));
  this->closeExpandedTransform = glm::scale(this->closeExpandedTransform, glm::vec3(0.5f, 0.25f, 1.0f));
  float remainingHeight = 2-this->height-this->yTopPadding;
  this->coverOverflowBottomTransform = glm::translate(this->transform, glm::vec3(0.0f, 1-this->height-this->yTopPadding-remainingHeight/2.0f, -0.002f));
  this->coverOverflowBottomTransform = glm::scale(this->coverOverflowBottomTransform, glm::vec3(2.0f, remainingHeight, 1.0f));
  this->coverOverflowTopTransform = glm::translate(this->transform, glm::vec3(0.0f, 1-this->yTopPadding/2.0f, -0.002f));
  this->coverOverflowTopTransform = glm::scale(this->coverOverflowTopTransform, glm::vec3(2.0f, this->yTopPadding, 1.0f));
  this->transform = glm::translate(this->transform, glm::vec3(-this->width/2, (2.0f-this->height)/2.0f-this->yTopPadding, 0.0f));
  this->transform = glm::scale(this->transform, glm::vec3(1.0f, renderer->GetAspectRatio(), 1.0f));

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
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // scale
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, estimatedMax*sizeof(TextureVertex), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
}

bool ExpandedStackCardGroupRenderer::IsCursorHoveringOnCard(
  const CursorData& renderData,
  int size,
  int* cardIndex
) const {
  float cardHeight = cardScaleXY*CardRenderingData::cardHeightRatio;
  for (int i = 0; i < size; ++i) {
    glm::vec4 pos = glm::vec4(this->GetCardPosition(i), 1.0f);
    glm::vec4 topLeftPos = glm::vec4(pos.x-0.5f*cardScaleXY, pos.y+cardHeight*0.5f, pos.z, pos.w);
    glm::vec4 bottomRightPos = glm::vec4(pos.x+0.5f*cardScaleXY, pos.y-cardHeight*0.5f, pos.z, pos.w);
    glm::vec2 screenPosTopLeft = renderer->GetScreenPositionFromCamera(this->transform * topLeftPos);
    glm::vec2 screenPosBottomRight = renderer->GetScreenPositionFromCamera(this->transform*bottomRightPos);

    bool onCard = ((renderData.cursorX <= screenPosBottomRight.x)&&
    (renderData.cursorX >= screenPosTopLeft.x)&&
    (renderData.cursorY <= screenPosBottomRight.y)&&
    (renderData.cursorY >= screenPosTopLeft.y));

    if (onCard) {
      *cardIndex = i;
      return true;
    }

  }
  return false;
}

bool ExpandedStackCardGroupRenderer::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* collisionInfo
) const {
  CollisionInfo bestCollisionInfo;
  double bestCollisionZ = MAXFLOAT;

  const int numElements = 3;
  const SceneObject* elements[numElements];
  const SceneObject* selectedObject = nullptr;
  elements[0] = &closeExpandedView;
  elements[1] = &cardContainerPlane;
  elements[2] = &backingPlane;

  for (int i = 0; i < numElements; ++i) {
    CollisionInfo localCollisionInfo;
    double localCollisionZ = MAXFLOAT;
    bool objectCollision = elements[i]->CheckCollision(renderer, x, y, &localCollisionZ, &localCollisionInfo);

    if (objectCollision && localCollisionZ < bestCollisionZ) {
      bestCollisionZ = localCollisionZ;
      selectedObject = elements[i];
      bestCollisionInfo = std::move(localCollisionInfo);
    }
  }

  if (selectedObject == nullptr) return false;

  *collisionZ = bestCollisionZ;
  if (selectedObject == &cardContainerPlane) {
    std::cout << "Unimplemented collision function for cards of ExpandedStackCardGroup" << std::endl;
    collisionInfo->innerCollision = nullptr;

    int cardIndexOfHovering;
    bool isHovering = this->IsCursorHoveringOnCard(
      {.cursorX = x, .cursorY = y}, 
      this->cardsPointer->size(), 
      &cardIndexOfHovering
    );

    if (isHovering) {
      collisionInfo->cardIndex = cardIndexOfHovering;
      collisionInfo->isCard = true;
    } else {
      collisionInfo->isCard = false;
    }


    // So it's sort of unintuitive, but it makes more sense for this object
    // to just handle the collision event in the expandedstackrenderer itself.
    // For example, this scroll method should be called instead of the cardContainer plane,
    // since that's just a plane with no event handlers.
    collisionInfo->groupPointer = (SceneObject*) this;
  } else {
    collisionInfo->innerCollision = std::make_unique<CollisionInfo>();
    collisionInfo->innerCollision->groupPointer = bestCollisionInfo.groupPointer;
    collisionInfo->innerCollision->cardIndex = bestCollisionInfo.cardIndex;
    collisionInfo->innerCollision->isCard = bestCollisionInfo.isCard;
    collisionInfo->innerCollision->innerCollision = std::move(bestCollisionInfo.innerCollision);

    collisionInfo->isCard = false;
    collisionInfo->groupPointer = (SceneObject*) this;
  }

  return true;
}

glm::vec3 ExpandedStackCardGroupRenderer::GetCardPosition(int i) const {
  const float xGap = (this->width-1.0f*cardScaleXY)/(cardsPerRow-1.0f);
  float cardHeight = cardScaleXY*CardRenderingData::cardHeightRatio;
  float halfContainerHeight = this->height/2.0f/renderer->GetAspectRatio();

  return glm::vec3(
    (float)cardScaleXY*0.5f+(i%cardsPerRow)*xGap,
    (float)(-i/cardsPerRow)*(cardHeight+verticalWhitespaceInContainer)
      + halfContainerHeight
      - cardHeight/2.0f
      + this->scrollPosition,
    (float)-0.001f
  );
}

void ExpandedStackCardGroupRenderer::UpdateCardPositions() {
  const float rotationPerCard = glm::radians(4.0f);
  float yOffset = 0.0f;
  int size = this->cardsPointer->size();

  for (int i = 0; i < size; i++) {
    CardRenderingData& thisCard = (*cardsPointer)[i].renderData;
    glm::vec3 thisOffset = (isHovering && this->lastClosestIndex == i) ? glm::vec3(0.0f, 0.03f, 0.0f) : glm::vec3(0.0f);

    thisCard.SetActualTransform(
      this->GetCardPosition(i) + thisOffset,
      (float) (isHovering && this->lastClosestIndex == i)
        ? 0
        : ((i%cardsPerRow)-(float)(cardsPerRow-1)/2.0f)/cardsPerRow*rotationPerCard,
      cardScaleXY
    );
  }
}

void ExpandedStackCardGroupRenderer::Render(
  Renderer* renderer
) {
  CursorData cursor;
  renderer->GetCursorPosition(&cursor);

  bool insideHandBoundary = false;
  bool mouseMovedInBoundary = ((int)cursor.cursorX) != this->lastCursorX 
    || ((int)cursor.cursorY) != this->lastCursorY;

  CollisionInfo cInfo;
  double collisionZ;
  insideHandBoundary = cardContainerPlane.CheckCollision(
    renderer, 
    cursor.cursorX,
    cursor.cursorY,
    &collisionZ,
    &cInfo 
  );
  if (!insideHandBoundary) this->isHovering = false;

  if (this->wasInsideBoundary != insideHandBoundary) this->dirtyPosition = true;

  int size = this->cardsPointer->size();
  
  // check if the closest card changed.
  // if it didn't, we don't need to rerender
  if (mouseMovedInBoundary && size != 0) {
    // find closest card
    
    int closestIndex;
    bool newIsHovering = this->IsCursorHoveringOnCard(
      cursor,
      size,
      &closestIndex
    );

    if (
      this->wasInsideBoundary && 
      (
        (isHovering != newIsHovering) || 
        (isHovering && closestIndex != this->lastClosestIndex)
      )
    ) {
      this->dirtyPosition = true;
    }

    this->isHovering = newIsHovering;
    this->lastClosestIndex = closestIndex;
  }

  if (this->dirtyPosition) {
    this->UpdateCardPositions();
  }

  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);

  std::set<Shader*> shaders = {};
  for (CardItem& card : *this->cardsPointer){
    shaders.insert(card.renderData.shader);
  }

  if (this->dirtyDisplay) {
    // update buffer for relative position and rotation
    float buffer[transformVertexSize*size];

    // load display info from cards into buffer
    int offset = 0;
    for (Shader* const& shader : shaders) {
      int thisSize = this->GetCardsWithShader(shader);
      this->LoadPositions(buffer, shader, offset, thisSize, size, false);
      offset+=thisSize;
    }

    this->transformBuffer.OverwriteData(buffer, 0, size*sizeof(CardTransformVertex));
  }

  int maxBindableTextures = renderer->maxBindableTextures;
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;
  
  this->groupVao.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();

  glm::mat4 identity = glm::identity<glm::mat4>();

  this->PrepareTextures(&renderer->textureMap, 0, size);

  // rendering
  if (size != 0) {
    int offset = 0;
    for (Shader* const& shader : shaders) {
      shader->Bind();
      shader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(identity));
      shader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(identity));
      shader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
      shader->SetInstancedTextures(maxBindableTextures, &renderer->textureMap);
      int thisSize = this->GetCardsWithShader(shader);
      this->BindAndDrawAllFrontFaces(
        renderer,
        shader,
        maxBindableTextures,
        offset,
        thisSize,
        size, 
        zFlipped
      );
      offset+=thisSize;
    }
  }

  this->backingPlane.SetTransform(&backingPlaneTransform);
  this->backingPlane.Render(renderer);
  this->cardContainerPlane.SetTransform(&this->cardContainerTransform);
  this->cardContainerPlane.Render(renderer);
  this->coverOverflowBottom.SetTransform(&this->coverOverflowBottomTransform);
  this->coverOverflowBottom.Render(renderer);
  this->coverOverflowTop.SetTransform(&this->coverOverflowTopTransform);
  this->coverOverflowTop.Render(renderer);
  this->closeExpandedView.SetTransform(&this->closeExpandedTransform);
  this->closeExpandedView.Render(renderer);

  this->dirtyPosition = false;
  this->dirtyDisplay = false;

  this->lastCursorX = (int)cursor.cursorX;
  this->lastCursorY = (int)cursor.cursorY;
  this->wasInsideBoundary = insideHandBoundary;
}

const glm::mat4 ExpandedStackCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform*this->renderer->projMatrix*this->renderer->cameraMatrix);
}

ClickEvent ExpandedStackCardGroupRenderer::ProcessClick(CollisionInfo info) {
  if (info.innerCollision != nullptr && info.innerCollision->groupPointer == &this->closeExpandedView) {
    return this->closeExpandedView.ProcessClick(std::move(info));
  } else {
    std::cout << "Unsure what was clicked" << std::endl;
    return {};
  }
}

void ExpandedStackCardGroupRenderer::ProcessScroll(CollisionInfo info, double yOffset) {
  int rows = (this->cardsPointer->size()+cardsPerRow-1)/cardsPerRow; // equivalent to rounding up size/perRow
  this->scrollPosition = fmax(
    0,
    fmin(rows*CardRenderingData::cardHeightRatio*cardScaleXY + (rows-1) * verticalWhitespaceInContainer - this->height/renderer->GetAspectRatio(), this->scrollPosition - yOffset)
  );
  this->dirtyPosition = true;
  this->dirtyDisplay = true;
}

void ExpandedStackCardGroupRenderer::ReleaseClick() {
  this->closeExpandedView.ReleaseClick();
}

ClickEvent ExpandedStackCardGroupRenderer::ProcessPreClick(CollisionInfo info) {
  if (info.innerCollision && info.innerCollision->groupPointer == &this->closeExpandedView) {
    return this->closeExpandedView.ProcessPreClick(std::move(info));
  }
  return {};
}
