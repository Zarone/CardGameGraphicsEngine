#include <GL/glew.h>
#include <iostream>
#include <unordered_map>

#include "../include/Texture.h"
#include "../include/ErrorHandling.h"
#include "../include/external/stb_image.h"

Texture::Texture(const std::string& path) {
  //stbi_set_flip_vertically_on_load(1);


  std::cout << "loading texture from path " << path << std::endl;
  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;

  auto t1 = high_resolution_clock::now();

  this->buffer = stbi_load(path.c_str(), &(this->width), &(this->height), &(this->bitsPerPixel), 4);

  auto t2 = high_resolution_clock::now();

  /* Getting number of milliseconds as an integer. */
  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  /* Getting number of milliseconds as a double. */
  duration<double, std::milli> ms_double = t2 - t1;

  std::cout << ms_double.count() << "ms\n";


  if (!(this->buffer))
  {
    std::cout << "Error loading texture" << std::endl;
    std::cout << stbi_failure_reason() << std::endl;
  }

  GLCall(glGenTextures(1, &(this->textureID)));

  this->isBound = false;
  this->hasSetup = false;
}

Texture::~Texture() {
  GLCall(glDeleteTextures(1, &(this->textureID)));

  // if buffer never got freed
  if (this->buffer) {
    stbi_image_free(buffer);
  }
}

void Texture::Bind(unsigned int textureSlot) {
  GLCall(glActiveTexture(GL_TEXTURE0 + textureSlot));
  GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
  this->isBound = true;
  this->boundTo = textureSlot;

  if (!this->hasSetup) {
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA black
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->buffer));
    this->hasSetup = true;

    if (this->buffer) {
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
  os << "Texture: isBound: " << t.isBound << ", boundTo: " << t.boundTo;
  return os;
}
