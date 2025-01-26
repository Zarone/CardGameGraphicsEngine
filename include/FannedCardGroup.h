#pragma once
#include "../include/CardGroup.h"

class FannedCardGroup : public CardGroup 
{
private:
  glm::mat4 transform;

  // animated on hover if true
  bool isHand;

  float width;

  int lastCursorX;
  int lastCursorY;
  bool wasInsideBoundary;
  int lastClosestIndex;

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

  const glm::mat4 WorldSpaceToThisSpace();

  void MoveToGroup(int index, CardGroup* to);

  void Render(Renderer* renderer); 

  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;

};
