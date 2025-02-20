#include "../../include/SimpleRenderObjects/SimplePlane.h"

const float SimplePlane::vertexData[] = {
  -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
  0.5f ,  0.5f, 0.0f, 1.0f, 0.0f,
  0.5f , -0.5f, 0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
};

const unsigned int SimplePlane::indexData[] = {
  3, 2, 0,
  0, 2, 1
};

SimplePlane::SimplePlane(
  glm::mat4 transform,
  Material material
): SimpleRenderObject(transform, material)
{
  this->LoadIntoGPU();
}

int SimplePlane::GetVertexCount() {
  return 4;
}

void SimplePlane::FillData(float* vertexData, unsigned int* indexData) {
  memcpy(vertexData, this->vertexData, 5*4*sizeof(float));
  memcpy(indexData, this->indexData, 6*sizeof(unsigned int));
}
