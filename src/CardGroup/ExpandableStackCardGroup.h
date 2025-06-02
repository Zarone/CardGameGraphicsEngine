#pragma once
#include "CardGroup.h"
#include "StackedCardGroupRenderer.h"
#include "ExpandedStackCardGroupRenderer.h"

class ExpandableStackCardGroup : public CardGroup
{
private:
  StackedCardGroupRenderer stackRenderer;
  ExpandedStackCardGroupRenderer expandedRenderer;
  bool isExpanded = false;
  void MoveToGroup(int index, CardGroup* to);
public:
  ExpandableStackCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    bool zFlipped
  );

  void SetDirtyPosition(bool dirty);
  const glm::mat4 WorldSpaceToThisSpace();
  void OnClose();
  
  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
  void ProcessScroll(CollisionInfo info, double yOffset);
  ClickEvent ProcessClick(CollisionInfo info);
  ClickEvent ProcessPreClick(CollisionInfo info);
  void ReleaseClick();
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  bool const GetIsExpanded();
};
