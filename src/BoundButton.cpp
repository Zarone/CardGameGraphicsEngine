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

ClickEvent BoundButton::ProcessClick(CollisionInfo info) {
  onPress();

  return {
    .sceneSwap = false
  };
}
