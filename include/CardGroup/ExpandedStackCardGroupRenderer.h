#pragma once

#include "../../include/CardGroup/CardGroupRenderer.h"
#include "../../include/SimpleRenderObjects/SimplePlane.h"
#include "../../include/CardGroup/CardGroup.h"
#include "../../include/SimpleRenderObjects/BoundButton.h"

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

  float scrollPosition = 0;

  const float height = 1.5f;
  const float width = 1.7f;
  const float yTopPadding = 0.15f;

  const int cardsPerRow = 5;
  const double cardScaleXY = 0.28f;
  const float verticalWhitespaceInContainer = 0.05f;

  SimplePlane backingPlane;
  glm::mat4 backingPlaneTransform;
  SimplePlane cardContainerPlane;
  glm::mat4 cardContainerTransform;
  SimplePlane coverOverflowBottom;
  glm::mat4 coverOverflowBottomTransform;
  SimplePlane coverOverflowTop;
  glm::mat4 coverOverflowTopTransform;
  BoundButton closeExpandedView;
  glm::mat4 closeExpandedTransform;

public:
  ExpandedStackCardGroupRenderer(
    Renderer* renderer,
    std::function<void()> onClose
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
  void ProcessScroll(CollisionInfo info, double yOffset);
  ClickEvent ProcessClick(CollisionInfo info);
  void ReleaseClick();
};
