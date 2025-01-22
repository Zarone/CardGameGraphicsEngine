#pragma once

#include <vector>
#include <iostream>

#include "../include/SceneObject.h"
#include "../include/CardDatabaseSingleton.h"

class Scene
{
protected:
  Renderer renderer;
  std::vector<std::unique_ptr<SceneObject>> objects;
public:
  Scene(
    WindowManager* windowManager
  );
  void SetupCamera(
    const glm::vec3 cameraPosition,
    const glm::vec3 cameraDirection
  );
  void SetupCardDataBase(CardDatabaseSingleton* database);

  template<class T, class... Args>
  void AddObject(Args&&... args) {
    this->objects.push_back(std::make_unique<T>(std::forward<Args>(args)...));
  }
  void Reset();

  void ProcessCollision(double x, double y, double* collisionZ);
  void OnClick(GLFWwindow* window, int button, int action, int mods);
  void SetupMouseClickCallback(WindowManager* window);
  void Render();
  void UpdateTick(double deltaTime);
  virtual void Swap(unsigned int sceneIndex) = 0;
};
