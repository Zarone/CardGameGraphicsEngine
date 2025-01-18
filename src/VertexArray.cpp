#include "../include/VertexArray.h"
#include "../include/ErrorHandling.h"
#include <GL/glew.h>

VertexArray::VertexArray() {
  GLCall(glGenVertexArrays(1, &arrayID));
  this->numAttribs = 0;
}

void VertexArray::Bind() {
  GLCall(glBindVertexArray(arrayID));
}

void VertexArray::Unbind() {
  GLCall(glBindVertexArray(0));
}

unsigned int VertexArray::AddBuffer(VertexBuffer& buffer, const MemoryLayout& layout) {
  unsigned int currentIndex = 0;
  std::vector<MemoryElement> elements = layout.GetElements();
  this->Bind();
  buffer.Bind();
  for (int i = 0; i < elements.size(); ++i) {
    MemoryElement el = elements.at(i);
    unsigned int type = el.type;
    unsigned int count = el.count;
    GLCall(glEnableVertexAttribArray(numAttribs));
    GLCall(glVertexAttribPointer(numAttribs, count, type, GL_FALSE, layout.GetStride(), (const void*)currentIndex));
    if (el.instanced) {
      GLCall(glVertexAttribDivisor(numAttribs, 1));
    }
    currentIndex += count*MemoryLayout::SizeOfType(type);
    ++numAttribs;
  }
  return numAttribs-1;
}
