#include "./TextBox.h"
#include <glm/gtc/matrix_transform.hpp>

TextBox::TextBox(Renderer* renderer, glm::mat4 transform, Material material, const std::string& fontPath, unsigned int fontSize, const std::string& text, glm::vec4 textColor)
    : SimplePlane(transform, material), renderer(renderer), text(text), textColor(textColor), textScale(0.002f)
{
    this->perspective = false;
    textRenderer = new TextRenderer(renderer, fontPath, fontSize);
}

TextBox::~TextBox() {
    delete textRenderer;
}

void TextBox::SetText(const std::string& newText) {
    text = newText;
}

void TextBox::SetTextColor(const glm::vec4& color) {
    textColor = color;
}

void TextBox::SetTextScale(float scale) {
    textScale = scale;
}

void TextBox::Render(Renderer* renderer) {
    // Render the background plane
    SimplePlane::Render(renderer);
    // Center the text on the plane
    float scale = textScale;
    float textWidth = textRenderer->GetTextWidth(text, scale);
    float textHeight = textRenderer->GetTextHeight(text, scale);
    // Plane is centered at (0,0) in local space, so center text at (0,0)
    glm::vec4 center = this->GetTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float x = center.x - textWidth / 2.0f;
    float y = center.y - textHeight / 2.0f;
    textRenderer->RenderText(renderer, text, x, y, center.z, scale, textColor);
} 