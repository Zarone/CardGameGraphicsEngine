#pragma once
#include <signal.h>
#include <unordered_map>
#include <list>
#include <iostream>

#define DEBUG
#ifdef DEBUG
#define ASSERT(x) if (!(x)) {\
  std::cout << "FAILED ASSERT: " << #x << ", file: " << __FILE__ << ", line: " << __LINE__ << std::endl;raise(SIGTRAP);\
}
#else
#define ASSERT(x) {}
#endif

#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

template<typename T>
void PrintVector(std::ostream& os, std::list<T> const &map)
{
  os << "printing vector: ({addr}, {val})" << std::endl;
  for (auto &pair: map) {
    os << "(" << &pair << ", " << pair << ")" << std::endl;
  }
}
template<typename T>
void PrintVector(std::ostream& os, std::vector<T> const &map)
{
  os << "printing vector: ({addr}, {val})" << std::endl;
  for (auto &pair: map) {
    os << "(" << &pair << ", " << pair << ")" << std::endl;
  }
}

template<typename T>
void PrintArray(std::ostream& os, T* array, int size)
{
  os << "printing vector: ({addr}, {val})" << std::endl;
  for (int i = 0; i < size; ++i) {
    os << "(" << &(array[i]) << ", " << array[i] << ")" << std::endl;
  }
}

// Generic helper to print any value
template<typename T>
void PrintValue(std::ostream& os, T const& value) {
  os << value; // Default behavior for printable types
}

// Specialization for std::__list_iterator
template<typename K, typename V>
void PrintValue(std::ostream& os, std::__list_iterator<K, V> const& it) {
  if (it != std::__list_iterator<K, V>()) { // Check if the iterator is valid
    os << &*it;
  } else {
    os << "Invalid Iterator";
  }
}

template<typename K, typename V>
void PrintMapToStream(std::ostream& os, std::unordered_map<K, V> const& map) {
  os << "printing map: ({key}, {val})" << std::endl;
  for (const auto& pair : map) {
    os << "(" << pair.first << ", ";
    PrintValue(os, pair.second);
    os << ")" << std::endl;
  }
}
