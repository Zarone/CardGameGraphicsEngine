#include "GameState.h"

GameState::GameState() {
}

void GameState::AddObject(SceneObject* group) { 
  nonCardGroups.push_back(group);
}

void GameState::AddCardGroup(CardGroup* group, unsigned int pile) { 
  cardGroups.push_back(group);
  cardGroupMap.insert({pile, group});
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
  bool objectCollision = false;

  for (SceneObject* group : this->nonCardGroups) {
    CollisionInfo tempInfo;
    objectCollision = group->CheckCollision(renderer, x, y, &tempZ, &tempInfo);
    if (objectCollision && tempZ < minZ) {
      minZ = tempZ;
      collisionInfo = std::move(tempInfo);
      collisionInfo.groupPointer = group;
    }
  }
  for (SceneObject* group : this->cardGroups) {
    CollisionInfo tempInfo;
    objectCollision = group->CheckCollision(renderer, x, y, &tempZ, &tempInfo);
    if (objectCollision && tempZ < minZ) {
      minZ = tempZ;
      collisionInfo = std::move(tempInfo);
      // collisionInfo.groupPointer = group;
    }
  }

  if (collisionInfo.groupPointer != nullptr) {
    *collisionZ = minZ;
    *info = std::move(collisionInfo);
    return true;
  } else {
    return false;
  }
}

ClickEvent GameState::ProcessPreClick(CollisionInfo info) {
  SceneObject* src = (SceneObject*) info.groupPointer;
  return src->ProcessPreClick(std::move(info));
}

void GameState::ReleaseClick() {
  for (auto& object : this->nonCardGroups) object->ReleaseClick();
  for (auto& object : this->cardGroups) object->ReleaseClick();
}
