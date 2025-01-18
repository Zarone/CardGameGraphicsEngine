#include "../include/CardRenderingData.h"

float CardRenderingData::cardHeightRatio = 1.396f;

GLfloat CardRenderingData::cardPositions[] = {
  -0.5f,  0.5f*cardHeightRatio, 0.0f, 0.0f, 0.0f,
  0.5f ,  0.5f*cardHeightRatio, 0.0f, 1.0f, 0.0f,
  0.5f , -0.5f*cardHeightRatio, 0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f*cardHeightRatio, 0.0f, 0.0f, 1.0f,
};

GLuint CardRenderingData::cardIndices[] = {
  3, 2, 0,
  0, 2, 1
};

CardRenderingData::CardRenderingData() {
  this->actualPosition = glm::vec3(0,0,0);
  this->previousActualPosition = glm::vec3(0,0,0);
  this->completedDuration = animationDuration;
  this->completedAnimation = true;
}

#include <iostream>
void CardRenderingData::SetActualTransform(glm::vec3 position, double rotationZ) {
  std::cout << "set actual transform" << std::endl;
  this->previousActualPosition = this->actualPosition;
  this->previousActualRotationZ = this->actualRotationZ;
  this->actualPosition = position;
  this->actualRotationZ = rotationZ;
  this->completedDuration = 0;
  this->completedAnimation = false;
}

bool CardRenderingData::UpdateDisplayed(double deltaTime) {
  if (this->completedAnimation) return false;

  this->completedDuration += deltaTime;
  std::cout << "update displayed: completedDuration = " << completedDuration << std::endl;
  if (this->completedDuration >= this->animationDuration) {
    std::cout << "completedDuration" << std::endl;
    this->displayedPosition = this->actualPosition;
    this->displayedRotationZ = this->actualRotationZ;
    this->completedAnimation = true;
  } else {
    float t = (this->completedDuration / this->animationDuration);
    this->displayedPosition = t * this->actualPosition + (1-t) * this->previousActualPosition;
    this->displayedRotationZ = t * this->previousActualRotationZ + (1-t) * this->previousActualRotationZ;
    std::cout << this->displayedPosition.x << ", " << this->displayedPosition.y << ", " << this->displayedPosition.z << std::endl;
    std::cout << this->displayedRotationZ << std::endl;
  }

  return true;
}
