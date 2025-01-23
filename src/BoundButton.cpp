#include "../include/BoundButton.h"

BoundButton::BoundButton(
  Renderer* renderer,
  glm::mat4 transform,
  Material material,
  std::function<void()> func
):
  SimpleButton(renderer, transform, material)
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
