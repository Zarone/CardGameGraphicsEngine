#pragma once

#include "../include/Renderer.h"
#include "../include/CardGroup.h"

class GameState : public SceneObject
{
private:
  std::vector<SceneObject*> allGroups;
public:
  GameState();
  void AddObject(SceneObject* group);
  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
};
