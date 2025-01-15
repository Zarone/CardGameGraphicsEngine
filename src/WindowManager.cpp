#include "../include/WindowManager.h"
#include <cstdlib>

WindowManager::WindowManager() {
  /* Initialize the library */
  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  /* Create a windowed mode window and its OpenGL context */
  this->nx = 640;
  this->ny = 480;
  this->window = glfwCreateWindow(this->nx, this->ny, "My Window", NULL, NULL);
  if (!this->window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(this->window);
}

bool WindowManager::ShouldClose() {
  return glfwWindowShouldClose(window);
}

void WindowManager::SwapBuffers() {
  glfwSwapBuffers(window);
}

void WindowManager::PollEvents() {
  glfwPollEvents();
}

float WindowManager::AspectRatio() {
  return (float)nx/ny;
}

WindowManager::~WindowManager() {
  glfwTerminate();
}
