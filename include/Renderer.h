#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "../include/WindowManager.h"

class Renderer
{
private:
  WindowManager* window;
public:
  Renderer(WindowManager* window);
  void Setup3DTransforms(glm::mat4& proj, glm::mat4& cam);
};
