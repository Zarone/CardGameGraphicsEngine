#include "TextRenderer.h"
#include "../Helper/ErrorHandling.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unistd.h>

TextRenderer::TextRenderer(
  Renderer* renderer, 
  const std::string& fontPath, 
  unsigned int fontSize): fontPath(fontPath), renderer(renderer)
{
  // Initialize FreeType
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    throw std::runtime_error("Failed to initialize FreeType");
  }

  // Load font
  if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font from path: " << fontPath << std::endl;
    std::cout << "Current working directory: ";
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      std::cout << cwd << std::endl;
    }
    FT_Done_FreeType(ft);
    throw std::runtime_error("Failed to load font file");
  }

  // Set font size
  FT_Set_Pixel_Sizes(face, 0, fontSize);

  // Disable byte-alignment restriction
  GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

  // Load first 128 ASCII characters
  for (unsigned char c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
      continue;
    }

    // Generate texture
    renderer->textureMap.SetupFontTexturePath(fontPath + std::to_string(c), face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);
    int textureSlot = renderer->textureMap.RequestBind(renderer->maxBindableTextures, fontPath + std::to_string(c));
    
    // Debug output
    std::cout << "Loaded character '" << c << "' with size: " << face->glyph->bitmap.width << "x" << face->glyph->bitmap.rows 
              << " at slot " << textureSlot << std::endl;

    // Store character for later use
    Character character = {
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      static_cast<unsigned int>(face->glyph->advance.x)
    };
    Characters.insert(std::pair<char, Character>(c, character));
  }

  // Clean up FreeType resources
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  // Configure VAO/VBO for texture quads
  GLCall(glGenVertexArrays(1, &VAO));
  GLCall(glGenBuffers(1, &VBO));
  GLCall(glBindVertexArray(VAO));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
  GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW));
  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GLCall(glBindVertexArray(0));

  // Create and compile shaders
  const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex;
    out vec2 TexCoords;

    uniform mat4 projection;

    void main() {
      // Use orthographic projection for 2D text
      gl_Position = vec4(vertex.xy, -0.0001, 1.0);
      TexCoords = vertex.zw;
    }
  )";

  const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoords;
    out vec4 color;

    uniform sampler2D text;
    uniform vec4 textColor;

    void main() {
      vec4 sampled = vec4(1.0, 0.0, 0.0, texture(text, TexCoords).r);
      color = textColor * sampled;
    }
  )";

  textShader = new Shader(vertexShaderSource, fragmentShaderSource);
}

TextRenderer::~TextRenderer() {
  delete textShader;
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void TextRenderer::RenderText(Renderer* renderer, const std::string& text, float x, float y, float scale, glm::vec4& color) {
  // Activate corresponding render state
  textShader->Bind();
  textShader->SetUniform4f("textColor", color);
  textShader->SetUniform4fv("projection", false, glm::value_ptr(renderer->projMatrix));

  GLCall(glActiveTexture(GL_TEXTURE0));
  GLCall(glBindVertexArray(VAO));

  // Iterate through all characters of string
  for (char c : text) {
    Character ch = Characters[c];
    std::cout << "c: " << c << std::endl;

    float xpos = x + ch.Bearing.x * scale;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;

    // Update VBO for each character
    float vertices[6][4] = {
      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos,     ypos,       0.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 1.0f },

      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos + w, ypos,       1.0f, 1.0f },
      { xpos + w, ypos + h,   1.0f, 0.0f }
    };

    // Render glyph texture over quad
    int textureSlot = renderer->textureMap.RequestBind(
      renderer->maxBindableTextures,
      this->fontPath + std::to_string(c)
    );
    std::cout << "textureSlot: " << textureSlot << std::endl;
    
    // Make sure texture is bound before setting uniform
    GLCall(glActiveTexture(GL_TEXTURE0 + textureSlot));
    textShader->SetUniform1i("text", textureSlot);

    // Update content of VBO memory
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    // Render quad
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

    // Advance cursors for next glyph
    x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    std::cout << "x: " << x << std::endl;
  }
  std::cout << "x END" << std::endl;

}

float TextRenderer::GetTextWidth(const std::string& text, float scale) {
    float width = 0.0f;
    for (char c : text) {
      Character ch = Characters[c];
      width += (ch.Advance >> 6) * scale;
    }
    return width;
}

float TextRenderer::GetTextHeight(const std::string& text, float scale) {
  float height = 0.0f;
  for (char c : text) {
    Character ch = Characters[c];
    height = std::max(height, static_cast<float>(ch.Size.y) * scale);
  }
  return height;
} 
