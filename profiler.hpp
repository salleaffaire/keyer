#include <x86intrin.h>

#include <chrono>
#include <functional>

uint64_t rdtsc() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

void __PROFILE(std::function<void()> f, bool type = true) {
  if (type) {
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    // Call the function
    f();

    // End time
    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    double duration = elapsed.count();
    std::cout << duration << " seconds." << std::endl;
  } else {
    // Start time
    auto start = rdtsc();

    // Call the function
    f();

    // End time
    auto finish = rdtsc();

    std::cout << finish - start << " ticks." << std::endl;
  }
}