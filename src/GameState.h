#pragma once

#include "Renderer.h"
#include "CardGroup/CardGroup.h"
#include <map>

class GameState : public SceneObject
{
protected:
  std::vector<SceneObject*> nonCardGroups;
  std::vector<CardGroup*> cardGroups;
  std::map<unsigned int, CardGroup*> cardGroupMap;
public:
  GameState();
  void AddObject(SceneObject* group);

  /*
  *
  * Adds a new card group to the current game state for the sake of rendering and such
  *
  * @param group The new card group
  * @param pile The id for the pile (recommended to use enum for this arg)
  *
  */
  void AddCardGroup(CardGroup* group, unsigned int pile);

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
