#include <GL/glew.h>
#include <cstdlib>
#include <iostream>
#include "../include/MemoryLayout.h"

MemoryLayout::MemoryLayout() {
  stride = 0;
  this->elements = {};
}

unsigned int MemoryLayout::SizeOfType(unsigned int type) {
  switch (type) {
    case GL_UNSIGNED_INT: return sizeof(GLuint);
    case GL_FLOAT: return sizeof(GLfloat);
    case GL_INT: return sizeof(GLint);
    default:
      exit(EXIT_FAILURE);
  };
}

void MemoryLayout::AddMemoryElement(unsigned int type, unsigned int count, bool instanced) {
  this->stride += count * SizeOfType(type);
  elements.push_back({
    .type = type,
    .count = count,
    .instanced = instanced
  });
}

std::vector<MemoryElement> MemoryLayout::GetElements() const {
  return this->elements;
}

unsigned int MemoryLayout::GetStride() const {
  return this->stride;
}
