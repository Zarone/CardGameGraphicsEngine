#pragma once

#include "../include/Renderer.h"

class SceneObject;

typedef struct CollisionInfo {
  // if the scene object has multiple items
  // inside it, this identifies which one
  // was actually hit
  unsigned int collisionIndex; 

  // if the scene object has multiple groups
  // inside it, this identifies which one was 
  // hit. Individual collision indices are inside
  // this group.
  SceneObject* groupPointer;

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
  /**
  *
  * Checks collision against this scene object
  *
  * @param renderer The scene renderer needs to be passed in here
  * @param x The x-coordinate of the cursor on the screen
  * @param y The y-coordinate of the cursor on the screen
  * @param collisionZ The z-coordinate of the collision is stored inside this pointer
  * @param info Additional information about the collision is populated here
  *
  * @returns True if and only if there is a collision
  *
  */
  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const = 0;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
  virtual ClickEvent ProcessPreClick(CollisionInfo info) = 0;
  virtual void ProcessScroll(CollisionInfo info) {}
  virtual void ReleaseClick() = 0;
  virtual ~SceneObject() = default;
};
