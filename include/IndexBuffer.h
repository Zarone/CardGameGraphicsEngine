#pragma once

class IndexBuffer
{
private:
  unsigned int bufferID;
public:
  IndexBuffer(void* data, unsigned int triangleCount);
  void Bind();
  void Unbind();
};
