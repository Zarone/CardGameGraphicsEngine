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
    std::vector<BoundButton> buttons;
    Renderer* renderer;
    TextRenderer* textRenderer;

public:
    CommandPalette(Renderer* renderer);
    ~CommandPalette();

    void SetButtons(std::vector<ButtonArgs> buttons);
    void Render(Renderer* renderer) override;
    void UpdateTick(double deltaTime) override;
    bool CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* collisionInfo) const override;
    ClickEvent ProcessClick(CollisionInfo info) override;
    ClickEvent ProcessPreClick(CollisionInfo info) override;
    void ReleaseClick() override;
};
