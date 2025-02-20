#include <glm/gtc/type_ptr.hpp>
#include "../../include/CardGroup/ExpandedStackCardGroupRenderer.h"

ExpandedStackCardGroupRenderer::ExpandedStackCardGroupRenderer(
  Renderer* renderer,
  std::function<void()> onClose
) : 
  //lastCursorX(0),
  //lastCursorY(0),
  //lastClosestIndex(-1),
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

  this->backingPlaneTransform = glm::translate(this->transform, glm::vec3(0.0f, 0.0f, 0.01f));
  this->backingPlaneTransform = glm::scale(this->backingPlaneTransform, glm::vec3(2.0f, 2.0f, 1.0f));
  this->cardContainerTransform = glm::translate(this->transform, glm::vec3(0.0f, (2.0f-this->height)/2.0f-this->yTopPadding, 0.0f));
  this->cardContainerTransform = glm::scale(this->cardContainerTransform, glm::vec3(this->width, this->height, 1.0f));
  float buttonHeight = 0.25f;
  this->closeExpandedTransform = glm::translate(this->transform, glm::vec3(0.0f, 1-this->height-this->yTopPadding-buttonHeight/2, 0.0f));
  this->closeExpandedTransform = glm::scale(this->closeExpandedTransform, glm::vec3(0.5f, 0.25f, 1.0f));
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
  std::cout << "Unimplemented collision function for cards of ExpandedStackCardGroup" << std::endl;
  return closeExpandedView.CheckCollision(renderer, x, y, collisionZ, collisionInfo);
}

void ExpandedStackCardGroupRenderer::UpdateCardPositions() {
  const float rotationPerCard = glm::radians(10.0f);
  float yOffset = 0.0f;
  const double scaleXY = 0.28f;
  int size = this->cardsPointer->size();
  const float whitespace = 0.05f;
  int perRow = 5;
  const float xGap = (this->width-1.0f*scaleXY)/(perRow-1.0f);

  for (int i = 0; i < size; i++) {
    CardRenderingData& thisCard = (*cardsPointer)[i].renderData;

    thisCard.SetActualTransform(
      glm::vec3(
        (float)scaleXY*0.5f+(i%perRow)*xGap,
        (float)(-i/perRow)/2.0f
          +(
            this->height/2.0f
          )/renderer->GetAspectRatio()
          -scaleXY*0.5f*CardRenderingData::cardHeightRatio,
        (float)-0.001f
      ),
      (float)((i%perRow)-(float)(perRow-1)/2.0f)/perRow*rotationPerCard,
      scaleXY
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
  this->closeExpandedView.SetTransform(&this->closeExpandedTransform);
  this->closeExpandedView.Render(renderer);

  this->dirtyPosition = false;
  this->dirtyDisplay = false;
}

const glm::mat4 ExpandedStackCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform*this->renderer->projMatrix*this->renderer->cameraMatrix);
}

ClickEvent ExpandedStackCardGroupRenderer::ProcessClick(CollisionInfo info) {
  std::cout << (info.groupPointer == (void*)&this->closeExpandedView) << std::endl;
}

void ExpandedStackCardGroupRenderer::ReleaseClick() {
  this->closeExpandedView.ReleaseClick();
}
