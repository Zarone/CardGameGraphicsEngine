#pragma once

#include <GL/glew.h>

class VertexBuffer
{
private:
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
