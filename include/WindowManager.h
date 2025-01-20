#pragma once

#include <GLFW/glfw3.h>

class WindowManager 
{
private:
  GLFWwindow* window;
  unsigned int nx;
  unsigned int ny;
public:
  WindowManager();
  void SetupOpenGL();
  bool ShouldClose();
  void GetCursorPosition(double* x, double* y);
  void SwapBuffers();
  void PollEvents();

  /*
  *
  * @brief Returns the width divided by the height
  *
  */
  float AspectRatio();

  bool CursorInBounds(double posX, double posY);
  int GetWidth();
  int GetHeight();
  GLFWwindow* GetRawPointer();

  ~WindowManager();
};
