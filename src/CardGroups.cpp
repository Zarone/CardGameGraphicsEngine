#include <glm/ext/matrix_transform.hpp>

#include "../include/CardGroup.h"
#include "../include/ErrorHandling.h"
#include "../include/shaders/allShaders.h"


CardGroup::CardGroup(
  TextureMap* textureMap, 
  glm::vec3 position, 
  float rotationX, 
  float rotationZ, 
  float scaleX, 
  float scaleY, 
  bool zFlipped
) 
: textureMap(textureMap), zFlipped(zFlipped), dirty(true)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::rotate(this->transform, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
  this->transform = glm::scale(this->transform, glm::vec3(scaleX, scaleY, zFlipped ? -1.0f : 1.0f));

  this->groupVao = VertexArray();
  this->groupVao.Bind();
  this->staticBuffer = VertexBuffer(CardRenderingData::cardPositions, 5*4*sizeof(float));
  this->staticBufferLayout = MemoryLayout();
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 3); // position
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 2); // texture uv coordinates
  this->staticBuffer.Bind();
  this->groupVao.AddBuffer(this->staticBuffer, this->staticBufferLayout);

  this->transformBuffer = VertexBuffer(NULL, estimatedMax*3*sizeof(float), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 2); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1); // rotation
  this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);

  this->cardShader = Shader(myShaders::cardVertex, myShaders::cardFragment);

  if (zFlipped) {
    this->textureMap->SetupBack();
  } else {
    for (CardItem& cardItem : this->cards) {
      Card& card = cardItem.card;
      this->textureMap->SetupCard(card.GetID());
    }
  }
}

void CardGroup::Render(unsigned int maxBindableTextures) {
  if (this->dirty) {
    // update buffer for relative position and rotation
  }

  int size = this->cards.size();
  int i = 0;

  this->groupVao.Bind();
  this->staticBuffer.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();
  this->cardShader.Bind();

  if (this->zFlipped) {
    GLCall(glDrawElementsInstanced(
      GL_TRIANGLES, 
      6, 
      GL_UNSIGNED_INT, 
      (const void*) 0,
      size
    ));
  }

  while (i < size) {
    i += maxBindableTextures;

    // bind all textures about to be used
    for (int j = i; j < i+maxBindableTextures; ++j) {
    }

    // update texture buffer using newly bound addresses

    GLCall(glDrawElementsInstanced(
      GL_TRIANGLES, 
      6, 
      GL_UNSIGNED_INT, 
      (const void*) (i*this->transformBufferLayout.GetStride()),
      fmin(maxBindableTextures, size-i)
    ));
  }
}
