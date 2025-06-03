#pragma once

#include <GLFW/glfw3.h>
#include <chrono>

typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1LL, 1000000000LL>>> Time;
typedef std::chrono::duration<float, std::ratio<1, 1000>> TimeDifference;

class WindowManager 
{
private:
  GLFWwindow* window;
  unsigned int nx;
  unsigned int ny;

  Time currentTime = std::chrono::high_resolution_clock::now();
  Time lastTime = currentTime;
  TimeDifference diffTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(currentTime - lastTime);
  double deltaTime = 0;
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
  int GetMaxBindableTextures();

  double DeltaTime();

  ~WindowManager();
};
