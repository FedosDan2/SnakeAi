#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

int glfwWindow_width = 640;
int glfwWindow_height = 480;

float glfwColorRed = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorGreen = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorBlue = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorAlpha = static_cast<float>(std::rand() % 256) / 255.0f; // usually set to 1.0 for opaque


void glfwWindowsSizeCallback(GLFWwindow * pWindow, int width, int height)
{
    glfwWindow_width = width;
    glfwWindow_height = height;
    glViewport(0, 0, glfwWindow_width, glfwWindow_height);
}

void glfwColorRandomCallback()
{
    glfwColorRed = static_cast<float>(std::rand() % 256) / 255.0f;
    glfwColorGreen = static_cast<float>(std::rand() % 256) / 255.0f;
    glfwColorBlue = static_cast<float>(std::rand() % 256) / 255.0f;
    glfwColorAlpha = static_cast<float>(std::rand() % 256) / 255.0f;

    glClearColor(glfwColorRed, glfwColorGreen, glfwColorBlue, glfwColorAlpha);
}

void glfwKeyCallback(GLFWwindow * pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        glfwColorRandomCallback();
    }
}

int main()
{
    /* Initialize the library */
    if (!glfwInit()){
        std::cout << "glfwInit error\n";
        return -1;
    }

    /* OpenGL version restrictions */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* pWindow = glfwCreateWindow(glfwWindow_width, glfwWindow_height, "SnakeAI", nullptr, nullptr);
    if (!pWindow){
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pWindow, glfwWindowsSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    /* Make the window's context current */
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can`t load GLAD\n";
        return -1;
    }
    
    std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;
    glClearColor(glfwColorRed, glfwColorGreen, glfwColorBlue, glfwColorAlpha);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(pWindow))
    {        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(pWindow);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}