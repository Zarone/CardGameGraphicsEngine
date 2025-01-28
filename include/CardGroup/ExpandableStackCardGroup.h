#pragma once
#include "CardGroup.h"
#include "StackedCardGroupRenderer.h"

class ExpandableStackCardGroup : public CardGroup
{
private:
  StackedCardGroupRenderer stackRenderer;
  //ExpandedCardGroupRenderer expandedRenderer;
  bool isExpanded = false;
public:
  ExpandableStackCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    bool zFlipped
  );

  void SetNumHighlightedCards(int num);
  void SetDirtyPosition(bool dirty);
  const glm::mat4 WorldSpaceToThisSpace();
  void MoveToGroup(int index, CardGroup* to);
  
  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
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
};
