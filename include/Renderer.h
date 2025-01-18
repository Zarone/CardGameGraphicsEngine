#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "../include/WindowManager.h"

class Renderer
{
private:
  WindowManager* window;
public:
  glm::mat4 projMatrix;
  glm::mat4 cameraMatrix;
  Renderer(WindowManager* window);
  void Setup3DTransforms(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection);
};
