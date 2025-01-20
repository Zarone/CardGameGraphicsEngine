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
  // std::cout << "colission at z = " << this->CheckCollision(this->renderer, x, y) << std::endl;
}

void Scene::SetupMouseClickCallback(WindowManager* window) {
    glfwSetWindowUserPointer(window->GetRawPointer(), this);

    glfwSetMouseButtonCallback(window->GetRawPointer(), [](GLFWwindow* window, int button, int action, int mods) {
        // Retrieve the Scene instance
        Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
        ASSERT(scene);
        scene->OnClick(window, button, action, mods);
    });
}
