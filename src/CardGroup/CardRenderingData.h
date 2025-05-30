#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Material.h"

class CardRenderingData
{
private:
  // the point of displayed/actual variables
  // is so that the card can be animated
  glm::vec3 actualPosition;
  double actualRotationZ;
  double actualScaleXY;
  glm::vec3 previousActualPosition;
  double previousActualRotationZ;
  double previousActualScaleXY;

  static const double animationDuration;
  
  // between 0 and animationDuration, tracks completed
  // duration of animation between previousActual and actual
  double completedDuration;
  bool completedAnimation;
public:
  static GLfloat cardPositions[];
  static GLuint cardIndices[];
  static float cardHeightRatio;

  Shader* shader = nullptr;

  glm::vec3 displayedPosition;
  double displayedRotationZ;
  double displayedScaleXY;

  CardRenderingData();
  void SetActualTransform(
    glm::vec3 position, 
    double rotationZ,
    double scaleXY
  );
  bool UpdateDisplayed(double deltaTime);
  friend std::ostream& operator<<(std::ostream& os, const CardRenderingData& d);
};
