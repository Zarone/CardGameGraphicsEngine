#include <iostream>
#include "../include/SimpleButton.h"
#include "../include/shaders/allShaders.h"
#include "../include/MathHelper.h"

SimpleButton::SimpleButton(
  Renderer* renderer,
  glm::mat4 transform,
  glm::vec4 color
):
  SimplePlane(
    myShaders::basicVertex, 
    myShaders::basicFragment, 
    transform,
    color
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
  //std::cout << "Checking button collision" << std::endl;
  //std::cout << "Cursor X: " << x << std::endl;
  //std::cout << "Cursor Y: " << y << std::endl;

  //std::cout << "projectedVertices" << std::endl;
  //std::cout << projectedVertices[0].x << ", " << projectedVertices[0].y << ", " << projectedVertices[0].z << std::endl;
  //std::cout << projectedVertices[1].x << ", " << projectedVertices[1].y << ", " << projectedVertices[1].z << std::endl;
  //std::cout << projectedVertices[2].x << ", " << projectedVertices[2].y << ", " << projectedVertices[2].z << std::endl;
  //std::cout << projectedVertices[3].x << ", " << projectedVertices[3].y << ", " << projectedVertices[3].z << std::endl;

  // Iterate over all edges of the rectangle
  for (int i = 0; i < 4; ++i) {
    const glm::vec3& v1 = this->projectedVertices[i];
    const glm::vec3& v2 = this->projectedVertices[(i + 1) % 4];
    //std::cout << "i = " << i << std::endl;
    //std::cout << "v1: " << v1.x << ", " << v1.y << std::endl;
    //std::cout << "v2: " << v2.x << ", " << v2.y << std::endl;

    // Convert vertices and point to 2D
    glm::vec2 v1ToPoint(v1.x - x, v1.y - y);
    glm::vec2 v2ToPoint(v2.x - x, v2.y - y);
    //std::cout << "v1ToPoint: " << v1ToPoint.x << ", " << v1ToPoint.y << std::endl;
    //std::cout << "v2ToPoint: " << v2ToPoint.x << ", " << v2ToPoint.y << std::endl;

    // If the cross product is positive, the point is outside this edge
    // (usually the opposite is true, but screen space is y-flipped)
    if (determinant(v1ToPoint, v2ToPoint) > 0) {
      //std::cout << "outside" << std::endl;
      return false;
    }
  }
  *collisionZ = 0.0f;
  return true;

  //// If we reach here, the point is inside the rectangle
  //// Find the collision Z value by interpolating from the Z values of the rectangle's plane

  //// Use the first three vertices to compute the plane equation: Ax + By + Cz + D = 0
  //const glm::vec3& v0 = this->projectedVertices[0];
  //const glm::vec3& v1 = this->projectedVertices[1];
  //const glm::vec3& v2 = this->projectedVertices[2];

  //glm::vec3 edge1 = v1 - v0;
  //glm::vec3 edge2 = v2 - v0;
  //glm::vec3 normal = glm::cross(edge1, edge2); // Normal of the plane

  //// Equation of the plane: 
  //// normal.x * x + normal.y * y + normal.z * z + D = 0
  //double D = -(normal.x * v0.x + normal.y * v0.y + normal.z * v0.z);

  //// Solve for z: 
  //// z = -(normal.x * x + normal.y * y + D) / normal.z
  //if (normal.z != 0) {
    //*collisionZ = -(normal.x * x + normal.y * y + D) / normal.z;
  //} else {
    //// If normal.z is 0, the plane is parallel to the screen, return false
    //return false;
  //}

  //return true;
}

ClickEvent SimpleButton::ProcessClick(CollisionInfo info) {
  std::cout << "Process button Click" << std::endl;
  return {};
}

// must be counter-clockwise
void SimpleButton::SetupProjectedVertices(glm::mat4* transform) {
  this->projectedVertices[0] = this->renderer->Get3DScreenPositionFromCamera(
    this->renderer->projMatrix * 
    this->renderer->cameraMatrix * 
    *transform * 
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[1] = this->renderer->Get3DScreenPositionFromCamera(
    this->renderer->projMatrix * 
    this->renderer->cameraMatrix * 
    *transform * 
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[2] = this->renderer->Get3DScreenPositionFromCamera(
    this->renderer->projMatrix * 
    this->renderer->cameraMatrix * 
    *transform * 
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)
  );
  this->projectedVertices[3] = this->renderer->Get3DScreenPositionFromCamera(
    this->renderer->projMatrix * 
    this->renderer->cameraMatrix * 
    *transform * 
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)
  );
}

void SimpleButton::SetTransform(glm::mat4* transform) {
  this->SetupProjectedVertices(transform);
  SimplePlane::SetTransform(transform);
}

ClickEvent SimpleButton::ProcessPreClick(CollisionInfo info) {
  this->color.w *= 0.5f;
  this->pressedDown = true;
  return {};
}

void SimpleButton::ReleaseClick() {
  if (this->pressedDown){
    this->pressedDown = false;
    this->color.w *= 2.f;
  }
}
