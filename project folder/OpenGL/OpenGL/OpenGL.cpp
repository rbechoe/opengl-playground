#include <iostream>
#include <glad\glad.h>
#include "GLFW\glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include "stb_image.h"
#include <shaders/shader.h> // is in include directory
#include "utils.h"

// forward declarations
void RenderTerrain(glm::mat4 view, glm::mat4 projection);
void RenderSkybox(glm::mat4 view, glm::mat4 projection);
void SetupResources();

// settings
float lightIntensity = 0.25;
float lightColor[] = { 1, 1, 1 };
int sunSize = 128;
int posX = 0, posY = 200, posZ = 0;
float moveSpeed = 0.25;
glm::vec3 sunColor(0.85, 0.55, 0.15);
glm::vec3 cameraPosition(posX, posY, posZ), cameraForward(0, 0, 1), cameraUp(0, 1, 0);

unsigned int plane, size, VAO, VBO, EBO, cubeSize;
unsigned int skyProgram, myProgram;
unsigned int diffuseTex, heightmapID, normalmapID, dirtID, sandID, grassID, rockID, snowID;
int lightIntensityLoc;
int lightColorLoc;
int width = 1280, height = 720;

void handleInput(GLFWwindow* window, float deltaTime) {
    static bool w, s, a, d, space, ctrl, shift;
    static double cursorX = -1, cursorY = -1, lastCursorX, lastCursorY;
    static float pitch, yaw;
    static float speed = 100.0f;

    float sensitivity = 33.0f * deltaTime;
    float step = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)				w = true;
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)		w = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)				s = true;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)		s = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)				a = true;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)		a = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)				d = true;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)		d = false;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)				space = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)		space = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)		ctrl = true;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)	ctrl = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)		    shift = true;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)	shift = false;

    if (cursorX == -1) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
    }

    lastCursorX = cursorX;
    lastCursorY = cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    glm::vec2 mouseDelta(cursorX - lastCursorX, cursorY - lastCursorY);

    // TODO: calculate rotation & movement
    yaw -= mouseDelta.x * sensitivity;
    pitch += mouseDelta.y * sensitivity;

    if (pitch < -90.0f) pitch = -90.0f;
    else if (pitch > 90.0f) pitch = 90.0f;
    if (yaw < -180.0f) yaw += 360;
    else if (yaw > 180.0f) yaw -= 360;

    glm::vec3 translation(0, 0, 0);
    if (shift) moveSpeed = 1; else moveSpeed = 0.25;
    if (w) translation.z = 1 * moveSpeed;
    if (a) translation.x = 1 * moveSpeed;
    if (s) translation.z = -1 * moveSpeed;
    if (d) translation.x = -1 * moveSpeed;

    glm::vec3 euler(glm::radians(pitch), glm::radians(yaw), 0);
    glm::quat q(euler);

    // update camera position / forward & up
    cameraPosition += q * translation;

    cameraUp = q * glm::vec3(0, 1, 0);
    cameraForward = q * glm::vec3(0, 0, 1);
}

int main()
{
    static double previousT = 0;

    std::cout << "Konichiwa World!\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Open GL Test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, width, height);

    SetupResources();

    // MATRIX SETUP
    glUseProgram(myProgram);
    int worldLoc = glGetUniformLocation(myProgram, "world");
    int viewLoc = glGetUniformLocation(myProgram, "view");
    int projLoc = glGetUniformLocation(myProgram, "projection");
    lightIntensityLoc = glGetUniformLocation(myProgram, "vLightIntensity");
    lightColorLoc = glGetUniformLocation(myProgram, "vLightColor");
    // END MATRIX SETUP


    // OPENGL SETTINGS
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window))
    {
        double t = glfwGetTime();

        handleInput(window, (float)(t - previousT));
        previousT = t;

        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), width / (float)height, 0.1f, 1000.0f);

        // clean screen
        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // render
        RenderSkybox(view, projection);
        RenderTerrain(view, projection);

        // clean up data or something idk
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // memory management
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void RenderSkybox(glm::mat4 view, glm::mat4 projection)
{
    glUseProgram(skyProgram);
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);
    glm::mat4 world = glm::mat4(1.0f);

    world = glm::translate(world, cameraPosition);

    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(skyProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
    glUniform3fv(glGetUniformLocation(skyProgram, "sunColor"), 1, glm::value_ptr(sunColor));
    glUniform1i(glGetUniformLocation(skyProgram, "sunSize"), sunSize);
    glUniform1f(lightIntensityLoc, lightIntensity);
    glUniform3fv(lightColorLoc, 3, lightColor);

    // sun
    float t = glfwGetTime();
    glUniform3f(glGetUniformLocation(skyProgram, "lightDirection"), glm::cos(t), -0.5f, glm::sin(t));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cubeSize, GL_UNSIGNED_INT, 0);
}

void RenderTerrain(glm::mat4 view, glm::mat4 projection)
{
    glUseProgram(myProgram);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glm::mat4 world = glm::mat4(1.0f);

    world = glm::translate(world, glm::vec3(0, 0, 0));

    glUniformMatrix4fv(glGetUniformLocation(myProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(myProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(myProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(myProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

    // sun cycle
    float t = glfwGetTime();
    glUniform3f(glGetUniformLocation(myProgram, "lightDirection"), glm::cos(t), -0.5f, glm::sin(t));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmapID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalmapID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dirtID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, sandID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, grassID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, rockID);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, snowID);

    glBindVertexArray(plane);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
}

void SetupResources()
{
    // Vertices of our triangle!
    // need 24 vertices for normal/uv-mapped Cube
    float vertices[] = {
        // positions            //colors            // tex coords   // normals
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 2.f,       0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   -1.f, 0.f,      -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,
        -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       0.f, 1.f, 0.f
    };

    unsigned int indices[] = {  // note that we start from 0!
        // DOWN
        0, 1, 2,   // first triangle
        0, 2, 3,    // second triangle
        // BACK
        14, 6, 7,   // first triangle
        14, 7, 15,    // second triangle
        // RIGHT
        20, 4, 5,   // first triangle
        20, 5, 21,    // second triangle
        // LEFT
        16, 8, 9,   // first triangle
        16, 9, 17,    // second triangle
        // FRONT
        18, 10, 11,   // first triangle
        18, 11, 19,    // second triangle
        // UP
        22, 12, 13,   // first triangle
        22, 13, 23,    // second triangle
    };

    cubeSize = sizeof(indices);

    VAO; // vertex array object
    VBO; // vertex buffer object
    EBO; // element buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int stride = sizeof(float) * 11;

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    // normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // END SETUP OBJECT

    heightmapID;
    plane = GeneratePlane("heightmap.png", GL_RGBA, 4, 1.0f, 1.0f, size, heightmapID);

    // terrain textures
    diffuseTex = loadTexture("wall.jpg", GL_RGB, 3);
    normalmapID = loadTexture("normalmap.png", GL_RGBA, 4);
    grassID = loadTexture("grass.png", GL_RGBA, 4);
    dirtID = loadTexture("dirt.jpg", GL_RGB, 3);
    sandID = loadTexture("sand.jpg", GL_RGB, 3);
    rockID = loadTexture("rock.jpg", GL_RGB, 3);
    snowID = loadTexture("snow.jpg", GL_RGB, 3);

    // SHADER PROGRAM
    stbi_set_flip_vertically_on_load(true);

    unsigned int vertShader, fragShader;
    CreateShader("vertex.shader", GL_VERTEX_SHADER, vertShader);
    CreateShader("fragment.shader", GL_FRAGMENT_SHADER, fragShader);

    unsigned int vertSky, fragSky;
    CreateShader("vertexSky.shader", GL_VERTEX_SHADER, vertSky);
    CreateShader("fragmentSky.shader", GL_FRAGMENT_SHADER, fragSky);

    myProgram = glCreateProgram();
    glAttachShader(myProgram, vertShader);
    glAttachShader(myProgram, fragShader);
    glLinkProgram(myProgram);

    skyProgram = glCreateProgram();
    glAttachShader(skyProgram, vertSky);
    glAttachShader(skyProgram, fragSky);
    glLinkProgram(skyProgram);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    glDeleteShader(vertSky);
    glDeleteShader(fragSky);
    // END SHADER PROGRAM

    glUseProgram(myProgram);
    glUniform1i(glGetUniformLocation(myProgram, "heightmap"), 0);
    glUniform1i(glGetUniformLocation(myProgram, "normalmap"), 1);

    glUniform1i(glGetUniformLocation(myProgram, "dirt"), 2);
    glUniform1i(glGetUniformLocation(myProgram, "sand"), 3);
    glUniform1i(glGetUniformLocation(myProgram, "grass"), 4);
    glUniform1i(glGetUniformLocation(myProgram, "rock"), 5);
    glUniform1i(glGetUniformLocation(myProgram, "snow"), 6);
}
