#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/CardGroup/StackedCardGroupRenderer.h"

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

  this->transformBuffer = VertexBuffer(NULL, 2*sizeof(CardTransformVertex), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 3, true); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // rotation
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // scaleXY
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, 2*sizeof(TextureVertex), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
};

void StackedCardGroupRenderer::Render(Renderer* renderer) {
  const unsigned int renderMax = 2;

  int totalSize = cardsPointer->size();
  if (totalSize==0) return;
  int renderSize = fmin(renderMax, totalSize);

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
   
  // if we need to update the
  // displayed position
  if (this->dirtyDisplay) {

    // update buffer for relative position and rotation
    float buffer[transformVertexSize*renderSize];

    // load display info from cards into buffer
    Shader* cardShader = renderer->GetShader("cardShader");
    this->LoadPositions(buffer, cardShader, 0, renderSize-this->highlightedCards, totalSize, true);
    cardShader = renderer->GetShader("highlightCardShader");
    this->LoadPositions(buffer, cardShader, renderSize-this->highlightedCards, this->highlightedCards, totalSize, true);

    this->transformBuffer.OverwriteData(buffer, 0, renderSize*sizeof(CardTransformVertex));
  }

  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;

  this->groupVao.Bind();
  this->staticBuffer.Bind();
  this->indexBuffer.Bind();
  
  Shader* cardShader = renderer->GetShader("cardShader");
  cardShader->Bind();
  cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
  cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
  cardShader->SetInstancedTextures(renderer->maxBindableTextures, &renderer->textureMap);

  this->PrepareTextures(&renderer->textureMap, totalSize-renderSize, totalSize);

  if (renderSize != 0) {
    this->BindAndDrawAllFrontFaces(
      renderer,
      cardShader,
      renderer->maxBindableTextures,
      0,
      renderSize-this->highlightedCards,
      renderSize, zFlipped
    );
    if (this->highlightedCards != 0) {
      cardShader = renderer->GetShader("highlightCardShader");
      cardShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
      cardShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
      cardShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));
      cardShader->SetInstancedTextures(renderer->maxBindableTextures, &renderer->textureMap);
      this->BindAndDrawAllFrontFaces(
        renderer,
        cardShader,
        renderer->maxBindableTextures,
        renderSize-this->highlightedCards,
        this->highlightedCards,
        renderSize, zFlipped
      );
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
    return true;
  }
}
