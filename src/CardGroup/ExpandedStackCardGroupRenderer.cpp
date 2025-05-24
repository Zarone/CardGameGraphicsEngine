#include <glm/gtc/type_ptr.hpp>
#include "../../include/CardGroup/ExpandedStackCardGroupRenderer.h"

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
          .shader=renderer->GetShader("basicShader"),
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
          .shader=renderer->GetShader("basicShader"),
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
          .shader=renderer->GetShader("basicShader"),
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
          .shader=renderer->GetShader("basicShader"),
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
        .shader=renderer->GetShader("basicShader"),
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

//int FannedCardGroupRenderer::GetClosestCardIndex(
  //double projectedLeftBoundary,
  //double margin,
  //double xGap,
  //double xScale,
  //const CursorData& renderData,
  //int size
//) {
//}

bool ExpandedStackCardGroupRenderer::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* collisionInfo
) const {

  CollisionInfo closeButtonCollisionInfo;
  double closeButtonCollisionZ;
  CollisionInfo cardContainerCollisionInfo;
  double cardContainerCollisionZ;
  bool closeButtonCollision = closeExpandedView.CheckCollision(renderer, x, y, &closeButtonCollisionZ, &closeButtonCollisionInfo);
  bool cardContainerCollision = cardContainerPlane.CheckCollision(renderer, x, y, &cardContainerCollisionZ, &cardContainerCollisionInfo);

  if(!cardContainerCollision && !closeButtonCollision) {
    return false;
  } else if (!cardContainerCollision || (closeButtonCollision && closeButtonCollisionZ <= cardContainerCollisionZ)) {
    *collisionZ = closeButtonCollisionZ;
    *collisionInfo = closeButtonCollisionInfo;
    return true;
  } else if (!closeButtonCollision || (cardContainerCollision && cardContainerCollisionZ <= closeButtonCollision)) {
    std::cout << "Unimplemented collision function for cards of ExpandedStackCardGroup" << std::endl;
    *collisionZ = cardContainerCollisionZ;

    // So it's sort of unintuitive, but it makes more sense for this object
    // to just handle the collision event in the expandedstackrenderer itself.
    // For example, this scroll method should be called instead of the cardContainer plane,
    // since that's just a plane with no event handlers.
    collisionInfo->groupPointer = (SceneObject*) this;

    return true;
  } else {
    ASSERT(false);
    return false;
  }

}

void ExpandedStackCardGroupRenderer::UpdateCardPositions() {
  const float rotationPerCard = glm::radians(2.0f);
  float yOffset = 0.0f;
  int size = this->cardsPointer->size();
  const float xGap = (this->width-1.0f*cardScaleXY)/(cardsPerRow-1.0f);

  for (int i = 0; i < size; i++) {
    CardRenderingData& thisCard = (*cardsPointer)[i].renderData;

    float cardHeight = cardScaleXY*CardRenderingData::cardHeightRatio;
    float halfContainerHeight = this->height/2.0f/renderer->GetAspectRatio();

    thisCard.SetActualTransform(
      glm::vec3(
        (float)cardScaleXY*0.5f+(i%cardsPerRow)*xGap,
        (float)(-i/cardsPerRow)*(cardHeight+verticalWhitespaceInContainer)
          + halfContainerHeight
          - cardHeight/2.0f
          + this->scrollPosition,
        (float)-0.001f
      ),
      (float)((i%cardsPerRow)-(float)(cardsPerRow-1)/2.0f)/cardsPerRow*rotationPerCard,
      cardScaleXY
    );
  }
}

void ExpandedStackCardGroupRenderer::Render(
  Renderer* renderer
) {
  int size = this->cardsPointer->size();

  if (this->dirtyPosition) {
    this->UpdateCardPositions();
  }

  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);
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
  glm::mat4 identity = glm::identity<glm::mat4>();
  cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(identity));
  cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(identity));
  //cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
  //cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
  cardShader->SetInstancedTextures(maxBindableTextures, &renderer->textureMap);

  this->PrepareTextures(&renderer->textureMap, 0, size);

  // rendering
  if (size != 0) {
    this->BindAndDrawAllFrontFaces(
      renderer,
      cardShader,
      maxBindableTextures,
      0,
      size-this->highlightedCards,
      size, false
    );
    if (this->highlightedCards != 0) {
      cardShader = renderer->GetShader("highlightCardShader");
      cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(identity));
      cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(identity));
      //cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
      //cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
      cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
      cardShader->SetInstancedTextures(maxBindableTextures, &renderer->textureMap);
      this->BindAndDrawAllFrontFaces(
        renderer,
        cardShader,
        maxBindableTextures,
        size-this->highlightedCards,
        this->highlightedCards,
        size, false
      );
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
}

const glm::mat4 ExpandedStackCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform*this->renderer->projMatrix*this->renderer->cameraMatrix);
}

ClickEvent ExpandedStackCardGroupRenderer::ProcessClick(CollisionInfo info) {
  std::cout << "Inside ExpandedStackCardGroupRenderer" << std::endl;
  std::cout << (info.groupPointer == (void*)&this->closeExpandedView) << std::endl;
  return {};
}

void ExpandedStackCardGroupRenderer::ProcessScroll(CollisionInfo info, double yOffset) {
  int rows = (this->cardsPointer->size()+cardsPerRow-1)/cardsPerRow; // equivalent to rounding up size/perRow
  this->scrollPosition = fmax(
    0,
    fmin(rows*CardRenderingData::cardHeightRatio*cardScaleXY + (rows-1) * verticalWhitespaceInContainer - this->height/renderer->GetAspectRatio(), this->scrollPosition - yOffset)
  );
  std::cout << "Scroll Position: " << this->scrollPosition << std::endl;
  this->dirtyPosition = true;
  this->dirtyDisplay = true;
}

void ExpandedStackCardGroupRenderer::ReleaseClick() {
  this->closeExpandedView.ReleaseClick();
}
