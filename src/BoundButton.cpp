#include "../include/BoundButton.h"

BoundButton::BoundButton(
  Renderer* renderer,
  glm::mat4 transform,
  glm::vec4 color,
  std::function<void()> func
):
  SimpleButton(renderer, transform, color)
{
  onPress = func;
}

#include <iostream>
ClickEvent BoundButton::ProcessClick(CollisionInfo info) {
  std::cout << "bound button process click" << std::endl;
  onPress();

  return {
    .sceneSwap = false
  };
}
