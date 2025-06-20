#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "StackedCardGroupRenderer.h"

StackedCardGroupRenderer::StackedCardGroupRenderer(
  Renderer* renderer,
  glm::vec3 position, 
  float rotationX, 
  bool zFlipped
): 
  CardGroupRenderer(renderer, zFlipped)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::translate(this->transform, position);
  this->transform = glm::translate(this->transform, glm::vec3(-0.5f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));

  this->groupVao = VertexArray();
  this->groupVao.Bind();

  this->staticBuffer = VertexBuffer(CardRenderingData::cardPositions, 4*sizeof(CardVertex));
  this->staticBufferLayout = MemoryLayout();
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 3); // position
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 2); // texture uv coordinates
  this->groupVao.AddBuffer(this->staticBuffer, this->staticBufferLayout);

  this->transformBuffer = VertexBuffer(NULL, RenderMax*sizeof(CardTransformVertex), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 3, true); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // rotation
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // scaleXY
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, RenderMax*sizeof(TextureVertex), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
};

void StackedCardGroupRenderer::Render(Renderer* renderer) {

  int totalSize = cardsPointer->size();
  if (totalSize==0) return;
  int renderSize = fmin(RenderMax, totalSize);

  if (this->dirtyPosition) {
    const float zGap = 0.01f;
    // set all cards to position 0
    for (int i = 0; i < totalSize; i++) {
      CardRenderingData& thisCard = (*cardsPointer)[i].renderData;

      thisCard.SetActualTransform(
        glm::vec3(
          0.5f,
          0.0f,
          zGap*i
        ),
        0.0f,
        1.0f
      );
    }
  }

  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);
   
  int index = 0;
  std::set<Shader*> shaders = {};
  for (CardItem& card : *this->cardsPointer){
    if (index >= totalSize-renderSize) shaders.insert(card.renderData.shader);
    ++index;
  }

  // if we need to update the
  // displayed position
  if (this->dirtyDisplay) {
    // update buffer for relative position and rotation
    float buffer[transformVertexSize*renderSize];

    // load display info from cards into buffer
    int offset = 0;
    for (Shader* const& shader : shaders) {
      int size = this->GetCardsWithShader(shader, totalSize-renderSize);
      this->LoadPositions(buffer, shader, offset, size, totalSize, true);
      offset+=size;
    }

    this->transformBuffer.OverwriteData(buffer, 0, renderSize*sizeof(CardTransformVertex));
  }

  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;

  this->groupVao.Bind();
  this->staticBuffer.Bind();
  this->indexBuffer.Bind();

  this->PrepareTextures(&renderer->textureMap, totalSize-renderSize, totalSize);

  if (renderSize != 0) {

    int offset = 0;
    for (Shader* const& shader : shaders) {
      shader->Bind();
      shader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
      shader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
      shader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
      shader->SetUniform1i("u_applyPerspective", true);
      shader->SetInstancedTextures(renderer->maxBindableTextures, &renderer->textureMap);
      int size = this->GetCardsWithShader(shader, totalSize-renderSize);
      this->BindAndDrawAllFrontFaces(
        renderer,
        shader,
        renderer->maxBindableTextures,
        offset,
        size,
        totalSize, 
        zFlipped,
        true
      );
      offset+=size;
    }

  }
  this->dirtyDisplay = false;
  this->dirtyPosition = false;
}

const glm::mat4 StackedCardGroupRenderer::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform);
}

bool StackedCardGroupRenderer::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ,
  CollisionInfo* collisionInfo
) const {
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
      1.0f, 
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
      1.0f, 
      -0.5f*CardRenderingData::cardHeightRatio, 
      0.0f, 
      1.0f
    ),
    1.0f,
    &leftBoundary,
    &zAtCursor,
    &xScale
  )) return false;
  else {
    *collisionZ = zAtCursor;
    return true;
  }
}
