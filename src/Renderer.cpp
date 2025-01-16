#include "../include/Renderer.h"

Renderer::Renderer(WindowManager* window): window(window) {
}

void Renderer::Setup3DTransforms(glm::mat4& proj, glm::mat4& cam) {
  proj = glm::perspective(
    glm::radians(45.0f), window->AspectRatio(),
    0.1f, 1000.0f
  );
  glm::vec3 eyeLocation = glm::vec3(0.0f, 0.0f, 5.5f);
  cam = glm::lookAt(
    eyeLocation,
    eyeLocation+glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
}
