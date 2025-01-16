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
#include "../include/Texture.h"

int main(void)
{
  WindowManager myWindow = WindowManager();

  if (glewInit() != GLEW_OK) {
    std::cout << "Error with glew" << std::endl;
  }

  std::cout << glGetString(GL_VERSION) << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  glm::mat4 projMatrix = glm::perspective(
    glm::radians(45.0f), myWindow.AspectRatio(),
    0.1f, 1000.0f
  );

  //glm::vec3 eyeLocation = glm::vec3(0.0f, -1.6f, 5.5f);
  glm::vec3 eyeLocation = glm::vec3(0.0f, 0.0f, 5.5f);
  glm::mat4 cameraMatrix = glm::lookAt(
    eyeLocation,
    eyeLocation+glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );

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

  float z3 = 0.0f;
  GLfloat cardPositions[] = {
    -0.5f,  0.5f*1.396f, z3, 0.0f, 0.0f,
    0.5f ,  0.5f*1.396f, z3, 1.0f, 0.0f,
    0.5f , -0.5f*1.396f, z3, 1.0f, 1.0f,
    -0.5f, -0.5f*1.396f, z3, 0.0f, 1.0f,
  };

  GLuint cardIndices[] = {
    3, 2, 0,
    0, 2, 1
  };


  VertexArray vao = VertexArray();
  vao.Bind();
  VertexBuffer buffer = VertexBuffer(positions, 8*3*sizeof(float));
  MemoryLayout layout = MemoryLayout();
  layout.AddMemoryElement(GL_FLOAT, 3);
  vao.AddBuffer(buffer, layout);
  IndexBuffer indexBuffer = IndexBuffer(indices, 6);
  Shader shader = Shader(myShaders::basicVertex, myShaders::basicFragment);
  shader.Bind();

  GLint projID = shader.GetUniformLocation("projMatrix");
  glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(projMatrix));
  GLint camID = shader.GetUniformLocation("cameraMatrix");
  glUniformMatrix4fv(camID, 1, GL_FALSE, glm::value_ptr(cameraMatrix));

  glm::mat4 identity = glm::mat4(1.0f);
  identity = glm::rotate(identity, glm::radians(9.9f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotationMatrix = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::translate(rotationMatrix, glm::vec3(0.0f, 1.6f, 0.0f));
  GLint rotID = shader.GetUniformLocation("rotationMatrix");
  glUniformMatrix4fv(rotID, 1, GL_FALSE, glm::value_ptr(rotationMatrix));



  VertexArray cardVao = VertexArray();
  cardVao.Bind();
  VertexBuffer cardBuffer = VertexBuffer(cardPositions, 5*4*sizeof(float));
  cardBuffer.Bind();
  MemoryLayout cardMemoryLayout = MemoryLayout();
  cardMemoryLayout.AddMemoryElement(GL_FLOAT, 3);
  cardMemoryLayout.AddMemoryElement(GL_FLOAT, 2);
  cardVao.AddBuffer(cardBuffer, cardMemoryLayout);
  IndexBuffer cardIndexBuffer = IndexBuffer(cardIndices, 2);
  cardIndexBuffer.Bind();
  Shader cardShader = Shader(myShaders::cardVertex, myShaders::cardFragment);
  cardShader.Bind();

  Texture cardTex = Texture("../assets/card.png");
  cardTex.Bind(0);
  GLint cardTextureUniformID = cardShader.GetUniformLocation("frontCardTexture");
  glUniform1i(cardTextureUniformID, 0);

  Texture backCardTex = Texture("../assets/back.png");
  cardTex.Bind(1);
  GLint backCardTextureUniformID = cardShader.GetUniformLocation("backCardTexture");
  glUniform1i(backCardTextureUniformID, 1);

  GLint cardProjID = cardShader.GetUniformLocation("projMatrix");
  glUniformMatrix4fv(cardProjID, 1, GL_FALSE, glm::value_ptr(projMatrix));
  GLint cardCamID = cardShader.GetUniformLocation("cameraMatrix");
  glUniformMatrix4fv(cardCamID, 1, GL_FALSE, glm::value_ptr(cameraMatrix));
  GLint cardRotID = cardShader.GetUniformLocation("rotMatrix");
  
  float x = 0.1;

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    x += 0.5;
    glm::mat4 rotationMatrix = glm::rotate(identity, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::translate(rotationMatrix, glm::vec3(0.0f, 1.6f, 0.0f));
    vao.Bind();
    buffer.Bind();
    indexBuffer.Bind();
    shader.Bind();
    glUniformMatrix4fv(rotID, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    GLCall(glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr));


    cardVao.Bind();
    cardBuffer.Bind();
    cardIndexBuffer.Bind();
    cardShader.Bind();
    cardTex.Bind(0);
    backCardTex.Bind(1);
    rotationMatrix = glm::translate(identity, glm::vec3(0.0f, 0.5f, 3.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(cardRotID, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    
    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

