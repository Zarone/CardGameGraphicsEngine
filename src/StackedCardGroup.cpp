#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/StackedCardGroup.h"

StackedCardGroup::StackedCardGroup(
  Renderer* renderer,
  glm::vec3 position, 
  float rotationX, 
  bool zFlipped
): 
  CardGroup(zFlipped)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::translate(this->transform, position);
  this->transform = glm::translate(this->transform, glm::vec3(-0.5f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::scale(this->transform, glm::vec3(1.0f, 1.0f, zFlipped ? -1.0f : 1.0f));

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
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, 2*sizeof(TextureVertex), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
};

void StackedCardGroup::Render(Renderer* renderer) {
  const unsigned int renderMax = 2;

  int totalSize = this->cards.size();
  if (totalSize==0) return;
  int renderSize = fmin(renderMax, totalSize);
  glm::mat4& projMatrix = renderer->projMatrix;
  glm::mat4& camMatrix = renderer->cameraMatrix;

  this->groupVao.Bind();
  this->staticBuffer.Bind();
  this->indexBuffer.Bind();
  
  int textureBuffer[renderer->maxBindableTextures];
  Shader* topShader = this->cards[0].renderData.shader;
  topShader->Bind();

  const int transformVertexSize = sizeof(CardTransformVertex)/sizeof(float);

  // set all cards to position 0
  for (int i = 0; i < totalSize; i++) {
    CardRenderingData& thisCard = cards[i].renderData;

    thisCard.SetActualTransform(
      glm::vec3(
        0.5f,
        0.0f,
        0.0f
      ),
      0.0f 
    );
  }
   

  // if we need to update the
  // displayed position
  if (this->dirtyDisplay) {

    // update buffer for relative position and rotation
    float buffer[transformVertexSize*renderSize];

    // load display info from cards into buffer
    Shader* cardShader = renderer->GetShader("cardShader");
    this->LoadPositions(buffer, cardShader, 0, renderSize-this->highlightedCards, renderSize, true);
    cardShader = renderer->GetShader("highlightCardShader");
    this->LoadPositions(buffer, cardShader, renderSize-this->highlightedCards, this->highlightedCards, renderSize, true);

    this->transformBuffer.OverwriteData(buffer, 0, renderSize*sizeof(CardTransformVertex));
  }

  if (renderSize != 0) {
    if (this->zFlipped) {
      topShader->SetInstancedTextures(renderer->maxBindableTextures, &renderer->textureMap);
      topShader->SetUniform4fv("u_projMatrix", false, glm::value_ptr(projMatrix));
      topShader->SetUniform4fv("u_cameraMatrix", false, glm::value_ptr(camMatrix));
      topShader->SetUniform4fv("u_modelMatrix", false, glm::value_ptr(this->transform));

      int boundTo = renderer->textureMap.RequestBind(renderer->maxBindableTextures, "back");
      int boundToArr[renderMax];
      for (int i = 0; i < renderMax; ++i) {
        boundToArr[i] = boundTo;
      }

      long transformOffset = fmax((totalSize-renderSize), 0);

      this->textureIDBuffer.OverwriteData(
        boundToArr, 
        0,
        renderSize*sizeof(GLint)
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

      GLCall(glDrawElementsInstanced(
        GL_TRIANGLES, 
        6, 
        GL_UNSIGNED_INT, 
        nullptr,
        renderSize
      ));
    } else {
      std::cout << "UNIMPLEMENTED STACKED CARD GROUP" << std::endl;
    }
  }
}

const glm::mat4 StackedCardGroup::WorldSpaceToThisSpace() {
  return glm::inverse(this->transform);
}

bool StackedCardGroup::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ,
  CollisionInfo* collisioInfo
) const {
  return false;
}
