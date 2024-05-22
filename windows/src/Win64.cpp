#include "../headers/Win64.h"

Win64::Win64(int width, int height, const char *title, const char *className)
    : Window(width, height, title), m_className(className) {
  // init the window class
  WNDCLASS wndClass = {};
  wndClass.lpszClassName = m_className;
  wndClass.hInstance = m_hInstance;
  wndClass.lpfnWndProc = handleMsgSetUp; // fix later

  // register the window class
  RegisterClass(&wndClass);

  // set up the window style
  m_style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

  // create the window
  m_hWnd = CreateWindowEx(0,           // Optional window styles.
                          m_className, // Window class
                          m_title,     // Window text
                          m_style,     // Window style

                          CW_USEDEFAULT, CW_USEDEFAULT, m_width,
                          m_height, // window size

                          NULL,        // Parent window
                          NULL,        // Menu
                          m_hInstance, // Instance handle
                          this         // Additional application data
  );

  // display the window
  // ShowWindow func also determine how window will present in the system
  // for example: show, hide, minimize, maximize and etc.
  ShowWindow(m_hWnd, SW_SHOW);
}

Win64::~Win64() {
  // unregister the window class
  UnregisterClass(m_className, m_hInstance);
  // destory the window
  destoryWindow();
}

bool Win64::ProcessMessages() {
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

void Win64::createWindow(int width, int height, const char *title) {}

void Win64::destoryWindow() { DestroyWindow(m_hWnd); }

// handle the devices input
void Win64::handleKeyboardMsg(int key) {
  std::cout << "handling keyboard message\n";
}

void Win64::handleMouseButtonMsg(int button) {}

void Win64::handleMouseScroolMsg(float offset) {}

LRESULT CALLBACK Win64::handleMsgSetUp(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam) {
  // we pass the pointer to this window instance in CreateWindowEx func
  // we can store it when the window is created
  // and then get the user data, set message proc to handleMsgThunk
  if (uMsg == WM_NCCREATE) {
    // extract ptr ro window class from creation data
    // as we store the window class pointer as a additional info
    // in CreateWindowEx func
    const CREATESTRUCT *const pCreate =
        reinterpret_cast<CREATESTRUCT *>(lParam);
    Win64 *const pWnd = static_cast<Win64 *>(pCreate->lpCreateParams);
    // set winapi-managed user data to store ptr
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
    // set message proc to handleMsgThunk
    SetWindowLongPtr(hWnd, GWLP_WNDPROC,
                     reinterpret_cast<LONG_PTR>(&Win64::handleMsgThunk));
    // forward message to window class handler, which is wndProc here
    return pWnd->wndProc(hWnd, uMsg, wParam, lParam);
  }

  // handle the messages before WM_NCCREATE
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Win64::handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam) {
  // retrive ptr to window class
  Win64 *const pWnd =
      reinterpret_cast<Win64 *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  // forward message to wndProc
  return pWnd->wndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win64::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  /* you can print the msg if you want,
   * but you can't directly get the msg
   * you have to invoke some specific func
   * or you can map all the msg into string*/

  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_KEYDOWN:
    handleKeyboardMsg(wParam);
    break;
  case WM_KEYUP:
    break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
