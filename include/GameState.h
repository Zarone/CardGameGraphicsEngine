#pragma once

#include "../include/Renderer.h"
#include "../include/CardGroup.h"

class GameState : public SceneObject
{
protected:
  std::vector<SceneObject*> nonCardGroups;
  std::vector<CardGroup*> cardGroups;
public:
  GameState();
  void AddObject(SceneObject* group);
  void AddObject(CardGroup* group);
  virtual void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
  virtual ClickEvent ProcessPreClick(CollisionInfo info);
  virtual void ReleaseClick();
};
