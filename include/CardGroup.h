#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "CardRenderingData.h"
#include "Card.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureMap.h"
#include "SimplePlane.h"

typedef struct CardItem {
  Card card;
  CardRenderingData renderData;
} CardItem;
 
typedef struct CardTransformVertex {
  float x;
  float y;
  float z;
  float rotationZ;
} CardTransformVertex;

typedef struct CardVertex {
  float x;
  float y;
  float z;
  float u;
  float v;
} CardVertex;

typedef struct TextureVertex {
  unsigned int index;
} TextureVertex;

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

  bool isHand;

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

  int GetClosestCardIndex(
    double projectedLeftBoundary,
    double margin,
    double xGap,
    double xScale,
    const CursorData& renderData,
    int size
  );

  void UpdateHandPosition(
    const CursorData& renderData,
    bool insideHandBoundary,
    double xGap,
    double margin,
    int size,
    double whitespace,
    double zGap
  );

  void BindAndDrawAllFrontFaces(
    Renderer* renderer,
    int maxBindableTextures,
    int size
  );

  // just where the cards are
  SimplePlane strictBackingPlane;
  glm::mat4 strictBackingPlaneTransform;

  // doesn't subtract margin
  SimplePlane extendedBackingPlane;
  glm::mat4 extendedBackingPlaneTransform;

  // doesn't subtract spacing margin, includes
  // horizontal/vectical margin
  SimplePlane fullBackingPlane;
  glm::mat4 fullBackingPlaneTransform;

public:
  CardGroup(
    glm::vec3 position, 
    float rotationX, 
    float width, 
    bool zFlipped,
    bool isHand
  );

  void GroupPositionToScreen(
    Renderer* renderer, 
    glm::vec4& src, 
    glm::vec2& dest
  ) const;

  void GroupPositionTo3DScreen(
    Renderer* renderer, 
    glm::vec4& src, 
    glm::vec3& dest
  ) const;

  bool GetInsideHandBoundary(
    Renderer* renderer,
    const CursorData& renderData,
    double horizontalOffset,
    double verticalOffset,
    bool& mouseMovedInBoundary,
    double& xScale,
    double& projectedLeftBoundary
  );

  /*
  *
  * @brief Must be called after cards are added to group
  *
  */
  void PrepareTextures(TextureMap* textureMap);

  void Render(Renderer* renderer);

  void AddCard(unsigned int id);

  Card GetCard(unsigned int index);

  void UpdateTick(double deltaTime);

  /*
  *
  * @brief Checks for collision between this card group
  * and the cursor
  *
  */
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;

  void MoveToGroup(int index, CardGroup* to);
};
