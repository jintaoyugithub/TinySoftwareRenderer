#include "../headers/win64.h"
#include <iostream>
#include <memory>

typedef Win64 window_t;

int main() {
  // std::cout << "Creating window\n";

  std::unique_ptr<Win64> pWindow = std::make_unique<window_t>();

  if (pWindow == nullptr) {
    std::cout << "fail to create a window\n";
    return 0;
  }

  bool running = true;

  while (running) {
    // std::cout << "in render loop\n";

    if (!pWindow->ProcessMessages()) {
      std::cout << "Closing Window\n";
      running = false;
    }
  }

  return 0;
}
