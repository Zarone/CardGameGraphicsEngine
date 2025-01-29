#include <iostream>

#include "../../include/SimpleRenderObjects/SimpleButton.h"
#include "../../include/shaders/allShaders.h"
#include "../../include/MathHelper.h"

SimpleButton::SimpleButton(
  Renderer* renderer,
  glm::mat4 transform,
  Material material
):
  SimplePlane(
    transform,
    material
  ),
  renderer(renderer)
{
  this->SetupProjectedVertices(&transform);
}

bool SimpleButton::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  // cross product method

  // Iterate over all edges of the rectangle
  for (int i = 0; i < 4; ++i) {
    const glm::vec3& v1 = this->projectedVertices[i];
    const glm::vec3& v2 = this->projectedVertices[(i + 1) % 4];

    // Convert vertices and point to 2D
    glm::vec2 v1ToPoint(v1.x - x, v1.y - y);
    glm::vec2 v2ToPoint(v2.x - x, v2.y - y);

    // If the cross product is positive, the point is outside this edge
    // (usually the opposite is true, but screen space is y-flipped)
    if (determinant(v1ToPoint, v2ToPoint) > 0) {
      return false;
    }
  }

  // If we reach here, the point is inside the rectangle
  // Find the collision Z value by interpolating from the Z values of the rectangle's plane

  // Use the first three vertices to compute the plane equation: Ax + By + Cz + D = 0
  const glm::vec3& v0 = this->projectedVertices[0];
  const glm::vec3& v1 = this->projectedVertices[1];
  const glm::vec3& v2 = this->projectedVertices[2];

  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 normal = glm::cross(edge1, edge2); // Normal of the plane

  // Equation of the plane: 
  // normal.x * x + normal.y * y + normal.z * z + D = 0
  double D = -(normal.x * v0.x + normal.y * v0.y + normal.z * v0.z);

  // Solve for z: 
  // z = -(normal.x * x + normal.y * y + D) / normal.z
  if (normal.z != 0) {
    *collisionZ = -(normal.x * x + normal.y * y + D) / normal.z;
  } else {
    // If normal.z is 0, the plane is parallel to the screen, return false
    return false;
  }

  SceneObject* test = (SceneObject*)this;
  info->groupPointer = (SimpleButton*)this;
  return true;
}

ClickEvent SimpleButton::ProcessClick(CollisionInfo info) {
  std::cout << "simpleButton ProcessClick, which does nothing..." << std::endl;
  return {};
}

// must be counter-clockwise
void SimpleButton::SetupProjectedVertices(glm::mat4* transform) {
  glm::mat4 transformation = this->perspective ?
    this->renderer->projMatrix * 
    this->renderer->cameraMatrix * 
    *transform
    : *transform;

  this->projectedVertices[0] = this->renderer->Get3DScreenPositionFromCamera(
    transformation *
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[1] = this->renderer->Get3DScreenPositionFromCamera(
    transformation *
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[2] = this->renderer->Get3DScreenPositionFromCamera(
    transformation *
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[3] = this->renderer->Get3DScreenPositionFromCamera(
    transformation *
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)
  );
}

void SimpleButton::SetTransform(glm::mat4* transform) {
  this->SetupProjectedVertices(transform);
  SimplePlane::SetTransform(transform);
}

ClickEvent SimpleButton::ProcessPreClick(CollisionInfo info) {
  this->material.color.w *= 0.5f;
  this->pressedDown = true;
  return {};
}

void SimpleButton::ReleaseClick() {
  if (this->pressedDown){
    this->pressedDown = false;
    this->material.color.w *= 2.f;
  }
}
