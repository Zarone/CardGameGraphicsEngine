#include "../include/VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray() {
  glGenVertexArrays(1, &arrayID); 
}

void VertexArray::Bind() {
  glBindVertexArray(arrayID);
}

void VertexArray::Unbind() {
  glBindVertexArray(0);
}

void VertexArray::AddBuffer(VertexBuffer& buffer, const MemoryLayout& layout) {
  unsigned int currentIndex = 0;
  std::vector<MemoryElement> elements = layout.GetElements();
  unsigned int numAttribs = 0;
  buffer.Bind();
  for (int i = 0; i < elements.size(); ++i) {
    MemoryElement el = elements.at(i);
    unsigned int type = el.type;
    unsigned int count = el.count;
    unsigned int size = count*MemoryLayout::SizeOfType(type);
    glEnableVertexAttribArray(numAttribs);
    glVertexAttribPointer(numAttribs, count, type, GL_FALSE, size, (void*)currentIndex);
    currentIndex += size;
    ++numAttribs;
  }
}
