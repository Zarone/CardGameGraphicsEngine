#include "../include/Renderer.h"

Renderer::Renderer(WindowManager* window): window(window) {
}

void Renderer::Setup3DTransforms(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection) {
  this->projMatrix = glm::perspective(
    glm::radians(45.0f), window->AspectRatio(),
    0.1f, 1000.0f
  );
  this->cameraMatrix= glm::lookAt(
    cameraPosition,
    cameraPosition+cameraDirection,
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
}
