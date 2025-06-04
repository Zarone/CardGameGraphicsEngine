#pragma once

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

struct Character {
  glm::ivec2   Size;      // Size of glyph
  glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRenderer {
private:
  std::map<char, Character> Characters;
  unsigned int VAO, VBO;
  Shader* textShader;
  FT_Library ft;
  FT_Face face;
  const std::string fontPath;
  Renderer* renderer;

public:
  TextRenderer(Renderer* renderer, const std::string& fontPath, unsigned int fontSize);
  ~TextRenderer();

  void RenderText(Renderer* renderer, const std::string& text, float x, float y, float scale, glm::vec4& color);
  float GetTextWidth(const std::string& text, float scale);
  float GetTextHeight(const std::string& text, float scale);
}; 
