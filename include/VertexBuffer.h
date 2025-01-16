#pragma once

class VertexBuffer
{
private:
  unsigned int bufferID;
public:
  VertexBuffer();
  VertexBuffer(void* data, unsigned int size, bool dynamicDraw = false);
  void Bind();
  void Unbind();
};
