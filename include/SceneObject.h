#pragma once

#include "../include/Renderer.h"
#include "../include/CursorData.h"

typedef struct CollisionInfo {
  // if the scene object has multiple items
  // inside it, this identifies which one
  // was actually hit
  int collisionIndex; 

  // if the scene object has multiple groups
  // inside it, this identifies which one was 
  // hit. Individual collision indices are inside
  // this group.
  void* groupPointer;

} CollisionInfo;

// this class is basically used when I want
// to send a command from an object back to
// the scene. For example, change the scene
// on button press.
typedef struct ClickEvent {
  bool sceneSwap; 
  unsigned int sceneIndex;
} ClickEvent;

class SceneObject
{
public:
  virtual void Render(Renderer* renderer) = 0;
  virtual void UpdateTick(double deltaTime) = 0;
  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const = 0;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
  virtual ~SceneObject() = default;
};
