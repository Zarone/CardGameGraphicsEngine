cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 11)
project(GraphicsEngine LANGUAGES CXX)
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
)
FetchContent_MakeAvailable(glm)

find_package(GLEW)

# Add the executable with all source files
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/shaders/BasicVertex.cpp
    src/shaders/BasicFragment.cpp
    src/shaders/CardVertex.cpp
    src/shaders/CardFragment.cpp
    src/WindowManager.cpp
    src/Shader.cpp
    src/ErrorHandling.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/MemoryLayout.cpp
    src/Texture.cpp
    src/external/stb_image.cpp
    src/Renderer.cpp
    src/CardRenderingData.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw GLEW::GLEW glm)

if (APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

