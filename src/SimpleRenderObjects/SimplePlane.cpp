#include "SimplePlane.h"

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

void SimplePlane::MakeClickable() {
  this->isClickable = true;
}
void SimplePlane::MakeUnclickable() {
  this->isClickable = false;
}

bool SimplePlane::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  if (!isClickable) return false;

  if (this->perspective) {
    std::cout << "No implemented way to check collision with perspective-enabled simple plane object" << std::endl;
  } else {
    glm::vec3 projectedTopLeft = renderer->Get3DScreenPositionFromCamera(this->transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f));
    glm::vec3 projectedBottomRight = renderer->Get3DScreenPositionFromCamera(this->transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f));

    if (x < projectedTopLeft.x || x > projectedBottomRight.x || y < projectedTopLeft.y || y > projectedBottomRight.y) {
      return false;
    }

    *collisionZ = 0.0f;
    info->groupPointer = (SimplePlane*)this;
    return true;
  }
  return false;
}

