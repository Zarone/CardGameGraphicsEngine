#include "../include/GameObject.h"

void GameObject::Render(Renderer* renderer, const RenderData& renderData) {
  game.Render(renderer, renderData);
}

void GameObject::UpdateTick(double deltaTime) {
  game.UpdateTick(deltaTime);
}

bool GameObject::CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* info) const {
  return this->game.CheckCollision(renderer, x, y, collisionZ, info);
}

void GameObject::ProcessClick(CollisionInfo info) {
  this->game.ProcessClick(info);
}
