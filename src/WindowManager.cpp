#include <cstdlib>
#include <GL/glew.h>
#include <iostream>
#include "../include/WindowManager.h"
#include "../include/ErrorHandling.h"

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

void WindowManager::SetupOpenGL() {
  if (glewInit() != GLEW_OK) {
    std::cout << "Error with glew" << std::endl;
  }

  std::cout << glGetString(GL_VERSION) << std::endl;

  GLCall(glEnable(GL_DEPTH_TEST));
  GLCall(glDepthFunc(GL_LESS));

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

bool WindowManager::ShouldClose() {
  return glfwWindowShouldClose(window);
}

void WindowManager::GetCursorPosition(double* x, double* y) {
  glfwGetCursorPos(this->window, x, y);
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

bool WindowManager::CursorInBounds(double posX, double posY) {
  return posX > 0 && posY > 0 && posX < this->nx && posY < this->ny;
}

int WindowManager::GetWidth() {
  return this->nx;
}

int WindowManager::GetHeight() {
  return this->ny;
}

GLFWwindow* WindowManager::GetRawPointer() {
  return this->window;
}

int WindowManager::GetMaxBindableTextures() {
  int maxBindableTextures;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBindableTextures);
  return maxBindableTextures;
}

WindowManager::~WindowManager() {
  glfwTerminate();
}
