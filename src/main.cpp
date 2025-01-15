#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/shaders/allShaders.h"
#include "../include/WindowManager.h"
#include "../include/Shader.h"
#include "../include/ErrorHandling.h"
#include "../include/VertexBuffer.h"
#include "../include/IndexBuffer.h"
#include "../include/VertexArray.h"

int main(void)
{
  WindowManager myWindow = WindowManager();

  if (glewInit() != GLEW_OK) {
    std::cout << "Error with glew" << std::endl;
  }

  std::cout << glGetString(GL_VERSION) << std::endl;

  float z1 = -0.5f;
  float z2 = 0.5f;
  GLfloat positions[] = {
    -0.5f, -0.5f, z1,
    -0.5f, 0.5f, z1,
    0.5f, -0.5f, z1,
    0.5f, 0.5f, z1,
    -0.5f, -0.5f, z2,
    -0.5f, 0.5f, z2,
    0.5f, -0.5f, z2,
    0.5f, 0.5f, z2,
  };

  GLuint indices[] = {
    0, 1, 2,
    2, 1, 3,

    4, 5, 6,
    6, 5, 7,

    1, 5, 3,
    3, 5, 7,
  };

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  VertexArray vao = VertexArray();
  vao.Bind();

  VertexBuffer buffer = VertexBuffer(positions, 3*8*sizeof(float));

  MemoryLayout layout = MemoryLayout();
  layout.AddMemoryElement(GL_FLOAT, 3);

  vao.AddBuffer(buffer, layout);

  IndexBuffer indexBuffer = IndexBuffer(indices, 6);

  Shader shader = Shader(myShaders::basicVertex, myShaders::basicFragment);

  shader.Bind();

  glm::mat4 projMatrix = glm::perspective(
    glm::radians(45.0f), myWindow.AspectRatio(),
    0.1f, 1000.0f
  );
  GLint projID = shader.GetUniformLocation("projMatrix");
  glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(projMatrix));

  glm::vec3 eyeLocation = glm::vec3(0.0f, -1.6f, 5.5f);
  glm::mat4 cameraMatrix = glm::lookAt(
    eyeLocation,
    eyeLocation+glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
  GLint camID = shader.GetUniformLocation("cameraMatrix");
  glUniformMatrix4fv(camID, 1, GL_FALSE, glm::value_ptr(cameraMatrix));

  glm::mat4 identity = glm::mat4(1.0f);
  identity = glm::rotate(identity, glm::radians(9.9f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotationMatrix = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  GLint rotID = shader.GetUniformLocation("rotationMatrix");
  glUniformMatrix4fv(rotID, 1, GL_FALSE, glm::value_ptr(rotationMatrix));

  float x = 0.1;

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    x+=0.5;
    glm::mat4 rotationMatrix = glm::rotate(identity, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(rotID, 1, GL_FALSE, glm::value_ptr(rotationMatrix));

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLCall(glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr));

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

