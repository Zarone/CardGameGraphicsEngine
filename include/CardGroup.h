#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "CardRenderingData.h"
#include "Card.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureMap.h"

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
  TextureMap* textureMap;
  bool zFlipped;
  
  // keeps track of whether or not 
  // we need to update position info 
  // before the next render.
  bool dirty; 

  const static unsigned int estimatedMax;
public:
  CardGroup(
    TextureMap* textureMap, 
    glm::vec3 position, 
    float rotationX, 
    float rotationZ, 
    float scaleX, 
    float scaleY,
    bool zFlipped
  );
  void Render(unsigned int maxBindableTextures);
};
