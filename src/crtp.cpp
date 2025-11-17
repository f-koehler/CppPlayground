#include <iostream>

#ifdef CPP23_VERSION

template <class Derived> class Interface {
protected:
  // prevent construction of Interface objects ([[Undefined Behavior]])
  Interface() = default;

public:
  // new deducing this syntax to avoid having to cast
  void print_name(this auto &&self) { self.print_name_impl(); }
};

class D1 : public Interface<D1> {
public:
  void print_name_impl() { std::cout << "D1\n"; }
};
class D2 : public Interface<D2> {
public:
  void print_name_impl() { std::cout << "D2\n"; }
};

#else

template <class Derived> class Interface {
protected:
  // prevent construction of Interface objects ([[Undefined Behavior]])
private:
  Interface() = default;

protected:
public:
  void print_name() { static_cast<Derived *>(this)->print_name_impl(); }
  friend Derived;
};

class D1 : public Interface<D1> {
public:
  static void print_name_impl() { std::cout << "D1\n"; }
};
class D2 : public Interface<D2> {
public:
  static void print_name_impl() { std::cout << "D2\n"; }
};

#endif

int main() {
  D1 d1;
  D2 d2;
  d1.print_name();
  d2.print_name();
}
