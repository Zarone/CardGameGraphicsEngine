#include "../include/CardRenderingData.h"

GLfloat CardRenderingData::cardPositions[] = {
  -0.5f,  0.5f*1.396f, 0.0f, 0.0f, 0.0f,
  0.5f ,  0.5f*1.396f, 0.0f, 1.0f, 0.0f,
  0.5f , -0.5f*1.396f, 0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f*1.396f, 0.0f, 0.0f, 1.0f,
};

GLuint CardRenderingData::cardIndices[] = {
  3, 2, 0,
  0, 2, 1
};

bool CardRenderingData::IsTextureBound() {
  return this->textureBound;
}
