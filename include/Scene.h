#pragma once

#include <vector>

#include "../include/SceneObject.h"
#include "../include/CardDatabaseSingleton.h"
#include "../include/SceneData.h"

class Scene
{
protected:
  Renderer renderer;
  std::vector<std::unique_ptr<SceneObject>> objects;
  unsigned int currentScene = 0;
  std::shared_ptr<SceneData> sceneData;
public:
  Scene(
    WindowManager* windowManager,
    CardDatabaseSingleton* database
  );
  void SetupCamera(
    const glm::vec3 cameraPosition,
    const glm::vec3 cameraDirection
  );

  template<class T, class... Args>
  void AddObject(Args&&... args) {
    this->objects.push_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

  void ClearScreen();
  void Reset();
  void ProcessCollision(double x, double y, double* collisionZ, bool preClick);
  void OnClick(GLFWwindow* window, int button, int action, int mods, bool preClick);
  void SendReleaseClick();
  void SetupMouseClickCallback(WindowManager* window);
  void Render();
  void UpdateTick(double deltaTime);
  virtual void Swap(unsigned int sceneIndex) = 0;
};
