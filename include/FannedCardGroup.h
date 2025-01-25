#include "../include/CardGroup.h"

class FannedCardGroup : public CardGroup 
{
private:
  glm::mat4 transform;

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

  // animated on hover if true
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
    Shader* shader,
    int maxBindableTextures,
    int offset,
    int groupSize,
    int totalSize
  );

  bool GetInsideHandBoundary(
    Renderer* renderer,
    const CursorData& renderData,
    double horizontalOffset,
    double verticalOffset,
    bool& mouseMovedInBoundary,
    double& xScale,
    double& projectedLeftBoundary
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

  #ifdef DEBUG

  // just where the cards are
  SimplePlane strictBackingPlane;
  glm::mat4 strictBackingPlaneTransform;

  // doesn't subtract margin
  SimplePlane extendedBackingPlane;
  glm::mat4 extendedBackingPlaneTransform;

  #endif

  // doesn't subtract spacing margin, includes
  // horizontal/vectical margin
  SimplePlane fullBackingPlane;
  glm::mat4 fullBackingPlaneTransform;
public:
  FannedCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    float width, 
    bool zFlipped,
    bool isHand
  );

  void MoveToGroup(int index, FannedCardGroup* to);

  void UpdateTick(double deltaTime);

  void Render(Renderer* renderer); 

  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;

};
