#include "../include/Scene.h"
#include "../include/ErrorHandling.h"

Scene::Scene(
  WindowManager* windowManager, 
  const glm::vec3 cameraPosition,
  const glm::vec3 cameraDirection
):
  renderer(windowManager, windowManager->GetMaxBindableTextures())
{
  renderer.Setup3DTransforms(cameraPosition, cameraDirection);
}

bool Scene::CheckCollision(double x, double y, double* collisionZ) {
  // since you can have multiple collisions
  // you need to find the one closest to the
  // camera (lowest z)

  std::unique_ptr<SceneObject>* selectedObject = nullptr;
  double minZ = MAXFLOAT;
  CollisionInfo collisionInfo;
  for (auto& object : this->objects) {
    double z = MAXFLOAT;
    CollisionInfo info;
    bool objectCollision = object->CheckCollision(&this->renderer, x, y, &z, &info);

    if (objectCollision && z < minZ) {
      minZ = z;
      selectedObject = &object;
      collisionInfo = info;
    }
  }

  if (selectedObject != nullptr) {
    (*selectedObject)->ProcessClick(collisionInfo);

    *collisionZ = minZ;
    return true;
  }
  return false;
}

void Scene::OnClick(GLFWwindow* window, int button, int action, int mods) {
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  std::cout << "here " << x << ", " << y << std::endl;
  double z;
  if (this->CheckCollision(x, y, &z)) {
    std::cout << "colission at z = " << z << std::endl;
  }
}

void Scene::SetupMouseClickCallback(WindowManager* window) {
  glfwSetWindowUserPointer(window->GetRawPointer(), this);

  glfwSetMouseButtonCallback(window->GetRawPointer(), [](GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
      // Retrieve the Scene instance
      Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
      ASSERT(scene);
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        scene->OnClick(window, button, action, mods);
      } else {
        std::cout << "Right button press" << std::endl;
      }

    } else {
      std::cout << "Mouse up" << std::endl;
    }
      
  });
}

void Scene::Render(const RenderData& renderData) {
  for (auto& object : this->objects) {
    object->Render(&this->renderer, renderData);
  }
}

void Scene::UpdateTick(double deltaTime) {
  for (auto& object : this->objects) {
    object->UpdateTick(deltaTime);
  }
}
