#pragma once

#include <vector>

#include "../include/CardGroup.h"

class Scene
{
private:
  Renderer* renderer;
  std::vector<CardGroup*> objects;
public:
  Scene(Renderer* renderer);
  void AddObject(CardGroup* object);
  void OnClick(GLFWwindow* window, int button, int action, int mods);
  bool CheckCollision(double x, double y, double* collisionZ);
  void SetupMouseClickCallback(WindowManager* window);
};
