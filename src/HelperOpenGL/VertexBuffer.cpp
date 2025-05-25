#include <GL/glew.h>
#include <iostream>
#include "VertexBuffer.h"
#include "../ErrorHandling.h"

VertexBuffer::VertexBuffer() {
  #ifdef DEBUG
  this->tmpUnwritten = true;
  #endif
}

VertexBuffer::VertexBuffer(void* data, unsigned int size, bool dynamicDraw) {
  #ifdef DEBUG
  this->tmpUnwritten = false;
  #endif
  GLCall(glGenBuffers(1, &(this->bufferID)));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

void VertexBuffer::Bind() {
  #ifdef DEBUG
  if (this->tmpUnwritten) 
    std::cout << "unsafe VertexBuffer bind" << std::endl;
    ASSERT(!this->tmpUnwritten);
  #endif
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
}

void VertexBuffer::Unbind() {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::OverwriteData(void* data, unsigned int offset, unsigned int size) {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void VertexBuffer::OverwriteAttrib(
  unsigned int attribIndex,
  int count,
  int type,
  GLsizei stride,
  const void* offset
) {
  this->Bind();
  GLCall(glVertexAttribPointer(
    attribIndex, 
    count, 
    type, 
    GL_FALSE,
    stride, 
    offset 
  ));
}
