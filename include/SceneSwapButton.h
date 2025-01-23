#pragma once

#include "./SimpleButton.h"
#include "../example/TestSceneData.h"

class SceneSwapButton : public SimpleButton
{
private:
  TestSceneID id;
public:
  SceneSwapButton(
    Renderer* renderer,
    glm::mat4 transform,
    glm::vec4 color,
    TestSceneID id
  );

  ClickEvent ProcessClick(CollisionInfo info);
};
