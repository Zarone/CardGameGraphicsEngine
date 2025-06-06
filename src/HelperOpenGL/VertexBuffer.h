#pragma once

#include <GL/glew.h>
#include "../Helper/ErrorHandling.h"

class VertexBuffer
{
private:
  #ifdef DEBUG
  bool tmpUnwritten = false;
  #endif

  unsigned int bufferID;
public:
  VertexBuffer();
  VertexBuffer(void* data, unsigned int size, bool dynamicDraw = false);
  void Bind();
  void Unbind();
  void OverwriteData(void* data, unsigned int offset, unsigned int size);
  void OverwriteAttrib(
    unsigned int attribIndex,
    int count,
    int type,
    GLsizei stride,
    const void* offset
  );
};
