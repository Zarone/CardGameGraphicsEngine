#include "GameState.h"

GameState::GameState() {
}

void GameState::AddObject(SceneObject* group) { 
  nonCardGroups.push_back(group);
}

void GameState::AddObject(CardGroup* group) { 
  cardGroups.push_back(group);
}

void GameState::Render(Renderer* renderer) {
  for (SceneObject* group : this->nonCardGroups) {
    group->Render(renderer);
  }
  for (SceneObject* group : this->cardGroups) {
    group->Render(renderer);
  }
}

void GameState::UpdateTick(double deltaTime) {
  for (SceneObject* group : this->nonCardGroups) {
    group->UpdateTick(deltaTime);
  }
  for (SceneObject* group : this->cardGroups) {
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

  for (SceneObject* group : this->nonCardGroups) {
    objectCollision = group->CheckCollision(renderer, x, y, &tempZ, &tempInfo);
    if (objectCollision && tempZ < minZ) {
      minZ = tempZ;
      collisionInfo = tempInfo;
      collisionInfo.groupPointer = group;
    }
  }
  for (SceneObject* group : this->cardGroups) {
    objectCollision = group->CheckCollision(renderer, x, y, &tempZ, &tempInfo);
    if (objectCollision && tempZ < minZ) {
      minZ = tempZ;
      collisionInfo = tempInfo;
      //collisionInfo.groupPointer = (void*)group;
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

ClickEvent GameState::ProcessPreClick(CollisionInfo info) {
  SceneObject* src = (SceneObject*) info.groupPointer;
  return src->ProcessPreClick(info);
}

void GameState::ReleaseClick() {
  for (auto& object : this->nonCardGroups) object->ReleaseClick();
  for (auto& object : this->cardGroups) object->ReleaseClick();
}
