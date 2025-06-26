#include "./TestGameplayPiles.h"
#include <stdexcept>

Pile::Pile() {
  this->data = "";
}

Pile::Pile(const std::string& str) {
  bool found = false;
  for (int i = 0; i < sizeof(this->ValidPiles)/sizeof(std::string); ++i) {
    if (str.compare(this->ValidPiles[i])==0) {
      found = true;
      break;
    }
  }
  if (!found) throw std::runtime_error("could not find pile" + str);
  this->data = str;
}

const std::string& Pile::GetInString() const {
  return this->data;
}

bool operator== (const Pile& p1, const Pile& p2) {
  return p1.GetInString().compare(p2.GetInString()) == 0;
}

bool operator== (const Pile& p1, const std::string& s) {
  return p1.GetInString().compare(s) == 0;
}

bool operator!= (const Pile& p1, const Pile& p2) {
  return !(p1 == p2);
}
