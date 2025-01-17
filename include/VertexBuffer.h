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
  void RewriteData(void* data, unsigned int size, bool dynamicDraw);
  void OverwriteData(void* data, unsigned int offset, unsigned int size);
};
