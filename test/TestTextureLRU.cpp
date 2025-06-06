#include "TestTextureLRU.h"
#include "../src/Helper/ErrorHandling.h"
#include "../src/HelperRendering/TextureLRU.h"
#include "../src/HelperRendering/Texture.h"

void TestTextureLRUBasic(){
  TextureLRU lru;

  Texture tex1("../assets/card1.jpg", true);
  Texture tex2("../assets/card2.jpg", true);
  Texture tex3("../assets/card3.jpg", true);

  lru.Push(&tex1);
  lru.Push(&tex2);
  lru.Push(&tex3);

  ASSERT(lru.GetIndex(0) == &tex3);
  ASSERT(lru.GetIndex(1) == &tex2);
  ASSERT(lru.GetIndex(2) == &tex1);

  lru.Access(&tex1);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);
  ASSERT(lru.GetIndex(2) == &tex2);

  Texture* tex = lru.PopLRU();
  ASSERT(tex == &tex2);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);

  tex = lru.PopLRU();
  ASSERT(tex == &tex3);
  ASSERT(lru.GetIndex(0) == &tex1);
}

void TestTextureLRURepeated(){
  TextureLRU lru;

  Texture tex1("../assets/card1.jpg", true);
  Texture tex2("../assets/card2.jpg", true);
  Texture tex3("../assets/card3.jpg", true);

  lru.Push(&tex1);
  lru.Push(&tex2);
  lru.Push(&tex3);

  ASSERT(lru.GetIndex(0) == &tex3);
  ASSERT(lru.GetIndex(1) == &tex2);
  ASSERT(lru.GetIndex(2) == &tex1);

  lru.Access(&tex1);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);
  ASSERT(lru.GetIndex(2) == &tex2);

  lru.Access(&tex1);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);
  ASSERT(lru.GetIndex(2) == &tex2);

  lru.Access(&tex1);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);
  ASSERT(lru.GetIndex(2) == &tex2);

  Texture* tex = lru.PopLRU();
  ASSERT(tex == &tex2);
  ASSERT(lru.GetIndex(0) == &tex1);
  ASSERT(lru.GetIndex(1) == &tex3);

  tex = lru.PopLRU();
  ASSERT(tex == &tex3);
  ASSERT(lru.GetIndex(0) == &tex1);
}

void TestTextureLRU() {
  TestTextureLRUBasic();
  TestTextureLRURepeated();
}
