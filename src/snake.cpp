#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include <cmath>


GLuint textureID;
GLuint vaoColor, vboColorVertices, vboColorColors;
GLuint vaoTexture, vboTextureVertices, vboTextureTexCoords;
GLuint colorShaderProgram, textureShaderProgram;

int glfwWindow_width = 480;
int glfwWindow_height = 480;

float glfwColorRed = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorGreen = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorBlue = static_cast<float>(std::rand() % 256) / 255.0f;
float glfwColorAlpha = static_cast<float>(std::rand() % 256) / 255.0f; // usually set to 1.0 for opaque

// Вершины первого треугольника (цветной)
GLfloat verticesColor[] = {
    -0.9f, -0.5f, 0.0f,
     0.0f, -0.5f, 0.0f,
    -0.45f, 0.5f, 0.0f
};

// Цвета для первого треугольника
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f, // красный
    0.0f, 1.0f, 0.0f, // зелёный
    0.0f, 0.0f, 1.0f  // синий
};

// Вершины второго треугольника (текстурированный)
GLfloat verticesTexture[] = {
     0.0f, -0.5f, 0.0f,
     0.9f, -0.5f, 0.0f,
     0.45f, 0.5f, 0.0f
};

// Текстурные координаты
GLfloat texCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};

const char* colorVertexSource = R"(
    #version 460
    layout(location = 0) in vec3 vertex_position;
    layout(location = 1) in vec3 vertex_color;

    out vec3 frag_color;

    void main() {
        frag_color = vertex_color;
        gl_Position = vec4(vertex_position, 1.0);
    }
)";

const char* colorFragmentSource = R"(
    #version 460
    in vec3 frag_color;
    out vec4 out_color;

    void main() {
        out_color = vec4(frag_color, 1.0);
    }
)";

const char* textureVertexSource = R"(
    #version 460
    layout(location = 0) in vec3 vertex_position;
    layout(location = 2) in vec2 vertex_texture;

    out vec2 textCord;

    void main() {
        textCord = vertex_texture;
        gl_Position = vec4(vertex_position, 1.0);
    }
)";

const char* textureFragmentSource = R"(
    #version 460
    in vec2 textCord;
    out vec4 frag_color;

    uniform sampler2D ourTexture;

    void main() {
        frag_color = texture(ourTexture, textCord);
    }
)";

void openImageANDGenerationText(const char * image)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(image, &width, &height, &nrChannels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << image << std::endl;
        return;
    }

    glGenTextures(1, &textureID);  
    glBindTexture(GL_TEXTURE_2D, textureID); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

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

    float center_X = 0.0f;
    float center_Y = 0.0f;

    size_t i = 0;
    for (int k = 0; k < 3; ++k, i += 3)
    {
        center_X += vertices[i];
        center_Y += vertices[i + 1];
    }
    center_X /= 3.0f;
    center_Y /= 3.0f;

    i = 0;
    // Вращаем вершины
    for (int k = 0; k < 3; ++k, i += 3)
    {
        float x = vertices[i];
        float y = vertices[i + 1];

        float x_translated = x - center_X;
        float y_translated = y - center_Y;

        float x_rotated = x_translated * cos_ang - y_translated * sin_ang;
        float y_rotated = x_translated * sin_ang + y_translated * cos_ang;

        vertices[i]     = x_rotated + center_X;
        vertices[i + 1] = y_rotated + center_Y;
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

void glfwGenerateTriangle()
{

    {
        GLuint vs = compileShader(GL_VERTEX_SHADER, colorVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, colorFragmentSource);
        colorShaderProgram = glCreateProgram();
        glAttachShader(colorShaderProgram, vs);
        glAttachShader(colorShaderProgram, fs);
        glLinkProgram(colorShaderProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    {
        GLuint vs = compileShader(GL_VERTEX_SHADER, textureVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, textureFragmentSource);
        textureShaderProgram = glCreateProgram();
        glAttachShader(textureShaderProgram, vs);
        glAttachShader(textureShaderProgram, fs);
        glLinkProgram(textureShaderProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
 
    // === Цветной треугольник ===
    glGenVertexArrays(1, &vaoColor);
    glBindVertexArray(vaoColor);

    glGenBuffers(1, &vboColorVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboColorVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &vboColorColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColorColors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);

    // === Текстурированный треугольник ===
    glGenVertexArrays(1, &vaoTexture);
    glBindVertexArray(vaoTexture);

    glGenBuffers(1, &vboTextureVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextureVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTexture), verticesTexture, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &vboTextureTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextureTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

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
    glfwGenerateTriangle();
    openImageANDGenerationText("container.jpg");

    float angle = 0.01f; // rafians

    while (!glfwWindowShouldClose(pWindow))
    {        
        /* React to resizing */
        glfwSetWindowSizeCallback(pWindow, glfwWindowsSizeCallback);

        /* Working with keys */
        glfwSetKeyCallback(pWindow, glfwKeyCallback);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glfwTriangleSpin(angle, verticesColor, sizeof(verticesColor)/sizeof(verticesColor[0]));
        glBindBuffer(GL_ARRAY_BUFFER, vboColorVertices);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesColor), verticesColor);

        glfwTriangleSpin(angle, verticesTexture, sizeof(verticesTexture)/sizeof(verticesTexture[0]));
        glBindBuffer(GL_ARRAY_BUFFER, vboTextureVertices);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesTexture), verticesTexture);
    
// GLuint vaoColor, vboColorVertices, vboColorColors;
// GLuint vaoTexture, vboTextureVertices, vboTextureTexCoords;
// GLuint colorShaderProgram, textureShaderProgram;

        glUseProgram(colorShaderProgram);
        glBindVertexArray(vaoColor);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(textureShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID); // Привязка текстуры

        GLint texLoc = glGetUniformLocation(textureShaderProgram, "ourTexture");
        if (texLoc != -1) {
            glUniform1i(texLoc, 0); // Текстурный юнит 0
        }

        glBindVertexArray(vaoTexture);
        glDrawArrays(GL_TRIANGLES, 0, 3); // Исправлено: начало с 0

        /* Swap front and back buffers */
        glfwSwapBuffers(pWindow);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vaoColor);
    glDeleteProgram(colorShaderProgram);
    glDeleteVertexArrays(1, &vaoTexture);
    glDeleteProgram(textureShaderProgram);

    glfwTerminate();
    return 0;
}