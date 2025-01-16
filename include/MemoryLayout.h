#pragma once

#include <vector>

typedef struct MemoryElement {
  unsigned int type;
  unsigned int count;
} Element;

class MemoryLayout
{
private:
  unsigned int stride;
  std::vector<MemoryElement> elements;
public:
  MemoryLayout();
  static unsigned int SizeOfType(unsigned int type);
  void AddMemoryElement(unsigned int type, unsigned int count);
  std::vector<MemoryElement> GetElements() const;
  unsigned int GetStride() const;
};
