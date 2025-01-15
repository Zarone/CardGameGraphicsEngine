#include "GLFW/glfw3.h"

class WindowManager 
{
private:
  GLFWwindow* window;
  unsigned int nx;
  unsigned int ny;
public:
  WindowManager();
  bool ShouldClose();
  void SwapBuffers();
  void PollEvents();

  /*
  *
  * @brief Returns the width divided by the height
  *
  */
  float AspectRatio();

  ~WindowManager();
};
