#include "../headers/Window.h"
#include <iostream>

int main() {
  // std::cout << "Creating window\n";

  Window *pWindow = new Window();

  if (pWindow == nullptr) {
    std::cout << "window is null\n";
    delete pWindow;
    return 0;
  }

  bool running = true;

  while (running) {
    std::cout << "in render loop\n";

    if (!pWindow->ProcessMessages()) {
      std::cout << "Closing Window\n";
      running = false;
    }

    // render
    Sleep(10);
  }

  delete pWindow;

  return 0;
}
