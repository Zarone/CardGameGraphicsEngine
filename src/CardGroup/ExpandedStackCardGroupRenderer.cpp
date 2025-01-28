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
          .color=glm::vec4(0.3f,0.3f,0.3f,0.9f)
        }
      )
    )
  ),
  CardGroupRenderer(false) 
{
  // render straight to screen without projection
  this->transform = glm::inverse(renderer->projMatrix*renderer->cameraMatrix);
  this->backingPlaneTransform = glm::scale(this->transform, glm::vec3(1.9f, 1.9f, 1.0f));

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

void ExpandedStackCardGroupRenderer::Render(
  Renderer* renderer
) {
  this->backingPlane.SetTransform(&backingPlaneTransform);
  this->backingPlane.Render(renderer);
  std::cout << "Unimplemented render function for ExpandedStackCardGroup" << std::endl;
}

const glm::mat4 ExpandedStackCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform);
}
