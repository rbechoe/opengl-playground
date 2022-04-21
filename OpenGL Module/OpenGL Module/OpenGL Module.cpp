#include <iostream>
#include <glad\glad.h>
#include "GLFW\glfw3.h"

int main()
{
    std::cout << "Konichiwa World!\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(640, 480, "Open GL Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 640, 480);

    while (!glfwWindowShouldClose(window))
    {
        float sinRed = (float)sin(glfwGetTime() * 1.1f);
        float sinGreen = (float)sin(glfwGetTime() * 1.3f);
        float sinBlue = (float)sin(glfwGetTime() * 1.7f);

        glClearColor(sinRed, sinGreen, sinBlue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
