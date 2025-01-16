#include <GL/glew.h>
#include "../include/VertexBuffer.h"
#include "../include/ErrorHandling.h"

VertexBuffer::VertexBuffer(void* data, unsigned int size) {
  GLCall(glGenBuffers(1, &(this->bufferID)));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::Bind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
}

void VertexBuffer::Unbind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
