#pragma once

#include <string>
#include <vector>

class Pile {
private:
  std::string data;
  constexpr static std::string ValidPiles[] = {
    "HAND", 
    "DECK", 
    "DISCARD", 
    "OPP_HAND", 
    "OPP_DECK", 
    "OPP_DISCARD"
  };
public:
  Pile();
  Pile(const std::string& str);
  const std::string& GetInString() const;
  friend bool operator== (const Pile& p1, const Pile& p2);
  friend bool operator== (const Pile& p, const std::string& s);
};
