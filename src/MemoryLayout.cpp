#include <GL/glew.h>
#include <cstdlib>
#include "../include/MemoryLayout.h"

MemoryLayout::MemoryLayout() {
}

unsigned int MemoryLayout::SizeOfType(unsigned int type) {
  switch (type) {
    case GL_UNSIGNED_INT: return sizeof(GLuint);
    case GL_FLOAT: return sizeof(GLfloat);
    default:
      exit(EXIT_FAILURE);
  };
}

void MemoryLayout::AddMemoryElement(unsigned int type, unsigned int count) {
  this->stride += count * SizeOfType(type);
  elements.push_back({
    .type = type,
    .count = count
  });
}

std::vector<MemoryElement> MemoryLayout::GetElements() const {
  return this->elements;
}

unsigned int MemoryLayout::GetStride() const {
  return this->stride;
}
