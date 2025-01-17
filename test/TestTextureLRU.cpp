#include "../include/ErrorHandling.h"
#include "TestTextureLRU.h"
#include "../include/TextureLRU.h"
#include "../include/Texture.h"

void TestTextureLRU() {
  TextureLRU lru;

  lru.Push((Texture*)5);
  lru.Push((Texture*)10);
  lru.Push((Texture*)15);

  ASSERT(lru.GetIndex(0) == (Texture*)5);
  std::cout << "here in test" << std::endl;
  ASSERT(lru.GetIndex(1) == (Texture*)10);
  ASSERT(lru.GetIndex(2) == (Texture*)15);

  lru.PrintData();

  lru.Access((Texture*)5);

  lru.PrintData();

  lru.PopLRU();

  lru.PrintData();

  lru.PopLRU();

  lru.PrintData();
}
