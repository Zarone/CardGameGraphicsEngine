#pragma once

#include "SimplePlane.h"
#include "../HelperRendering/TextRenderer.h"
#include <string>

class TextBox : public SimplePlane {
private:
    TextRenderer* textRenderer;
    std::string text;
    glm::vec4 textColor;
    float textScale;
    Renderer* renderer;
public:
    TextBox(Renderer* renderer, glm::mat4 transform, Material material, const std::string& fontPath, unsigned int fontSize, const std::string& text = "", glm::vec4 textColor = glm::vec4(0,0,0,1));
    ~TextBox();
    void SetText(const std::string& newText);
    void SetTextColor(const glm::vec4& color);
    void SetTextScale(float scale);
    void Render(Renderer* renderer) override;
}; 