#include <GL/glew.h>
#include "../include/IndexBuffer.h"

IndexBuffer::IndexBuffer(void* data, unsigned int triangleCount) {
  glGenBuffers(1, &(this->bufferID));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->bufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount*3*sizeof(GLuint), data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->bufferID);
}

void IndexBuffer::Unbind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


