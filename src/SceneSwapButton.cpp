#include "../include/SceneSwapButton.h"

SceneSwapButton::SceneSwapButton(
  Renderer* renderer,
  glm::mat4 transform,
  glm::vec4 color,
  TestSceneID id
):
  SimpleButton(renderer, transform, color),
  id(id)
{
}

ClickEvent SceneSwapButton::ProcessClick(CollisionInfo info) {
  std::cout << "scene swap process click" << std::endl;
  return {
    .sceneSwap = true,
    .sceneIndex = id
  };
}

