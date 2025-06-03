#pragma once

#include "SceneObject.h"
#include "../SimpleRenderObjects/BoundButton.h"
#include <vector>

struct ButtonArgs {
  const std::string& name;
  std::function<void()> func;
};

class CommandPalette : public SceneObject {
private:
  std::vector<BoundButton> buttons = {};
  Renderer* renderer;

public:
  CommandPalette(Renderer* renderer);
  void SetButtons(std::vector<ButtonArgs> buttons);

  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  ClickEvent ProcessClick(CollisionInfo info);
  ClickEvent ProcessPreClick(CollisionInfo info);
  void ReleaseClick();
};
