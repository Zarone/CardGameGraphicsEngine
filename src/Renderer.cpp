#include "../include/Renderer.h"

Renderer::Renderer(WindowManager* window, int maxBindableTextures): 
  window(window),
  maxBindableTextures(maxBindableTextures)
{
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

bool Renderer::InsideWindowBounds(double posX, double posY) const {
  return this->window->CursorInBounds(posX, posY);
}

glm::vec2 Renderer::GetScreenPositionFromCamera(glm::vec4 screenCoords) {
  return glm::vec2(
    (screenCoords.x/screenCoords.w+1)/2*window->GetWidth(),
    (-screenCoords.y/screenCoords.w+1)/2*window->GetHeight()
  );
}

glm::vec3 Renderer::Get3DScreenPositionFromCamera(glm::vec4 screenCoords) {
  return glm::vec3(
    (screenCoords.x/screenCoords.w+1)/2*window->GetWidth(),
    (-screenCoords.y/screenCoords.w+1)/2*window->GetHeight(),
    screenCoords.z/screenCoords.w
  );
}
