#pragma once

#include <Windows.h>
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
public:
  Window(int width = 800, int height = 600, char const *title = "Title");
  ~Window();

  // process the events
  bool ProcessMessages();

  // don't want copy operation
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

private:
  int m_width;
  int m_height;
  char *m_title;
  HWND m_hWnd;
  HINSTANCE m_hInstance;
  DWORD m_style;
};
