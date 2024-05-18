#include "../headers/Window.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window(int width, int height, char const *title)
    : m_width(width), m_height(height), m_hInstance(GetModuleHandle(nullptr)) {
  const char *CLASS_NAME = "window class";

  // init the window class
  WNDCLASS wndClass = {};
  wndClass.lpszClassName = CLASS_NAME;
  wndClass.hInstance = m_hInstance;
  wndClass.lpfnWndProc = WindowProc; // fix later

  // register the window
  RegisterClass(&wndClass);

  m_style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

  m_hWnd = CreateWindowEx(0,          // Optional window styles.
                          CLASS_NAME, // Window class
                          m_title,    // Window text
                          m_style,    // Window style

                          250, 250, m_width, m_height,

                          NULL,        // Parent window
                          NULL,        // Menu
                          m_hInstance, // Instance handle
                          NULL         // Additional application data
  );

  ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window() {
  const char *CLASS_NAME = "window class";
  UnregisterClass(CLASS_NAME, m_hInstance);
}

bool Window::ProcessMessages() {
  MSG msg = {};

  while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return true;
}
