#include "../include/Scene.h"
#include "../include/ErrorHandling.h"

Scene::Scene(Renderer* renderer)
:
renderer(renderer) 
{
}

void Scene::AddObject(CardGroup* object) {
  objects.push_back(object);
}

void Scene::OnClick(GLFWwindow* window, int button, int action, int mods) {
  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  std::cout << "here " << x << ", " << y << std::endl;
  double z;
  this->CheckCollision(x, y, &z);
  std::cout << "colission at z = " << z << std::endl;
}

bool Scene::CheckCollision(double x, double y, double* collisionZ) {
  // since you can have multiple collisions
  // you need to find the one closest to the
  // camera (lowest z)

  CardGroup* selectedObject = nullptr;
  double minZ = MAXFLOAT;
  int collisionCardIndex;
  for (CardGroup* object : this->objects) {
    double z = MAXFLOAT;
    int thisCardIndex;
    bool objectCollision = object->CheckCollision(this->renderer, x, y, &z, &thisCardIndex);

    if (objectCollision && z < minZ) {
      minZ = z;
      selectedObject = object;
      collisionCardIndex = thisCardIndex;
    }
  }

  if (selectedObject != nullptr) {
    selectedObject->ProcessCardClick(collisionCardIndex);
  }

  return minZ;
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
