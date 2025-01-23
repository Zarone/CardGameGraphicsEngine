#pragma once
#include "SimpleButton.h"

class BoundButton : public SimpleButton 
{
private:
  std::function<void()> onPress;
public:
  BoundButton(
    Renderer* renderer,
    glm::mat4 transform,
    glm::vec4 color,
    std::function<void()> func
  );

  ClickEvent ProcessClick(CollisionInfo info);
};
