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

    GLFWwindow* window = glfwCreateWindow(640, 480, "Open GL Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 640, 480);

    // START SETUP OBJECT
    // vertices of a quad
    float vertices[] =
    {
        // position             // color                    // uv
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.5f, -0.5f, 0.5f,      0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f, 1.0f,     0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f, 1.0f,     1.0f, 1.0f,
    };
    // indices of quad
    unsigned int indices[] =
    {
        0, 1, 2,
        1, 3, 2,
    };

    unsigned int VAO; // vertex array object
    glGenVertexArrays(1, &VAO);
    unsigned int VBO; // vertex buffer object
    glGenBuffers(1, &VBO);
    unsigned int EBO; // element buffer object
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 7));
    glEnableVertexAttribArray(2);

    // clean up the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // END SETUP OBJECT



    // SETUP SHADER PROGRAM
    const char* vertexSource = "#version 330 core\n"
        "layout(location=0) in vec3 vPos;\n"
        "layout(location=1) in vec4 vColor;\n"
        "layout(location=2) in vec2 vUV;\n"
        "out vec4 color;\n"
        "out vec2 uv;\n"
        "void main() {\n"
        "\tgl_Position = vec4(vPos, 1.0);\n"
        "\tcolor = vColor;\n"
        "\tuv = vUV;\n"
        "}\n"
        "\0";

    const char* fragmentSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec4 color;\n"
        "in vec2 uv;\n"
        "void main() {\n"
        "\tFragColor = vec4(uv, 0.0, 1.0);\n"
        "}\n"
        "\0";

    unsigned int vertID, fragID;
    vertID = glCreateShader(GL_VERTEX_SHADER);
    fragID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertID, 1, &vertexSource, nullptr);
    glShaderSource(fragID, 1, &fragmentSource, nullptr);

    int success;
    char infoLog[512];

    glCompileShader(vertID);
    glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    glCompileShader(fragID);
    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    unsigned int myProgram = glCreateProgram();
    glAttachShader(myProgram, vertID);
    glAttachShader(myProgram, fragID);
    glLinkProgram(myProgram);

    glDeleteShader(vertID);
    glDeleteShader(fragID);
    // END SHADER PROGRAM

    while (!glfwWindowShouldClose(window))
    {
        float sinRed = (float)sin(glfwGetTime() * 1.1f);
        float sinGreen = (float)sin(glfwGetTime() * 1.3f);
        float sinBlue = (float)sin(glfwGetTime() * 1.7f);

        glUseProgram(myProgram);

        glClearColor(sinRed, sinGreen, sinBlue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}