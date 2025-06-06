#pragma once

#include <vector>
#include <string>
#include <functional>
#include "../HelperRendering/Renderer.h"
#include "../SimpleRenderObjects/BoundButton.h"
#include "../HelperRendering/TextRenderer.h"

struct ButtonArgs {
    std::string text;
    std::function<void()> func;
};

class CommandPalette : public SceneObject {
private:
  std::vector<std::pair<const std::string, BoundButton>> buttons;
  Renderer* renderer;
  TextRenderer* textRenderer;
  static constexpr float buttonScale = 0.1;
  static constexpr int maxSize = 5;
  static constexpr float angleStep = -M_PI / 2 / maxSize;
  static constexpr float radius = 0.4;
  static constexpr float xOffset = -1.0f;
  static constexpr float yOffset = 0.5f;
  static constexpr float xScale = 4.0f;
  float currentScroll = 0.0f;
  SimplePlane backingPlane;

public:
  CommandPalette(Renderer* renderer);
  ~CommandPalette();

  void SetButtons(std::vector<ButtonArgs> buttons);
  void SetButtonLocations();
  void Render(Renderer* renderer) override;
  void UpdateTick(double deltaTime) override;
  bool CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* collisionInfo) const override;
  ClickEvent ProcessClick(CollisionInfo info) override;
  ClickEvent ProcessPreClick(CollisionInfo info) override;
  void ProcessScroll(CollisionInfo info, double yOffset) override;
  void ReleaseClick() override;
};
