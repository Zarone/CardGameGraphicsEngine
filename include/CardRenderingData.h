#pragma once

#include <GL/glew.h>

class CardRenderingData
{
private:
  bool textureBound;
public:
  static GLfloat cardPositions[];
  static GLuint cardIndices[];
  bool IsTextureBound();
};
