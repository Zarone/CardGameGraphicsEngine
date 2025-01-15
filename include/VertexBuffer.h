#pragma once

class VertexBuffer
{
private:
  unsigned int bufferID;
public:
  VertexBuffer(void* data, unsigned int size);
  void Bind();
  void Unbind();
};
