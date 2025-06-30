#include "../../src/pch.h"

#include "Texture.h"
#include "../Helper/ErrorHandling.h"
#include "../external/stb_image.h"

Texture::Texture(const std::string& path, bool headless): headless(headless) {
  //stbi_set_flip_vertically_on_load(1);

  this->path = path;
  this->buffer = stbi_load(path.c_str(), &(this->width), &(this->height), &(this->bitsPerPixel), 4);
  this->loadedFromImage = true;
  if (!(this->buffer))
  {
    std::cout << "Error loading texture" << std::endl;
    std::cout << stbi_failure_reason() << std::endl;
  }

  if (!headless) {
    GLCall(glGenTextures(1, &(this->textureID)));
  }

  this->isBound = false;
  this->hasSetup = false;
}

Texture::Texture(unsigned char* buffer, unsigned int width, unsigned int height) : headless(false) {
  this->buffer = buffer;
  this->width = width;
  this->height = height;

  GLCall(glGenTextures(1, &(this->textureID)));

  this->isBound = false;
  this->hasSetup = false;
}

Texture::~Texture() {
  if (!this->headless) {
    GLCall(glDeleteTextures(1, &(this->textureID)));
  }
  // if buffer never got freed
  if (this->loadedFromImage && this->buffer) {
    stbi_image_free(buffer);
  }
}

void Texture::Bind(unsigned int textureSlot) {
  GLCall(glActiveTexture(GL_TEXTURE0 + textureSlot));
  GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
  this->isBound = true;
  this->boundTo = textureSlot;

  if (!this->hasSetup) {
    // For font textures, use GL_RED format and proper texture parameters
    if (!this->loadedFromImage) {
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
      
      // Enable blending for font textures
      GLCall(glEnable(GL_BLEND));
      GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      
      GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_UNSIGNED_BYTE, this->buffer));
    } else {
      // For regular textures, use RGBA format
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
      GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

      float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA black
      GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

      GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->buffer));
    }
    this->hasSetup = true;

    if (this->loadedFromImage && this->buffer) {
      stbi_image_free(buffer);
      this->buffer = 0;
    }
  }
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
  os << "Texture: isBound: " << t.isBound << ", boundTo: " << t.boundTo << ", loadedFromImage:" << t.loadedFromImage;
  if (t.loadedFromImage) {
    os << ", path: " << t.path;
  }
  return os;
}
