#pragma once

#include <vector>

#include "../include/SceneObject.h"

class Scene
{
protected:
  Renderer renderer;
  std::vector<std::unique_ptr<SceneObject>> objects;
public:
  Scene(
    WindowManager* windowManager, 
    const glm::vec3 cameraPosition,
    const glm::vec3 cameraDirection
  );
  bool CheckCollision(double x, double y, double* collisionZ);
  void OnClick(GLFWwindow* window, int button, int action, int mods);
  void SetupMouseClickCallback(WindowManager* window);
  void Render(const RenderData& renderData);
  void UpdateTick(double deltaTime);
};
