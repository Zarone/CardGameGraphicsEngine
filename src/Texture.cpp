#include <GL/glew.h>
#include <iostream>
#include <unordered_map>

#include "../include/Texture.h"
#include "../include/ErrorHandling.h"
#include "../include/external/stb_image.h"

Texture::Texture(const std::string& path) {
  //stbi_set_flip_vertically_on_load(1);
  this->buffer = stbi_load(path.c_str(), &(this->width), &(this->height), &(this->bitsPerPixel), 4);

  if (!(this->buffer))
  {
    std::cout << "Error loading texture" << std::endl;
    std::cout << stbi_failure_reason() << std::endl;
  }

  GLCall(glGenTextures(1, &(this->textureID)));
  GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));

  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

  float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA black
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->buffer));
  GLCall(glBindTexture(GL_TEXTURE_2D, 0));

  if (this->buffer) {
    stbi_image_free(buffer);
  }
}

Texture::~Texture() {
  GLCall(glDeleteTextures(1, &(this->textureID)));
}

void Texture::Bind(unsigned int textureSlot) {
  GLCall(glActiveTexture(GL_TEXTURE0 + textureSlot));
  GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
  this->isBound = true;
  this->boundTo = textureSlot;
}

void Texture::Unbind() {
  GLCall(glActiveTexture(GL_TEXTURE0 + this->boundTo));
  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
  this->isBound = false;
}

bool Texture::GetIsBound() {
  return this->isBound;
}

int Texture::GetBoundSlot() {
  return this->boundTo;
}

std::ostream& operator<<(std::ostream&os, const Texture& t) {
  os << "Texture: isBound: " << t.isBound << ", boundTo: " << t.boundTo;
  return os;
}
