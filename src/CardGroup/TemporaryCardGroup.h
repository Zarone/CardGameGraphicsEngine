#pragma once
#include "CardGroup.h"
#include "StackedCardGroupRenderer.h"
#include "ExpandedStackCardGroupRenderer.h"

class TemporaryCardGroup : public CardGroup
{
private:
  BoundButton showButton;
  ExpandedStackCardGroupRenderer expandedRenderer;
  bool isExpanded = true;
  bool isEnabled = false;
  void MoveToGroup(int index, CardGroup* to);
public:
  TemporaryCardGroup(
    Renderer* renderer,
    glm::vec3 position
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
  void EnableWithCards(const std::vector<unsigned int>);
  void Disable();
  bool const GetIsExpanded();

};
