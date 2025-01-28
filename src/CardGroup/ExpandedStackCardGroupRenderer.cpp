#include <glm/gtc/type_ptr.hpp>
#include "../../include/CardGroup/ExpandedStackCardGroupRenderer.h"

ExpandedStackCardGroupRenderer::ExpandedStackCardGroupRenderer(
  Renderer* renderer
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
  CardGroupRenderer(false) 
{
  // render straight to screen without projection
  this->transform = glm::inverse(renderer->projMatrix*renderer->cameraMatrix);
  this->backingPlaneTransform = glm::translate(this->transform, glm::vec3(0.0f, 0.0f, 0.01f));
  this->backingPlaneTransform = glm::scale(this->backingPlaneTransform, glm::vec3(2.0f, 2.0f, 1.0f));
  this->cardContainerTransform = glm::translate(this->transform, glm::vec3(0.0f, (2.0f-this->width)/2.0f-0.05f, 0.0f));
  this->cardContainerTransform = glm::scale(this->cardContainerTransform, glm::vec3(1.9f, this->width, 1.0f));

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

//void ExpandedStackCardGroupRenderer::UpdateHandPosition(
  //const CursorData& renderData,
  //bool insideHandBoundary,
  //double xGap,
  //double margin,
  //int size,
  //double whitespace,
  //double zGap
//) {
//}

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
  std::cout << "Unimplemented collision function for ExpandedStackCardGroup" << std::endl;
  return false;
}

void ExpandedStackCardGroupRenderer::UpdateCardPositions() {
  //const float rotationPerCard = glm::radians(10.0f);
  //float yOffset = 0.0f;
  //int perRow = (int)this->width;
  //int size = this->cardsPointer->size();
  //for (int i = 0; i < size; i++) {
    //int cardIndex = i;

    //CardRenderingData& thisCard = (*cardsPointer)[cardIndex].renderData;

    //thisCard.SetActualTransform(
      //glm::vec3(
        //(float)cardIndex*xGap+0.5f+margin,
        //0.0f,
        //(float)cardIndex*zGap+0.01f
      //),
      //(float)(cardIndex-(float)(size-1)/2.0f)/size*rotationPerCard
    //);
  //}
}

void ExpandedStackCardGroupRenderer::Render(
  Renderer* renderer
) {
  this->backingPlane.SetTransform(&backingPlaneTransform);
  this->backingPlane.Render(renderer);
  this->cardContainerPlane.SetTransform(&this->cardContainerTransform);
  this->cardContainerPlane.Render(renderer);
  std::cout << "Unimplemented render function for ExpandedStackCardGroup" << std::endl;
}

const glm::mat4 ExpandedStackCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform);
}
