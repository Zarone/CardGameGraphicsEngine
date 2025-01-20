#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../include/WindowManager.h"

class Renderer
{
private:
  WindowManager* window;
public:
  glm::mat4 projMatrix;
  glm::mat4 cameraMatrix;
  int maxBindableTextures;
  Renderer(WindowManager* window, int maxBindableTextures);
  void Setup3DTransforms(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection);
  bool InsideWindowBounds(double posX, double posY) const;

  /*
  *
  * @brief Takes the vec4 position from projecting point
  * onto screen (x between -1 and 1), and gives the exact 
  * pixel coordinate on the screen.
  *
  */
  glm::vec2 GetScreenPositionFromCamera(glm::vec4 screenCoords);
};
