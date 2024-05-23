#pragma once

#include <Windows.h>
#include <iostream>

#include "Buffer.h"
#include "Window.h"

class Win64 : public Window {
public:
  // why pure class will need a constructor
  Win64(int width = 800, int height = 600, const char *title = "Title",
        const char *className = "win64 class");
  ~Win64();

  void createWindow(int width, int height, const char *title) override;
  void destoryWindow() override;

  // handle the devices input
  bool ProcessMessages();

  void handleKeyboardMsg(keyboardInput key) override;
  void handleMouseButtonMsg(mouseInput button) override;
  void handleMouseScroolMsg(float offset) override;

  // handle the graphics drawing
  template <typename T> void drawBuffer(Buffer<T> *buffer);

  // derived class will inherit the behavior of parents by default
  // no need to delete assignment and copy constructor again

private:
  // you can't use window procedure directly as a member func
  // that's why I defined these two static funcs to eventually call
  // the func wndProc
  static LRESULT CALLBACK handleMsgSetUp(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam);
  static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam);
  LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  HWND m_hWnd;
  HINSTANCE m_hInstance;
  DWORD m_style;
  const char *m_className;
};
