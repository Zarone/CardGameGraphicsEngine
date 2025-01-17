#include <GL/glew.h>
#include <iostream>
#include "../include/VertexBuffer.h"
#include "../include/ErrorHandling.h"

VertexBuffer::VertexBuffer() {
}

VertexBuffer::VertexBuffer(void* data, unsigned int size, bool dynamicDraw) {
  GLCall(glGenBuffers(1, &(this->bufferID)));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

void VertexBuffer::Bind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
}

void VertexBuffer::Unbind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::RewriteData(void* data, unsigned int size, bool dynamicDraw) {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

void VertexBuffer::OverwriteData(void* data, unsigned int offset, unsigned int size) {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}
