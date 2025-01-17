#include <GL/glew.h>
#include <iostream>

#include "../include/ErrorHandling.h"

void GLClearError() {
  while (glGetError()) {
    std::cout << "unhandled error?" << std::endl;
  }
}

bool GLLogCall(const char* function, const char* file, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error]: (" << error << ")" << function << " " << file << " " << line << std::endl;
    return false;
  }
  return true;
}
