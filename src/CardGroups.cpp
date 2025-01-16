#include <glm/ext/matrix_transform.hpp>

#include "../include/CardGroup.h"
#include "../include/shaders/allShaders.h"


CardGroup::CardGroup(glm::vec3 position, float rotationX, float rotationY, float scaleX, float scaleY) {
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::rotate(this->transform, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
  this->transform = glm::scale(this->transform, glm::vec3(scaleX, scaleY, 1.0f));

  this->dirty = true;

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

  // request but don't yet bind textures
}

/**
*
* Make single buffer, with constant position data
* On render, if dirty bit, update buffer for relative position and rotation
*
*/
