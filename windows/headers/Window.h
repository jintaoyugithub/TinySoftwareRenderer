#pragma once

#include "Buffer.h"

typedef enum { KEY_A, KEY_S, KEY_D, KEY_W, KEY_SPACE } keyboardInput;
typedef enum { MOUSE_L, MOUSE_R, MOUSE_MID } mouseInput;

class Window {
public:
  Window(int width, int height, const char *title)
      : m_width(width), m_height(height), m_title(title){};
  ~Window() = default;

  // basic window funcs
  virtual void createWindow(int width = 800, int height = 600,
                            const char *title = "Title") = 0;
  virtual void destoryWindow() = 0;
  virtual void handleKeyboardMsg(keyboardInput key) = 0;
  virtual void handleMouseButtonMsg(mouseInput mouse) = 0;
  virtual void handleMouseScroolMsg(float offset) = 0;

  inline size_t getWidth() const { return m_width; }
  inline size_t getHeight() const { return m_height; }

  // be able to draw different types of buffers
  template <typename T> void drawBuffer(Buffer<T> *buffer);

  // don't want copy operation
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

protected:
  size_t m_width;
  size_t m_height;
  const char *m_title;
};
