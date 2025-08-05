# CppPlayground

This is my playground to explore C++ concepts, ideas or investigate how things work by implementing them myself.

## Current Experiments

### Compile-Time GCD

Demonstrate how to comput the greated common divisor of two numbers at compile time.

### `ThreadRacer`

Demonstrates multiple C++ concurrency/threading concepts (`<atomic>`, `<mutex>`, `<condition_variable>`, `std::jthread`) by letting multiple threads race against each other.
Aims to achieve low latency (currently usually few hundred nanoseconds depending on platform and number of threads) when starting the threads and shows how to measure time with `<chrono>`.

### `CharString`

A basic string implementation with allocation-free short-string-optimization while demonstrating the rule of five/rule of zero.

### `ArcPointer`

Atomically reference counted smart pointer for a shared memory resource (similar to `std::shared_ptr` or Rust's `std::sync::Arc`)

### `AllocationSizes`

Demonstrate the short-string-optimization of `std::string` and the growth of capacity ins `std::vector`
