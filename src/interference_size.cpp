#include <cstddef>
#include <new>
#include <print>

struct MyStruct {
  alignas(std::hardware_destructive_interference_size) int a;
  alignas(std::hardware_destructive_interference_size) int b;
};

int main() {
  std::println("std::hardware_constructive_interference_size = {}",
               std::hardware_constructive_interference_size);
  std::println("std::hardware_destructive_interference_size  = {}",
               std::hardware_destructive_interference_size);
  std::println("sizeof(MyStruct)                             = {}",
               sizeof(MyStruct));
  std::println("offsetof(MyStruct, a)                        = {}",
               offsetof(MyStruct, a));
  std::println("offsetof(MyStruct, b)                        = {}",
               offsetof(MyStruct, b));
}
