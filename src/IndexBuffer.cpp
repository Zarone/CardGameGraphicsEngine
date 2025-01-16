#include <GL/glew.h>
#include "../include/IndexBuffer.h"
#include "../include/ErrorHandling.h"

IndexBuffer::IndexBuffer() {}

IndexBuffer::IndexBuffer(void* data, unsigned int triangleCount) {
  GLCall(glGenBuffers(1, &(this->bufferID)));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->bufferID));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount*3*sizeof(GLuint), data, GL_STATIC_DRAW));
}

void IndexBuffer::Bind() {
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->bufferID));
}

void IndexBuffer::Unbind() {
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


