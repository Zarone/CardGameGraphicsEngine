#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "CardRenderingData.h"
#include "Card.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

typedef struct CardItem
{
  Card card;
  CardRenderingData cardRenderingData;
} CardItem;

class CardGroup
{
private:
  glm::mat4 transform;
  std::vector<CardItem> cards = {};
  VertexArray groupVao;
  VertexBuffer staticBuffer;
  IndexBuffer indexBuffer;
  MemoryLayout staticBufferLayout;
  VertexBuffer transformBuffer;
  MemoryLayout transformBufferLayout;
  Shader cardShader;
  
  // keeps track of whether or not 
  // we need to update position info 
  // before the next render.
  bool dirty; 

  const static unsigned int estimatedMax;
public:
  CardGroup(glm::vec3 position, float rotationX, float rotationY, float scaleX, float scaleY);
  void Render();
};
