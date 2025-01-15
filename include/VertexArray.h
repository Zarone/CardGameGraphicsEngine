#pragma once

#include "MemoryLayout.h"
#include "VertexBuffer.h"

class VertexArray
{
private:
  unsigned int arrayID;
  unsigned int numAttribs;
public:
  VertexArray();
  void Bind();
  void Unbind();
  void AddBuffer(VertexBuffer& buffer, const MemoryLayout& layout);
};
