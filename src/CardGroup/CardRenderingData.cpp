#include "../CardRenderingData.h"

const double CardRenderingData::animationDuration = 0.1f;

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
  this->displayedPosition = glm::vec3(0,0,0);
  this->completedDuration = animationDuration;
  this->completedAnimation = true;
}

void CardRenderingData::SetActualTransform(
  glm::vec3 position, 
  double rotationZ,
  double scaleXY
) {
  this->previousActualPosition = this->displayedPosition;
  this->previousActualRotationZ = this->displayedRotationZ;
  this->previousActualScaleXY = this->displayedScaleXY;
  this->actualPosition = position;
  this->actualRotationZ = rotationZ;
  this->actualScaleXY = scaleXY;
  this->completedDuration = 0;
  this->completedAnimation = false;
}

bool CardRenderingData::UpdateDisplayed(double deltaTime) {
  if (this->completedAnimation) return false;

  this->completedDuration += deltaTime;
  if (this->completedDuration >= this->animationDuration) {
    this->displayedPosition = this->actualPosition;
    this->displayedRotationZ = this->actualRotationZ;
    this->displayedScaleXY = this->actualScaleXY;
    this->completedAnimation = true;
  } else {
    float t = (this->completedDuration / this->animationDuration);
    this->displayedPosition = t * this->actualPosition + (1-t) * this->previousActualPosition;
    this->displayedRotationZ = t * this->actualRotationZ + (1-t) * this->previousActualRotationZ;
    this->displayedScaleXY = t * this->actualScaleXY + (1-t) * this->previousActualScaleXY;
  }

  return true;
}
