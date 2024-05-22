#pragma once

template <typename T> class Buffer {
public:
  virtual void cleanBuffer(T value);
  // get access to the buffer data at a specific location
  virtual T &operator[](int idx);

  virtual inline size_t getWidth() const { return m_width; }
  virtual inline size_t getHeight() const { return m_height; }

private:
  size_t m_width;
  size_t m_height;
  T *data;
};
