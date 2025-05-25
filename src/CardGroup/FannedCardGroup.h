#pragma once
#include "CardGroup.h"
#include "FannedCardGroupRenderer.h"

class FannedCardGroup : public CardGroup
{
private:
  FannedCardGroupRenderer thisRenderer;
public:
  FannedCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    float width, 
    bool zFlipped,
    bool isHand
  );

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

  void SetNumHighlightedCards(int num);
  void SetDirtyPosition(bool dirty);
  const glm::mat4 WorldSpaceToThisSpace();
  void MoveToGroup(int index, CardGroup* to);
};
