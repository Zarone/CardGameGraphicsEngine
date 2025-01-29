#include "../include/Scene.h"
#include "../include/ErrorHandling.h"

Scene::Scene(
  WindowManager* windowManager,
  CardDatabaseSingleton* database
): renderer(windowManager)
{
  this->renderer.SetupDatabaseForTexturing(database);
}

void Scene::SetupCamera(
  const glm::vec3 cameraPosition,
  const glm::vec3 cameraDirection
) {
  renderer.Setup3DTransforms(cameraPosition, cameraDirection);
}

void Scene::ClearScreen() {
  GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Scene::Reset() {
  this->objects = std::vector<std::unique_ptr<SceneObject>>();
}

void Scene::ProcessCollision(double x, double y, double* collisionZ, bool preClick) {
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
    if (objectCollision) {
      //std::cout << "Collision at z = " << z << std::endl;
    }

    if (objectCollision && z < minZ) {
      minZ = z;
      selectedObject = &object;
      collisionInfo = info;
    }

    if (!preClick) {
      object->ReleaseClick();
    }
  }

  if (selectedObject != nullptr) {
    *collisionZ = minZ;
    ClickEvent event;

    if (preClick) {
      event = (*selectedObject)->ProcessPreClick(collisionInfo);
    } else {
      event = (*selectedObject)->ProcessClick(collisionInfo);
    }

    if (!preClick && event.sceneSwap) {
      this->Swap(event.sceneIndex);
    }
    return;
  }
  return;
}

void Scene::OnClick(GLFWwindow* window, int button, int action, int mods, bool preClick) {
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  double z;
  this->ProcessCollision(x, y, &z, preClick);
}

void Scene::SetupMouseClickCallback(WindowManager* window) {
  glfwSetWindowUserPointer(window->GetRawPointer(), this);

  glfwSetMouseButtonCallback(window->GetRawPointer(), [](GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
      Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
      ASSERT(scene);
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        scene->OnClick(window, button, action, mods, true);
      } else {
        //std::cout << "Right button press" << std::endl;
      }
    } else if (action == GLFW_RELEASE) {
      Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
      ASSERT(scene);
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        scene->OnClick(window, button, action, mods, false);
      } else {
        //std::cout << "Right button release" << std::endl;
      }
    }
      
  });
}

void Scene::Render() {
  for (auto& object : this->objects) {
    object->Render(&this->renderer);
  }
}

void Scene::UpdateTick(double deltaTime) {
  for (auto& object : this->objects) {
    object->UpdateTick(deltaTime);
  }
}
