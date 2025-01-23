#include "../include/GameState.h"

GameState::GameState() {
}

void GameState::AddObject(SceneObject* group) { 
  allGroups.push_back(group);
}

void GameState::Render(Renderer* renderer) {
  for (SceneObject* group : this->allGroups) {
    group->Render(renderer);
  }
}

void GameState::UpdateTick(double deltaTime) {
  for (SceneObject* group : this->allGroups) {
    group->UpdateTick(deltaTime);
  }
}

bool GameState::CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* info) const {
  double minZ = MAXFLOAT;
  CollisionInfo collisionInfo = {
    .groupPointer = nullptr
  };

  double tempZ;
  CollisionInfo tempInfo;
  bool objectCollision = false;

  for (SceneObject* group : allGroups) {
    objectCollision = group->CheckCollision(renderer, x, y, &tempZ, &tempInfo);
    if (objectCollision && tempZ < minZ) {
      minZ = tempZ;
      collisionInfo = tempInfo;
      collisionInfo.groupPointer = (void*)group;
    }
  }

  if (collisionInfo.groupPointer != nullptr) {
    *collisionZ = minZ;
    *info = collisionInfo;
    return true;
  } else {
    return false;
  }

}
