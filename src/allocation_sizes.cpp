#include <cassert>
#include <print>
#include <string>
#include <vector>

int main() {
  std::println("std::string (default ctor):");
  std::string string_default_ctor;
  std::println("\tsizeof:   {} B", sizeof(string_default_ctor));
  std::println("\tcapacity: {}", string_default_ctor.capacity());
  std::println("\tsize:     {}", string_default_ctor.size());
  std::println("\tempty:    {}", string_default_ctor.empty());

  std::println("std::string (assign short string):");
  const std::string short_string = "aa";
  assert((short_string.size() <= string_default_ctor.capacity()) &&
         "Short string to long for implementations SSO:");
  string_default_ctor = short_string;
  std::println("\tsizeof:   {} B", sizeof(string_default_ctor));
  std::println("\tcapacity: {}", string_default_ctor.capacity());
  std::println("\tsize:     {}", string_default_ctor.size());
  std::println("\tempty:    {}", string_default_ctor.empty());

  std::println("std::string (assign long string):");
  const std::string long_string =
      std::string(string_default_ctor.capacity() + 1, 'a');
  string_default_ctor = long_string;
  std::println("\tsizeof:   {} B", sizeof(string_default_ctor));
  std::println("\tcapacity: {}", string_default_ctor.capacity());
  std::println("\tsize:     {}", string_default_ctor.size());
  std::println("\tempty:    {}", string_default_ctor.empty());

  std::println("std::string (assign long string that should fit):");
  string_default_ctor = std::string(string_default_ctor.capacity(), 'a');
  std::println("\tsizeof:   {} B", sizeof(string_default_ctor));
  std::println("\tcapacity: {}", string_default_ctor.capacity());
  std::println("\tsize:     {}", string_default_ctor.size());
  std::println("\tempty:    {}", string_default_ctor.empty());

  std::println("std::string (assign long string that needs a new allocation):");
  string_default_ctor = std::string(string_default_ctor.capacity() + 1, 'a');
  std::println("\tsizeof:   {} B", sizeof(string_default_ctor));
  std::println("\tcapacity: {}", string_default_ctor.capacity());
  std::println("\tsize:     {}", string_default_ctor.size());
  std::println("\tempty:    {}", string_default_ctor.empty());

  std::println("std::vector<uint64_t> (default ctor):");
  std::vector<uint64_t> vector_uint64_t_default_ctor;
  std::println("\tsizeof:   {} B", sizeof(vector_uint64_t_default_ctor));
  std::println("\tcapacity: {}", vector_uint64_t_default_ctor.capacity());
  std::println("\tsize:     {}", vector_uint64_t_default_ctor.size());
  std::println("\tempty:    {}", vector_uint64_t_default_ctor.empty());

  std::println("std::vector<uint64_t> (insert one element):");
  vector_uint64_t_default_ctor.emplace_back(42);
  std::println("\tsizeof:   {} B", sizeof(vector_uint64_t_default_ctor));
  std::println("\tcapacity: {}", vector_uint64_t_default_ctor.capacity());
  std::println("\tsize:     {}", vector_uint64_t_default_ctor.size());
  std::println("\tempty:    {}", vector_uint64_t_default_ctor.empty());

  std::println("std::vector<uint64_t> (insert one element):");
  vector_uint64_t_default_ctor.emplace_back(999);
  std::println("\tsizeof:   {} B", sizeof(vector_uint64_t_default_ctor));
  std::println("\tcapacity: {}", vector_uint64_t_default_ctor.capacity());
  std::println("\tsize:     {}", vector_uint64_t_default_ctor.size());
  std::println("\tempty:    {}", vector_uint64_t_default_ctor.empty());
}
