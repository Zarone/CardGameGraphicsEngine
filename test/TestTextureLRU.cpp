#include "../include/ErrorHandling.h"
#include "TestTextureLRU.h"
#include "../include/TextureLRU.h"
#include "../include/Texture.h"

void TestTextureLRU() {
  TextureLRU lru;

  lru.Push((Texture*)5);
  lru.Push((Texture*)10);
  lru.Push((Texture*)15);

  ASSERT(lru.GetIndex(0) == (Texture*)15);
  ASSERT(lru.GetIndex(1) == (Texture*)10);
  ASSERT(lru.GetIndex(2) == (Texture*)5);

  lru.Access((Texture*)5);
  ASSERT(lru.GetIndex(0) == (Texture*)5);
  ASSERT(lru.GetIndex(1) == (Texture*)15);
  ASSERT(lru.GetIndex(2) == (Texture*)10);

  Texture* tex = lru.PopLRU();
  ASSERT(tex == (Texture*)10);
  ASSERT(lru.GetIndex(0) == (Texture*)5);
  ASSERT(lru.GetIndex(1) == (Texture*)15);

  tex = lru.PopLRU();
  ASSERT(tex == (Texture*)15);
  ASSERT(lru.GetIndex(0) == (Texture*)5);
}
