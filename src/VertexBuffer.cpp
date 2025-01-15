#include <GL/glew.h>
#include "../include/VertexBuffer.h"

VertexBuffer::VertexBuffer(void* data, unsigned int size) {
  glGenBuffers(1, &(this->bufferID));
  glBindBuffer(GL_ARRAY_BUFFER, this->bufferID);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() {
  glBindBuffer(GL_ARRAY_BUFFER, this->bufferID);
}

void VertexBuffer::Unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
