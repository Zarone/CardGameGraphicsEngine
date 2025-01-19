#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "CardRenderingData.h"
#include "Card.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureMap.h"

typedef struct RenderData {
  bool isHand;
  double cursorX;
  double cursorY;
} RenderData;

typedef struct CardItem {
  Card card;
  CardRenderingData renderData;
} CardItem ;

class CardGroup
{
private:
  const static unsigned int estimatedMax;

  glm::mat4 transform;

  std::vector<CardItem> cards = {};

  VertexArray groupVao;
  VertexBuffer staticBuffer;
  IndexBuffer indexBuffer;
  MemoryLayout staticBufferLayout;
  VertexBuffer transformBuffer;
  MemoryLayout transformBufferLayout;
  unsigned int transformEndAttribID;
  VertexBuffer textureIDBuffer;
  MemoryLayout textureIDBufferLayout;
  unsigned int textureEndAttribID;
  Shader cardShader;

  TextureMap* textureMap;

  // if this is true, only the back 
  // of the cards is shown
  bool zFlipped; 

  float width;
  
  // keeps track of whether or not 
  // we need to update position info 
  // before the next render.
  bool dirtyDisplay; 
  bool dirtyPosition;

  int lastCursorX;
  int lastCursorY;
  bool wasInsideBoundary;
  int lastClosestIndex;

  void DrawElements(int size);
public:
  CardGroup(
    TextureMap* textureMap, 
    glm::vec3 position, 
    float rotationX, 
    float rotationZ, 
    float width, 
    bool zFlipped
  );

  bool GetInsideHandBoundary(
    Renderer& renderer,
    const RenderData& renderData,
    double horizontalOffset,
    double verticalOffset,
    bool& mouseMovedInBoundary,
    double& xScale,
    double& projectedLeftBoundary
  );

  int GetClosestCardIndex(
    double projectedLeftBoundary,
    double margin,
    double xGap,
    double xScale,
    const RenderData& renderData,
    int size
  );

  void UpdateHandPosition(
    const RenderData& renderData,
    bool insideHandBoundary,
    double xGap,
    double margin,
    int size,
    double whitespace,
    double zGap
  );

  void BindAndDrawAllFrontFaces(
    int maxBindableTextures,
    int transformVertexSize,
    int size
  );

  void Render(Renderer& renderer, const RenderData& renderData);
  void AddCard(unsigned int id);

  /*
  *
  * @brief Must be called after cards are added to group
  *
  */
  void PrepareTextures();

  void UpdateTick(double deltaTime);
};
