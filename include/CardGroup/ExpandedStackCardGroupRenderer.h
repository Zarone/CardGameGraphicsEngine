#pragma once

#include "../../include/CardGroup/CardGroupRenderer.h"
#include "../../include/SimpleRenderObjects/SimplePlane.h"
#include "../../include/CardGroup/CardGroup.h"

class ExpandedStackCardGroupRenderer : public CardGroupRenderer
{
private:
  // TODO: Implement onhover system with these
  //int lastCursorX;
  //int lastCursorY;
  //int lastClosestIndex;
  //int GetClosestCardIndex(
    //double projectedLeftBoundary,
    //double margin,
    //double xGap,
    //double xScale,
    //const CursorData& renderData,
    //int size
  //);
  //void UpdateHandPosition(
    //const CursorData& renderData,
    //bool insideHandBoundary,
    //double xGap,
    //double margin,
    //int size,
    //double whitespace,
    //double zGap
  //);
  //bool GetInsideHandBoundary(
    //Renderer* renderer,
    //const CursorData& renderData,
    //double horizontalOffset,
    //double verticalOffset,
    //bool& mouseMovedInBoundary,
    //double& xScale,
    //double& projectedLeftBoundary
  //);

  SimplePlane backingPlane;
  glm::mat4 backingPlaneTransform;
  SimplePlane cardContainerPlane;
  glm::mat4 cardContainerTransform;

  float width = 1.5f;
public:
  ExpandedStackCardGroupRenderer(
    Renderer* renderer
  );

  void UpdateCardPositions();

  const glm::mat4 WorldSpaceToThisSpace();
  void Render(Renderer* renderer); 
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;
};
