#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include <cmath>

GLuint VAO, vertices_VBO, colors_VBO, shader_program;

int glfwWindow_width = 480;
int glfwWindow_height = 480;

float glfwColorRed = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorGreen = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorBlue = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorAlpha = static_cast<float>(std::rand() % 256) / 255.0f; // usually set to 1.0 for opaque

/* GLobal coordinates of triangle in float */
GLfloat vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
        // second triangle
         0.0f, -0.5f, 0.0f,  // left
         0.9f, -0.5f, 0.0f,  // right
         0.45f, 0.5f, 0.0f   // top 
    }; 



GLfloat colors[] = {
    // Первый треугольник
    1.0f, 0.0f, 0.0f,   // красный
    0.0f, 1.0f, 0.0f,   // зелёный
    0.0f, 0.0f, 1.0f,   // синий

    // Второй треугольник
    1.0f, 1.0f, 0.0f,   // жёлтый
    0.0f, 1.0f, 1.0f,   // голубой
    1.0f, 0.0f, 1.0f    // фиолетовый
};

const char *vertexShaderSource = 
    "#version 460\n"
    "layout(location = 0) in vec3 vertex_position;"
    "layout(location = 1) in vec3 vertex_color;"
    "out vec3 color;"
    "void main(){"
    "   color = vertex_color;"
    "   gl_Position = vec4(vertex_position, 1.0);"
"}";

const char *fragmentShaderSource = 
    "#version 460\n"
    "in vec3 color;"
    "out vec4 frag_color;"
    "void main(){"
    "   frag_color = vec4(color, 1.0);"
"}";



void glfwWindowsSizeCallback(GLFWwindow * pWindow, int width, int height)
{
    glfwWindow_width = width;
    glfwWindow_height = height;
    glViewport(0, 0, glfwWindow_width, glfwWindow_height);
}

void glfwTriangleSpin(float angle, GLfloat* vertices, size_t vertexCount)
{
    float cos_ang = cos(angle);
    float sin_ang = sin(angle);

    for (size_t j = 0; j < 2; j++)
    {
        float currentCos = (j == 0) ? cos_ang : cos(-angle);
        float currentSin = (j == 0) ? sin_ang : sin(-angle);

        float center_X = 0.0f;
        float center_Y = 0.0f;

        size_t i = j * 9;
        for (int k = 0; k < 3; ++k, i += 3)
        {
            center_X += vertices[i];
            center_Y += vertices[i + 1];
        }
        center_X /= 3.0f;
        center_Y /= 3.0f;

        // Вращаем вершины
        for (int k = 0, i = j * 9; k < 3; ++k, i += 3)
        {
            float x = vertices[i];
            float y = vertices[i + 1];

            float x_translated = x - center_X;
            float y_translated = y - center_Y;

            float x_rotated = x_translated * currentCos - y_translated * currentSin;
            float y_rotated = x_translated * currentSin + y_translated * currentCos;

            vertices[i]     = x_rotated + center_X;
            vertices[i + 1] = y_rotated + center_Y;
        }
    }
}

void glfwColorRandom()
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
        glfwColorRandom();
    }
}

GLuint compileShader(GLenum type, const char* source)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void glfwGenerateTriangle(GLfloat *vertices, size_t vertexSize, GLfloat *colors, size_t colorSize)
{
    /* Сreate a vertex shader for the object */
    GLuint id_vertex_shader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    /* Сreate a fragment shader for the object */
    GLuint id_fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);


    /* Link shaders to work together */
    shader_program =  glCreateProgram();
    glAttachShader(shader_program, id_vertex_shader);
    glAttachShader(shader_program, id_fragment_shader);
    glLinkProgram(shader_program);

    /* Clearing the memory occupied by shaders, since they are already linked */
    glDeleteShader(id_vertex_shader);
    glDeleteShader(id_fragment_shader); 

    // Generate buffer for vertices
    /* Generate it with buffer id */
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    /* Copies previously defined vertex data into buffer memory */
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_DYNAMIC_DRAW);
    /*  
        GL_STREAM_DRAW : данные задаются только один раз и используются графическим процессором максимум несколько раз.
        GL_STATIC_DRAW : данные устанавливаются только один раз и используются многократно.
        GL_DYNAMIC_DRAW : данные часто изменяются и используются много раз.
    */

    // Generate buffer for vertices
    /* Generate it with buffer id */
    glGenBuffers(1, &colors_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, colors_VBO);
    /* Copies previously defined vertex data into buffer memory */
    glBufferData(GL_ARRAY_BUFFER, colorSize, colors, GL_STATIC_DRAW);
    

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);  
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);  
    glBindBuffer(GL_ARRAY_BUFFER, colors_VBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindVertexArray(0);
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


    /* Make the window's context current */
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can`t load GLAD\n";
        return -1;
    }
    
    std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;
    glClearColor(glfwColorRed, glfwColorGreen, glfwColorBlue, glfwColorAlpha);
    glfwGenerateTriangle(vertices, sizeof(vertices), colors, sizeof(colors));

    float angle = 0.01f; // rafians

    while (!glfwWindowShouldClose(pWindow))
    {        
        /* React to resizing */
        glfwSetWindowSizeCallback(pWindow, glfwWindowsSizeCallback);

        /* Working with keys */
        glfwSetKeyCallback(pWindow, glfwKeyCallback);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glfwTriangleSpin(angle, vertices, sizeof(vertices)/sizeof(vertices[0]));

        /* */
        glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        /* Swap front and back buffers */
        glfwSwapBuffers(pWindow);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader_program);
    glfwTerminate();
    return 0;
}